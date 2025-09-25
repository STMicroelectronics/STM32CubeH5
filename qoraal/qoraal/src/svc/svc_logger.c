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


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <inttypes.h>
#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/common/lists.h"
#include "qoraal/common/memdbg.h"
#if !defined CFG_COMMON_MEMLOG_DISABLE
#include "qoraal/common/mlog.h"
#endif

#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"

#define SERVICE_LOGGER_TASK                     1

static LOGGGER_CHANNEL_FILTER_T     _logger_filter =  {SVC_LOGGER_MASK, SVC_LOGGER_SEVERITY_NEVER} ;
#if !defined CFG_COMMON_MEMLOG_DISABLE
static LOGGGER_CHANNEL_FILTER_T     _logger_filter_mem = {SVC_LOGGER_MASK, SVC_LOGGER_SEVERITY_LOG} ;
#endif

static SVC_TASK_PRIO_T      _logger_task_prio ;
static uint16_t             _logger_id = 0 ;
static int32_t              _logger_debug_sending = 0 ;

static LISTS_LINKED_DECL    (_logger_channels) ;
static OS_MUTEX_DECL        (_logger_mutex) ;

#define LOG_MESSAGE_SIZE    0
typedef struct LOGGER_TASK_S {
    SVC_TASKS_T             task ;
    LOGGER_TYPE_T          type ;
    uint8_t                 facility ;
    uint8_t                 reserved ;
    uint16_t                id ;
    char                    message[0] ;
} LOGGER_TASK_T ;


__attribute__((weak))  char __memlog_base__;
__attribute__((weak))  char __memlog_end__;

/**
 * @brief
 *
 * @return              error.
 *
 * @svc
 */
int32_t
svc_logger_init (SVC_TASK_PRIO_T  prio)
{
    os_mutex_init (&_logger_mutex) ;
    linked_init (&_logger_channels) ;

    _logger_task_prio = prio ;

    if (!mlog_started()) {
        uintptr_t base = (uintptr_t)&__memlog_base__;
        uintptr_t end = (uintptr_t)&__memlog_end__;
        uint32_t size = (end - base) ;
        if (size >= 4*1024) {
             mlog_init ((uint32_t *)base, size - 512, 
                    (uint32_t *)(base + size - 512), 512) ;

        }

    }

    return EOK ;
}

/**
 * @brief
 *
 * @return              error.
 *
 * @svc
 */
int32_t
svc_logger_start (void)
{
    return EOK ;
}


/**
* @brief   SVC Task callback to send log message to the registered log channels.
*
* @param[in] task
* @param[in] parm
* @param[in] reason
*
* @return              Error.
*
* @notapi
*/
static void
logger_task_callback (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    LOGGER_TASK_T *logger_task = (LOGGER_TASK_T*) task ;
    if (reason == SERVICE_CALLBACK_REASON_RUN) {

        LOGGER_CHANNEL_T * start ;

        if (linked_head (&_logger_channels)) {
            os_mutex_lock (&_logger_mutex) ;
            for ( start = (LOGGER_CHANNEL_T*)linked_head (&_logger_channels) ;
                (start!=NULL_LLO)
                    ; ) {

                int i;
                for (i=0 ;i<SVC_LOGGER_FILTER_CNT; i++) {

                LOGGERT_MASK_T mask ;

                if (!logger_task->facility) mask = SVC_LOGGER_MASK ;
                else mask = SVC_LOGGER_FACILITY_MASK(logger_task->facility) ;

                if (
                        SVC_LOGGER_GET_SEVERITY(logger_task->type) <=
                                (SVC_LOGGER_GET_SEVERITY(start->filter[i].type)) &&
                            (mask & start->filter[i].mask) &&
                        start->fp
                    ) {
                    const char* msg ;

                    if (!(logger_task->type & SVC_LOGGER_FLAGS_PROGRESS) ||
                                (start->filter[i].type & SVC_LOGGER_FLAGS_PROGRESS)) {

                        int offset = 0 ;

#if SVC_LOGGER_APPEND_TIMESTAMP
                            if ((start->filter[i].type & SVC_LOGGER_FLAGS_NO_TIMESTAMP) && (logger_task->message[0] == '[')) {
                                offset = 12 ;
#ifdef SVC_LOGGER_MEMSTAT_HEAP
                            offset += 15 ;
#endif
                        }
#endif

                        msg = (const char*)&logger_task->message[offset] ;

                        start->fp (start, logger_task->type, logger_task->facility, msg) ;
                            break ;

                    }
                }


                }

                start = (LOGGER_CHANNEL_T*)linked_next ((plists_t)start, OFFSETOF(LOGGER_CHANNEL_T, next));

            }
            os_mutex_unlock (&_logger_mutex) ;
        }


    }
    os_sys_lock();
    _logger_debug_sending-- ;
    os_sys_unlock();
    svc_tasks_complete (task) ;
    qoraal_free(QORAAL_HeapAuxiliary, task) ;
}



/**
* @brief   Allocate a logger task and format the log message.
*
* @param[in] format_str    format string
* @param[in] args           argument list
*
* @return              LOGGER_TASK_T
*
* @notapi
*/
static LOGGER_TASK_T*
logger_create_task (LOGGER_TYPE_T type, uint8_t facility, const char *format_str, va_list  args)
{
    LOGGER_TASK_T* task;
#if !SVC_LOGGER_APPEND_CRLF
#define EXTRA_CHARS 2
#else
#define EXTRA_CHARS 4
#endif

    uint32_t len = 0 ;
    uint32_t message_size =  LOG_MESSAGE_SIZE ;
    if (!message_size) {
        va_list args_copy;
        va_copy(args_copy, args);
        int32_t strlen =  vsnprintf(0, 0, (char*)format_str, args_copy);
        va_end(args_copy);
        if (strlen < 0) return 0 ;
        message_size = strlen + 32 + EXTRA_CHARS ;

    }
    task = (LOGGER_TASK_T*)qoraal_malloc(QORAAL_HeapAuxiliary, sizeof(LOGGER_TASK_T) + message_size + EXTRA_CHARS);

    if (!task) {
        return 0;
    }
    memset(task, 0, sizeof(LOGGER_TASK_T) + message_size + EXTRA_CHARS);
    task->id = _logger_id++ ;
    task->type = type ;
    task->facility = facility ;

#if SVC_LOGGER_APPEND_TIMESTAMP
    if ( !(type & (SVC_LOGGER_FLAGS_NO_FORMATTING|SVC_LOGGER_FLAGS_NO_TIMESTAMP)) ) {
        uint32_t seconds ;
        uint32_t mseconds ;
        mseconds = (unsigned int)os_sys_timestamp() ;
        seconds = mseconds / 1000;
        mseconds %= 1000 ;
        len += snprintf((char*)&(task->message[len]), 
                message_size - EXTRA_CHARS,
                "[%05u.%03u] ",
                (unsigned int)(seconds % 100000),
                (unsigned int)(mseconds % 1000));
#ifdef SVC_LOGGER_MEMSTAT_HEAP
        uint32_t memalloc, memfree ;
        heap_stats (SVC_LOGGER_MEMSTAT_HEAP, &memalloc, &memfree) ;
        len += snprintf((char*)&(task->message[len]), message_size, "[%.5u/%.5u] ",
                (unsigned int)memalloc, (unsigned int)memfree);
#endif
    }
#endif
    len += vsnprintf((char*)&task->message[len], message_size - len - EXTRA_CHARS, (char*)format_str, args);
    if (!(type & SVC_LOGGER_FLAGS_NO_FORMATTING)) {
        if (len >= message_size - EXTRA_CHARS) len = message_size - EXTRA_CHARS -1 ;
        if (task->message[len-1] == '\n') len-- ;
        if (task->message[len-1] == '\r') len-- ;
#if SVC_LOGGER_APPEND_CRLF
        strcpy(&task->message[len], "\r\n") ;
#else
        task->message[len] = '\0' ;
#endif

    }

    svc_tasks_init_task(&task->task);

    return task ;
}

/**
 * @brief   Adds a message to the logger queue.
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 * @param[in] args          argument list
 *
 * @return              Error.
 *
 * @notapi
 */
static int32_t
svc_logger_vlogx (LOGGER_TYPE_T type, uint8_t facility, const char *format_str, va_list    args)
{
    LOGGER_TASK_T* task;
    //static uint16_t id = 0 ;

#if defined SERVICE_LOGGER_TASK && SERVICE_LOGGER_TASK
    int32_t status ;
#endif

    if (_logger_debug_sending >= SVC_LOGGER_MAX_QUEUE_SIZE) {
        return E_TIMEOUT ;
    }

    task = logger_create_task (type, facility, format_str, args);

    if (task == 0) {
        return E_NOMEM;
    }

#if !defined CFG_COMMON_MEMLOG_DISABLE
    if (    (mlog_started() && !(type & SVC_LOGGER_FLAGS_PROGRESS)) &&
            (SVC_LOGGER_GET_SEVERITY(type) <= SVC_LOGGER_GET_SEVERITY(_logger_filter_mem.type)) &&
            (SVC_LOGGER_FACILITY_MASK(facility) & _logger_filter_mem.mask)
        ) {
        mlog_log (facility, SVC_LOGGER_GET_SEVERITY(task->type), (char*)task->message) ;
    }
#endif

#if defined SERVICE_LOGGER_TASK && SERVICE_LOGGER_TASK
    if (    (SVC_LOGGER_GET_SEVERITY(type) > SVC_LOGGER_GET_SEVERITY(_logger_filter.type)) ||
            (status = svc_tasks_schedule((SVC_TASKS_T*)task, logger_task_callback, 0, _logger_task_prio, 0) != EOK)
        ) {
        qoraal_free(QORAAL_HeapAuxiliary, task);
    }
    else {
        os_sys_lock();
        _logger_debug_sending++;
        os_sys_unlock();
    }

    return status ;
#else
    _logger_debug_sending++;
    logger_task_callback ((SVC_TASKS_T*)task, 0, SERVICE_CALLBACK_REASON_RUN) ;
    return EOK;
#endif

}

/**
 * @brief   Adds a message to the logger queue.
 * @note    If there is a log channel with with a severity logging lower than this message.
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 * @param[in] args          argument list
 *
 * @return              Error.
 *
 * @svc
 */
uint32_t
svc_logger_would_log (LOGGER_TYPE_T type, uint8_t facility)
{
    if (
            SVC_LOGGER_WOULD_LOG(_logger_filter, type, facility)
#if !defined CFG_COMMON_MEMLOG_DISABLE
            || (mlog_started() && (!(_logger_filter_mem.type & SVC_LOGGER_FLAGS_PROGRESS) &&
            SVC_LOGGER_WOULD_LOG(_logger_filter_mem, type, facility)))
#endif
        )
        {
            return 1 ;
        }

    return 0 ;
}



/**
 * @brief   Adds a message to the logger queue.
 * @note    If there is a log channel with with a severity logging lower than this message.
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 * @param[in] args          argument list
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_logger_type_vlog (LOGGER_TYPE_T type, uint8_t facility, const char *format_str, va_list    args)
{
    if (svc_logger_would_log(type, facility)) {
        return svc_logger_vlogx (type, facility, format_str, args) ;

    }

    return EOK ;
}

/**
 * @brief   Adds a message to the logger queue.
 * @note    If there is a log channel with with a severity logging lower than this message.
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_logger_type_log (LOGGER_TYPE_T type, uint8_t facility, const char *format_str, ...)
{
    va_list         args;
    va_start(args, format_str);
    int32_t res = svc_logger_type_vlog (type, facility, format_str, args) ;
    va_end (args) ;
    return res ;
}

/**
 * @brief   Adds a message to the logger queue with severity report.
 * @note    Intended to be used like printf
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_logger_printf (const char *format_str, ...)
{
    va_list         args;
    va_start(args, format_str);
    int32_t res = svc_logger_type_vlog (
            SVC_LOGGER_TYPE(SVC_LOGGER_SEVERITY_REPORT,0), 0,
            format_str, args) ;
    va_end (args) ;
    return res ;
}

/**
 * @brief   Adds a message to the logger queue with severity report.
 * @note    Intended to be used like vprintf
 *
 * @param[in] type          logger type, severity, facility and flags defined in svc_logger.h
 * @param[in] format_str    format string
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_logger_vprintf (const char *format_str, va_list args)
{
    return svc_logger_type_vlog (SVC_LOGGER_TYPE(SVC_LOGGER_SEVERITY_REPORT,0),
            0, format_str, args) ;
}

/**
 * @brief   Adds a message to the logger queue.
 * @note    Intended to append content. No formatting will be applied like CR/LF.
 *
 * @param[in] str    string
 * @param[in] len    number of characters to print.
 *
 * @return              Error.
 *
 * @svc
 */
int32_t
svc_logger_put (const char *str, uint32_t len)
{
    LOGGER_TASK_T* task;
    int32_t status ;

    task = (LOGGER_TASK_T*)qoraal_malloc(QORAAL_HeapAuxiliary, sizeof(LOGGER_TASK_T) +len+1);

    if (!task) {
        return 0;
    }
    memset(task, 0, sizeof(LOGGER_TASK_T) +len+1);
    task->id = _logger_id++ ;
    task->type = SVC_LOGGER_TYPE(SVC_LOGGER_SEVERITY_REPORT,
            SVC_LOGGER_FLAGS_NO_FORMATTING|SVC_LOGGER_FLAGS_NO_TIMESTAMP|SVC_LOGGER_FLAGS_PROGRESS) ;

    strncpy((char*)&task->message[0],  (char*)str, len);
    task->message[len] = '\0';
    svc_tasks_init_task(&task->task);

#if defined SERVICE_LOGGER_TASK && SERVICE_LOGGER_TASK
    status = svc_tasks_schedule((SVC_TASKS_T*)task, logger_task_callback, 0, _logger_task_prio, 0) ;
    if (status  != EOK) {
        qoraal_free(QORAAL_HeapAuxiliary, task);
    }
    else {
        os_sys_lock();
        _logger_debug_sending++;
        os_sys_unlock();
    }
    return status ;
#else
    _logger_debug_sending++;
    logger_task_callback ((SVC_TASKS_T*)task, 0, SERVICE_CALLBACK_REASON_RUN) ;
    return EOK;
#endif

}

/**
* @brief   Adds a message to the logger queue.
*
* @param[in] format_str    format string
*
* @return              Error.
*
* @svc
*/
int32_t
svc_logger_vlog_state (int inst, const char *format_str, va_list    args)
{
    LOGGER_TASK_T* task  ;
    static uint32_t cnt = 0 ;
    uint32_t seconds ;
    uint32_t mseconds ;
    uint32_t len  ;

    if (
                (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter.type))
#if !defined CFG_COMMON_MEMLOG_DISABLE
                || (mlog_started() && (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter_mem.type)))
#endif
            ) {


        uint32_t message_size =  LOG_MESSAGE_SIZE ;
        if (!message_size) {
            va_list args_copy;
            va_copy(args_copy, args);
            int32_t strlen =  vsnprintf(0, 0, (char*)format_str, args_copy);
            va_end(args_copy);
            if (strlen < 0) return 0 ;
            message_size = strlen + 32  ;

        }


        task = (LOGGER_TASK_T*)qoraal_malloc(QORAAL_HeapAuxiliary, sizeof(LOGGER_TASK_T) + message_size) ;

        if (task == 0) {
            return E_NOMEM ;
        }
        memset(task, 0, sizeof(LOGGER_TASK_T));

        //msg = &task->msg ;

         //msg->type = NSHELL_NOTIFY_LOG_STATE ;
         task->id = (uint32_t)inst ;
         mseconds = (unsigned int)os_sys_timestamp() ;
         seconds = mseconds / 1000;
         mseconds %= 1000 ;

         task->type = SVC_LOGGER_TYPE (SVC_LOGGER_SEVERITY_WARNING, 0) ;
         task->facility = 0 ;


        if (inst >= 0 ) {
            len = snprintf ((char*)task->message, message_size, "<%d>[%.5u.%.3u - %.3u] ", inst, (unsigned int)seconds, (unsigned int)mseconds, (unsigned int)cnt++) ;
        } else {
            len = snprintf ((char*)task->message, message_size, "<->[%.5u.%.3u - %.3u] ", (unsigned int)seconds, (unsigned int)mseconds, (unsigned int)cnt++) ;
        }
        len += vsnprintf ((char*)&(task->message[len]), message_size - len, (char*)format_str, args) ;


        //msg->length = sizeof (NSHELL_NOTIFY_LOG_T) - message_size + len + 1 ;

#if !defined CFG_COMMON_MEMLOG_DISABLE
        if (mlog_started() && (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter_mem.type))) {
            mlog_dbg (SVC_LOGGER_GET_SEVERITY(task->type), _logger_id++, (char*)task->message) ;
        }
#endif

#if defined SERVICE_LOGGER_TASK && SERVICE_LOGGER_TASK
        if (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter.type)) {
            svc_tasks_init_task ((SVC_TASKS_T*)task) ;
            if (
                    (SVC_LOGGER_SEVERITY_REPORT > SVC_LOGGER_GET_SEVERITY(_logger_filter.type)) ||
                    !linked_head (&_logger_channels) ||
                    (svc_tasks_schedule ((SVC_TASKS_T*)task, logger_task_callback, 0, _logger_task_prio, 0) != EOK)
                ) {
                qoraal_free(QORAAL_HeapAuxiliary, task) ;

            } else {
                os_sys_lock();
                _logger_debug_sending++;
                os_sys_unlock();

            }
#else
            logger_state_task_callback ((SVC_TASKS_T*)task, (uint32_t)0, SERVICE_CALLBACK_REASON_RUN) ;

#endif
        }

    }

    return EOK ;
}



int32_t
svc_logger_type_mem (LOGGER_TYPE_T type, uint8_t facility, const char* mem, uint32_t size, const char * head, const char * tail)
{
    int32_t status = 0 ;
    int len  ;

    if (_logger_debug_sending >= SVC_LOGGER_MAX_QUEUE_SIZE) {
        return E_TIMEOUT ;
    }


    if (
            (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter.type))
#if !defined CFG_COMMON_MEMLOG_DISABLE
              || (mlog_started() && (SVC_LOGGER_SEVERITY_WARNING <= SVC_LOGGER_GET_SEVERITY(_logger_filter_mem.type)))
#endif
                ) {


        LOGGER_TASK_T* task;

        uint32_t message_size =  size * 6 + 96 ;


        task = (LOGGER_TASK_T*)qoraal_malloc(QORAAL_HeapAuxiliary, sizeof(LOGGER_TASK_T)+message_size);

        if (!task) {
            return E_NOMEM ;
        }
        memset(task, 0, sizeof(LOGGER_TASK_T));
        task->id = _logger_id++ ;
        task->type = type ;
        task->facility = facility ;
        len = snprintf(task->message, message_size, "%s", head);

#if 1
        dbg_format_mem_dump_width (&task->message[len], message_size-len, mem, size, 0, 16) ;
#else
        len += snprintf(&task->message[len], message_size-len, "0x%p: ", mem);
        for (int i=0; i<size; i++) {
            len += sprintf (&task->message[len], "%.2X ", (char)mem[i]&0xFF);
            if (len >= message_size-4) break ;
        }
        len += snprintf(&task->message[len], message_size, "%s", tail);
#endif

#if !defined CFG_COMMON_MEMLOG_DISABLE
    if (mlog_started() && (SVC_LOGGER_GET_SEVERITY(type) <= SVC_LOGGER_GET_SEVERITY(_logger_filter_mem.type))) {
        mlog_log (0, SVC_LOGGER_GET_SEVERITY(type), (char*)task->message) ;
    }
#endif

#if defined SERVICE_LOGGER_TASK && SERVICE_LOGGER_TASK
    if (    (SVC_LOGGER_GET_SEVERITY(type) > SVC_LOGGER_GET_SEVERITY(_logger_filter.type)) ||
            (status = svc_tasks_schedule((SVC_TASKS_T*)task, logger_task_callback, 0, _logger_task_prio, 0) != EOK)
        ) {
        qoraal_free(QORAAL_HeapAuxiliary, task);
    }
    else {
        os_sys_lock();
        _logger_debug_sending++;
        os_sys_unlock();
    }
    return status ;
#else
    _logger_debug_sending++;
    logger_task_callback ((SVC_TASKS_T*)task, 0, SERVICE_CALLBACK_REASON_RUN) ;
    return EOK;
#endif

    }


    return EOK ;
}


/**
 * @brief       Gets the lowest severity of all the registered log channels.
 * @note        All logging with a higher severity will be scheduled on the logger queue for logging.
 *
 * @return              severity.
 *
 * @notapi
 */
static void
severity_channel_available(void)
{
    uint8_t severity = SVC_LOGGER_SEVERITY_NEVER ;
    LOGGERT_MASK_T mask = 0 ;
    LOGGER_CHANNEL_T * start ;

    for ( start = (LOGGER_CHANNEL_T*)linked_head (&_logger_channels) ;
        (start!=NULL_LLO)
            ; ) {

        int i ;

        for (i=0; i<SVC_LOGGER_FILTER_CNT; i++) {

            if (!start->filter[i].mask) continue ;
            if (severity < SVC_LOGGER_GET_SEVERITY(start->filter[i].type)) {
                severity = SVC_LOGGER_GET_SEVERITY(start->filter[i].type) ;
            }
            mask |= start->filter[i].mask ;

        }

        start = (LOGGER_CHANNEL_T*)linked_next ((plists_t)start, OFFSETOF(LOGGER_CHANNEL_T, next));

    }

    _logger_filter.type = SVC_LOGGER_TYPE(severity, 0) ;
    _logger_filter.mask = mask ;

}

/**
 * @brief   Set filter for logging to memory.
 * @note    Memory logging is done immediately and not in the task context with
 *          the registered log channels.
 *
 * @param[in] filter
 *
 * @svc
 */

void
svc_logger_set_mem_filter (LOGGGER_CHANNEL_FILTER_T filter)
{
#if !defined CFG_COMMON_MEMLOG_DISABLE
    _logger_filter_mem = filter ;
#endif
}


/**
 * @brief   Adds a log channel for receiving log message.
 * @note    Caller must fill out all the members of the structure.
 *
 * @param[in] channel
 *
 * @svc
 */
void
svc_logger_channel_add (LOGGER_CHANNEL_T * channel)
{
    os_mutex_lock (&_logger_mutex) ;
    linked_add_tail (&_logger_channels, channel, OFFSETOF(LOGGER_CHANNEL_T, next)) ;
    severity_channel_available () ;
    os_mutex_unlock (&_logger_mutex) ;

}

/**
 * @brief   Remove the registered log channel.
 * @note    To change a log channel severity, remove it, change the severity and add it again.
 *
 * @param[in] channel
 *
 * @svc
 */
void
svc_logger_channel_remove (LOGGER_CHANNEL_T * channel)
{
    os_mutex_lock (&_logger_mutex) ;
    linked_remove (&_logger_channels, channel, OFFSETOF(LOGGER_CHANNEL_T, next)) ;
    severity_channel_available () ;
    os_mutex_unlock (&_logger_mutex) ;

}

/**
 * @brief   get the active LOGGGER_CHANNEL_FILTER_T for all log channels.
 *
 * @svc
 */
LOGGGER_CHANNEL_FILTER_T 
svs_logger_get_filter (void)
{
    return _logger_filter ;
}

/**
 * @brief   Wait till there is at least one slot available for the logger queue
 * @note
 * @param[in] timeout
 *
 * @svc
 */
int32_t
svc_logger_wait (uint32_t timeout)
{
    int32_t res = EOK ;
    while (_logger_debug_sending >= (SVC_LOGGER_MAX_QUEUE_SIZE-1)) {
        if ((res = svc_tasks_wait_queue (_logger_task_prio, timeout)) == E_TIMEOUT) {
            break ;
        }
    }


    return res ;
}

int32_t
svc_logger_wait_all (uint32_t timeout)
{
#if 0
    int32_t res = EOK ;
#endif
    while (_logger_debug_sending>0) {
        if (timeout <= SVC_TASK_MS2TICKS(10)) break ;
        os_thread_sleep (10) ;
        timeout -= SVC_TASK_MS2TICKS(10) ;

#if 0
        if ((res = svc_tasks_wait_queue (_logger_task_prio, timeout)) == E_TIMEOUT) {
            break ;
        }
#endif
    }


    return _logger_debug_sending ? EFAIL : EOK ;
}

const char *
svs_logger_severity_str (LOGGER_TYPE_T type)
{
    uint8_t severity = SVC_LOGGER_GET_SEVERITY(type) ;
    const char * names[] = {
            "never"   ,
            "assert"  ,
            "error"   ,
            "warning" ,
            "report"  ,
            "log"     ,
            "info"    ,
            "debug",
            "(undefined)",
    };

    if (severity >= sizeof(names)/sizeof(names[0])) {
        severity = sizeof(names)/sizeof(names[0]) - 1 ;
    }

    return names[severity] ;
}

