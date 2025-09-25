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
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/common/lists.h"


#define SVC_WDT_FLAGS_ACTIVE                (1<<0)
#define SVC_WDT_FLAGS_KICKED                (1<<1)
#define SVC_WDT_FLAGS_FLAGGED               (1<<2)
#define SVC_WDT_FLAGS_REPORTED              (1<<3)


#ifndef CFG_SVC_WDT_DISABLE_PLATFORM

static void     svc_wdt_task_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason) ;
static uint8_t  svc_wdt_process (SVC_WDT_TIMEOUTS_T id) ;

static uint32_t             _svc_wdt_interval = 0 ;
static OS_MUTEX_DECL        (_svc_wdt_mutex) ;
static stack_t              _svc_wdt_handler_stack[TIMEOUT_LAST] ;
static uint32_t             _svc_wdt_counter = 0 ;
static SVC_TASKS_DECL		(_svc_wdt_task)  ;
#endif


/**
 * @brief   Initializes events.
 *
 * @svc
 */
int32_t
svc_wdt_init (void)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    int i ;
    DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_INFO, " -->> svc_wdt_init") ;
    os_mutex_init (&_svc_wdt_mutex) ;

    for (i=0; i<TIMEOUT_LAST; i++) {
        stack_init (&_svc_wdt_handler_stack[i]) ;
    }
#endif
    return EOK ;
}

/**
 * @brief   Start the wdt.
 *
 *
 * @svc
 */
int32_t
svc_wdt_start (void)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    _svc_wdt_interval = qoraal_wdt_kick () ;

    if (_svc_wdt_interval) {
        svc_tasks_cancel (&_svc_wdt_task) ;
        return svc_tasks_schedule (&_svc_wdt_task, svc_wdt_task_cb, 0, SERVICE_PRIO_QUEUE0, SVC_TASK_S2TICKS(_svc_wdt_interval/2)) ;

    }

    return EOK ;
#else
    return EOK ;
#endif
}

/**
 * @brief   Stop the wdt.
 *
 *
 * @svc
 */
void
svc_wdt_stop (void)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    svc_tasks_cancel (&_svc_wdt_task) ;

#endif
    return ;
}

/**
 * @brief   Register a watchdog handler listener.
 *
 * @param[in] handler       Caller allocated handle structure
 * @param[in] id            TImeout group
 *
 * @return              Error.
 *
 * @svc
 */
void
svc_wdt_register (SVC_WDT_HANDLE_T * handler, SVC_WDT_TIMEOUTS_T id)
{
    memset (handler, 0, sizeof(SVC_WDT_HANDLE_T)) ;
    handler->thread = os_thread_current () ; ;
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    if (id < TIMEOUT_LAST) {
        os_mutex_lock (&_svc_wdt_mutex) ;

        //stack_remove (&_svc_wdt_handler_stack[id], (plists_t)handler, OFFSETOF(SVC_WDT_HANDLE_T, next)) ;
        stack_add_head (&_svc_wdt_handler_stack[id], handler, OFFSETOF(SVC_WDT_HANDLE_T, next)) ;

        os_mutex_unlock (&_svc_wdt_mutex) ;
    }
#endif
    return  ;
}

/**
 * @brief   Remove a watchdog handler listener.
 *
 * @param[in] handler       Caller allocated handle structure
 * @param[in] id            TImeout group
 *
 * @svc
 */
void 
svc_wdt_unregister (SVC_WDT_HANDLE_T * handler, SVC_WDT_TIMEOUTS_T id)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    if (id < TIMEOUT_LAST) {
        os_mutex_lock (&_svc_wdt_mutex) ;
        stack_remove (&_svc_wdt_handler_stack[id], (plists_t)handler, OFFSETOF(SVC_WDT_HANDLE_T, next)) ;
        os_mutex_unlock (&_svc_wdt_mutex) ;
    }
#endif
}


void
svc_wdt_activate (SVC_WDT_HANDLE_T * handler)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    p_thread_t thread = os_thread_current () ;
    os_mutex_lock (&_svc_wdt_mutex) ;
    handler->thread = thread ;
    handler->flags = (SVC_WDT_FLAGS_ACTIVE|SVC_WDT_FLAGS_KICKED) ;
    os_mutex_unlock (&_svc_wdt_mutex) ;
#endif
}

void
svc_wdt_set_id (SVC_WDT_HANDLE_T * handler, uintptr_t id)
{
    handler->id = id ;
}


void
svc_wdt_deactivate (SVC_WDT_HANDLE_T * handler)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    if (handler->flags & SVC_WDT_FLAGS_REPORTED) {
        DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_REPORT,
                "WDT   : : deactivate '%s' (0x%x) as reported",
                os_thread_get_name(&handler->thread), handler->id) ;

    }

    os_mutex_lock (&_svc_wdt_mutex) ;
    handler->flags &= ~(SVC_WDT_FLAGS_ACTIVE|SVC_WDT_FLAGS_KICKED|SVC_WDT_FLAGS_FLAGGED|SVC_WDT_FLAGS_REPORTED) ;
    os_mutex_unlock (&_svc_wdt_mutex) ;
#endif
}

void
svc_wdt_handler_kick (SVC_WDT_HANDLE_T * handler)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    if (handler->flags & SVC_WDT_FLAGS_REPORTED) {
        DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_REPORT,
                "WDT   : : kick '%s' (0x%x) as reported",
                os_thread_get_name(&handler->thread), handler->id) ;

    }

    os_mutex_lock (&_svc_wdt_mutex) ;
    handler->flags |= (SVC_WDT_FLAGS_KICKED) ;
    handler->flags &= (SVC_WDT_FLAGS_REPORTED|SVC_WDT_FLAGS_FLAGGED) ;
    os_mutex_unlock (&_svc_wdt_mutex) ;
#endif
}

void
svc_wdt_kick (void)
{
#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
    DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_INFO,
            "WDT   : : kick");
    qoraal_wdt_kick () ;
#endif
}

#ifndef CFG_SVC_WDT_DISABLE_PLATFORM
uint8_t
svc_wdt_process (SVC_WDT_TIMEOUTS_T id)
{
    int kick = 1 ;
    SVC_WDT_HANDLE_T* start ;

    for ( start = (SVC_WDT_HANDLE_T*)stack_head (&_svc_wdt_handler_stack[id]) ;
        (start!=NULL_LLO)
            ; ) {

        if (start->flags & SVC_WDT_FLAGS_ACTIVE) {

            if (start->flags & SVC_WDT_FLAGS_KICKED) {
                start->flags &= ~(SVC_WDT_FLAGS_KICKED|SVC_WDT_FLAGS_FLAGGED|SVC_WDT_FLAGS_REPORTED) ;

            } else {
                if (start->flags & SVC_WDT_FLAGS_FLAGGED) {

                    if (!(start->flags & SVC_WDT_FLAGS_REPORTED)) {
                        start->flags |= SVC_WDT_FLAGS_REPORTED ;
                        DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_WARNING,
                            "WDT   : : reported '%s' (0x%x)",
                            os_thread_get_name(&start->thread), start->id) ;
                    }
                    kick = 0 ;
#if 0
                    if (id == TIMEOUT_10_SEC) STATS_COUNTER_INC(wdt_10) ;
                    else if (id == TIMEOUT_30_SEC) STATS_COUNTER_INC(wdt_30) ;
                    else if (id == TIMEOUT_60_SEC) STATS_COUNTER_INC(wdt_60) ;
#endif
                } else {

                    start->flags |= SVC_WDT_FLAGS_FLAGGED ;
                    DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_LOG,
                            "WDT   : : flagging '%s' (0x%x)",
                            os_thread_get_name(&start->thread), start->id) ;

                }

            }

        }

        start = (SVC_WDT_HANDLE_T*)stack_next ((plists_t)start, OFFSETOF(SVC_WDT_HANDLE_T, next));

    }

    return kick ;
}

static void
svc_wdt_task_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    if (reason == SERVICE_CALLBACK_REASON_RUN) {

        static uint8_t kick10 = 1, kick30 = 1, kick60 = 1  ;
        
        os_mutex_lock (&_svc_wdt_mutex) ;
        _svc_wdt_counter++ ;
        kick10 = svc_wdt_process (TIMEOUT_10_SEC) ;
        if (!kick30 || !(_svc_wdt_counter%(30/_svc_wdt_interval))) {
            kick30 = svc_wdt_process (TIMEOUT_30_SEC) ;
        }
        if (!kick60 || !(_svc_wdt_counter%(60/_svc_wdt_interval))) {
            kick60 = svc_wdt_process (TIMEOUT_60_SEC) ;
        }

        if (kick10 && kick30 && kick60) {
            _svc_wdt_interval =  qoraal_wdt_kick () ;
            DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_INFO,
                    "WDT   : : task kick");

        } else {
            DBG_MESSAGE_SVC_WDT (DBG_MESSAGE_SEVERITY_INFO,
                    "WDT   : : task NOT kicked");

        }
        os_mutex_unlock (&_svc_wdt_mutex) ;

        if (_svc_wdt_interval) {
            svc_tasks_schedule (&_svc_wdt_task, svc_wdt_task_cb, 0, SERVICE_PRIO_QUEUE0, SVC_TASK_S2TICKS(_svc_wdt_interval/2)) ;

        }

    }

    svc_tasks_complete (&_svc_wdt_task) ;

}
#endif


