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
 * @file    os.h
 * @brief   OS abstraction layer.
 * @details Provides abstraction macros, constants, and function declarations
 *          for RTOS operations.
 *
 * @defgroup RTOS OS Abstraction
 * @{
 */

#ifndef __OS_H__
#define __OS_H__

#include <stdint.h>

/*===========================================================================*/
/* RTOS constants.                                                           */
/*===========================================================================*/

/**
* @brief   Thread priorities.
*/
#define OS_THREAD_PRIO_LOWEST                   (8*1)
#define OS_THREAD_PRIO_1                        (8*2)
#define OS_THREAD_PRIO_2                        (8*3)
#define OS_THREAD_PRIO_3                        (8*4)
#define OS_THREAD_PRIO_4                        (8*5)
#define OS_THREAD_PRIO_5                        (8*6)
#define OS_THREAD_PRIO_6                        (8*7)
#define OS_THREAD_PRIO_7                        (8*8)
#define OS_THREAD_PRIO_8                        (8*9)
#define OS_THREAD_PRIO_9                        (8*10)
#define OS_THREAD_PRIO_10                       (8*11)
#define OS_THREAD_PRIO_11                       (8*12)
#define OS_THREAD_PRIO_12                       (8*13)
#define OS_THREAD_PRIO_HIGHEST                  (8*14)

/**
* @name    Special thread constants
 * @{ */

/**
 * @brief   Zero time specification for functions with a timeout.
*/
#define OS_TIME_IMMEDIATE                           0

/**
 * @brief   Infinite time specification for functions with a timeout.
*/
#define OS_TIME_INFINITE                            0xffffffffUL

/** @} */

/**
 * @name    Systick Conversion Macros
 * @{ */
#define OS_S2TICKS(sec)             ((uint32_t)((uint32_t)(sec) * (uint32_t)OS_ST_FREQUENCY))
#define OS_MS2TICKS(msec)           ((uint32_t)(((((uint32_t)(msec)) *  ((uint32_t)OS_ST_FREQUENCY)) + 999UL) / 1000UL))
#define OS_TICKS2MS(ticks)          (((ticks) * 1000UL + OS_ST_FREQUENCY - 1UL) / OS_ST_FREQUENCY)
#define OS_TICKS2S(ticks)           (((ticks) + OS_ST_FREQUENCY - 1UL) / OS_ST_FREQUENCY)
/** @} */

/**
 * @name    Systick Compare Macros
 * @{ */
#define OS_TIME_AFTER32(a, b)       ((int32_t)((uint32_t)(b) - (uint32_t)(a)) < 0)
#define OS_TIME_BEFORE32(b, a)      OS_TIME_AFTER32(a, b)
/** @} */

/*===========================================================================*/
/* OS data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief Typedef for a Thread.
 */
typedef void * p_thread_t;

/**
 * @brief Typedef for a Semaphore.
 */
typedef void * p_sem_t;

/**
 * @brief Typedef for an Event.
 */
typedef void * p_event_t;

/**
 * @brief Typedef for a Mutex.
 */
typedef void * p_mutex_t;

/**
 * @brief Typedef for a MLock.
 */
typedef void * p_mlock_t;

/**
 * @brief Typedef for a Thread Function Pointer.
 */
typedef void (*p_thread_function_t)( void * );

/**
 * @brief Typedef for a Timer.
 */
typedef void * p_timer_t;

/**
 * @brief Typedef for a Timer Function Pointer.
 */
typedef void (*p_timer_function_t)( void * );

/*===========================================================================*/
/* Platform static data declarations.                                         */
/*===========================================================================*/
#if defined CFG_OS_CHIBIOS || defined CFG_OS_FREERTOS || \
    defined CFG_OS_THREADX || defined CFG_OS_POSIX
    #if (defined CFG_OS_CHIBIOS + defined CFG_OS_FREERTOS + \
        defined CFG_OS_THREADX + defined CFG_OS_POSIX) != 1
        #error "Exactly one of CFG_OS_CHIBIOS, CFG_OS_FREERTOS, CFG_OS_THREADX, or CFG_OS_POSIX must be defined."
    #endif
    #define CFG_OS_STATIC_DECLARATIONS 1
#endif

#if defined CFG_OS_STATIC_DECLARATIONS
#include "os_static.h"
#else
/**
 * @brief Retrieves the system tick frequency.
 */
#define OS_ST_FREQUENCY             os_sys_tick_freq()
#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

    extern void         os_sys_start (void);
    extern int          os_sys_started (void) ;
    extern void         os_sys_lock (void);
    extern void         os_sys_unlock (void);
    extern uint32_t     os_sys_is_irq (void) ;
    extern uint32_t     os_sys_ticks (void) ;
    extern uint32_t     os_sys_tick_freq (void) ;
    extern uint32_t     os_sys_timestamp (void) ;
    extern uint32_t     os_sys_us_timestamp (void) ;
    extern void         os_sys_stop (void) ;
    extern void         os_sys_halt (const char * msg) ;

    extern int32_t      os_thread_create  (uint16_t stack_size, uint32_t prio, p_thread_function_t pf, void *arg, p_thread_t* thread, const char* name) ;
    extern int32_t      os_thread_create_static (void *wsp, uint16_t size, uint32_t prio, p_thread_function_t pf, void *arg, p_thread_t* thread, const char* name) ;
    extern const char*  os_thread_get_name (p_thread_t* thread);
    extern p_thread_t   os_thread_current (void) ;
    extern void         os_thread_sleep (uint32_t msec);
    extern void         os_thread_sleep_ticks (uint32_t ticks);
    extern void         os_thread_join (p_thread_t* thread) ;
    extern int32_t      os_thread_join_timeout (p_thread_t* thread, uint32_t ticks) ;
    extern void         os_thread_release (p_thread_t* thread) ;
    extern uint32_t     os_thread_get_prio (void);
    extern uint32_t     os_thread_set_prio (p_thread_t* thread, uint32_t prio);
    extern int32_t      os_thread_tls_alloc (int32_t * index) ;
    extern void         os_thread_tls_free (int32_t  index) ;
    extern int32_t      os_thread_tls_set (int32_t idx, uint32_t value) ;
    extern uint32_t     os_thread_tls_get (int32_t idx) ;
    extern p_sem_t*     os_thread_thdsem_get (void) ;
    extern int32_t*     os_thread_errno (void) ;
    extern int32_t      os_thread_wait (uint32_t ticks) ;
    extern int32_t      os_thread_notify (p_thread_t* thread, int32_t msg) ;
    extern int32_t      os_thread_notify_isr (p_thread_t* thread, int32_t msg) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_mutex_init (p_mutex_t* mutex) ;
    extern void      	os_mutex_deinit (p_mutex_t* mutex) ;
#endif
    extern int32_t      os_mutex_create (p_mutex_t* mutex) ;
    extern void         os_mutex_delete (p_mutex_t* mutex) ;
    extern int32_t      os_mutex_lock (p_mutex_t* mutex) ;
    extern void         os_mutex_unlock (p_mutex_t* mutex) ;
    extern int32_t      os_mutex_trylock (p_mutex_t* mutex) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_sem_init (p_sem_t* sem, int32_t cnt) ;
    extern void      	os_sem_deinit (p_sem_t* sem) ;
#endif
    extern int32_t      os_sem_create (p_sem_t* sem, int32_t cnt) ;
    extern void         os_sem_delete (p_sem_t* sem) ;
    extern int32_t      os_sem_reset (p_sem_t* sem, int32_t cnt) ;
    extern int32_t      os_sem_wait (p_sem_t* sem) ;
    extern int32_t      os_sem_wait_timeout (p_sem_t* sem, uint32_t ticks) ;
    extern void         os_sem_signal (p_sem_t* sem) ;
    extern void         os_sem_signal_isr (p_sem_t* sem) ;
    extern int32_t      os_sem_count (p_sem_t* sem) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_bsem_init (p_sem_t* sem, int32_t taken) ;
    extern void      	os_bsem_deinit (p_sem_t* sem) ;
#endif
    extern int32_t      os_bsem_create (p_sem_t* sem, int32_t taken) ;
    extern void         os_bsem_delete (p_sem_t* sem) ;
    extern int32_t      os_bsem_reset (p_sem_t* sem, int32_t taken) ;
    extern int32_t      os_bsem_wait (p_sem_t* sem) ;
    extern int32_t      os_bsem_wait_timeout (p_sem_t* sem, uint32_t ticks) ;
    extern void         os_bsem_signal (p_sem_t* sem) ;
    extern void         os_bsem_signal_isr (p_sem_t* sem) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_event_init (p_event_t* event) ;
    extern void      	os_event_deinit (p_event_t* event) ;
#endif
    extern int32_t      os_event_create (p_event_t* event) ;
    extern void         os_event_delete (p_event_t* event) ;
    extern uint32_t     os_event_wait (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, uint32_t all) ;
    extern uint32_t     os_event_wait_timeout (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, uint32_t all, uint32_t ticks) ;
    extern void         os_event_clear (p_event_t* event, uint32_t mask) ;
    extern void         os_event_signal (p_event_t* event, uint32_t mask) ;
    extern void         os_event_signal_isr (p_event_t* event, uint32_t mask) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_timer_init (p_timer_t* timer, p_timer_function_t fp, void * parm) ;
    extern void         os_timer_deinit (p_timer_t* timer) ;
#endif    
    extern int32_t      os_timer_create (p_timer_t* timer, p_timer_function_t fp, void * parm) ;
    extern void         os_timer_delete (p_timer_t *timer) ;
    extern void         os_timer_set (p_timer_t* timer, uint32_t ticks) ;
    extern void         os_timer_set_i (p_timer_t* timer, uint32_t ticks) ;
    extern int32_t      os_timer_is_set (p_timer_t* timer) ;
    extern void         os_timer_reset (p_timer_t* timer) ;

#if CFG_OS_STATIC_DECLARATIONS
    extern int32_t      os_mlock_init (p_mlock_t* mlock, uint32_t prio) ;
    extern void      	os_mlock_deinit (p_mlock_t* mlock) ;
#endif
    extern int32_t      os_mlock_create (p_mlock_t* mlock, uint32_t prio) ;
    extern void         os_mlock_delete (p_mlock_t* mlock) ;
    extern void         os_mlock_lock (p_mlock_t* mlock) ;
    extern void         os_mlock_unlock (p_mlock_t* mlock) ;
    extern uint32_t     os_mlock_trylock (p_mlock_t* mlock) ;

#ifdef __cplusplus
}
#endif

#endif /* __OS_H__ */

/** @} */