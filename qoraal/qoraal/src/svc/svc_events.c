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
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/common/lists.h"

static stack_t                          _svc_events_handler_stack[SVC_EVENTS_LAST] ;

#if !CFG_OS_OS_LESS

#if !defined CFG_OS_MUTEX_DISABLE
static OS_MUTEX_DECL            (_svc_events_mutex) ;
#endif
#define WORKING_THREAD_SIZE     (3072)
static p_thread_t               _svc_events_thread = 0 ;
static OS_THREAD_WORKING_AREA   (wa_svc_events_thread, WORKING_THREAD_SIZE);
static OS_EVENT_DECL            (_svc_events_event) ;

static bool                     _events_stop = false ;

static void
svc_events_thread(void *arg) 
{
    SVC_WDT_HANDLE_T hwdt ;
    svc_wdt_register (&hwdt, TIMEOUT_10_SEC) ;
    while (!_events_stop) {
        uint32_t eid;

        svc_wdt_deactivate (&hwdt) ;

        uint32_t  events = os_event_wait (&_svc_events_event, 1, ~0, 0) ;

        eid = 0;

        svc_wdt_activate (&hwdt) ;

#if !defined CFG_OS_MUTEX_DISABLE
        os_mutex_lock (&_svc_events_mutex) ;
#endif

        while (events && !_events_stop) {
            if (events & (1<<eid)) {

                events &= ~(1<<eid);

                SVC_EVENTS_HANDLER_T* start ;

                for ( start = (SVC_EVENTS_HANDLER_T*)stack_head (&_svc_events_handler_stack[eid]) ;
                    (start!=NULL_LLO)
                        ; ) {

                    start->fp (eid, start->ctx) ;
                    start = (SVC_EVENTS_HANDLER_T*)stack_next ((plists_t)start, OFFSETOF(SVC_EVENTS_HANDLER_T, next));

                }


            }
            eid++;

          }

#if !defined CFG_OS_MUTEX_DISABLE
          os_mutex_unlock (&_svc_events_mutex) ;
#endif

    }

    svc_wdt_unregister (&hwdt, TIMEOUT_10_SEC) ;

    return  ;
}
#else

static uint32_t                         _svc_events_event ;

void
svc_events_handle (void)
{
    uint32_t events  ;

    osalSysLock () ;
    events = _svc_events_event ;
    _svc_events_event = 0 ;
    osalSysUnlock () ;
    
    uint32_t eid = 0 ;

    while (events) {
    if (events & (1<<eid)) {

        events &= ~(1<<eid);

        SVC_EVENTS_HANDLER_T* start ;

        for ( start = (SVC_EVENTS_HANDLER_T*)stack_head (&_svc_events_handler_stack[eid]) ;
            (start!=NULL_LLO)
                ; ) {

            start->fp (eid, start->ctx) ;
            start = (SVC_EVENTS_HANDLER_T*)stack_next ((plists_t)start, OFFSETOF(SVC_EVENTS_HANDLER_T, next));

        }



        STATS_COUNTER_INC(event_cnt) ;
    }
    eid++;

    }

}
#endif /* CFG_OS_OS_LESS */

/**
 * @brief   Initializes events.
 *
 * @svc
 */
int32_t
svc_events_init (void)
{
    int i ;
#if !defined CFG_OS_MUTEX_DISABLE
    os_mutex_init (&_svc_events_mutex) ;
#endif

    for (i=0; i<SVC_EVENTS_LAST; i++) {
        stack_init (&_svc_events_handler_stack[i]) ;
    }

    os_event_init (&_svc_events_event) ;

    return EOK ;
}

/**
 * @brief   Start the events.
 *
 *
 * @svc
 */
int32_t
svc_events_start (void)
{
    _events_stop = false ;
#if !CFG_OS_OS_LESS
    return os_thread_create_static (wa_svc_events_thread, sizeof(wa_svc_events_thread),
            OS_THREAD_PRIO_HIGHEST, svc_events_thread,
            0, &_svc_events_thread, "svc-events") ;
#endif
    return EOK ;
}

/**
 * @brief   Start the events.
 *
 *
 * @svc
 */
void
svc_events_stop (void)
{
    if (!_events_stop) {
        _events_stop = true ;
        svc_events_signal (0) ;
    }
}

/**
 * @brief   Register an event listener.
 *
 * @param[in] id            Event identifier
 * @param[in] handler       User allocated entry for the event list
 * @param[in] fp            Callback handler
 *
 * @return              Error.
 *
 * @svc
 */
void
svc_events_register (SVC_EVENTS_T id, SVC_EVENTS_HANDLER_T * handler, SVC_EVENTS_CALLBACK_T fp, void * ctx)
{
#if !defined CFG_OS_MUTEX_DISABLE
    os_mutex_lock (&_svc_events_mutex) ;
#else
    osalSysLock () ;
#endif

    stack_remove (&_svc_events_handler_stack[id], (plists_t)handler, OFFSETOF(SVC_EVENTS_HANDLER_T, next)) ;
    handler->fp = fp ;
    handler->ctx = ctx ;
    stack_add_head (&_svc_events_handler_stack[id], handler, OFFSETOF(SVC_EVENTS_HANDLER_T, next)) ;

#if !defined CFG_OS_MUTEX_DISABLE
    os_mutex_unlock (&_svc_events_mutex) ;
#else
    osalSysUnlock () ;
#endif
    return  ;
}

/**
 * @brief   Remove an event listener.
 *
 * @param[in] id            Event identifier
 * @param[in] handler       registered event handler
 *
 * @svc
 */
void 
svc_events_unregister (SVC_EVENTS_T id,  SVC_EVENTS_HANDLER_T * handler)
{
#if !defined CFG_OS_MUTEX_DISABLE
    os_mutex_lock (&_svc_events_mutex) ;
#else
    osalSysLock () ;
#endif

    stack_remove (&_svc_events_handler_stack[id], (plists_t)handler, OFFSETOF(SVC_EVENTS_HANDLER_T, next)) ;

#if !defined CFG_OS_MUTEX_DISABLE
    os_mutex_unlock (&_svc_events_mutex) ;
#else
    osalSysUnlock () ;
#endif
}

/**
 * @brief   Signal an event from an interrupt context.
 *
 * @param[in] id            Event identifier
 *
 * @svc
 */
void
svc_events_signal_isr (SVC_EVENTS_T id)
{
#if !CFG_OS_OS_LESS
    os_event_signal_isr (&_svc_events_event, (1<<id)) ;
#else
    osalSysLockFromISR () ;
    _svc_events_event |= (1<<id);
    osalSysUnlockFromISR () ;
#endif
}

/**
 * @brief   Signal an event.
 *
 * @param[in] id            Event identifier
 *
 * @svc
 */
void
svc_events_signal (SVC_EVENTS_T id)
{
#if !CFG_OS_OS_LESS    
    os_event_signal (&_svc_events_event, (1<<id)) ;
#else
    osalSysLock () ;
    _svc_events_event |= (1<<id);
    osalSysUnlock () ;
#endif
}



