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

/*===========================================================================
 * MLock
 * @brief   Basic Priority Ceiling Protocol
 *          Mutex lock with Non Preemption Protocol
 *          Caller is assigned the highest priority if it succeeds in locking.
 *          Highest priority=non preemtion!
 *          Caller is assigned its own priority back when it releases.
 *  @note   To avoid lock-order problem with regular RTOS mutex.
 *
 * @{
 *===========================================================================*/

/*===========================================================================*/
/* MLock module data structures and types.                                   */
/*===========================================================================*/

#include "qoraal/config.h"
#if !defined CFG_OS_MLOCK_DISABLE
#include "qoraal/qoraal.h"

#define MLOCK_USE_PRIORITIES        1
#if 1
#define DBG_MESSAGE_MLOCK(severity, fmt_str, ...)
#define DBG_ASSERT_MLOCK(cond, fmt_str, ...)
#else
#define DBG_MESSAGE_MLOCK(severity, fmt_str, ...) 	DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__) ;
#define DBG_ASSERT_MLOCK(cond, fmt_str, ...)       	DBG_ASSERT_T(cond, fmt_str, ##__VA_ARGS__)
#endif




static int32_t _mlock_tls_prio = -1 ;
static int32_t _mlock_tls_cnt = -1 ;


#define SEM_INIT(s, cnt)                    os_sem_init((p_sem_t*)(s), cnt)
#define SEM_WAIT_S(s, timeout)              os_sem_wait_timeout((p_sem_t*)(s), timeout)
#define SEM_SIGNAL_S(s)                     os_sem_signal((p_sem_t*)(s))
#define SEM_DEINIT(s)                       os_sem_deinit ((p_sem_t*)(s)) ;
#define THD_CURRENT()                       os_thread_current ()

/*===========================================================================*/
/* MLock external declarations.                                              */
/*===========================================================================*/

int32_t
os_mlock_init (p_mlock_t* mlock, uint32_t prio)
{
    MLock *mp = (MLock *) *mlock ;

    DBG_ASSERT_MLOCK(mp != NULL, "MLockInit");
    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_LOG, "---- %p MLockInit %p",
                    os_thread_current(), mp) ;

    os_sys_lock () ;
    if (_mlock_tls_prio < 0) {
        os_thread_tls_alloc (&_mlock_tls_prio) ;
    }
    if (_mlock_tls_cnt < 0) {
        os_thread_tls_alloc (&_mlock_tls_cnt) ;
    }

    p_sem_t ps = (p_sem_t)&mp->s ;
    SEM_INIT (&ps, 1) ;
    mp->owner = 0 ;
    mp->prio = prio ;
    mp->cnt = 0;
    os_sys_unlock () ;

    return EOK ;
}

void
os_mlock_deinit (p_mlock_t* mlock)
{
    MLock *mp = (MLock *) *mlock ;

    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_LOG, "---- MLockDestroy %p", mp)

    while (mp->owner) {
        os_mlock_unlock(mlock);

    }
    p_sem_t ps = (p_sem_t)&mp->s ;
    SEM_DEINIT(&ps) ;
}

int32_t
os_mlock_create (p_mlock_t* mlock, uint32_t prio)
{
    *(mlock) = qoraal_malloc (QORAAL_HeapOperatingSystem, (sizeof(MLock)) ) ;
    if (!*(mlock)) return E_NOMEM ;
    return os_mlock_init (mlock, prio) ;
}

void
os_mlock_delete (p_mlock_t* mlock)
{
    os_mlock_deinit (mlock) ;
    qoraal_free (QORAAL_HeapOperatingSystem, *(mlock)) ;
}

void
os_mlock_lock (p_mlock_t* mlock)
{
    MLock *mp = (MLock *) *mlock ;

    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_INFO,
          "-->> %p MLockLock %p (cnt %d)",
          os_thread_current(), mp, mp->cnt);

    os_sys_lock () ;
    if (mp->owner == THD_CURRENT()) {
        mp->cnt++;
        os_sys_unlock () ;
        return;
    }
    os_sys_unlock () ;

#if MLOCK_USE_PRIORITIES
    if (os_thread_tls_get (_mlock_tls_prio) == 0) {
        uint32_t  prio = os_thread_get_prio () ;
        os_thread_tls_set (_mlock_tls_prio, prio) ;
        if (prio < mp->prio) {
            os_thread_set_prio(0, mp->prio) ;

        }

    }
#endif

    p_sem_t ps = (p_sem_t)&mp->s ;
    SEM_WAIT_S (&ps, OS_TIME_INFINITE)  ;

    os_sys_lock () ;
    mp->owner = THD_CURRENT() ;
    mp->cnt=1;
    os_sys_unlock () ;
    os_thread_tls_set (_mlock_tls_cnt, os_thread_tls_get (_mlock_tls_cnt) + 1 ) ;


    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_LOG,
          "<<-- %p MLockLock %p (cnt %d)",
          os_thread_current(), mp, mp->cnt);

}

void
os_mlock_unlock (p_mlock_t* mlock)
{
    MLock *mp = (MLock *) *mlock ;
    uint32_t cnt ;

    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_LOG,
            ">>-- %p MLockUnlock %p (cnt %d)",
            os_thread_current(), mp, mp->cnt)

    DBG_ASSERT_MLOCK(mp,  "MLockUnlock()");
    DBG_ASSERT_MLOCK(mp->owner == THD_CURRENT(), "MLockUnlock(), #2, ownership failure");
    DBG_ASSERT_MLOCK(mp->cnt > 0, "MLockUnlock(), #2, ownership failure");

    os_sys_lock () ;
    mp->cnt--;
    if (mp->cnt==0) {
        mp->owner = 0 ;
        os_sys_unlock () ;
        cnt = os_thread_tls_get (_mlock_tls_cnt) ;
        cnt-- ;
        os_thread_tls_set (_mlock_tls_cnt, cnt ) ;
#if MLOCK_USE_PRIORITIES
        if (cnt == 0) {
            uint32_t prio = os_thread_tls_get (_mlock_tls_prio) ;
            DBG_ASSERT_MLOCK(prio > 0, "MLockUnlock(), #3, ownership failure");
            if (prio != os_thread_get_prio()){
                os_thread_set_prio(0, prio) ;
            }
        }
#endif
        p_sem_t ps = (p_sem_t)&mp->s ;
        SEM_SIGNAL_S(&ps) ;

    } else {
        os_sys_unlock () ;

    }

    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_INFO,
            "--<< %p MLockUnlock %p (cnt %d)",
            os_thread_current(), mp, mp->cnt)

}

uint32_t
os_mlock_trylock (p_mlock_t* mlock)
{
    MLock *mp = (MLock *) *mlock ;
    int32_t ret = EOK ;
    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_INFO,
          "-->> %p MLockTryLock %p (cnt %d)",
          os_thread_current(), mp, mp->cnt)
    DBG_ASSERT_MLOCK(mp, "MLockUnlock()");

    DBG_ASSERT_MLOCK(mp != NULL, "MLockTryLockS");
    os_sys_lock () ;
    if (mp->owner == THD_CURRENT()) {
        mp->cnt++;
        os_sys_unlock () ;
    } else {

        os_sys_unlock () ;

        p_sem_t ps = (p_sem_t)&mp->s ;
        ret = SEM_WAIT_S (&ps, OS_TIME_IMMEDIATE)  ;

        if (ret == EOK) {

#if MLOCK_USE_PRIORITIES
            if (os_thread_tls_get (_mlock_tls_prio) == 0) {
                os_thread_tls_set (_mlock_tls_prio, os_thread_get_prio()) ;
                if (os_thread_get_prio() < mp->prio) {
                    os_thread_set_prio(0, mp->prio) ;
                }
            }
#endif
            os_sys_lock () ;
            mp->owner = THD_CURRENT() ;
            mp->cnt=1;
            os_sys_unlock () ;
            os_thread_tls_set (_mlock_tls_cnt, os_thread_tls_get (_mlock_tls_cnt) + 1 ) ;

        }

    }

    DBG_MESSAGE_MLOCK (DBG_MESSAGE_SEVERITY_LOG,
          "<<-- %p MLockTryLock %p (cnt %d) %s",
          os_thread_current(), mp, mp->cnt, ret == EOK ? "PASS" : "FAIL")

    return ret == EOK ;
}

#endif /* CFG_OS_MLOCK_DISABLE */
