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


#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS

#define OS_THREAD_PRIO(prio)        (prio)
#define OS_THREAD_GET_PRIO(prio)    (prio)

typedef unsigned int                os_mutex_t[5] ;
#define OS_MUTEX_DECL(hmtx)         unsigned int __mtx_##hmtx[5] = \
                                    {0} ; \
                                    p_mutex_t hmtx = (p_mutex_t) __mtx_##hmtx ;

typedef unsigned int                os_sem_t[3] ;
#define OS_SEMAPHORE_DECL(hsem)     unsigned int __sem_##hsem[3] = \
                                    {0} ; \
                                    p_sem_t hsem = (p_sem_t) __sem_##hsem ;

typedef unsigned int                os_event_t[3] ;
#define OS_EVENT_DECL(hevent)       unsigned int __event_##hevent[3] = \
                                    {0} ; \
                                    p_event_t hevent = (p_event_t) __event_##hevent ;

typedef unsigned int                os_timer_t[5] ;
#define OS_TIMER_DECL(htimer)       unsigned int __timer_##htimer[5] = \
                                    {0, 0, 0, 0, 0} ; \
                                    p_timer_t htimer = (p_timer_t) __timer_##htimer ;

#define OS_THREAD_WA_SIZE(n)        MEM_ALIGN_NEXT(sizeof(thread_t) + PORT_WA_SIZE(n), PORT_STACK_ALIGN)
#define OS_THREAD_WORKING_AREA(s, n) THD_WORKING_AREA(s, n)

#elif defined CFG_OS_FREERTOS && CFG_OS_FREERTOS

#include "FreeRTOS.h"

#define OS_THREAD_PRIO(prio)        (prio/8)
#define OS_THREAD_GET_PRIO(prio)    (prio*8)

typedef StaticQueue_t               os_mutex_t ;
#define OS_MUTEX_DECL(hmtx)         StaticQueue_t __mtx##hmtx = \
                                    {0} ; \
                                    p_mutex_t hmtx = (p_mutex_t) &__mtx##hmtx ;

typedef StaticQueue_t               os_sem_t ;
#define OS_SEMAPHORE_DECL(hsem)     StaticQueue_t __sem_##hsem = \
                                    {0} ; \
                                    p_sem_t hsem = (p_sem_t) &__sem_##hsem ;

typedef StaticEventGroup_t          os_event_t ;
#define OS_EVENT_DECL(hevent)       StaticEventGroup_t __event_##hevent = \
                                    {0} ; \
                                    p_event_t hevent = (p_event_t) &__event_##hevent ;

typedef StaticTimer_t               os_timer_t ;
#define OS_TIMER_DECL(htimer)       StaticTimer_t __timer_##htimer = \
                                    {0} ; \
                                    p_timer_t htimer = (p_timer_t) &__timer_##htimer ;

#define OS_THREAD_WA_SIZE(stack_size) (sizeof(StaticTask_t) + 6*sizeof(uint32_t) + stack_size)
#define OS_THREAD_WORKING_AREA(s, n)  uint64_t s[OS_THREAD_WA_SIZE(n) / sizeof (uint64_t)]



#elif defined CFG_OS_THREADX && CFG_OS_THREADX

#include "tx_api.h"

#define OS_THREAD_PRIO(prio)        (32 - ((prio)/4))
#define OS_THREAD_GET_PRIO(prio)    (4*(32 - prio))

typedef TX_MUTEX                    os_mutex_t ;
#define OS_MUTEX_DECL(hmtx)         TX_MUTEX __mtx_##hmtx = \
                                    {0} ; \
                                    p_mutex_t hmtx = (p_mutex_t) &__mtx_##hmtx ;

typedef TX_SEMAPHORE                os_sem_t ;
#define OS_SEMAPHORE_DECL(hsem)     TX_SEMAPHORE __sem_##hsem = \
                                    {0} ; \
                                    p_sem_t hsem = (p_sem_t) &__sem_##hsem ;

typedef TX_EVENT_FLAGS_GROUP        os_event_t ;
#define OS_EVENT_DECL(hevent)       TX_EVENT_FLAGS_GROUP __event_##hevent = \
                                    {0} ; \
                                    p_event_t hevent = (p_event_t) &__event_##hevent ;


typedef TX_TIMER                    os_timer_t ;
#define OS_TIMER_DECL(htimer)       TX_TIMER __timer_##htimer = \
                                    {0} ; \
                                    p_timer_t htimer = (p_timer_t) &__timer_##htimer ;

#define OS_THREAD_WA_SIZE(stack_size) (sizeof (TX_THREAD) + 6*sizeof(uint32_t) + stack_size)
#define OS_THREAD_WORKING_AREA(s, n)                                             \
  uint64_t s[OS_THREAD_WA_SIZE(n) / sizeof (uint64_t)]

#elif defined CFG_OS_POSIX && CFG_OS_POSIX

#include <pthread.h>
#include <semaphore.h>
#include <time.h>

typedef pthread_mutex_t             os_mutex_t ;
#define OS_MUTEX_DECL(hmtx)         pthread_mutex_t __mtx_##hmtx = PTHREAD_MUTEX_INITIALIZER ; \
                                                                        static p_mutex_t hmtx = (p_mutex_t) &__mtx_##hmtx ;
typedef sem_t                       os_sem_t ;
#define OS_SEMAPHORE_DECL(hsem)     sem_t __sem_##hsem = \
                                    {0} ; \
                                    p_sem_t hsem = (p_sem_t) &__sem_##hsem ;

// Event structure definition
typedef struct os_event_s {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    uint32_t flags;
} os_event_t ;

#define OS_EVENT_DECL(hevent)       os_event_t __event_##hevent = \
                                    {.mutex = PTHREAD_MUTEX_INITIALIZER, .cond = PTHREAD_COND_INITIALIZER} ; \
                                    p_event_t hevent = (p_event_t) &__event_##hevent ;


// Timer structure
typedef struct os_timer_s {
    uint64_t expire;                // Expiration time in milliseconds
    p_timer_function_t callback;    // Callback function
    void *callback_param;           // Callback parameter
    int is_set;                     // Is timer active
    int in_processing;              // Is timer being processed
    struct os_timer_s *next;        // Pointer to the next timer
} os_timer_t ;

#define OS_TIMER_DECL(htimer)       os_timer_t __timer_##htimer = \
                                    {0} ; \
                                    p_timer_t htimer = (p_event_t) &__timer_##htimer ;

#define OS_THREAD_WA_SIZE(stack_size) (128*sizeof(uint32_t))
#define OS_THREAD_WORKING_AREA(s, n) uint64_t s[OS_THREAD_WA_SIZE(n) / sizeof (uint64_t)]

#else

// For libraries using exported OS functions no RTOS will be defined so no static declarations allowed.
#undef CFG_OS_STATIC_DECLARATIONS
#define CFG_OS_STATIC_DECLARATIONS          0   

#endif


#if CFG_OS_STATIC_DECLARATIONS

typedef struct MLock_s {

  os_sem_t     s;
  p_thread_t *  owner ;
  int           cnt;
  uint32_t      prio ;

} MLock ;

typedef MLock                       os_mlock_t ;

#define OS_MLOCK_DECL(hmlock)       os_mlock_t __mlock##hmlock = {0} ; \
                                    p_mlock_t hmlock = (p_mlock_t) &__mlock##hmlock ;


#endif

#if CFG_OS_STATIC_DECLARATIONS

/**
* @brief   System tick frequency.
*/
#if defined CFG_OS_CHIBIOS && CFG_OS_CHIBIOS
#define OS_ST_FREQUENCY             1024
#elif defined CFG_OS_FREERTOS && CFG_OS_FREERTOS
#define OS_ST_FREQUENCY             configTICK_RATE_HZ
#elif defined CFG_OS_THREADX && CFG_OS_THREADX
#define OS_ST_FREQUENCY             100
#elif defined CFG_OS_OS_LESS && CFG_OS_OS_LESS
#define OS_ST_FREQUENCY             1000
#elif defined CFG_OS_POSIX && CFG_OS_POSIX
#define OS_ST_FREQUENCY             1000
#endif
/** @} */
#endif