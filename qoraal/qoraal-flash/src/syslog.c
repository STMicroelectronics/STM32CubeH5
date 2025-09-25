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

#include "qoraal-flash/config.h"
#if !defined CFG_SYSLOG_DISABLE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "qoraal/qoraal.h"
#include "qoraal-flash/syslog.h"
#include "qoraal-flash/nvram/nlog2.h"


extern void    keep_syslogcmds (void) ;


static p_mutex_t            _syslog_mutex ;
static char                 _syslog_log_buffer[SYSLOGLOG_MAX_MSG_SIZE + sizeof(QORAAL_LOG_MSG_T)]   ;
static SYSLOG_INSTANCE_T *  _syslog_instance = 0 ;
static uint32_t             _syslog_instance_cnt = 0 ;


typedef struct _SYSLOG_IT_S {
    QORAAL_LOG_IT_T platform_it ;
    SYSLOG_ITERATOR_T it ;
} _SYSLOG_IT_T ;


/**
 * @brief   Starts nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t syslog_init (SYSLOG_INSTANCE_T * inst)
{
    keep_syslogcmds () ; 
    _syslog_instance = inst ;
    _syslog_instance_cnt = 0 ;

    return EOK ;
}


/**
 * @brief   Starts nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t syslog_start (void)
{
    int32_t status = 0 ;

    if (!_syslog_instance) {
        return E_UNEXP ;
    }

    for (_syslog_instance_cnt=0; 
            _syslog_instance_cnt<SYSLOG_LOG_MAX; 
            _syslog_instance_cnt++) {
        status = nlog2_init (&_syslog_instance->log[_syslog_instance_cnt]) ;
        if (status != 0) {
            break ;
            
        }

    }

    if (os_mutex_create (&_syslog_mutex) != EOK) {
        return EFAIL ;
    }
    
    return status ? EFAIL : EOK ;
}

/**
 * @brief   Stops nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t syslog_stop (void)
{
    _syslog_instance_cnt = 0 ;
    os_mutex_lock (&_syslog_mutex) ;
    os_mutex_unlock (&_syslog_mutex) ;
    os_mutex_delete (&_syslog_mutex) ;

    return  EOK ;
}

/**
 * @brief   Starts nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t syslog_reset (uint32_t idx)
{
    int32_t res ;
    if (idx >= _syslog_instance_cnt) {
        return EFAIL;
    }

    os_mutex_lock (&_syslog_mutex) ;
    res = nlog2_reset (&_syslog_instance->log[idx]) ;
    os_mutex_unlock (&_syslog_mutex) ;

    return res ;
}


/**
 * @brief   nlog_append
 *
 * @param[in] msg    msg
 *
 * @return              Error.
 *
 * @init
 */
void 
syslog_append (uint32_t idx, uint16_t facillity, uint16_t severity, const char* msg)
{
    QORAAL_LOG_MSG_T * syslog = (QORAAL_LOG_MSG_T*) _syslog_log_buffer ;

    if (idx >= _syslog_instance_cnt) {
        return ;
    }


    os_mutex_lock (&_syslog_mutex) ;

    syslog->id = nlog2_get_id (&_syslog_instance->log[idx]) ;
    rtc_localtime (rtc_time(), &syslog->date, &syslog->time) ;
    syslog->facillity = facillity ;
    syslog->severity = severity ;
    strncpy (syslog->msg, msg, SYSLOGLOG_MAX_MSG_SIZE - 1) ;

    nlog2_append (&_syslog_instance->log[idx], 1 << severity, _syslog_log_buffer,
            sizeof(QORAAL_LOG_MSG_T) + strlen(msg) + 1) ;

    os_mutex_unlock (&_syslog_mutex) ;


}


void
syslog_vappend_fmtstr (int32_t idx, int16_t facillity, int16_t severity, const char* format, va_list    args)
{
    QORAAL_LOG_MSG_T * syslog = (QORAAL_LOG_MSG_T*) _syslog_log_buffer ;

    if (idx >= _syslog_instance_cnt) {
        return ;
    }


    os_mutex_lock (&_syslog_mutex) ;
    syslog->id = nlog2_get_id (&_syslog_instance->log[idx]) ;
    rtc_localtime (rtc_time(), &syslog->date, &syslog->time) ;
    syslog->facillity = facillity ;
    syslog->severity = severity ;
    syslog->len = vsnprintf (syslog->msg, SYSLOGLOG_MAX_MSG_SIZE - 1, format, args) ;

    nlog2_append (&_syslog_instance->log[idx], 1 << severity, _syslog_log_buffer,
            sizeof(QORAAL_LOG_MSG_T) + syslog->len + 1) ;

    os_mutex_unlock (&_syslog_mutex) ;

}


/**
 * @brief   nlog_append
 *
 * @param[in] msg    msg
 *
 * @return              Error.
 *
 * @init
 */
void
syslog_append_fmtstr (uint32_t idx, uint16_t facillity, uint16_t severity, const char* format, ...)
{
    va_list         args;
    va_start (args, format) ;
    syslog_vappend_fmtstr (idx, facillity, severity, format, args) ;
    va_end (args) ;
}


int32_t
syslog_iterator_init (uint32_t idx, uint16_t severity, SYSLOG_ITERATOR_T *it)
{
    int32_t res ;

    if (idx >= _syslog_instance_cnt) {
        return E_PARM ;
    }

    os_mutex_lock (&_syslog_mutex) ;
    res = nlog2_iterator_init (&_syslog_instance->log[idx], ~((1 << severity)-1), it) ;
    os_mutex_unlock (&_syslog_mutex) ;

    return res ;
}

int32_t
syslog_iterator_prev (SYSLOG_ITERATOR_T *it)
{
    int32_t res ;

    os_mutex_lock (&_syslog_mutex) ;
    res = nlog2_iterator_prev (it) ;
    os_mutex_unlock (&_syslog_mutex) ;

    return res ;
}

int32_t
syslog_iterator_next (SYSLOG_ITERATOR_T *it)
{
    int32_t res ;

    os_mutex_lock (&_syslog_mutex) ;
    res = nlog2_iterator_prev (it) ;
    os_mutex_unlock (&_syslog_mutex) ;

    return res ;

}

int32_t
syslog_iterator_read (SYSLOG_ITERATOR_T *it, QORAAL_LOG_MSG_T *msg, uint32_t len)
{
    int32_t res ;

    os_mutex_lock (&_syslog_mutex) ;
    res = nlog2_iterator_read (it, (char*)msg, len) ;
    os_mutex_unlock (&_syslog_mutex) ;

    return res ;

}

static int32_t 
_it_prev(struct QORAAL_LOG_IT_S * it)
{
    _SYSLOG_IT_T *syslogit = (_SYSLOG_IT_T*) it ;
    return syslog_iterator_prev (&syslogit->it) ;

}

static int32_t 
_it_get(struct QORAAL_LOG_IT_S * it, QORAAL_LOG_MSG_T * msg, uint32_t len)
{
    _SYSLOG_IT_T *syslogit = (_SYSLOG_IT_T*) it ;
    return syslog_iterator_read (&syslogit->it, msg, len) ;
}


QORAAL_LOG_IT_T * 
syslog_platform_it_create (uint32_t idx)
{
    if (idx >= _syslog_instance_cnt) {
        return 0 ;
    }

    _SYSLOG_IT_T * it = qoraal_malloc(QORAAL_HeapAuxiliary, sizeof(_SYSLOG_IT_T)) ;
    if (it) {
        if (syslog_iterator_init (idx, SYSLOG_SEVERITY_DEBUG, &it->it) != EOK) {
            qoraal_free (QORAAL_HeapAuxiliary, it) ;
            it = 0 ;

        } else {
            it->platform_it.prev = _it_prev ;
            it->platform_it.get = _it_get ;
        }
    }

    return (QORAAL_LOG_IT_T *)it ;
}

void                
syslog_platform_it_destroy (QORAAL_LOG_IT_T * it)
{
    qoraal_free (QORAAL_HeapAuxiliary, it) ;
}

#endif

