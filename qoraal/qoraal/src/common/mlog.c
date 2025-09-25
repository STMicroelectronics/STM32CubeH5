/*
 *  Copyright (C) 2015-2025, Navaro, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of CORAL Connect (https://navaro.nl)
 */

#include "qoraal/config.h"
#if !defined CFG_COMMON_MEMLOG_DISABLE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "qoraal/qoraal.h"
#include "qoraal/common/mlog.h"
#include "qoraal/common/cbuffer.h"

typedef struct MLOG_INST_S {
    CBUFFER_QUEUE_T     cbuffer ;
    uint32_t            logbuffer_size ;        
    uint32_t            logbuffer[] ;
} MLOG_INST_T ;


typedef struct _MEMLOG_IT_S {
    QORAAL_LOG_IT_T     platform_it ;
    void *              it ;
    uint16_t            log ;
} _MEMLOG_IT_T ;

static char*            _mlog_print_buffer[MLOG_LOGS_MSG_SIZE_MAX]  ;
static MLOG_INST_T  *   _mlog_inst[2] = {0} ;
static uint16_t         _memlog_started = 0 ;
static uint16_t         _memlog_cnt = 0 ;
static OS_MUTEX_DECL    (_mlog_mutex) ;

CBUFFER_QUEUE_T*
get_cqueue (MLOG_TYPE_T log)
{
    if (!_memlog_started)  return 0 ;
    if (_mlog_inst[log]) {
        return &_mlog_inst[log]->cbuffer ;
    }
    return 0 ;
}

int32_t 
mlog_reset (MLOG_TYPE_T log)
{
    if (!_memlog_started)  return E_UNEXP ;
    int32_t lock = 0 ;
    if (os_sys_started()) lock = 1 ;
    if (lock) os_mutex_lock (&_mlog_mutex) ;

    if (_mlog_inst[log]) {
        cqueue_init(&_mlog_inst[log]->cbuffer, _mlog_inst[log]->logbuffer, 
                    _mlog_inst[log]->logbuffer_size/sizeof(uint32_t)) ;

    }
    if (lock) os_mutex_unlock (&_mlog_mutex) ;

    return EOK ;
}

int32_t 
mlog_init (uint32_t * log_buffer, uint32_t log_size, uint32_t *  assert_buffer, uint32_t assert_size)
{
    os_mutex_init (&_mlog_mutex) ;

    if (log_buffer && log_size >= MLOG_LOGS_BUFFERSIZE_MIN) {
        _mlog_inst[MLOG_DBG] = (MLOG_INST_T*)log_buffer ;
        _mlog_inst[MLOG_DBG]->logbuffer_size = log_size - sizeof (MLOG_INST_T) ;
        if (!cqueue_validate(&_mlog_inst[MLOG_DBG]->cbuffer, _mlog_inst[MLOG_DBG]->logbuffer, 
                        _mlog_inst[MLOG_DBG]->logbuffer_size/sizeof(uint32_t))) {
            cqueue_init(&_mlog_inst[MLOG_DBG]->cbuffer, _mlog_inst[MLOG_DBG]->logbuffer,
                        _mlog_inst[MLOG_DBG]->logbuffer_size/sizeof(uint32_t)) ;

        } 
    }

    if (assert_buffer && assert_size >= MLOG_LOGS_BUFFERSIZE_MIN) {
        _mlog_inst[MLOG_ASSERT] = (MLOG_INST_T*)assert_buffer ;
        _mlog_inst[MLOG_ASSERT]->logbuffer_size = assert_size  - sizeof (MLOG_INST_T) ;
        if (!cqueue_validate(&_mlog_inst[MLOG_ASSERT]->cbuffer, _mlog_inst[MLOG_ASSERT]->logbuffer, 
                    _mlog_inst[MLOG_ASSERT]->logbuffer_size/sizeof(uint32_t))) {
            cqueue_init(&_mlog_inst[MLOG_ASSERT]->cbuffer, _mlog_inst[MLOG_ASSERT]->logbuffer,
                        _mlog_inst[MLOG_ASSERT]->logbuffer_size/sizeof(uint32_t)) ;

        } 

    }   


    _memlog_started = 1 ;

    return EOK ;
}

bool            
mlog_started (void)
{
    return _memlog_started ? true : false ;
}

static int32_t  
_append (uint16_t type, uint16_t id, MLOG_TYPE_T log, const char* fmtstr, va_list  args)
{
    int cnt = 10 ;
    CBUFFER_QUEUE_T* logqueue = get_cqueue (log) ;
    if (!logqueue) {
        return E_UNEXP ;
    }

    int msglen = vsnprintf ((char*)_mlog_print_buffer, MLOG_LOGS_MSG_SIZE_MAX-2, fmtstr, args) ;
    _mlog_print_buffer[MLOG_LOGS_MSG_SIZE_MAX-1] = '\0' ;
    int len = (sizeof(QORAAL_LOG_MSG_T) + msglen + 1 + sizeof(uint32_t)  ) / sizeof(uint32_t) ;
    CBUFFER_ITEM_T* buffer = cqueue_enqueue (logqueue, len) ;
    while (!buffer && cqueue_dequeue (logqueue) && cnt--) {
        buffer = cqueue_enqueue (logqueue, len) ;
    }
    if (buffer) {
        QORAAL_LOG_MSG_T * msg = (QORAAL_LOG_MSG_T *)buffer->data ;

        if (++msg->cnt >= 1000)  msg->cnt = 0 ;

        msg->type = type ;
        msg->id = id ;
        msg->cnt = _memlog_cnt++ ;

        msg->date = rtc_get_date () ;
        msg->time = rtc_get_time () ;
        msg->len = msglen + 1;
        memcpy(msg->msg, (char*)_mlog_print_buffer, msg->len);

        for (cnt = 1; cnt < 3; cnt++) {
            if ((msg->msg[msglen-cnt] == '\r') || (msg->msg[msglen-cnt] == '\n')) {
                msg->msg[msglen-cnt] = '\0' ;
                msg->len-- ;
            }
        }

        cqueue_flush_item (logqueue, buffer) ;

    }

    return EOK ;
}

static int32_t  
mlog_append (uint16_t type, uint16_t id, MLOG_TYPE_T log, const char* msg, va_list args)
{
    if (!_memlog_started)  return E_UNEXP ;
    os_mutex_lock (&_mlog_mutex) ;
    int32_t res = _append (type, id, log, msg, args) ;
    os_mutex_unlock (&_mlog_mutex) ;
    return res ;
}


QORAAL_LOG_MSG_T* 
mlog_get (MLOG_TYPE_T log, uint16_t idx)
{
    if (!_memlog_started)  return 0 ;
    QORAAL_LOG_MSG_T * logmsg = 0 ;
    CBUFFER_QUEUE_T * queue  ;

    queue  = get_cqueue (log) ;
    if (queue) {

        os_mutex_lock (&_mlog_mutex) ;

        CBUFFER_ITEM_T* it =    cqueue_front (queue) ;

        while (idx-- && it) {
            it = cqueue_backwards (queue, it) ;
        }

        if (it) {
            logmsg = (QORAAL_LOG_MSG_T*)it->data ;
        }
        os_mutex_unlock (&_mlog_mutex) ;

    }

    return logmsg ;
}

int32_t 
mlog_vdbg (uint16_t type, uint16_t id, const char* msg, va_list args)
{
    return mlog_append(type, id, MLOG_DBG, msg, args) ;
}

int32_t 
mlog_dbg (uint16_t type, uint16_t id, const char* msg, ...)
{
    va_list         args;
    va_start (args, msg) ;
    int32_t res = mlog_append(type, id, MLOG_DBG, msg, args) ;
    va_end (args) ;
    return res ;
}

int32_t         
mlog_log (int16_t facillity,  int16_t severity, const char* msg, ...)
{
    union {
    uint16_t        type ;
    struct {
        uint8_t     severity ;
        uint8_t     facillity ;
    } ;
    } logtype ;
    logtype.facillity = facillity ;
    logtype.severity = severity ;

    va_list         args;
    va_start (args, msg) ;
    int32_t res = mlog_append (logtype.type, 0, MLOG_DBG, msg, args) ;
    va_end (args) ;
    return res ;
}

int32_t 
mlog_assert (const char* msg, ...)
{
    va_list         args;
    va_start (args, msg) ;
    int32_t res = _append (0, 0, MLOG_ASSERT, msg, args) ;
    return res ;
}

int32_t 
mlog_total (uint16_t log)
{
    if (!_memlog_started)  return 0 ;
    CBUFFER_QUEUE_T * queue  ;
    int32_t count = 0 ;
    queue  = get_cqueue (log) ;

    if (!queue) {
        return 0 ;
    }

    os_mutex_lock (&_mlog_mutex) ;
    count = cqueue_count (queue) ;
    os_mutex_unlock (&_mlog_mutex) ;

    return count ;
}

int32_t 
mlog_count (uint16_t log, uint16_t type)
{
    if (!_memlog_started)  return 0 ;    
    CBUFFER_QUEUE_T * queue  ;
    CBUFFER_ITEM_T* it = 0 ;
    int32_t count = 0 ;

    queue  = get_cqueue (log) ;
    if (queue) {
        os_mutex_lock (&_mlog_mutex) ;
        it =    cqueue_front (queue) ;
        while (it) {
            if (!cqueue_validate_item(queue, it)) {
                break ;

            }
            if (!type || ((((QORAAL_LOG_MSG_T *)it->data)->type) & type)) {
                count++ ;

            }
            it =    cqueue_backwards (queue, it) ;

        }
        os_mutex_unlock (&_mlog_mutex) ;

    }

    return count ;
}

void* 
mlog_itertor_first (MLOG_TYPE_T log, uint16_t type)
{
    if (!_memlog_started)  return 0 ;
    CBUFFER_QUEUE_T * queue  ;
    CBUFFER_ITEM_T* it = 0 ;

    queue  = get_cqueue (log) ;
    if (queue) {

        os_mutex_lock (&_mlog_mutex) ;
        it =    cqueue_front (queue) ;
        while (it) {
            if (! cqueue_validate_item(queue, it)) {
                it = 0 ;
                break ;
            }
            if (!type || ((((QORAAL_LOG_MSG_T *)it->data)->type) & type)) {
                break ;
            }
            it =    cqueue_backwards (queue, it) ;
        }
        os_mutex_unlock (&_mlog_mutex) ;

    }

    return (void*) it ;
}

void* 
mlog_itertor_last (MLOG_TYPE_T log, uint16_t type)
{
    if (!_memlog_started)  return 0 ;    
    CBUFFER_QUEUE_T * queue  ;
    CBUFFER_ITEM_T* it = 0 ;

    queue  = get_cqueue (log) ;

    if (queue) {

        os_mutex_lock (&_mlog_mutex) ;
        it =    cqueue_back (queue) ;
        while (it) {
            if (! cqueue_validate_item(queue, it)) {
                it = 0 ;
                break ;
            }
            if (!type || ((((QORAAL_LOG_MSG_T *)it->data)->type) & type)) {
                break ;
            }
            it =    cqueue_forwards (queue, it) ;
        }
        os_mutex_unlock (&_mlog_mutex) ;

    }

    return (void*) it ;

}

void* 
mlog_itertor_prev (MLOG_TYPE_T log, void * iterator, uint16_t type)
{
    if (!_memlog_started)  return 0 ;
    CBUFFER_QUEUE_T * queue  ;
    CBUFFER_ITEM_T* it = (CBUFFER_ITEM_T*) iterator ;

    queue = get_cqueue (log) ;

    if (queue) {
        os_mutex_lock (&_mlog_mutex) ;
        it =    cqueue_backwards (queue, it) ;
        while (it) {
            if (!cqueue_validate_item(queue, it)) {
                it = 0  ;
                break ;
            }
            if (!type || ((((QORAAL_LOG_MSG_T *)it->data)->type) & type)) {
                break ;
            }
            it =    cqueue_backwards (queue, it) ;
        }

        os_mutex_unlock (&_mlog_mutex) ;
    } else {
        return 0 ;
    }

    return it ;
}

void* 
mlog_itertor_next (MLOG_TYPE_T log, void * iterator, uint16_t type)
{
    if (!_memlog_started)  return 0 ;
    CBUFFER_QUEUE_T * queue  ;
    CBUFFER_ITEM_T* it = (CBUFFER_ITEM_T*) iterator ;

    queue  = get_cqueue (log) ;

    if (queue) {
        os_mutex_lock (&_mlog_mutex) ;
        it =    cqueue_forwards (queue, it) ;
        while (it) {
            if (!cqueue_validate_item(queue, it)) {
                it = 0  ;
                break ;
            }
            if (!type || ((((QORAAL_LOG_MSG_T *)it->data)->type) & type)) {
                break ;
            }
            it =    cqueue_forwards (queue, it) ;
        }
        os_mutex_unlock (&_mlog_mutex) ;
    } else {
        return 0 ;
    }

    return it ;

}

QORAAL_LOG_MSG_T*  
mlog_itertor_get (MLOG_TYPE_T log, void * it)
{
    if (!_memlog_started)  return 0 ;
    QORAAL_LOG_MSG_T * msg = 0 ;
    CBUFFER_QUEUE_T * queue   ;
    CBUFFER_ITEM_T* i  ;

    queue  = get_cqueue (log) ;
    i = (CBUFFER_ITEM_T*) it ;

    os_mutex_lock (&_mlog_mutex) ;
    if (queue && i) {

        if (cqueue_validate_item(queue, i)) {
            msg = (QORAAL_LOG_MSG_T*)i->data ;
        }
    }
    if (!msg) os_mutex_unlock (&_mlog_mutex) ;

    return msg ;
}

void  
mlog_itertor_release (MLOG_TYPE_T log, void * it)
{
    if (!_memlog_started)  return ;
    os_mutex_unlock (&_mlog_mutex) ;
}


static int32_t 
_it_prev(struct QORAAL_LOG_IT_S * it)
{
    _MEMLOG_IT_T *syslogit = (_MEMLOG_IT_T*) it ;
    syslogit->it = mlog_itertor_prev (syslogit->log, syslogit->it, 0) ;
    if (!syslogit->it) {
        return E_BOF ;

    }

    return EOK ;
}

static int32_t 
_it_get(struct QORAAL_LOG_IT_S * it, QORAAL_LOG_MSG_T * msg, uint32_t len)
{
    int32_t res = EFAIL ;
    _MEMLOG_IT_T *syslogit = (_MEMLOG_IT_T*) it ;
    QORAAL_LOG_MSG_T*  m = mlog_itertor_get (syslogit->log, syslogit->it) ;
    if (m) {
        if (len > sizeof(QORAAL_LOG_MSG_T) + m->len) {
            len = sizeof(QORAAL_LOG_MSG_T) + m->len ;
        }
        memcpy (msg, m, len) ;
        mlog_itertor_release (syslogit->log, syslogit->it) ;
        msg->msg[m->len] = '\0' ;
        res = (int32_t) len ;

    }

    return res ;
}


QORAAL_LOG_IT_T * 
mlog_platform_it_create (MLOG_TYPE_T log)
{
    if (!_memlog_started)  return 0 ;
    _MEMLOG_IT_T * it = qoraal_malloc(QORAAL_HeapOperatingSystem, sizeof(_MEMLOG_IT_T)) ;
    if (it) {
        it->it = mlog_itertor_first (log, 0) ;
        if (!it->it) {
        	qoraal_free (QORAAL_HeapOperatingSystem, it) ;
            it = 0 ;

        } else {
            it->log = log ;
            it->platform_it.prev = _it_prev ;
            it->platform_it.get = _it_get ;

        }
    }

    return (QORAAL_LOG_IT_T *)it ;
}

void                
mlog_platform_it_destroy (QORAAL_LOG_IT_T * it)
{
	qoraal_free (QORAAL_HeapOperatingSystem, it) ;
}


#endif /* CFG_COMMON_MEMLOG_DISABLE */
