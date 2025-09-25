/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
    SPDX-License-Identifier: MIT

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */




#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/common/lists.h"


static uint32_t            _svc_threads_list_count = 0 ;
static LISTS_LINKED_DECL   (_svc_threads_list) ;
static LISTS_LINKED_DECL   (_svc_threads_complete) ;
static OS_MUTEX_DECL       (_svc_threads_mutex) ;

static void		svc_threads_terminate (void) ;

#if !defined CFG_SVC_THREADS_DISABLE_IDLE
static OS_SEMAPHORE_DECL (_svc_threads_sem) ;
static p_thread_t   _svc_threads_idle = 0 ;

static void svc_threads_idle( void * arg)
{
    while (1) {
        os_sem_wait (&_svc_threads_sem) ;
        svc_threads_count () ;
    }
}
#endif

/**
 * @brief   svc_threads_init
 * @return              Error.
 * @svc
 */
int32_t
svc_threads_init (void)
{
    linked_init (&_svc_threads_list) ;
    linked_init (&_svc_threads_complete) ;
    os_mutex_init (&_svc_threads_mutex) ;
#if !defined CFG_SVC_THREADS_DISABLE_IDLE
    os_sem_init (&_svc_threads_sem, 0) ;
#endif

    return EOK ;
}

/**
 * @brief   svc_threads_init
 * @return              Error.
 * @svc
 */
int32_t
svc_threads_start (void)
{
    int32_t res ;

#if !defined CFG_SVC_THREADS_DISABLE_IDLE
    res = os_thread_create (1024, OS_THREAD_PRIO_LOWEST, svc_threads_idle,
                            0, &_svc_threads_idle, "svc_threads_idle") ;
#else
    res = EOK ;
#endif

    return res ;
}


static void 
svc_thread_start (void * parm)
{
    SVC_THREADS_T* svc_thread = (SVC_THREADS_T*)parm ;
    svc_thread->pf (svc_thread->arg) ;
    svc_threads_terminate () ;
}

/**
 * @brief   svc_threads_create
 *
 * @param[in] thread        caller allocated SVC_THREADS_T structure
 * @param[in] complete      callback for complete or errors
 * @param[in] stack_size
 * @param[in] prio
 * @param[in] pf
 * @param[in] arg
 * @param[in] name
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_threads_create (SVC_THREADS_T* thread, SVC_THREADS_COMPLETE_CALLBACK_T complete,
                            size_t stack_size, uint32_t prio, p_thread_function_t pf,
                            void *arg, const char* name)
{
    int32_t res = EFAIL ;

    thread->arg = arg ;
    thread->complete = complete ;
    thread->pf = pf ;

    if (os_sys_started()) os_mutex_lock(&_svc_threads_mutex) ;
    res = os_thread_create (stack_size, prio, svc_thread_start,
                            thread, &thread->thread, name) ;

    if (res == EOK) {
        _svc_threads_list_count++ ;
        linked_add_tail (&_svc_threads_list, thread, OFFSETOF(SVC_THREADS_T, next)) ;

        DBG_MESSAGE_SVC_THREADS (DBG_MESSAGE_SEVERITY_INFO, 
                "SVC   : : svc_threads_register '%s' (0x%x -> 0x%x) count %d",
                name, thread, thread->thread, _svc_threads_list_count+1) ;

    } else {
        thread->pf = 0 ;

    }
    if (os_sys_started()) os_mutex_unlock(&_svc_threads_mutex) ;

    return res ;
}

/**
 * @brief   svc_threads_terminate
 * @note    move the thread to the start of the list for cleanup.
 *
 * @param[in] arg  argument for callback function
 *
 * @return              Error.
 *
 * @svc
 */
void
svc_threads_terminate (void)
{
    p_thread_t  thread ;
    SVC_THREADS_T* start ;

    os_mutex_lock(&_svc_threads_mutex) ;
    thread = os_thread_current () ;

    for ( start = (SVC_THREADS_T*)linked_head (&_svc_threads_list) ;
        (start!=NULL_LLO)
            ; ) {

        if (thread == start->thread) {
            DBG_MESSAGE_SVC_THREADS (DBG_MESSAGE_SEVERITY_INFO, 
                "SVC   : : svc_threads_terminate (obj 0x%x, thd 0x%x)",
                start, start->thread) ;
             break ;
        }

        start = (SVC_THREADS_T*)linked_next (start, OFFSETOF(SVC_THREADS_T, next));
    }

    if (start) {
        _svc_threads_list_count-- ;
        linked_remove (&_svc_threads_list, start, OFFSETOF(SVC_THREADS_T, next)) ;
        linked_add_head (&_svc_threads_complete, start, OFFSETOF(SVC_THREADS_T, next)) ;

#if !defined CFG_SVC_THREADS_DISABLE_IDLE
        os_sem_signal (&_svc_threads_sem) ;
#endif

    }

    os_mutex_unlock(&_svc_threads_mutex) ;

}

static inline int32_t
svc_threads_complete_check (void)
{
    SVC_THREADS_T* start  ;

    start = (SVC_THREADS_T*)linked_head (&_svc_threads_complete) ;
    if (start==NULL_LLO) {
       return E_SRCH ;

    }

    os_mutex_lock(&_svc_threads_mutex) ;
    start = (SVC_THREADS_T*)linked_head (&_svc_threads_complete) ;
    if (start!=NULL_LLO) {
        p_thread_t thread = start->thread ;
        linked_remove_head (&_svc_threads_complete, OFFSETOF(SVC_THREADS_T, next));

        DBG_MESSAGE_SVC_THREADS (DBG_MESSAGE_SEVERITY_INFO, 
                "SVC   : : svc_threads_complete_check delete %x",
                thread) ;

        os_thread_join (&thread) ;

        SVC_THREADS_COMPLETE_CALLBACK_T complete = start->complete ;
        void* arg = start->arg ;
        start->pf = 0 ;

        os_mutex_unlock(&_svc_threads_mutex) ;

        if (complete) {
            complete (start, arg) ;
        }

        return EOK ;

    } 
    os_mutex_unlock(&_svc_threads_mutex) ;

    return E_SRCH ;
}

/**
 * @brief   svc_threads_count
 * @note    calls the cleanup function for terminated threads.
 *
 *
 * @return              number of threads.
 *
 * @svc
 */
uint32_t
svc_threads_count (void)
{
    while (svc_threads_complete_check () == EOK) ;

    return _svc_threads_list_count ;
}

/**
 * @brief   svc_threads_is_active
 *
 * @param[in] svc_thread
 *
 * @return              status of thread
 *
 * @svc
 */
bool
svc_threads_is_active (SVC_THREADS_T* svc_thread)
{
    return svc_thread->pf != 0 ;
}

/**
 * @brief   vApplicationIdleHook
 * @note    FreeRTOS idle hook to call cleanup function for terminated threads.
 *
 *
 * @svc
 */
#if defined CFG_SVC_THREADS_DISABLE_IDLE && defined CFG_OS_FREERTOS
void vApplicationIdleHook(void)
{
    svc_threads_complete_check () ;
}
#endif
