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
#if CFG_OS_CHIBIOS || CFG_OS_FREERTOS || CFG_OS_THREADX
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"


#if CFG_OS_CHIBIOS
#include "ch.h"
#include "common/chevtflags.h"
#endif
#if CFG_OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"
#include "event_groups.h"
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
#include "tx_api.h"
#ifndef TX_DISABLE_ERROR_CHECKING
#define TX_ASSERT(x, v)   DBG_ASSERT_T(x == v, "TXASSERT")
#else 
#define TX_ASSERT(x, v)
#endif
#endif


/**
 * @name    Debug Level
 * @{
 */
#define DBG_MESSAGE_OS(severity, fmt_str, ...)    DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_OS                               DBG_ASSERT_T

#define OS_HEAP_SPACE               HEAP_SPACE

/*
 * Static global variables.
 */
#define MAX_TLS_ID      4
static  uint8_t         _os_tls_values[MAX_TLS_ID] = {0};
static  int             _os_started = 0 ;


#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS

typedef struct OS_THREAD_WA_S {
    StaticTask_t                tcb ;
    int                         heap ;
    StaticQueue_t               thread_sem ;
    p_sem_t               pthread_sem ;
    StaticQueue_t               join_sem ;
    int32_t                     suspend_msg ;
    int32_t                     errorno ;
    void *                      arg ;
    p_thread_function_t         pf ;
    StackType_t                 stack[0] ;
} OS_THREAD_WA_T ;

void task_start (void* arg)
{
    OS_THREAD_WA_T * wa = (OS_THREAD_WA_T*) arg ;

    wa->pf (wa->arg) ;

    xSemaphoreGive (&wa->join_sem) ;
    if (!wa->heap) {
        vTaskDelete (0);

    } else {
        vTaskSuspend (0) ;

    }

}

#endif

#if defined CFG_OS_THREADX && CFG_OS_THREADX

static volatile uint32_t _tx_control_enter = 0 ;


typedef struct OS_THREAD_WA_S {
    TX_THREAD                   tcb ;
    int                         heap ;
    TX_SEMAPHORE                thread_sem ;
    p_sem_t               pthread_sem ;
    TX_SEMAPHORE                join_sem ;
    TX_EVENT_FLAGS_GROUP        suspend_evt ;
    int32_t                     suspend_msg ;
    int32_t                     errorno ;
    void *                      arg ;
    p_thread_function_t         pf ;
    uint32_t                    tls[MAX_TLS_ID] ;
    uint32_t                    stack[0] ;
} OS_THREAD_WA_T ;

void 
task_start (unsigned long arg)
{
    OS_THREAD_WA_T * wa = (OS_THREAD_WA_T*) arg ;

    wa->pf (wa->arg) ;

    tx_semaphore_put (&wa->join_sem) ;

}

#endif

int32_t
os_thread_create (uint16_t stack_size, uint32_t prio, p_thread_function_t pf,
                   void *arg, p_thread_t* thread,
                   const char* name)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    void *wsp;
    thread_t *tp;
    size_t size = THD_WORKING_AREA_SIZE(stack_size)  ;
    //size_t size = (stack_size)  ;

    if (thread) *thread = 0 ;

    wsp = qoraal_malloc (QORAAL_HeapOperatingSystem, size) ;
    if (wsp == 0)
        return E_NOMEM;

    thread_descriptor_t td = {
      name,
      wsp,
      (stkalign_t *)((uint8_t *)wsp + size),
      OS_THREAD_PRIO(prio),
      pf,
      arg
    };

  #if CH_DBG_FILL_THREADS == TRUE
    _thread_memfill((uint8_t *)wsp,
                    (uint8_t *)wsp + size,
                    CH_DBG_STACK_FILL_VALUE);
  #endif

    chSysLock();
    tp = chThdCreateSuspendedI(&td);
    tp->flags = 0 ; //CH_FLAG_MODE_HEAP;

    tp->wa = wsp ;
    tp->heap = 1  ;

    chSchWakeupS (tp, MSG_OK);
    chSysUnlock ();
    if (thread) *thread =  tp;

    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    size_t size = stack_size  ;

    if (thread) *thread = 0 ;
    OS_THREAD_WA_T * const wa = qoraal_malloc (QORAAL_HeapOperatingSystem, size  + sizeof(OS_THREAD_WA_T));
    if (!wa) {
        return E_NOMEM ;
    }

    wa->pf  = pf ;
    wa->arg = arg ;
    wa->heap = 1 ;
    xSemaphoreCreateCountingStatic ((UBaseType_t)-1, 0, (StaticSemaphore_t *)(&wa->join_sem)) ;
    xSemaphoreCreateCountingStatic ((UBaseType_t)-1, 0, (StaticSemaphore_t *)(&wa->thread_sem)) ;
    wa->pthread_sem = &wa->thread_sem ;

    TaskHandle_t h = xTaskCreateStatic( task_start,
            (char const*)name,
            size / sizeof(StackType_t),
            wa,
            OS_THREAD_PRIO(prio),
            (StackType_t *)wa->stack,
            (void*)&wa->tcb ) ;

    if (!h) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa);
        return EFAIL ;
    }

    if (thread) *thread = h ;

    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    if (thread) *thread = 0 ;
    OS_THREAD_WA_T * const wa = qoraal_malloc (QORAAL_HeapOperatingSystem, stack_size  + sizeof(OS_THREAD_WA_T));

    if (!wa) {
        return E_NOMEM ;
    }

    memset (wa, 0, sizeof(OS_THREAD_WA_T)) ;

    wa->pf  = pf ;
    wa->arg = arg ;
    wa->heap = 1 ;
    tx_event_flags_create (&wa->suspend_evt, "thread") ;
    tx_semaphore_create (&wa->join_sem, "thread", 0) ;
    tx_semaphore_create (&wa->thread_sem, "thread", 0) ;
    wa->pthread_sem = (p_sem_t)&wa->thread_sem ;


    UINT status = tx_thread_create(&wa->tcb, (char*)name, task_start, (ULONG)wa,
                wa->stack, stack_size,
                OS_THREAD_PRIO(prio), OS_THREAD_PRIO(prio), 0, TX_AUTO_START);

    if (status != TX_SUCCESS) {   
        tx_event_flags_delete (&wa->suspend_evt) ;
        tx_semaphore_delete (&wa->join_sem) ;
        tx_semaphore_delete (&wa->thread_sem) ;
        qoraal_free (QORAAL_HeapOperatingSystem, wa);
        return EFAIL ;

    }

    if (thread) *thread = (void*) wa  ;

    return EOK  ;
#endif
}


int32_t
os_thread_create_static (void *wsp, uint16_t size, uint32_t prio, 
                         p_thread_function_t pf, void *arg, 
                         p_thread_t* thread, const char* name)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    thread_t *tp;

    if (thread) *thread = 0 ;

     if (wsp == 0)
        return E_NOMEM;

    thread_descriptor_t td = {
      name,
      wsp,
      (stkalign_t *)((uint8_t *)wsp + size),
      OS_THREAD_PRIO(prio),
      pf,
      arg
    };

  #if CH_DBG_FILL_THREADS == TRUE
    _thread_memfill((uint8_t *)wsp,
                    (uint8_t *)wsp + size,
                    CH_DBG_STACK_FILL_VALUE);
  #endif

    chSysLock();
    tp = chThdCreateSuspendedI(&td);
    tp->flags = 0 ; //CH_FLAG_MODE_HEAP;

    tp->wa = wsp ;
    tp->heap = 0  ;

    chSchWakeupS (tp, MSG_OK);
    chSysUnlock ();
    if (thread) *thread =  tp;

    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS

    DBG_ASSERT_OS (size > sizeof(OS_THREAD_WA_T), "os_thread_create_static stacksize") ;

    if (thread) *thread = 0 ;
    OS_THREAD_WA_T * const wa = (OS_THREAD_WA_T *) wsp;

    wa->pf  = pf ;
    wa->arg = arg ;
    wa->heap = 0 ;
    xSemaphoreCreateCountingStatic ((UBaseType_t)-1, 0, (StaticSemaphore_t *)(&wa->join_sem)) ;
    xSemaphoreCreateCountingStatic ((UBaseType_t)-1, 0, (StaticSemaphore_t *)(&wa->thread_sem)) ;
    wa->pthread_sem = &wa->thread_sem ;

    TaskHandle_t h = xTaskCreateStatic( task_start,
            (char const*)name,      /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
            size / sizeof(StackType_t) - sizeof(OS_THREAD_WA_T),
            wa,
            OS_THREAD_PRIO(prio),
            (StackType_t *)wa->stack,
            (void*)&wa->tcb ) ;


    if (!h) {
        return EFAIL ;
    }

    if (thread) *thread = h ;

    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX

    DBG_ASSERT_OS (size > sizeof(OS_THREAD_WA_T), "os_thread_create_static stacksize") ;

    if (thread) *thread = 0 ;
    OS_THREAD_WA_T * const wa = (OS_THREAD_WA_T *) wsp;

    wa->pf  = pf ;
    wa->arg = arg ;
    wa->heap = 0 ;
    tx_event_flags_create (&wa->suspend_evt, "thread") ;
    tx_semaphore_create (&wa->join_sem, "thread", 0) ;
    tx_semaphore_create (&wa->thread_sem, "thread", 0) ;
    wa->pthread_sem = (p_sem_t)&wa->thread_sem ;


    UINT status = tx_thread_create(&wa->tcb, (char*)name, task_start, (ULONG)wa,
                wa->stack, size - sizeof(OS_THREAD_WA_T),
                OS_THREAD_PRIO(prio), OS_THREAD_PRIO(prio), 0, TX_AUTO_START);

    if (status != TX_SUCCESS) {
        tx_event_flags_delete (&wa->suspend_evt) ;
        tx_semaphore_delete (&wa->join_sem) ;
        tx_semaphore_delete (&wa->thread_sem) ;
        return EFAIL ;
    }

    if (thread) *thread = (void*) wsp  ;

    return EOK  ;

#endif
}

const char*
os_thread_get_name (p_thread_t* thread)
{
    p_thread_t t = 0 ;
    if (thread == 0) {
        thread = &t ;
    }
    if (*thread == 0) {
        *thread = os_thread_current () ;
    }

#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
     return chRegGetThreadNameX((thread_t*) *thread)  ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
     extern const char * getTaskName(void) ;
     return getTaskName () ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    TX_THREAD * tp = (TX_THREAD*) *thread ;
    return tp->tx_thread_name ;
#endif
}

p_thread_t
os_thread_current (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return (p_thread_t) chThdGetSelfX ();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return (p_thread_t)   xTaskGetCurrentTaskHandle () ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_thread_identify () ;
#endif
}

void
os_thread_join (p_thread_t* thread)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    thread_t* tp = (thread_t*)*thread ;
    uint32_t* wa = tp->wa ;
    chThdWait ((thread_t*)*thread);
    if (tp->heap) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa) ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;
    xSemaphoreTake( (SemaphoreHandle_t)&wa->join_sem, portMAX_DELAY );
    os_thread_release (thread) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;
    tx_semaphore_get (&wa->join_sem, TX_WAIT_FOREVER) ;
    os_thread_release (thread) ;
#endif
}

int32_t
os_thread_join_timeout (p_thread_t* thread, uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    thread_t* tp = (thread_t*)*thread ;
    heapspace heap = tp->heap ;
    uint32_t* wa = tp->wa ;
#error NOT IMPPLEMENTED YET!
    chThdWait ((thread_t*)*thread);
    if (heap) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa) ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;
    if (xSemaphoreTake( (SemaphoreHandle_t)&wa->join_sem, ticks ) == pdTRUE) {
        os_thread_release (thread) ;
        return EOK ;
        
    }
    return E_TIMEOUT ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;
    UINT res = tx_semaphore_get (&wa->join_sem, ticks) ;
    if (res == TX_SUCCESS) {
        os_thread_release (thread) ;
        return EOK ;

    }
    return E_TIMEOUT ;
#endif
}

void
os_thread_release (p_thread_t* thread)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    thread_t* tp = (thread_t*)*thread ;
    heapspace heap = tp->heap ;
    uint32_t* wa = tp->wa ;
    chThdRelease ((thread_t*)*thread);
    if (heap != HEAP_SPACE_NONE) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa) ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;
    vTaskDelete ((TaskHandle_t)wa);
    if (wa->heap) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa) ;

    }
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )*thread ;

    tx_thread_terminate (&wa->tcb) ;

    tx_semaphore_delete (&wa->thread_sem) ;
    tx_event_flags_delete (&wa->suspend_evt) ;
    tx_semaphore_delete (&wa->join_sem) ;

    tx_thread_delete (&wa->tcb) ;

    if (wa->heap) {
        qoraal_free (QORAAL_HeapOperatingSystem, wa) ;

    }
#endif
}

uint32_t
os_thread_get_prio (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return OS_THREAD_GET_PRIO(chThdGetPriorityX ()) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return  OS_THREAD_GET_PRIO(uxTaskPriorityGet(xTaskGetCurrentTaskHandle())) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT priority ;
    tx_thread_info_get(tx_thread_identify (), 0, 0, 0,
                    &priority, 0, 0, 0, 0) ;
    return OS_THREAD_GET_PRIO(priority) ;
#endif
}

uint32_t
os_thread_set_prio (p_thread_t* thread, uint32_t prio)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    if (!thread || !(*thread) || (*thread == chThdGetSelfX ())) {
        return OS_THREAD_GET_PRIO(chThdSetPriority (OS_THREAD_PRIO(prio))) ;
    } else {
        //ASSERT(0, "DONOTUSE");
        //return chThdSetThdPriority (*thread, prio) ;
        //return os_thread_get_prio () ;
        return OS_THREAD_GET_PRIO(((thread_t *)(*thread))->hdr.pqueue.prio)  ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    TaskHandle_t h ;
    if (!thread || !(*thread)) {
        h = xTaskGetCurrentTaskHandle();
    } else {
        h = (TaskHandle_t)(*thread) ;
    }
    uint32_t old = OS_THREAD_GET_PRIO(uxTaskPriorityGet(h)) ;
     vTaskPrioritySet(h, OS_THREAD_PRIO(prio));
     return old ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT old_priority = 0 ;
    TX_THREAD * tp ;
    if (!thread || !(*thread)) {
        tp = tx_thread_identify () ;
    } else {
        tp = (TX_THREAD*)(*thread) ;
    }
    tx_thread_priority_change(tp, OS_THREAD_PRIO(prio), &old_priority) ;
    return OS_THREAD_GET_PRIO(old_priority) ;
#endif
}

int32_t
os_thread_tls_alloc (int32_t * index)
{
    int i ;

    //os_sys_lock () ;
    *index = -1 ;
    for (i=0; i<MAX_TLS_ID; i++) {
        if (_os_tls_values[i] == 0) break ;
    }
    if (i >= MAX_TLS_ID) {
        //os_sys_unlock () ;
        DBG_CHECK_T (0, EFAIL, "os_thread_tls_alloc out of tls!!") ;
    }
    _os_tls_values[i] = 1 ;
    //os_sys_unlock () ;
    *index = i ;
    os_thread_tls_set (i, 0) ;
    return EOK;
}

void
os_thread_tls_free (int32_t  index)
{
    if ((index >= 0) && (index < MAX_TLS_ID)) {
        //os_sys_lock () ;
        _os_tls_values[index] = 0 ;
        //os_sys_unlock () ;
    }
}

int32_t
os_thread_tls_set (int32_t idx, uint32_t value)
{
    if ((idx < 0 ) || (idx >= MAX_TLS_ID)) {
        return E_PARM ;
    }
#if CFG_OS_CHIBIOS
    thread_t * tp = chThdGetSelfX () ;
    uint32_t * tls = (uint32_t*)  tp->tls ;
    tls[idx] = value ;
    return EOK ;
#endif
#if CFG_OS_FREERTOS
#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
    vTaskSetThreadLocalStoragePointer( 0,
                                            idx,
                                            (void *)value ) ;
#endif
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)tx_thread_identify () ;
    wa->tls[idx] = value ;
    return EOK ;
#endif
}

uint32_t
os_thread_tls_get (int32_t idx)
{
    if ((idx < 0 ) || (idx >= MAX_TLS_ID)) {
        return 0 ;
    }
#if CFG_OS_CHIBIOS
    thread_t * tp = chThdGetSelfX () ;
    uint32_t * tls = (uint32_t*)  tp->tls ;
    return tls[idx] ;
#endif
#if CFG_OS_FREERTOS
#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
    return (uint32_t)pvTaskGetThreadLocalStoragePointer(
                                     0,
                                     idx );
#else
    return 0 ;
#endif
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)tx_thread_identify () ;
    return wa->tls[idx] ;
#endif
}


p_sem_t*
os_thread_thdsem_get (void)
{
#if CFG_OS_CHIBIOS
    thread_t * tp = chThdGetSelfX () ;

    return  (p_sem_t*)  &tp->pthdsem ;
#endif
#if CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa ;
    wa = (OS_THREAD_WA_T * )xTaskGetCurrentTaskHandle() ;
    return (p_sem_t*)&wa->pthread_sem ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)tx_thread_identify () ;
    return (p_sem_t*)&wa->pthread_sem ;
#endif
}

int32_t*
os_thread_errno (void)
{
#if CFG_OS_CHIBIOS
    thread_t * tp = chThdGetSelfX () ;
    if (tp) return  &tp->errorno ;
    return 0
#endif
#if CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa = (OS_THREAD_WA_T * )xTaskGetCurrentTaskHandle() ;
    return &wa->errorno ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)tx_thread_identify () ;
    return &wa->errorno ;
#endif
}

/**
 * @brief   Sleep for the specified time.
 * @details If the time is zero the thread simply yields.
 *
 * @param[in]           Sleep time in ms
 *
 * @return              void
 *
 * @api
 */
void
os_thread_sleep (uint32_t msec)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
   if (msec) {
        chThdSleepMilliseconds (msec);
    }
    else {
        chThdYield () ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
   if (msec) {
       vTaskDelay (OS_MS2TICKS(msec));
   }
   else {
        taskYIELD() ;
   }
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
   tx_thread_sleep (OS_MS2TICKS(msec)) ;
#endif
}

void
os_thread_sleep_ticks (uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
   if (ticks) {
        chThdSleep (ticks);
    }
    else {
        chThdYield () ;
    }
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
   if (ticks) {
       vTaskDelay (ticks);
   }
   else {
        taskYIELD() ;
   }
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
   tx_thread_sleep (ticks) ;
#endif

}

int32_t
os_thread_wait (uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chEvtWaitAny(SVC_EVENTS_ALL) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa = (OS_THREAD_WA_T * )xTaskGetCurrentTaskHandle() ;
    return ulTaskNotifyTakeIndexed(1, pdFALSE, ticks ) ?  wa->suspend_msg : E_TIMEOUT ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    int32_t res ;
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)tx_thread_identify () ;
    ULONG flags ;
    if (tx_event_flags_get(&wa->suspend_evt, 1, 
            TX_OR_CLEAR, &flags,  ticks) != TX_SUCCESS) {
        res = E_TIMEOUT ;
    } else {
        res = wa->suspend_msg ;
    }
    return res ;
#endif
}

int32_t
os_thread_notify (p_thread_t* thread, int32_t msg)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    thread_t * tp = chThdGetSelfX () ;
    chEvtSignal (tp, msg) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa  = (OS_THREAD_WA_T * )*thread ;
    wa->suspend_msg = msg ;
    xTaskNotifyGiveIndexed( (TaskHandle_t)(*thread), 1 ) ;
#endif    
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)*thread ;
    wa->suspend_msg = msg ;
    tx_event_flags_set(&wa->suspend_evt, 1, TX_OR ) ;
    return EOK ;
#endif
    return E_NOIMPL ;
}

int32_t
os_thread_notify_isr (p_thread_t* thread, int32_t msg)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysLockFromISR();
    thread_t * tp = chThdGetSelfX () ;
    chEvtSignalI (tp, msg) ;
    chSysUnlockFromISR();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    OS_THREAD_WA_T *  wa  = (OS_THREAD_WA_T * )*thread ;
    wa->suspend_msg = msg ;

    TaskHandle_t h  = (TaskHandle_t)(*thread) ;
    BaseType_t xHigherPriorityTaskWoken, xResult;

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Set bit 0 and bit 4 in xEventGroup. */
    xResult = xTaskNotifyIndexedFromISR ( h, 1, msg, eSetBits, &xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
#endif    
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)*thread ;
    wa->suspend_msg = msg ;
    tx_event_flags_set(&wa->suspend_evt, 1, TX_OR ) ;
    return EOK ;
#endif
    return E_NOIMPL ;
}

/**
 * @brief   System start.
 * @details Start the scheduler.
 *
 * @return              void
 *
 * @api
 */
void
os_sys_start (void)
{
    if (!_os_started) {
        _os_started = 1 ;
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
        vTaskStartScheduler ();
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
        tx_kernel_enter ();
#endif

    }
}

/**
 * @brief   os_sys_started
 * @details return 1 if the scheduler was started
 *
 * @return              void
 *
 * @api
 */
int
os_sys_started (void)
{
    return _os_started ;
}


/**
 * @brief   System lock.
 * @details Locks the system for any access to any of the Wi-Fi libraries.
 *
 * @return              void
 *
 * @api
 */
void
os_sys_lock (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysLock ();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    taskENTER_CRITICAL ();
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
        tx_interrupt_control (TX_INT_DISABLE) ;
        _tx_control_enter++;
#endif

}

/**
 * @brief   Unlocks the system locked by platform_sys_lock().
 *
 * @return              void
 *
 * @api
 */
void
os_sys_unlock (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysUnlock ();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    taskEXIT_CRITICAL ();
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    //DBG_ASSERT_OS (!_tx_control_enter, "ASSERT : os_sys_unlock enter unexpected!") ;
    _tx_control_enter--;
    if ( _tx_control_enter == 0 ) {
         tx_interrupt_control (TX_INT_ENABLE) ;
     }
#endif
}

uint32_t
os_sys_tick_freq (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return CH_CFG_ST_FREQUENCY ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return configTICK_RATE_HZ ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return OS_ST_FREQUENCY ;
#endif
}


uint32_t
os_sys_ticks (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return stGetCounter32 () ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xTaskGetTickCount () ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_time_get () ;
#endif
#if defined CFG_OS_OS_LESS && CFG_OS_OS_LESS
    return osalOsGetSystemTimeX() ;
#endif
}

uint32_t
os_sys_timestamp (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
     return OS_TICKS2MS(stGetCounter32())   ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
     return OS_TICKS2MS(xTaskGetTickCount())    ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return OS_TICKS2MS(tx_time_get ()) ;
#endif
}

uint32_t
os_sys_us_timestamp (void)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
     return OS_TICKS2MS(stGetCounter32())*1000  ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#define NVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define LOAD_REG    ( SystemCoreClock / 1000 )
    extern uint32_t SystemCoreClock ;
     return OS_TICKS2MS(xTaskGetTickCount())*1000 +
             (((LOAD_REG - NVIC_SYSTICK_CURRENT_VALUE_REG) * 1000) / LOAD_REG)  ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return OS_TICKS2MS(tx_time_get ()) * 1000 ;
#endif
}

void
os_sys_stop(void)
{

}

void
os_sys_halt (const char * msg)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysHalt (msg) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    portDISABLE_INTERRUPTS();

    qoraal_debug_print("ASSERT: ") ;
    qoraal_debug_print(msg) ;
    qoraal_debug_print("\r\n") ;

    /* Harmless infinite loop.*/
    while (1) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    asm volatile ("cpsid i" : : : "memory");
    qoraal_debug_print("ASSERT: ") ;
    qoraal_debug_print(msg) ;
    qoraal_debug_print("\r\n") ;

    /* Harmless infinite loop.*/
    while (1) {
    }
#endif
}

uint32_t
os_sys_is_irq (void)
{
    uint32_t isr;
    __asm volatile ( "mrs %0, ipsr" : "=r" ( isr )::"memory" );
    return isr ;
} 

#if !defined CFG_OS_MUTEX_DISABLE
int32_t
os_mutex_init (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chMtxObjectInit ((mutex_t*)(*mutex)) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreCreateRecursiveMutexStatic (
                               (StaticSemaphore_t *)(*mutex) ) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT r =  tx_mutex_create((TX_MUTEX*)(*mutex), "", 1)  ;
    (void)r;
    TX_ASSERT(r, TX_SUCCESS);
    return r == TX_SUCCESS ? EOK : EFAIL ;

#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void
os_mutex_deinit (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT r = tx_mutex_delete ((TX_MUTEX*)(*mutex));
    (void)r;
    TX_ASSERT(r, TX_SUCCESS);
#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t
os_mutex_create (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    *(mutex) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(mutex_t)) ) ;
    if (!*(mutex)) return E_NOMEM ;
    chMtxObjectInit ((mutex_t*)*mutex) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    *(mutex) = (p_mutex_t)xSemaphoreCreateRecursiveMutex ( );
    return *(mutex) ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    *(mutex) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(TX_MUTEX)) ) ;
    if (!*(mutex)) return E_NOMEM ;
    UINT r = tx_mutex_create((TX_MUTEX*)(*mutex), "", 1) ;
    (void)r;
    TX_ASSERT(r, TX_SUCCESS);
    return r == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void
os_mutex_delete (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    qoraal_free (QORAAL_HeapOperatingSystem, *(mutex)) ;
    *mutex = 0 ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    vSemaphoreDelete( *(mutex) ) ;
    *mutex = 0 ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    os_mutex_deinit (mutex) ;   
    qoraal_free (QORAAL_HeapOperatingSystem, *(mutex)) ;
    *mutex = 0 ;
#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t
os_mutex_lock (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chMtxLock((mutex_t*) *(mutex)) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xSemaphoreTakeRecursive( (SemaphoreHandle_t) *(mutex),
                portMAX_DELAY  ) == pdTRUE ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT r = tx_mutex_get ((TX_MUTEX*)(*mutex),TX_WAIT_FOREVER) ;
    TX_ASSERT(r, TX_SUCCESS);
    return r == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void
os_mutex_unlock (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chMtxUnlock((mutex_t*)*mutex)  ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreGiveRecursive( (SemaphoreHandle_t) *(mutex) );
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT r = tx_mutex_put ((TX_MUTEX*)(*mutex));
    TX_ASSERT(r, TX_SUCCESS);
    (void)r;
#endif
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t
os_mutex_trylock (p_mutex_t* mutex)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return (uint32_t)chMtxTryLock((mutex_t*)*(mutex)) ? EOK : EFAIL ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xSemaphoreTakeRecursive( (SemaphoreHandle_t) *(mutex),
                    0  ) == pdTRUE ? EOK : EFAIL;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT r = tx_mutex_get ((TX_MUTEX*)(*mutex), TX_NO_WAIT);
    TX_ASSERT(r, TX_SUCCESS);
    return r  == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

int32_t
os_sem_init (p_sem_t* sem, int32_t cnt)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSemObjectInit ((semaphore_t*)(*sem), cnt) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreCreateCountingStatic ((UBaseType_t)-1, cnt,
                                   (StaticSemaphore_t *)(*sem)) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", cnt) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_sem_deinit (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
#endif
}

int32_t
os_sem_create (p_sem_t* sem, int32_t cnt)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    *(sem) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(semaphore_t)) ) ;
    if (!*(sem)) return E_NOMEM ;
    chSemObjectInit ((semaphore_t*)*sem, cnt) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    (*sem) = (p_sem_t) xSemaphoreCreateCounting (
                                   (UBaseType_t)-1, cnt ) ;
    if (!*(sem)) return E_NOMEM ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    *(sem) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(TX_SEMAPHORE)) ) ;
    if (!*(sem)) return E_NOMEM ;
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", cnt) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

int32_t
os_sem_reset (p_sem_t* sem, int32_t cnt)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSemReset ((semaphore_t*)*sem, cnt) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xQueueReset ((SemaphoreHandle_t)*sem) ;
    while (cnt--) xSemaphoreGive ((SemaphoreHandle_t)*sem) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", cnt) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_sem_delete (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    qoraal_free (QORAAL_HeapOperatingSystem, *(sem)) ;
    *sem = 0 ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    vSemaphoreDelete ((SemaphoreHandle_t)*sem) ;
    *sem = 0 ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
    qoraal_free (QORAAL_HeapOperatingSystem, *(sem)) ;
    *sem = 0 ;
#endif
}

int32_t
os_sem_wait (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return chSemWait((semaphore_t*) *(sem)) == MSG_OK ? EOK : EFAIL ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xSemaphoreTake( (SemaphoreHandle_t) *(sem),
                portMAX_DELAY  ) == pdTRUE ? EOK : EFAIL ;

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_semaphore_get ((TX_SEMAPHORE*)*sem, TX_WAIT_FOREVER) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

int32_t
os_sem_wait_timeout (p_sem_t* sem, uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
        return chSemWaitTimeout((semaphore_t*) *(sem), ticks) == MSG_OK ? EOK : EFAIL ;

#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
        return xSemaphoreTake( (SemaphoreHandle_t) *(sem),
                ticks  ) == pdTRUE ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_semaphore_get ((TX_SEMAPHORE*)*sem, ticks) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_sem_signal (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSemSignal((semaphore_t*) *(sem))  ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreGive ( (SemaphoreHandle_t) *(sem)) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_put ((TX_SEMAPHORE*)*sem) ;
#endif
}

void
os_sem_signal_isr (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysLockFromISR();
    chSemSignalI((semaphore_t*) *(sem))  ;
    //chSchRescheduleS();
    chSysUnlockFromISR();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR ( (SemaphoreHandle_t) *(sem), &xHigherPriorityTaskWoken) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_put ((TX_SEMAPHORE*)*sem) ;
#endif
}

int32_t
os_bsem_init (p_sem_t* sem, int32_t taken)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chBSemObjectInit ((binary_semaphore_t*)(*sem), taken) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreCreateBinaryStatic ((StaticSemaphore_t *)(*sem)) ;
    if (!taken)  xSemaphoreGive((StaticSemaphore_t *)(*sem)) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", taken ? 0 : 1) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_bsem_deinit (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
#endif
}

int32_t
os_bsem_create (p_sem_t* sem, int32_t taken)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    *(sem) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(semaphore_t)) ) ;
    if (!*(sem)) return E_NOMEM ;
    chBSemObjectInit ((binary_semaphore_t*)*sem, taken) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    (*sem) = (p_sem_t) xSemaphoreCreateBinary ( ) ;
    if (!*(sem)) return E_NOMEM ;
    if (!taken)  xSemaphoreGive((StaticSemaphore_t *)(*sem)) ;

    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    *(sem) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(TX_SEMAPHORE)) ) ;
    if (!*(sem)) return E_NOMEM ;
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", taken ? 0 : 1) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

int32_t
os_bsem_reset (p_sem_t* sem, int32_t taken)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chBSemReset ((binary_semaphore_t*)*sem, taken) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xQueueReset ((SemaphoreHandle_t)*sem) ;
    //while (cnt--) xSemaphoreGive ((SemaphoreHandle_t)*sem) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
    return tx_semaphore_create((TX_SEMAPHORE*)*sem, "os", taken ? 0 : 1) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_bsem_delete (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    qoraal_free (QORAAL_HeapOperatingSystem, *(sem)) ;
    *sem = 0 ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    vSemaphoreDelete ((SemaphoreHandle_t)*sem) ;
    *sem = 0 ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_delete ((TX_SEMAPHORE*)*sem) ;
    qoraal_free (QORAAL_HeapOperatingSystem, *(sem)) ;
    *sem = 0 ;
#endif
}

int32_t
os_bsem_wait (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return chBSemWait((binary_semaphore_t*) *(sem)) == MSG_OK ? EOK : EFAIL ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xSemaphoreTake( (SemaphoreHandle_t) *(sem),
                portMAX_DELAY  ) == pdTRUE ? EOK : EFAIL ;

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_semaphore_get ((TX_SEMAPHORE*)*sem, TX_WAIT_FOREVER) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

int32_t
os_bsem_wait_timeout (p_sem_t* sem, uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
        return chBSemWaitTimeout((binary_semaphore_t*) *(sem), ticks) == MSG_OK ? EOK : EFAIL ;

#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
        return xSemaphoreTake( (SemaphoreHandle_t) *(sem),
                ticks  ) == pdTRUE ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
        return tx_semaphore_get ((TX_SEMAPHORE*)*sem, ticks) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}

void
os_bsem_signal (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chBSemSignal((binary_semaphore_t*) *(sem))  ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xSemaphoreGive ( (SemaphoreHandle_t) *(sem)) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_ceiling_put ((TX_SEMAPHORE*)*sem, 1) ;
#endif
}

void
os_bsem_signal_isr (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chSysLockFromISR();
    chBSemSignalI((binary_semaphore_t*) *(sem))  ;
    chSysUnlockFromISR();
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR ( (SemaphoreHandle_t) *(sem), &xHigherPriorityTaskWoken) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_semaphore_ceiling_put ((TX_SEMAPHORE*)*sem, 1) ;
#endif
}

int32_t
os_sem_count (p_sem_t* sem)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    int32_t cnt ;
    chSysLock ();
    cnt = chSemGetCounterI((semaphore_t*) *(sem))  ;
    chSysUnlock ();
    return cnt ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return (int32_t)uxSemaphoreGetCount ( (SemaphoreHandle_t) *(sem)) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    ULONG suspended_count = 0 ;
    tx_semaphore_info_get((TX_SEMAPHORE*)*sem, 0, &suspended_count,
               0, 0,
                0) ;
    return (int32_t) suspended_count ;
#endif
}

#if !defined CFG_OS_EVENT_DISABLE
int32_t
os_event_init (p_event_t* event)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chEvtFlagsInit ((EventFlags*)(*event)) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xEventGroupCreateStatic ((StaticEventGroup_t*) *event) ;
    return EOK ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    return tx_event_flags_create ((TX_EVENT_FLAGS_GROUP*)(*event), "os")
            == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_deinit (p_event_t* event)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chEvtFlagsDelete ( *(event) )  ;
    DBG_ASSERT_OS (chEvtFlagsListenerCnt ((EventFlags*)*(event)) == 0, "ASSERT : os_event_delete unexpected!") ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_event_flags_delete ( (TX_EVENT_FLAGS_GROUP*)(*event) )  ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
int32_t
os_event_create (p_event_t* event)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    *(event) = (p_event_t)qoraal_malloc (QORAAL_HeapOperatingSystem, sizeof(EventFlags));
    if (!*(event)) return E_NOMEM ;
    chEvtFlagsInit ((EventFlags*)*(event)) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    *(event) = (p_event_t)xEventGroupCreate( );
    return *(event) ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    *(event) = (p_event_t)qoraal_malloc (QORAAL_HeapOperatingSystem, sizeof(TX_EVENT_FLAGS_GROUP));
    if (!*(event)) return E_NOMEM ;
    return tx_event_flags_create ((TX_EVENT_FLAGS_GROUP*)(*event), "os")
            == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_delete (p_event_t* event)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#ifndef NDEBUG
//  DBG_ASSERT_OS (chEvtFlagsListenerCnt ((EventFlags*)*(event)) == 0, "ASSERT : test os_event_delete!") ;
#endif
    chEvtFlagsDelete ( *(event) )  ;
    DBG_ASSERT_OS (chEvtFlagsListenerCnt ((EventFlags*)*(event)) == 0, "ASSERT : os_event_delete unexpected!") ;
    qoraal_free (QORAAL_HeapOperatingSystem,  *(event) ) ;
    *event = 0 ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    vEventGroupDelete( *(event) )   ;
    *event = 0 ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_event_flags_delete ( (TX_EVENT_FLAGS_GROUP*)(*event) )  ;
    qoraal_free (QORAAL_HeapOperatingSystem,  *event ) ;
    *event = 0 ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_signal (p_event_t* event, uint32_t mask)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;
    chEvtFlagsSignal((EventFlags*)*(event), mask) ;
    // chEvtBroadcast ((EventSource*)*(event)) ;

#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xEventGroupSetBits( *(event), mask & ~0xff000000UL )    ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_event_flags_set ((TX_EVENT_FLAGS_GROUP*)(*event), mask, TX_OR) ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_signal_isr (p_event_t* event, uint32_t mask)
{
    if (*(event)) {
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS

        DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;

        chSysLockFromISR();
        chEvtFlagsSignalI((EventFlags*)*(event),mask);
        chSysUnlockFromISR();

#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
        BaseType_t xHigherPriorityTaskWoken, xResult;

          /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
          xHigherPriorityTaskWoken = pdFALSE;

          uint32_t base_prio = taskENTER_CRITICAL_FROM_ISR();

          xResult = xEventGroupSetBitsFromISR( *(event), mask & ~0xff000000UL , &xHigherPriorityTaskWoken )    ;

          taskEXIT_CRITICAL_FROM_ISR(base_prio);

          /* Was the message posted successfully? */
          if( xResult != pdFAIL )
          {
              /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
              switch should be requested.  The macro used is port specific and will
              be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
              the documentation page for the port being used. */
              portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
          }
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_event_flags_set ((TX_EVENT_FLAGS_GROUP*)(*event), mask, TX_OR) ;
#endif
    }
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_clear (p_event_t* event, uint32_t mask)
{
    DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chEvtFlagsClear ((EventFlags*)*(event), mask) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xEventGroupClearBits( *(event), mask & ~0xff000000UL)    ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    ULONG tmp ;
    tx_event_flags_get((TX_EVENT_FLAGS_GROUP*)(*event), mask,
            TX_OR_CLEAR, &tmp, TX_NO_WAIT);
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void
os_event_clear_isr (p_event_t* event, uint32_t mask)
{
    DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chEvtFlagsClearS ((EventFlags*)*(event), mask) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xEventGroupClearBits( *(event), mask & ~0xff000000UL)    ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    ULONG tmp ;
    tx_event_flags_get((TX_EVENT_FLAGS_GROUP*)(*event), mask,
            TX_OR_CLEAR, &tmp, TX_NO_WAIT);
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
uint32_t
os_event_wait (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, uint32_t all)
{
    DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    mask = chEvtFlagsWait((EventFlags*)*(event), clear_on_exit, mask) ;

    return mask ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xEventGroupWaitBits( *(event), mask & ~0xff000000UL ,  clear_on_exit, all, portMAX_DELAY) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    ULONG events = 0 ;
    tx_event_flags_get((TX_EVENT_FLAGS_GROUP*)(*event), mask,
            all ?   (clear_on_exit ? TX_AND_CLEAR : TX_AND) :
                    (clear_on_exit ? TX_OR_CLEAR : TX_OR), 
                    &events, TX_WAIT_FOREVER) ;
    return events ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
uint32_t
os_event_wait_timeout (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, 
                            uint32_t all, uint32_t ticks)
{
    DBG_ASSERT_OS (mask, "ASSERT : os_event_signal") ;
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS

    mask = chEvtFlagsWaitTimeout((EventFlags*)*(event), clear_on_exit, mask,  ticks) ;

    return mask ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return xEventGroupWaitBits( *(event), mask & ~0xff000000UL,  clear_on_exit, all, ticks) ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    ULONG events = 0 ;
    tx_event_flags_get((TX_EVENT_FLAGS_GROUP*)(*event), mask,
            all ?   (clear_on_exit ? TX_AND_CLEAR : TX_AND) :
                    (clear_on_exit ? TX_OR_CLEAR : TX_OR),
            &events, ticks) ;
    return events ;
#endif
}
#endif

#if !defined CFG_OS_OS_TIMER_DISABLE
int32_t
os_timer_init (p_timer_t* timer, p_timer_function_t fp, void * parm)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTObjectInit ((virtual_timer_t *)*timer) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerCreateStatic
                      ( /* Just a text name, not used by the RTOS
                        kernel. */
                        "Timer",
                        /* The timer period in ticks, must be
                        greater than 0. */
                        1,
                        /* The timers will auto-reload themselves
                        when they expire. */
                        pdFALSE,
                        /* The ID is used to store a count of the
                        number of times the timer has expired, which
                        is initialised to 0. */
                        ( void * ) parm,
                        /* Each timer calls the same callback when
                        it expires. */
                        (TimerCallbackFunction_t)fp,
                        /* Pass in the address of a StaticTimer_t
                        variable, which will hold the data associated with
                        the timer being created. */
                        *timer );
    return EOK ;

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_create((TX_TIMER *)*timer, "os", (VOID (*)(ULONG))fp,
            (ULONG) (parm),1, 0, TX_NO_ACTIVATE) ;
    return EOK ;
#endif
}

#if !defined CFG_OS_OS_TIMER_DISABLE
void
os_timer_deinit (p_timer_t* timer)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTReset ((virtual_timer_t *)*timer) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerStop (*timer, (TickType_t)-1 );
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_delete ((TX_TIMER *)*timer) ;
#endif
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
int32_t
os_timer_create (p_timer_t* timer, p_timer_function_t fp, void * parm)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    *(timer) = (p_timer_t)qoraal_malloc (QORAAL_HeapOperatingSystem, sizeof(virtual_timer_t));
    if (!*(timer)) return E_NOMEM ;
     chVTObjectInit ((virtual_timer_t *)*timer) ;
    return EOK ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    *(timer) = (p_timer_t)    xTimerCreate
                      ( /* Just a text name, not used by the RTOS
                        kernel. */
                        "Timer",
                        /* The timer period in ticks, must be
                        greater than 0. */
                        1,
                        /* The timers will auto-reload themselves
                        when they expire. */
                        pdFALSE,
                        /* The ID is used to store a count of the
                        number of times the timer has expired, which
                        is initialised to 0. */
                        ( void * ) parm,
                        /* Each timer calls the same callback when
                        it expires. */
                        (TimerCallbackFunction_t)fp);
    return *(timer) ? EOK : EFAIL ;
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    *(timer) = (p_timer_t)qoraal_malloc (QORAAL_HeapOperatingSystem, sizeof(TX_TIMER));
    if (!*(timer)) return E_NOMEM ;
    return tx_timer_create((TX_TIMER *)*timer, "os", (VOID (*)(ULONG))fp,
            (ULONG) (parm),1, 0, TX_NO_ACTIVATE) == TX_SUCCESS ? EOK : EFAIL ;
#endif
}
#endif

#if !defined CFG_OS_OS_TIMER_DISABLE
void
os_timer_delete (p_timer_t* timer)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTReset ((virtual_timer_t *)*timer) ;
    qoraal_free (QORAAL_HeapOperatingSystem,  *timer ) ;
    *timer = 0 ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerDelete( *timer, (TickType_t)-1  );
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_delete ((TX_TIMER *)*timer) ;
    qoraal_free (QORAAL_HeapOperatingSystem,  *timer ) ;
    *timer = 0 ;
#endif
}
#endif

void
os_timer_set (p_timer_t* timer, uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTSet ((virtual_timer_t *)*timer, ticks, /* tofo: fixme*/,  /* tofo: fixme*/) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerStop (*timer, (TickType_t)-1 );
    xTimerChangePeriod( *timer, ticks, (TickType_t)-1);

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_deactivate ((TX_TIMER *)*timer) ;
    tx_timer_change ((TX_TIMER *)*timer, ticks, 0) ;
    tx_timer_activate ((TX_TIMER *)*timer) ;
#endif
}
#endif

#if !defined CFG_OS_OS_TIMER_DISABLE
void
os_timer_set_i (p_timer_t* timer, uint32_t ticks)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTSetI ((virtual_timer_t *)*timer, ticks, /* tofo: fixme*/, /* tofo: fixme*/) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerStop (*timer, (TickType_t)-1 );
    xTimerChangePeriod( *timer, ticks, (TickType_t)-1);

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_deactivate ((TX_TIMER *)*timer) ;
    tx_timer_change ((TX_TIMER *)*timer, ticks, 0) ;
    tx_timer_activate ((TX_TIMER *)*timer) ;
#endif
}
#endif

#if !defined CFG_OS_OS_TIMER_DISABLE
int32_t
os_timer_is_set (p_timer_t* timer)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    return chVTIsArmed ((virtual_timer_t *)*timer) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    return 1 ;

#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    UINT active = 0 ;
    tx_timer_info_get((TX_TIMER *)*timer, 0, &active, 0, 0, 0) ;
   return active  ;
#endif
}
#endif

#if !defined CFG_OS_OS_TIMER_DISABLE
void
os_timer_reset (p_timer_t* timer)
{
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
    chVTReset ((virtual_timer_t *)*timer) ;
#endif
#if defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
    xTimerStop (*timer, (TickType_t)-1 );
#endif
#if defined CFG_OS_THREADX && CFG_OS_THREADX
    tx_timer_deactivate ((TX_TIMER *)*timer) ;
#endif
}
#endif

#endif /* CFG_OS_CHIBIOS || CFG_OS_FREERTOS || CFG_OS_THREADX */
