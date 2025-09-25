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
#if CFG_OS_POSIX
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "qoraal/qoraal.h"


/* You can try to use the Linux-specific pthread_timedjoin_np if you want a timed join. 
   Otherwise define it away or set E_NOIMPL. */
#ifdef __linux__
#include <sys/syscall.h>
#define HAVE_PTHREAD_TIMEDJOIN 1
#endif





/**
 * @name    Debug Level
 * @{
 */
#define DBG_MESSAGE_OS(severity, fmt_str, ...)    DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_OS                             DBG_ASSERT_T

#define OS_HEAP_SPACE               HEAP_SPACE

/*
 * Static global variables.
 */
#define MAX_TLS_ID      4
static  uint8_t         _os_tls_values[MAX_TLS_ID] = {0};
static  int             _os_started = 0 ;

#if !defined CFG_OS_OS_TIMER_DISABLE
static void start_timer_manager(void) ;
static void stop_timer_manager(void) ;
#endif

/*--------------------------------------------------*
 *  POSIX TLS: we track a pointer to OS_THREAD_WA_T *
 *--------------------------------------------------*/
static pthread_key_t g_posix_wa_key;
static int           g_posix_wa_key_init = 0;


typedef struct OS_THREAD_WA_S {
        pthread_t                       tid;
        int                             heap ;
        void *                          arg ;
        int32_t                         errorno ;

        sem_t                           join_sem;
        pthread_mutex_t                 suspend_mutex ;
        pthread_cond_t                  suspend_cond ;
        int32_t                         suspend_msg ;

        p_thread_function_t             pf ;
        uint32_t                        tls[MAX_TLS_ID] ;

} OS_THREAD_WA_T ;

/* 
 * The actual function that runs in the new thread. 
 */
static void * 
task_start (void * arg)
{
        OS_THREAD_WA_T * wa = (OS_THREAD_WA_T*) arg ;

    /* Set up the TLS so we can do os_thread_tls_{set,get} properly. */
    if (g_posix_wa_key_init) {
        pthread_setspecific(g_posix_wa_key, wa);
    }

    /* Initialize these in creation or here? Let's do them here. */
    pthread_mutex_init(&wa->suspend_mutex, NULL);
    pthread_cond_init(&wa->suspend_cond, NULL);
        wa->suspend_msg = 0 ;

    /* Actually run user function. */
    wa->pf (wa->arg) ;
    /* Signal the join semaphore */
    sem_post(&wa->join_sem);

    return 0 ;
}

int 
map_custom_to_posix_priority (int custom_priority) 
{
    // Define custom priority range
    const int lowest_custom_priority = OS_THREAD_PRIO_LOWEST;
    const int highest_custom_priority = OS_THREAD_PRIO_HIGHEST;

    // Get POSIX priority range
    int min_prio = sched_get_priority_min(SCHED_FIFO);
    int max_prio = sched_get_priority_max(SCHED_FIFO);

    // Proportional mapping
    return min_prio + (custom_priority - lowest_custom_priority) * (max_prio - min_prio) / 
                      (highest_custom_priority - lowest_custom_priority);
}

int 
map_posix_to_custom_priority (int posix_priority) 
{
    // Define custom priority range
    const int lowest_custom_priority = OS_THREAD_PRIO_LOWEST;
    const int highest_custom_priority = OS_THREAD_PRIO_HIGHEST;

    // Get POSIX priority range
    int min_prio = sched_get_priority_min(SCHED_FIFO);
    int max_prio = sched_get_priority_max(SCHED_FIFO);

    // Proportional mapping from POSIX priority to custom priority
    return lowest_custom_priority +
           (posix_priority - min_prio) * (highest_custom_priority - lowest_custom_priority) /
           (max_prio - min_prio);
}

int32_t
os_thread_create (uint16_t stack_size, uint32_t prio, p_thread_function_t pf,
                   void *arg, p_thread_t* thread,  const char* name)
{
	pthread_attr_t tattr;
	int ret;
	struct sched_param param;
	(void)ret  ;
    (void) param;

    if (thread) 
        *thread = 0;

    /* If not done yet, create a TLS key to store wa pointer. */
    if (!g_posix_wa_key_init) {
        pthread_key_create(&g_posix_wa_key, NULL);
        g_posix_wa_key_init = 1;
    }

    /* We'll allocate the wrapper struct. */
        OS_THREAD_WA_T * const wa = qoraal_malloc (QORAAL_HeapOperatingSystem, sizeof(OS_THREAD_WA_T));
        if (!wa) {
            return E_NOMEM ;
        }
        memset (wa, 0, sizeof(OS_THREAD_WA_T)) ;

        wa->pf  = pf ;
        wa->arg = arg ;
        wa->heap = 1 ;

        if (sem_init(&wa->join_sem, 0, 0) != 0) {
            qoraal_free (QORAAL_HeapOperatingSystem, wa);
            return EFAIL;
        }

    /* Initialize attributes if you want to set priority. 
       Realistically, setting sched_priority requires root privileges on many systems,
       so don’t be shocked if this fails or is ignored. */
    pthread_attr_init(&tattr);
#if 1 /* If you actually want to attempt setting the priority: */
    pthread_attr_getschedparam(&tattr, &param);
    param.sched_priority = map_custom_to_posix_priority (prio) ;
    pthread_attr_setschedparam(&tattr, &param);
#endif

    ret = pthread_create(&wa->tid, &tattr, task_start, wa);
    pthread_attr_destroy(&tattr);

    if (ret != 0) {
        sem_destroy(&wa->join_sem);
        qoraal_free(QORAAL_HeapOperatingSystem, wa);
        return EFAIL;
    }

    if (thread) {
        *thread = (p_thread_t) wa; 
        /* We store wa, not wa->tid, so we can fetch it in e.g. os_thread_tls_{set,get}. */
    }
    return EOK  ;

}



int32_t
os_thread_create_static (void *wsp, uint16_t size, uint32_t prio, 
                         p_thread_function_t pf, void *arg, 
                         p_thread_t* thread, const char* name)
{
    /* 
     * There's no standard "static" thread creation in POSIX. 
     * We'll treat this similarly, but we’ll store data in 'wsp' 
     * and cast it to OS_THREAD_WA_T. 
     */
    pthread_attr_t tattr;
    int ret;
    struct sched_param param;

    if (thread) 
        *thread = 0;

    if (!g_posix_wa_key_init) {
        pthread_key_create(&g_posix_wa_key, NULL);
        g_posix_wa_key_init = 1;
    }

    if (!wsp) {
        return E_NOMEM;
    }
    if (size < sizeof(OS_THREAD_WA_T)) {
        return EFAIL; /* Not enough space, sorry honey. */
    }

    OS_THREAD_WA_T *wa = (OS_THREAD_WA_T *) wsp;
    memset(wa, 0, sizeof(OS_THREAD_WA_T));

    wa->pf  = pf ;
    wa->arg = arg ;
    wa->heap = 0 ;

    if (sem_init(&wa->join_sem, 0, 0) != 0) {
        return EFAIL;
    }

    pthread_attr_init(&tattr);
#if 1
    pthread_attr_getschedparam(&tattr, &param);
    param.sched_priority = map_custom_to_posix_priority (prio) ;
    pthread_attr_setschedparam(&tattr, &param);
#endif

    ret = pthread_create(&wa->tid, &tattr, task_start, wa);
    pthread_attr_destroy(&tattr);

    if (ret != 0) {
        sem_destroy(&wa->join_sem);
        return EFAIL;
    }

    if (thread) {
        *thread = (p_thread_t) wa;
    }
    return EOK  ;
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
    /* We can’t natively retrieve the name with standard POSIX 
       unless we use pthread_getname_np (also a GNU extension). 
       Let's just return a stub. */
    return "posix thread" ;
}

p_thread_t
os_thread_current (void)
{
    /* We'll return the pointer to our OS_THREAD_WA_T if we can. */
    if (!g_posix_wa_key_init) {
        /* key not inited, no real thread object known, fallback? */
        return (p_thread_t) pthread_self();
    }
    /* If we do have the key, get the WA pointer.  */
    OS_THREAD_WA_T* wa = pthread_getspecific(g_posix_wa_key);
    if (wa) {
        return (p_thread_t) wa;
    }
    /* If no wa, then we haven't done a fancy create, fallback to tid. */
    return (p_thread_t) pthread_self();
}

void 
os_thread_join (p_thread_t *thread) 
{
    OS_THREAD_WA_T *wa = (OS_THREAD_WA_T *)*thread;
    if (!wa) {
        return;
    }

    // Wait on the join semaphore
    sem_wait(&wa->join_sem);

    os_thread_release (thread); 
}

int32_t
os_thread_join_timeout (p_thread_t* thread, uint32_t ticks)
{
    OS_THREAD_WA_T *wa = (OS_THREAD_WA_T *)*thread;
    if (!wa) {
        return EFAIL;
    }

    // Wait on the join semaphore
    sem_wait(&wa->join_sem);

    os_thread_release (thread); 
    return EOK ;
}

void
os_thread_release (p_thread_t* thread)
{
    OS_THREAD_WA_T* wa = (OS_THREAD_WA_T*)*thread;
    if (!wa) return;

    // Clean up
    pthread_join(wa->tid, NULL);
    sem_destroy(&wa->join_sem);

    // Clean up suspend-related resources
    pthread_mutex_destroy(&wa->suspend_mutex);
    pthread_cond_destroy(&wa->suspend_cond);

    // Free memory if we allocated it
    if (wa->heap) {
        qoraal_free(QORAAL_HeapOperatingSystem, wa);
    }
    
    *thread = NULL;
}


uint32_t
os_thread_get_prio (void)
{
    p_thread_t* thread = os_thread_current () ;
    OS_THREAD_WA_T* wa = (OS_THREAD_WA_T*)*thread;
    if (!wa) {
        return 0; // Invalid thread wrapper
    }

    int policy;
    struct sched_param param;
    if (pthread_getschedparam(wa->tid, &policy, &param) == 0) {
        return map_posix_to_custom_priority(param.sched_priority); // Map and return custom priority
    }

    return 0; // If retrieval fails, return default priority
}

uint32_t
os_thread_set_prio (p_thread_t* thread, uint32_t prio)
{
    if (!thread || !*thread) {
        return 0; // Invalid thread, returning default priority
    }

    OS_THREAD_WA_T* wa = (OS_THREAD_WA_T*)*thread;
    if (!wa) {
        return 0; // Invalid thread wrapper
    }

    int policy;
    struct sched_param param;
    if (pthread_getschedparam(wa->tid, &policy, &param) == 0) {
        uint32_t old_custom_priority = map_posix_to_custom_priority(param.sched_priority); // Save old priority
        param.sched_priority = map_custom_to_posix_priority(prio); // Map new priority to POSIX

        if (pthread_setschedparam(wa->tid, policy, &param) == 0) {
            return old_custom_priority; // Return the old custom priority
        }
    }

    return 0; // If setting priority fails, return default priority
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
        os_sys_unlock () ;
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
    OS_THREAD_WA_T  * wa = pthread_getspecific(g_posix_wa_key);
    if (wa) {
        wa->tls[idx] = value;
        return EOK;
    }
    return EFAIL;
}

uint32_t
os_thread_tls_get (int32_t idx)
{
    if ((idx < 0 ) || (idx >= MAX_TLS_ID)) {
        return 0 ;
    }
    OS_THREAD_WA_T  * wa = pthread_getspecific(g_posix_wa_key);
    if (wa) {
        return wa->tls[idx];
    }
    return 0;
}

p_sem_t*
os_thread_thdsem_get (void)
{
    /* Nothing analogous, return NULL. */
    return NULL;
}

int32_t*
os_thread_errno (void)
{
    p_thread_t* thread = os_thread_current () ;
    OS_THREAD_WA_T* wa = (OS_THREAD_WA_T*)*thread;
    if (!wa) {
        return 0; // Invalid thread wrapper
    }

    return &wa->errorno ;
}

void
os_thread_sleep (uint32_t msec)
{
    if (msec) {
        usleep(msec * 1000);
    } else {
        /* yield */
        sched_yield();
    }
}

void
os_thread_sleep_ticks (uint32_t ticks)
{
    /* We'll pretend 1 'tick' ~ 1 ms. Adjust as needed. */
    if (ticks) {
        usleep(ticks * 1000);
    } else {
        sched_yield();
    }
}

int32_t
os_thread_wait (uint32_t ticks)
{
    int32_t res;
    OS_THREAD_WA_T  * wa = pthread_getspecific(g_posix_wa_key);
    if (!wa) {
        return E_NOIMPL;
    }
    pthread_mutex_lock(&wa->suspend_mutex);
    if (ticks == OS_TIME_INFINITE) {
        /* indefinite wait */
        pthread_cond_wait(&wa->suspend_cond, &wa->suspend_mutex);
        res = wa->suspend_msg;
    } else {
        struct timespec t;
        struct timeval  now;
        gettimeofday(&now, NULL);
        uint64_t ms     = (uint64_t) (ticks * 1000ULL / os_sys_tick_freq());
        uint64_t nsec   = (now.tv_usec + (ms % 1000) * 1000ULL) * 1000ULL;
        t.tv_sec        = now.tv_sec + (ms / 1000) + (nsec / 1000000000ULL);
        t.tv_nsec       = (long)(nsec % 1000000000ULL);

        int rc = pthread_cond_timedwait(&wa->suspend_cond, &wa->suspend_mutex, &t);
        if (rc == 0) {
            res = wa->suspend_msg;
        } else if (rc == ETIMEDOUT) {
            res = E_TIMEOUT;
        } else {
            res = EFAIL;
        }
    }
    pthread_mutex_unlock(&wa->suspend_mutex);
    return res;
}

int32_t
os_thread_notify (p_thread_t* thread, int32_t msg)
{
    OS_THREAD_WA_T  * wa = (OS_THREAD_WA_T*)*thread ;
    if (!wa) return E_NOIMPL;

    pthread_mutex_lock (&wa->suspend_mutex);
    wa->suspend_msg = msg ;
    pthread_cond_signal (&wa->suspend_cond);
    pthread_mutex_unlock (&wa->suspend_mutex);
    return EOK ;
}

int32_t
os_thread_notify_isr (p_thread_t* thread, int32_t msg)
{
    return os_thread_notify (thread, msg) ;
}

void
os_sys_start (void)
{
    if (!_os_started) {
#if !defined CFG_OS_OS_TIMER_DISABLE
        start_timer_manager() ;
#endif
        _os_started = 1 ;

    }
}

int
os_sys_started (void)
{
    return _os_started ;
}

void
os_sys_lock (void)
{

}


void
os_sys_unlock (void)
{

}

uint32_t
os_sys_tick_freq (void)
{
    /* Let's pretend we have 1000 ticks per second. Adjust if you want. */
    return 1000;
}


uint32_t
os_sys_ticks (void)
{
    /* We'll fake it by returning ms from gettimeofday. */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

uint32_t
os_sys_timestamp (void)
{
    /* Just do the same: get ms since start of epoch? */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

uint32_t
os_sys_us_timestamp (void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000000 + tv.tv_usec);
}

void
os_sys_stop (void)
{
    if (_os_started) {
#if !defined CFG_OS_OS_TIMER_DISABLE
        stop_timer_manager() ;
#endif
        _os_started = 0 ;

    }
}

void
os_sys_halt (const char * msg)
{
    qoraal_debug_print("ASSERT: ");
    qoraal_debug_assert(msg);
    while (1) { }
}

uint32_t
os_sys_is_irq (void)
{
    return 0 ;
} 

#if !defined CFG_OS_MUTEX_DISABLE
int32_t 
os_mutex_init (p_mutex_t* mutex)
{
    if (mutex == NULL) {
        return EFAIL;
    }


    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        return EFAIL;
    }

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    if (pthread_mutex_init((pthread_mutex_t*)*mutex, &attr) != 0) {
        pthread_mutexattr_destroy(&attr);
        
        return EFAIL;
    }

    pthread_mutexattr_destroy(&attr);
    return EOK;
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void 
os_mutex_deinit (p_mutex_t* mutex)
{
    if (mutex && *mutex) {
        pthread_mutex_destroy((pthread_mutex_t*)*mutex);
        *mutex = NULL;
    }
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t 
os_mutex_create (p_mutex_t* mutex)
{
    *mutex = qoraal_malloc(QORAAL_HeapOperatingSystem, sizeof(pthread_mutex_t)); // Allocate space for the mutex
    if (*mutex == NULL) {
        return EFAIL;
    }

    int res = os_mutex_init (mutex);
    if (res != EOK) {
        qoraal_free(QORAAL_HeapOperatingSystem, *mutex);
    }
    return res ;
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void 
os_mutex_delete (p_mutex_t* mutex)
{
    if (mutex && *mutex) {
        pthread_mutex_destroy((pthread_mutex_t*)*mutex);
        qoraal_free(QORAAL_HeapOperatingSystem, *mutex);
        *mutex = NULL;
    }
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t 
os_mutex_lock (p_mutex_t *mutex) 
{
    if (mutex == NULL || *mutex == NULL) {
        return EFAIL;
    }
    return pthread_mutex_lock((pthread_mutex_t *)*mutex) == 0 ? EOK : EFAIL;
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
void 
os_mutex_unlock (p_mutex_t *mutex) 
{
    if (mutex && *mutex) {
        pthread_mutex_unlock((pthread_mutex_t *)*mutex);
    }
}
#endif

#if !defined CFG_OS_MUTEX_DISABLE
int32_t 
os_mutex_trylock (p_mutex_t *mutex) 
{
    if (mutex == NULL || *mutex == NULL) {
        return EFAIL;
    }
    return pthread_mutex_trylock((pthread_mutex_t *)*mutex) == 0 ? EOK : EFAIL;
}
#endif

int32_t 
os_sem_init (p_sem_t* sem, int32_t cnt)
{
    if (sem == NULL) {
        return EFAIL;
    }

    return sem_init((sem_t*)(*sem), 0, cnt) == 0 ? EOK : EFAIL;
}

void 
os_sem_deinit (p_sem_t* sem)
{
    if (sem && *sem) {
        sem_destroy((sem_t*)(*sem));
    }
}

int32_t 
os_sem_create (p_sem_t* sem, int32_t cnt)
{
    *sem = qoraal_malloc(QORAAL_HeapOperatingSystem, sizeof(sem_t));
    if (*sem == NULL) {
        return EFAIL;
    }

    return os_sem_init(sem, cnt);
}

int32_t 
os_sem_reset (p_sem_t* sem, int32_t cnt)
{
    if (sem == NULL || *sem == NULL) {
        return EFAIL;
    }

    sem_destroy((sem_t*)(*sem));
    return os_sem_init(sem, cnt);
}

void 
os_sem_delete (p_sem_t* sem)
{
    if (sem && *sem) {
        sem_destroy((sem_t*)(*sem));
        qoraal_free(QORAAL_HeapOperatingSystem, *sem);
        *sem = NULL;
    }
}

int32_t 
os_sem_wait (p_sem_t* sem)
{
    if (sem == NULL || *sem == NULL) {
        return EFAIL;
    }
    return sem_wait((sem_t*)(*sem)) == 0 ? EOK : EFAIL;
}

int32_t 
os_sem_wait_timeout (p_sem_t* sem, uint32_t ticks)
{
    if (sem == NULL || *sem == NULL) {
        return EFAIL;
    }

    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    t.tv_sec += OS_TICKS2MS(ticks) / 1000;
    t.tv_nsec += (OS_TICKS2MS(ticks) % 1000) * 1000000;

    return sem_timedwait((sem_t*)(*sem), &t) == 0 ? EOK : EFAIL;
}

void 
os_sem_signal (p_sem_t* sem)
{
    if (sem && *sem) {
        sem_post((sem_t*)(*sem));
    }
}

int32_t
os_sem_count (p_sem_t* sem)
{
    if (sem == NULL || *sem == NULL) {
        return EFAIL;
    }

    int sval;
    if (sem_getvalue((sem_t*)(*sem), &sval) == -1) {
        return EFAIL;
    }
    return sval;
}

int32_t 
os_bsem_init (p_sem_t* sem, int32_t taken)
{
    return os_sem_init (sem, taken ? 0 : 1);
}

int32_t 
os_bsem_create (p_sem_t* sem, int32_t taken)
{
    return os_sem_create(sem, taken ? 0 : 1);
}

int32_t 
os_bsem_reset (p_sem_t* sem, int32_t taken)
{
    return os_sem_reset(sem, taken ? 0 : 1);
}

void 
os_bsem_delete (p_sem_t* sem)
{
    os_sem_delete(sem);
}

int32_t 
os_bsem_wait (p_sem_t* sem)
{
    return os_sem_wait(sem);
}

int32_t 
os_bsem_wait_timeout (p_sem_t* sem, uint32_t ticks)
{
    return os_sem_wait_timeout(sem, ticks);
}

void 
os_bsem_signal (p_sem_t* sem)
{
    os_sem_signal(sem);
}

#if !defined CFG_OS_EVENT_DISABLE
int32_t 
os_event_init (p_event_t* event)
{
    if (event == NULL) {
        return EFAIL;
    }

    os_event_t* pevent = (os_event_t*)(*event);
    if (pthread_cond_init(&pevent->cond, NULL) != 0 ||
        pthread_mutex_init(&pevent->mutex, NULL) != 0) {
        qoraal_free(QORAAL_HeapOperatingSystem, *event);
        *event = NULL;
        return EFAIL;
    }

    pevent->flags = 0;
    return EOK;
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void 
os_event_deinit (p_event_t* event)
{
    if (event && *event) {
        os_event_t* pevent = (os_event_t*)(*event);
        pthread_mutex_destroy(&pevent->mutex);
        pthread_cond_destroy(&pevent->cond);
    }
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
int32_t 
os_event_create (p_event_t* event)
{
    *event = qoraal_malloc(QORAAL_HeapOperatingSystem, sizeof(os_event_t));
    if (*event == NULL) {
        return EFAIL;
    }

    return os_event_init(event);
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void 
os_event_delete (p_event_t* event)
{
    if (event && *event) {
        os_event_deinit (event) ;
        qoraal_free(QORAAL_HeapOperatingSystem, *event);
        *event = NULL;
    }
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void 
os_event_signal (p_event_t* event, uint32_t mask)
{
    if (event && *event) {
        os_event_t* pevent = (os_event_t*)(*event);
        pthread_mutex_lock(&pevent->mutex);
        pevent->flags |= mask;
        pthread_cond_broadcast(&pevent->cond); // Signal all waiting threads
        pthread_mutex_unlock(&pevent->mutex);
    }
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void 
os_event_signal_isr (p_event_t* event, uint32_t mask)
{
    os_event_signal(event, mask) ;
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
void 
os_event_clear (p_event_t* event, uint32_t mask)
{
    if (event && *event) {
        os_event_t* pevent = (os_event_t*)(*event);
        pthread_mutex_lock(&pevent->mutex);
        pevent->flags &= ~mask;
        pthread_mutex_unlock(&pevent->mutex);
    }
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
uint32_t 
os_event_wait (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, uint32_t all)
{
    if (event == NULL || *event == NULL) {
        return 0;
    }

    os_event_t* pevent = (os_event_t*)(*event);
    uint32_t events = 0;

    pthread_mutex_lock(&pevent->mutex);
    while (all ? ((pevent->flags & mask) != mask) : !(pevent->flags & mask)) {
        pthread_cond_wait(&pevent->cond, &pevent->mutex);
    }
    events = pevent->flags & mask;
    if (clear_on_exit) {
        pevent->flags &= ~events;
    }
    pthread_mutex_unlock(&pevent->mutex);

    return events;
}
#endif

#if !defined CFG_OS_EVENT_DISABLE
uint32_t 
os_event_wait_timeout (p_event_t* event, uint32_t clear_on_exit, uint32_t mask, uint32_t all, uint32_t ticks)
{
    if (event == NULL || *event == NULL) {
        return 0;
    }

    os_event_t* pevent = (os_event_t*)(*event);
    uint32_t events = 0;

    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    t.tv_sec += OS_TICKS2MS(ticks) / 1000;
    t.tv_nsec += (OS_TICKS2MS(ticks) % 1000) * 1000000;

    // Normalize timespec
    t.tv_sec += t.tv_nsec / 1000000000;
    t.tv_nsec %= 1000000000;

    pthread_mutex_lock(&pevent->mutex);
    while (all ? ((pevent->flags & mask) != mask) : !(pevent->flags & mask)) {
        if (pthread_cond_timedwait(&pevent->cond, &pevent->mutex, &t) != 0) {
            // Timeout occurred
            pthread_mutex_unlock(&pevent->mutex);
            return 0;
        }
    }
    events = pevent->flags & mask;
    if (clear_on_exit) {
        pevent->flags &= ~events;
    }
    pthread_mutex_unlock(&pevent->mutex);

    return events;
}
#endif


#if !defined CFG_OS_OS_TIMER_DISABLE
// Timer manager structure
typedef struct TimerManager {
    os_timer_t *head;                      // Head of the sorted linked list
    pthread_mutex_t mutex;            // Mutex for thread-safe access
    pthread_cond_t cond;              // Condition variable for signaling
    bool quit;                        // Flag to stop the timer thread
} TimerManager;

TimerManager    os_timer_manager;
pthread_t       os_timer_thread;

uint64_t 
get_current_time_ms (void) 
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1e6;
}

void *
timer_thread (void *arg) 
{
    TimerManager *manager = &os_timer_manager ;

    while (true) {
        pthread_mutex_lock(&manager->mutex);

        while (!manager->head && !manager->quit) {
            pthread_cond_wait(&manager->cond, &manager->mutex);
        }

        if (manager->quit) {
            pthread_mutex_unlock(&manager->mutex);
            break;
        }

        uint64_t now = get_current_time_ms();
        uint64_t wait_time = manager->head->expire > now ? manager->head->expire - now : 0;

        if (wait_time > 0) {
            struct timespec ts;
            ts.tv_sec = wait_time / 1000;
            ts.tv_nsec = (wait_time % 1000) * 1e6;
            pthread_cond_timedwait(&manager->cond, &manager->mutex, &ts);
        }

        pthread_mutex_unlock(&manager->mutex);

        pthread_mutex_lock(&manager->mutex);
        while (manager->head && manager->head->expire <= get_current_time_ms()) {
            os_timer_t *expired_timer = manager->head;
            manager->head = manager->head->next;

            expired_timer->in_processing = true; // Mark as being processed
            pthread_mutex_unlock(&manager->mutex);

            // Execute the callback
            if (expired_timer->callback) {
                expired_timer->callback(expired_timer->callback_param);
            }

            pthread_mutex_lock(&manager->mutex);
            expired_timer->is_set = false;       // Mark the timer as expired
            expired_timer->in_processing = false; // No longer being processed
        }
        pthread_mutex_unlock(&manager->mutex);
    }

    return NULL;
}

void 
start_timer_manager (void) 
{
    os_timer_manager.head = NULL;

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        return ;
    }
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (pthread_mutex_init(&os_timer_manager.mutex, &attr) != 0) {
        pthread_mutexattr_destroy(&attr);
        return ;
    }

    pthread_mutexattr_destroy(&attr);


    pthread_cond_init(&os_timer_manager.cond, NULL);
    os_timer_manager.quit = false;

    pthread_create(&os_timer_thread, NULL, timer_thread, 0);
}

void 
stop_timer_manager (void) 
{
    pthread_mutex_lock(&os_timer_manager.mutex);
    if (!os_timer_manager.quit) {
        os_timer_manager.quit = true;
        pthread_cond_broadcast(&os_timer_manager.cond);
    }
    pthread_mutex_unlock(&os_timer_manager.mutex);

    pthread_join(os_timer_thread, NULL);

    pthread_mutex_lock(&os_timer_manager.mutex);
    os_timer_t *current = os_timer_manager.head;
    while (current) {
        os_timer_t *next = current->next;
        qoraal_free(QORAAL_HeapOperatingSystem, current);
        current = next;
    }
    pthread_mutex_unlock(&os_timer_manager.mutex);

    pthread_mutex_destroy(&os_timer_manager.mutex);
    pthread_cond_destroy(&os_timer_manager.cond);
}

int32_t 
os_timer_init (p_timer_t *timer, p_timer_function_t fp, void *parm) 
{
    os_timer_t *new_timer = (os_timer_t *)(*timer);

    new_timer->expire = 0;
    new_timer->in_processing = false;
    new_timer->callback = fp;
    new_timer->callback_param = parm;
    new_timer->is_set = false;
    new_timer->next = NULL;

    return EOK;
}

int32_t 
os_timer_create (p_timer_t *timer, p_timer_function_t fp, void *parm) 
{
    os_timer_t *new_timer = (os_timer_t *)qoraal_malloc(QORAAL_HeapOperatingSystem, sizeof(os_timer_t));
    if (!new_timer) {
        return E_NOMEM;
    }
    *timer = new_timer;
    return os_timer_init(timer,  fp, parm) ;
}

void os_timer_reset (p_timer_t *timer) 
{
    os_timer_t *reset_timer = (os_timer_t *)(*timer);
    if (!reset_timer) return;

    pthread_mutex_lock(&os_timer_manager.mutex);

    // If we're resetting a timer that's currently being processed in the callback,
    // skip the wait or you'll deadlock yourself.
    if (reset_timer->in_processing) {
        pthread_mutex_unlock(&os_timer_manager.mutex);
        return; 
    }

    // Remove the timer from the list if it's active
    os_timer_t **current = &os_timer_manager.head;
    while (*current && *current != reset_timer) {
        current = &(*current)->next;
    }

    if (*current == reset_timer) {
        *current = reset_timer->next;
    }

    reset_timer->is_set = false;
    pthread_mutex_unlock(&os_timer_manager.mutex);
}

void 
os_timer_set (p_timer_t *timer, uint32_t ticks) 
{
    os_timer_t *new_timer = (os_timer_t *)(*timer);
    if (!new_timer) return;

    os_timer_reset(timer); // Ensure the timer is not already in the list

    new_timer->expire = get_current_time_ms() + ticks;
    //new_timer->callback = fp;
    //new_timer->callback_param = parm;
    new_timer->is_set = true;

    pthread_mutex_lock(&os_timer_manager.mutex);

    // Insert the timer into the sorted linked list
    os_timer_t **current = &os_timer_manager.head;
    while (*current && (*current)->expire <= new_timer->expire) {
        current = &(*current)->next;
    }
    new_timer->next = *current;
    *current = new_timer;

    pthread_cond_signal(&os_timer_manager.cond);
    pthread_mutex_unlock(&os_timer_manager.mutex);
}

void 
os_timer_set_i (p_timer_t *timer, uint32_t ticks) 
{
    os_timer_set(timer, ticks); // Same as os_timer_set
}

int32_t 
os_timer_is_set (p_timer_t *timer) 
{
    os_timer_t *check_timer = (os_timer_t *)(*timer);
    return check_timer && check_timer->is_set;
}

void 
os_timer_delete (p_timer_t *timer) 
{
    if (!timer || !*timer) return; // Ensure the timer is valid
    os_timer_reset(timer);
    qoraal_free(QORAAL_HeapOperatingSystem, *timer);
    *timer = NULL;
}
#endif /* CFG_OS_OS_TIMER_DISABLE */
#endif /* CFG_OS_POSIX */
