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




#ifndef __SVC_TASKS_H__
#define __SVC_TASKS_H__

#include <stdint.h>

#include "qoraal/errordef.h"
#include "qoraal/os.h"
#include "svc_wdt.h"


/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

#define SERVICE_CALLBACK_REASON_RUN                     0
#define SERVICE_CALLBACK_REASON_COMPLETE                0
#define SERVICE_CALLBACK_REASON_CANCELED                1
#define SERVICE_CALLBACK_REASON_FAILED                  2

#define SERVICE_STATUS_COMPLETE                         0
#define SERVICE_STATUS_QUEUED                           1
#define SERVICE_STATUS_MASK                             0x7
#define SERVICE_STATUS_ACTIVE_BIT                       (1<<3)


typedef enum  {
    SERVICE_PRIO_QUEUE0 = 0,    //
    SERVICE_PRIO_QUEUE1,        //
    SERVICE_PRIO_QUEUE2,        //
    SERVICE_PRIO_QUEUE3,        //
    SERVICE_PRIO_QUEUE4,        //
    SERVICE_PRIO_QUEUE5,        //
    SERVICE_PRIO_QUEUE_MAX
} SVC_TASK_PRIO_T ;

#define SVC_TASK_CFG_MAX            5
#define SVC_TASK_CFG_DEFAULT  \
                                    {OS_THREAD_PRIO_11, 1024*3, "svc-task0", TIMEOUT_10_SEC}    \
                                    ,{OS_THREAD_PRIO_10, 1024*5, "svc-task1", TIMEOUT_10_SEC}    \
                                    ,{OS_THREAD_PRIO_5, 1024*5, "svc-task2", TIMEOUT_10_SEC}    \
                                    ,{OS_THREAD_PRIO_4, 1024*5, "svc-task3", TIMEOUT_10_SEC}    \
                                    ,{OS_THREAD_PRIO_3, 1024*5, "svc-task4", TIMEOUT_30_SEC}    

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

#define SVC_TASK_S2TICKS(sec)                           ((uint32_t)((uint32_t)(sec) * (uint32_t)OS_ST_FREQUENCY))
#define SVC_TASK_MS2TICKS(msec)                         ((uint32_t)(((((uint32_t)(msec)) *  ((uint32_t)OS_ST_FREQUENCY)) + 999UL) / 1000UL))
#define SVC_TASK_TICKS2MS(tick)                         (((tick) * 1000UL + OS_ST_FREQUENCY - 1UL) / OS_ST_FREQUENCY)

#define DBG_MESSAGE_SVC_TASKS(severity, fmt_str, ...)    DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_SVC_TASKS                             DBG_ASSERT_T

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

typedef struct SVC_TASK_POOL_S {
    uint32_t                    prio ;
    uint32_t                    stack ;
    const char *                name ;
    SVC_WDT_TIMEOUTS_T          wdt ;
} SVC_TASK_POOL_T ;

typedef struct SVC_TASK_CFG_S {
    SVC_TASK_POOL_T             pool[SVC_TASK_CFG_MAX] ;      
} SVC_TASK_CFG_T ;

struct SVC_TASKS_S ;
typedef void (*SVC_TASKS_CALLBACK_T)(struct SVC_TASKS_S * /*timer*/, uintptr_t /*parm*/, uint32_t /*reason*/) ;

typedef struct SVC_TASKS_S {
    struct SERVICE_TASK_S *     next ;
    SVC_TASKS_CALLBACK_T        callback ;
    uint8_t                     status ;
    uint8_t                     prio ;
    uint8_t                     flags ;
    uint8_t                     user ;
    uintptr_t                   parm ;
    uint32_t                    ticks ;
} SVC_TASKS_T ;

#define _SVC_TASKS_DATA()    {/*(struct SERVICE_TASK_S *)*/0,0,SERVICE_STATUS_COMPLETE,0,0,0,0,0}
#define SVC_TASKS_DECL(name)   SVC_TASKS_T name =  _SVC_TASKS_DATA()
#define SVC_TASKS_DECL_BSS3(name)   SVC_TASKS_T name /*__attribute__ ((section (".bss3")))*/

typedef struct SVC_WAITABLE_TASKS_S {
    SVC_TASKS_T     task ;
    p_event_t       event ;
} SVC_WAITABLE_TASKS_T ;


#define SVC_TASKS_FLAGS_WAITABLE                1

#define _SVC_WAITABLE_TASKS_DATA()    {{0,0,SERVICE_STATUS_COMPLETE,0,0,0,0,0}, {0}}
#define SVC_WAITABLE_TASKS_DECL(name)   SVC_WAITABLE_TASKS_T name =  _SVC_WAITABLE_TASKS_DATA()


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t         svc_tasks_init (const SVC_TASK_CFG_T * pool) ;
    int32_t         svc_tasks_start (void) ;
    int32_t         svc_tasks_stop (uint32_t timeout) ;

    void            svc_tasks_init_task (SVC_TASKS_T* task) ;
    int32_t         svc_tasks_init_waitable_task (SVC_WAITABLE_TASKS_T* task) ;
    void            svc_tasks_deinit_waitable_task (SVC_WAITABLE_TASKS_T* task) ;
    int32_t         svc_tasks_schedule (SVC_TASKS_T* timer, SVC_TASKS_CALLBACK_T complete, uintptr_t parm, uint16_t prio, uint32_t ticks) ;
    int32_t         svc_tasks_cancel (SVC_TASKS_T* task) ;
    int32_t         svc_tasks_cancel_wait (SVC_TASKS_T* task, uint32_t timeout) ;
    uint32_t        svc_tasks_status (SVC_TASKS_T* task) ;
    uint32_t        svc_tasks_is_active (SVC_TASKS_T* task) ;
    void            svc_tasks_complete (SVC_TASKS_T* task) ;
    int32_t         svc_tasks_wait (SVC_TASKS_T* task, uint32_t timeout) ;

    uint8_t         svc_tasks_get_flags (SVC_TASKS_T* task) ;
    void            svc_tasks_set_flags (SVC_TASKS_T* task, uint8_t flags) ;

    uint32_t        svc_tasks_ready_count (void) ;
    uint32_t        svc_tasks_queued_count (void) ;
    uint32_t        svc_tasks_scheduled_count (void) ;
    uint32_t        svc_task_get_active_ticks (uint16_t prio, SVC_TASKS_CALLBACK_T* callback, uintptr_t* parm) ;
    uint32_t        svc_task_expire (SVC_TASKS_T * task) ;
    uint32_t        svc_task_next_expire (void) ;

    int32_t         svc_tasks_wait_queue (uint32_t queue, uint32_t timeout) ;

#ifdef __cplusplus
}
#endif


#endif /* __SVC_TASKS_H__ */
/** @} */
