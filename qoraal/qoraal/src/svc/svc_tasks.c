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


/**
 * @file    services.c
 * @brief   Tasks (thread pool)
 *
 * @addtogroup svc_tasks
 * @details Tasks can be scheduled on a thread, either immediately or
 *          with a timeout.
 * @{
 */


#include <stdio.h>
#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/common/lists.h"

#define SVC_PRIO2QUEUE(prio)            (prio < _svc_tasks_pool_count ? prio : (_svc_tasks_pool_count - 1))

static SVC_TASK_CFG_T                   _svc_tasks_pool = {{SVC_TASK_CFG_DEFAULT}} ;
static uint32_t                         _svc_tasks_pool_count = SVC_TASK_CFG_MAX ;
LISTS_LINKED_DECL                       (_svc_tasks_list) ;
static uint32_t                         _svc_tasks_list_count = 0 ;
static OS_MUTEX_DECL                    (_svc_task_mutex) ;
static linked_t                         _svc_tasks_ready_list[SERVICE_PRIO_QUEUE_MAX] = {0} ;
static uint32_t                         _svc_tasks_ready_count = 0 ;
static uint32_t                         _svc_tasks_run = 1 ;
static uint32_t                         _svc_task_active_timer[SERVICE_PRIO_QUEUE_MAX] = {0};
static SVC_TASKS_T *                    _svc_task_active_task[SERVICE_PRIO_QUEUE_MAX] = {0};
static OS_TIMER_DECL                    (_svc_tasks_virtual_timer) ;
static SVC_EVENTS_HANDLER_T             _svc_tasks_event_handler ;
static p_thread_t                       _svc_task_threads[SERVICE_PRIO_QUEUE_MAX] = {0};
static OS_EVENT_DECL                    (_svc_tasks_complete_event) ;

static void svc_tasks_task_event (SVC_EVENTS_T id, void * ctx) ;
static uint32_t svc_tasks_service_task (linked_t* task_list, int thd_count, SVC_WDT_HANDLE_T * hwdt) ;

/**
 * @brief   OS timer callback.
 * @details If the timer callback occurs in an interrupt context (ChibiOS, ThreadX) the timer
 *          schedules an event to handle the timeout. If the timer is already
 *          on a thread context the event function is called directly (FreeRTOS, Posix).
 *
 * @param[in] p         timer context
 * @return              void.
 *
 * @notapi
 */
void svc_tasks_virtual_timer(void *p)
{
#if 1
	svc_events_signal_isr (SVC_EVENTS_TASK) ;
#else
    if (os_sys_is_irq ())  svc_events_signal_isr (SVC_EVENTS_TASK) ;
    else svc_tasks_task_event (SVC_EVENTS_TASK, 0) ;
#endif
}

/**
 * @brief   Task thread.
 * @details If signaled, it takes pending tasks from its queue and call the task function.
 *
 * @param[in] arg       thread context, queue number for this thread
 * @return              void.
 *
 * @notapi
 */
static void svc_tasks_thread (void *arg) {

    unsigned int count = (uintptr_t) arg ;
    SVC_WDT_HANDLE_T hwdt ;
    uint32_t mask =  1 << (count) ;

    svc_wdt_register (&hwdt, _svc_tasks_pool.pool[count].wdt) ;
    while (_svc_tasks_run) {

        os_event_signal (&_svc_tasks_complete_event, mask) ;
        svc_wdt_deactivate (&hwdt) ;
        os_thread_wait (OS_TIME_INFINITE) ;
        if (!_svc_tasks_run) {
            break ;

        }
        svc_wdt_activate (&hwdt) ;
        while (svc_tasks_service_task (&_svc_tasks_ready_list[count], count,  &hwdt)) ;

    }
    svc_wdt_unregister ( &hwdt, _svc_tasks_pool.pool[count].wdt) ;

    return  ;
}

uint32_t
svc_task_expire (SVC_TASKS_T * task)
{
    int32_t timeout = 0 ;
    os_mutex_lock (&_svc_task_mutex) ;
    if (svc_tasks_status (task) == SERVICE_STATUS_QUEUED) {
        timeout = task->ticks - os_sys_ticks () ;
        if (timeout < 0 ) timeout = 0 ;

    }
    os_mutex_unlock (&_svc_task_mutex) ;
    return (uint32_t)timeout ;
}

uint32_t
svc_task_next_expire (void)
{
    int32_t timeout = 0 ;
    SVC_TASKS_T* next ;
    os_mutex_lock (&_svc_task_mutex) ;
    next = (SVC_TASKS_T*)linked_head (&_svc_tasks_list) ;
    if (next) {
        timeout = next->ticks - os_sys_ticks () ;
        if (timeout < 0)  timeout = 0 ;

    }
    os_mutex_unlock (&_svc_task_mutex) ;
    return (uint32_t)timeout ;
}



uint32_t
svc_task_get_active_ticks (uint16_t prio, SVC_TASKS_CALLBACK_T* callback, uintptr_t* parm)
{
    uint32_t timer = 0 ;
    if (prio < _svc_tasks_pool_count) {
        os_sys_lock();
        if (_svc_task_active_task[prio]) {
            timer = os_sys_ticks() - _svc_task_active_timer[prio] ;
            if (callback) *callback = _svc_task_active_task[prio]->callback ;
            if (parm) *parm = _svc_task_active_task[prio]->parm ;

        }
        os_sys_unlock();

    }
    return timer  ;
}


/**
 * @brief   Initializes the tasks.
 *
 *
 * @return              Error.
 *
 * @notapi
 */
int32_t
svc_tasks_init (const SVC_TASK_CFG_T * pool)
{
    if (pool) {
        memcpy(&_svc_tasks_pool, pool, sizeof(SVC_TASK_CFG_T));
        for (_svc_tasks_pool_count=0; 
             _svc_tasks_pool.pool[_svc_tasks_pool_count].prio;
               _svc_tasks_pool_count++) ;

    }
    os_timer_init (&_svc_tasks_virtual_timer, svc_tasks_virtual_timer, 0) ;
    os_mutex_init (&_svc_task_mutex) ;
    os_event_init (&_svc_tasks_complete_event) ;
    return EOK ;
}

/**
 * @brief   Start the tasks.
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_tasks_start (void)
{
    uintptr_t i ;

    _svc_tasks_run = 1 ;

    svc_events_register (SVC_EVENTS_TASK, &_svc_tasks_event_handler, svc_tasks_task_event, 0) ;

    for (i=0; i<_svc_tasks_pool_count; i++) {
        os_event_clear (&_svc_tasks_complete_event, 1<<i) ;
        int32_t res = os_thread_create (_svc_tasks_pool.pool[i].stack, _svc_tasks_pool.pool[i].prio,
        		svc_tasks_thread, (void*)i, &_svc_task_threads[i], _svc_tasks_pool.pool[i].name) ;
        if (res != EOK) {
            DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_ASSERT,
                "SVC   :A: svc_tasks_start fail %d for task %d", res , i) ;

        } else {
            uint32_t started = os_event_wait_timeout (&_svc_tasks_complete_event, 1, 1<<i, 0, OS_MS2TICKS(1000)) ;
            if (!started) {
                DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_ASSERT,
                                "SVC   :A: svc_tasks_start timeout for task %d", i) ;

            }

        }

    }

    return EOK ;
}

int32_t
svc_tasks_stop (uint32_t timeout)
{
    /* ToDo: fixme  */
    svc_events_unregister (SVC_EVENTS_TASK, &_svc_tasks_event_handler) ;
    os_timer_reset (&_svc_tasks_virtual_timer) ;
    _svc_tasks_run = 0 ;

    return timeout ?  EOK : E_TIMEOUT ;
}

void
svc_tasks_init_task (SVC_TASKS_T* task)
{
    task->status = SERVICE_STATUS_COMPLETE ;
    task->flags = 0 ;
    task->user = 0 ;
}

int32_t
svc_tasks_init_waitable_task (SVC_WAITABLE_TASKS_T* task)
{
    svc_tasks_init_task(&task->task) ;
    task->event = 0 ;
    p_event_t event  ;
    int32_t res = os_event_create (&event) ;
    if (res == EOK) {
        task->event = event ;
        task->task.flags = SVC_TASKS_FLAGS_WAITABLE ;

    }
    return res ;
}

void
svc_tasks_deinit_waitable_task (SVC_WAITABLE_TASKS_T* task)
{
    p_event_t event = ((SVC_WAITABLE_TASKS_T*)task)->event ;
    os_event_delete (&event) ;
}

static void
task_to_ready (SVC_TASKS_T* task)
{
    uint16_t queue = SVC_PRIO2QUEUE(task->prio) ;
    uint32_t mask = 1 << queue ;
    DBG_ASSERT_SVC_TASKS ((queue < _svc_tasks_pool_count) && task->callback,
            "task_to_ready invalid: task 0x%x queue %d callback 0x%x",
            task, queue, task->callback) ;

    linked_add_tail (&_svc_tasks_ready_list[queue], task, OFFSETOF(SVC_TASKS_T, next)) ;
    _svc_tasks_ready_count++ ;

    os_event_clear (&_svc_tasks_complete_event, mask) ;

    if (_svc_task_threads[queue]) os_thread_notify (&_svc_task_threads[queue], EOK) ;
}

void
svc_tasks_task_event (SVC_EVENTS_T id, void * ctx)
{
    SVC_TASKS_T* start ;
    uint32_t now  ;
    (void)id ;

    DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_INFO,
        "SVC   : : svc_tasks_task_event count %d tasks %d", 
        _svc_tasks_list_count, _svc_tasks_ready_count);

    os_mutex_lock (&_svc_task_mutex) ;

    start = (SVC_TASKS_T*)linked_head (&_svc_tasks_list) ;
    if (start) {
        now = os_sys_ticks () ;
        if ((int32_t)(start->ticks - now) > 0) {
            os_timer_set (&_svc_tasks_virtual_timer, start->ticks - now) ;

        } else {
            do {
                DBG_ASSERT_SVC_TASKS (_svc_tasks_list_count,
                        "svc_tasks_rtc_event 1 _svc_tasks_list_count invalid") ;
                _svc_tasks_list_count-- ;
                linked_remove_head (&_svc_tasks_list, OFFSETOF(SVC_TASKS_T, next)) ;
                task_to_ready (start) ;
                start = (SVC_TASKS_T*)linked_head (&_svc_tasks_list) ;
                now = os_sys_ticks () ;

            } while (start && ((int32_t)(start->ticks - now) <= 0)) ;

            if (start) {
                os_timer_set (&_svc_tasks_virtual_timer, start->ticks - now) ;

            }

        }

    }

    os_mutex_unlock (&_svc_task_mutex) ;
    DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_INFO,
            "SVC   : : svc_tasks_rtc_event count %d tasks %d", 
            _svc_tasks_list_count, _svc_tasks_ready_count);

}

static uint32_t
svc_tasks_service_task ( linked_t* task_list, int thd_count, SVC_WDT_HANDLE_T * hwdt)
{
    SVC_TASKS_T* start ;
    uint32_t timer ;

    os_mutex_lock (&_svc_task_mutex) ;

    start = (SVC_TASKS_T*)linked_head (task_list) ;
    if (start) {
        uintptr_t parm = start->parm ;
        SVC_TASKS_CALLBACK_T callback = start->callback ;
        linked_remove_head (task_list, OFFSETOF(SVC_TASKS_T, next)) ;
        DBG_ASSERT_SVC_TASKS (_svc_tasks_ready_count, 
                "svc_tasks_service_task 1 _svc_tasks_ready_count invalid") ;
        _svc_tasks_ready_count-- ;
#ifndef NDEBUG
        if (!_svc_tasks_ready_count) {
            DBG_ASSERT_SVC_TASKS (!linked_head(task_list),
                "svc_tasks_service_task 2 _svc_tasks_ready_count invalid") ;

        }
#endif
        start->status = SERVICE_STATUS_ACTIVE_BIT ;
        os_mutex_unlock (&_svc_task_mutex) ;

        timer = os_sys_ticks() ;

        os_sys_lock();
        _svc_task_active_task[thd_count] = start ;
        _svc_task_active_timer[thd_count] = timer ;
        svc_wdt_set_id (hwdt, (uintptr_t)callback) ;
        os_sys_unlock();

        callback (start, parm, SERVICE_CALLBACK_REASON_RUN) ;

        os_sys_lock();
        _svc_task_active_task[thd_count] = 0 ;
        _svc_task_active_timer[thd_count] = 0 ;
        svc_wdt_set_id (hwdt, 0) ;
        os_sys_unlock();

#ifndef NDEBUG
        timer = os_sys_ticks() - timer;
        if (timer > OS_MS2TICKS(2500)) {
            DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_REPORT,
                "SVC   : : svc_tasks_service_task %d elapsed for 0x%x queue %d",
                OS_TICKS2MS(timer), callback, thd_count);

        }
#endif

    } else {
         os_mutex_unlock (&_svc_task_mutex) ;

    }

    return start ? 1 : 0 ;
}

void
svc_tasks_complete (SVC_TASKS_T* task)
{
    task->status &= ~SERVICE_STATUS_ACTIVE_BIT ;
    if (task->flags & SVC_TASKS_FLAGS_WAITABLE) {
        p_event_t event = ((SVC_WAITABLE_TASKS_T*)task)->event ;
        os_event_signal (&event, 1) ;

    }

}

int32_t
svc_tasks_add (SVC_TASKS_T* task, SVC_TASKS_CALLBACK_T callback, uintptr_t parm, uint16_t prio)
{
    int32_t status  ;

    os_mutex_lock (&_svc_task_mutex) ;
    if (svc_tasks_status(task) == SERVICE_STATUS_QUEUED) {
        status = E_BUSY ;

    } else {
        task->callback = callback ;
        task->parm = parm ;
        task->status = SERVICE_STATUS_QUEUED ;
        task->prio = prio ;
        task->ticks = 0 ;
        task_to_ready (task) ;
        status  = EOK  ;

    }
    os_mutex_unlock (&_svc_task_mutex) ;

    return status ;
}


int32_t
svc_tasks_schedule (SVC_TASKS_T* task, SVC_TASKS_CALLBACK_T callback, uintptr_t parm, uint16_t prio, uint32_t ticks)
{
    SVC_TASKS_T* previous ;
    SVC_TASKS_T* start ;

    if (ticks >= (((uint32_t)-1) / 2)) {
        DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_WARNING,
            "SVC   : : svc_tasks_schedule timeout %d out of range", ticks);
        ticks = (((uint32_t)-1) / 2) - 1 ;
    }

    DBG_ASSERT_SVC_TASKS (ticks < ((uint32_t)-1) / 2, 
            "svc_tasks_schedule invalid ticks %d!!", ticks) ;

    if (task->flags & SVC_TASKS_FLAGS_WAITABLE) {
        p_event_t event = ((SVC_WAITABLE_TASKS_T*)task)->event ;
        os_event_clear (&event, 1) ;
    }

    if (ticks == 0) {
        return svc_tasks_add ((SVC_TASKS_T*) task, callback, parm, prio) ;
    }

    os_mutex_lock (&_svc_task_mutex) ;

    if (svc_tasks_status(task) == SERVICE_STATUS_QUEUED) {
        os_mutex_unlock (&_svc_task_mutex) ;
        return  E_BUSY ;
    } 

    task->callback = callback ;
    task->parm = parm ;
    task->status = SERVICE_STATUS_QUEUED ;
    task->prio = prio ;
    task->ticks = ticks + os_sys_ticks () ;

    start = (SVC_TASKS_T*)linked_head (&_svc_tasks_list) ;

    previous = NULL_LLO ;
    for (  ;
            (start!=NULL_LLO) &&
            ((int32_t)(task->ticks - start->ticks) >= 0);

        ) {

        DBG_ASSERT_SVC_TASKS (start != task, 
                "svc_tasks_schedule task already in list!!") ;
        previous = start ;
        start = (SVC_TASKS_T*)linked_next (start, OFFSETOF(SVC_TASKS_T, next));

    }

    if (previous == NULL_LLO) {


        linked_add_head (&_svc_tasks_list, task, OFFSETOF(SVC_TASKS_T, next)) ;
        _svc_tasks_list_count++ ;

        os_mutex_unlock (&_svc_task_mutex) ;

        if (ticks) {
            os_timer_set (&_svc_tasks_virtual_timer, ticks) ;

        } else {
            svc_tasks_task_event(SVC_EVENTS_TASK, 0) ;

        }

    } else {

        linked_add_next (&_svc_tasks_list, previous, task, OFFSETOF(SVC_TASKS_T, next)) ;
        _svc_tasks_list_count++ ;

        os_mutex_unlock (&_svc_task_mutex) ;

        if (!os_timer_is_set (&_svc_tasks_virtual_timer)) {
            svc_tasks_task_event(SVC_EVENTS_TASK, 0) ;

        }

    }

    return EOK ;
}

static int32_t
task_ready_cancel (SVC_TASKS_T* task)
{
    int32_t res = E_BUSY ;

    if (svc_tasks_status(task) != SERVICE_STATUS_COMPLETE) {

        uint16_t prio = SVC_PRIO2QUEUE(task->prio) ;
        linked_t   * list = &_svc_tasks_ready_list[prio] ;

        if((linked_remove (list, task, OFFSETOF(SVC_TASKS_T, next)) != NULL_LLO)) {
            uint32_t parm = task->parm ;
            res = EOK ;
            DBG_ASSERT_SVC_TASKS (_svc_tasks_ready_count, ("task_ready_cancel _svc_tasks_ready_count invalid")) ;
            _svc_tasks_ready_count-- ;
#ifndef NDEBUG
            if (!_svc_tasks_ready_count) {
                DBG_ASSERT_SVC_TASKS (!linked_head(list), "task_ready_cancel 2_svc_tasks_ready_count invalid") ;
            }
#endif

            task->status = SERVICE_STATUS_COMPLETE ;
            os_mutex_unlock (&_svc_task_mutex) ;

            task->callback (task, parm, SERVICE_CALLBACK_REASON_CANCELED) ;

            os_mutex_lock (&_svc_task_mutex) ;

            if (task->flags & SVC_TASKS_FLAGS_WAITABLE) {
                p_event_t event = ((SVC_WAITABLE_TASKS_T*)task)->event ;
                os_event_signal (&event, 1) ;

            }

        } else {
            DBG_ASSERT_SVC_TASKS (0, "task_ready_cancel 0x%x not in list prio %d callback 0x%x!",
                    task, prio, task->callback) ;

        }

    } else {
           DBG_ASSERT_SVC_TASKS (0, "task_ready_cancel task 0x%x status %d callback 0x%x!",
                   task, svc_tasks_status(task), task->callback) ;

    }

    return res ;
}


int32_t
svc_tasks_cancel (SVC_TASKS_T* task)
{
    SVC_TASKS_T* start ;
    int32_t status ;

    DBG_ASSERT_SVC_TASKS (task, "svc_tasks_cancel null") ;

    os_mutex_lock (&_svc_task_mutex) ;

    start = (SVC_TASKS_T*)linked_head (&_svc_tasks_list) ;

    if (linked_remove (&_svc_tasks_list, task, OFFSETOF(SVC_TASKS_T, next)) != NULL_LLO) {
        uint32_t parm = task->parm ;

        DBG_ASSERT_SVC_TASKS (_svc_tasks_list_count, 
                    "svc_tasks_cancel _svc_tasks_list_count invalid") ;

#if DEBUG
        if (!_svc_tasks_list_count) {
            DBG_ASSERT_SVC_TASKS (!linked_head(&_svc_tasks_list),
                    "svc_tasks_cancel _svc_tasks_list_count invalid") ;
        }
#endif

        _svc_tasks_list_count-- ;

        if (start == task) {
            svc_tasks_task_event(SVC_EVENTS_TASK, 0) ;

        }

        task->status = SERVICE_STATUS_COMPLETE ;
        os_mutex_unlock (&_svc_task_mutex) ;
        task->callback (task, parm, SERVICE_CALLBACK_REASON_CANCELED) ;

        if (start->flags & SVC_TASKS_FLAGS_WAITABLE) {
            p_event_t event = ((SVC_WAITABLE_TASKS_T*)start)->event ;
            os_event_signal (&event, 1) ;
        }


        DBG_MESSAGE_SVC_TASKS (DBG_MESSAGE_SEVERITY_INFO, 
                "SVC   : : svc_tasks_cancel remove task count %d", 
                _svc_tasks_list_count);

        return EOK ;

    } 

    if (svc_tasks_status(task) == SERVICE_STATUS_COMPLETE) {
        int32_t res = task->status & SERVICE_STATUS_ACTIVE_BIT ? E_BUSY : EOK ;
        os_mutex_unlock (&_svc_task_mutex) ;
        return  res ;
        
    }

    status = task_ready_cancel (task) ;
    if (status == EOK) {
        svc_tasks_task_event(SVC_EVENTS_TASK, 0) ;

    }
    os_mutex_unlock (&_svc_task_mutex) ;

    return status ;
}

int32_t
svc_tasks_cancel_wait (SVC_TASKS_T* task, uint32_t timeout)
{
    int res = svc_tasks_cancel(task) ;
    if ((res == E_BUSY) && timeout) {
        res = svc_tasks_wait (task, timeout) ;
        svc_tasks_cancel(task) ;

    }

    return res  ;
}

uint32_t
svc_tasks_status (SVC_TASKS_T* task)
{
    return task->status & SERVICE_STATUS_MASK ;
}

uint32_t
svc_tasks_is_active (SVC_TASKS_T* task)
{
    return task->status & SERVICE_STATUS_ACTIVE_BIT ? 1 : 0 ;
}

uint32_t
svc_tasks_ready_count (void)
{
    return _svc_tasks_ready_count ;
}


uint32_t
svc_tasks_queued_count (void)
{
    return _svc_tasks_ready_count + _svc_tasks_list_count;
}

uint32_t
svc_tasks_scheduled_count (void)
{
    return _svc_tasks_list_count;
}

uint8_t
svc_tasks_get_flags (SVC_TASKS_T* task)
{
    return task->user ;
}

void
svc_tasks_set_flags (SVC_TASKS_T* task, uint8_t flags)
{
    task->user = flags ;
}

int32_t
svc_tasks_wait_queue (uint32_t queue, uint32_t timeout)
{
    if (queue < _svc_tasks_pool_count) {
        return  os_event_wait_timeout (&_svc_tasks_complete_event, 0, 1<<queue, 0, OS_MS2TICKS(timeout)) ;

    }

    return EOK ;
}

int32_t
svc_tasks_wait (SVC_TASKS_T* task, uint32_t timeout)
{
    if (task->flags & SVC_TASKS_FLAGS_WAITABLE) {
        p_event_t event = ((SVC_WAITABLE_TASKS_T*)task)->event ;
        return os_event_wait_timeout (&event, 0, 1, 0, SVC_TASK_MS2TICKS(timeout)) ?
                EOK : E_TIMEOUT ;

    }

    while (
            ((svc_tasks_status(task) != SERVICE_STATUS_COMPLETE) ||  svc_tasks_is_active(task))
                    && timeout) {
        os_thread_sleep (10) ;
        if (timeout >= 10) timeout -= 10;
        else timeout = 0 ;
    }


    return ((svc_tasks_status(task) != SERVICE_STATUS_COMPLETE) || svc_tasks_is_active(task)) ? E_TIMEOUT : EOK  ;

}


