/*
    Copyright (C) 2015-2023, Navaro, All Rights Reserved
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


#include "qoraal-engine/config.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <qoraal/qoraal.h>
#include <qoraal/svc/svc_shell.h>
#include <qoraal/common/lists.h>
#include <qoraal/common/strsub.h>
#include "qoraal-engine/engine.h"
#include "port.h"
#include "../tool/parse.h"


#define SERVICE_ENGINE_TASK_QUEUE       SERVICE_PRIO_QUEUE2
#define ENGINE_TASK_HEAP_STORE          (1<<0)
#define ENGINE_TASK_STORE_CNT           20
#define ENGINE_MAX_VARIABLES            100

#define ENGINE_USE_STRSUB               1


/**
 * A structure for a task with a pointer to a state machine.
 */
typedef struct ENGINE_EVENT_S {
    SVC_TASKS_T             task ;
    uint32_t                event ;
    int32_t                 event_register ;
    EVENT_TASK_CB           complete ;
} ENGINE_EVENT_T;

static LISTS_STACK_DECL(    _engine_task_store) ;
static int32_t              _engine_task_store_cnt = -1 ;
static int32_t              _engine_task_store_alloc = 0 ;
static p_mutex_t            _engine_task_mutex = 0 ;
static ENGINE_EVENT_T       _engine_task_store_heap[ENGINE_TASK_STORE_CNT] ;
static int32_t              _engine_variables[ENGINE_MAX_VARIABLES] = {0} ;

static uint32_t             _engine_alloc[2] = {0} ;
static uint32_t             _engine_alloc_max[2] = {0} ;

#if !defined CFG_ENGINE_STRSUB_DISABLE
static int32_t              engine_strsub_cb (STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg) ;
static STRSUB_HANDLER_T     _engine_strsub ;
#endif

static p_mutex_t            _engine_mutex ;


static inline ENGINE_EVENT_T*
engine_task_alloc (void) {
#if !ENGINE_TASK_STORE_CNT
    return (ENGINE_EVENT_T*)qoraal_malloc (QORAAL_HeapAuxiliary, sizeof(ENGINE_EVENT_T)) ;
#else
    os_mutex_lock (&_engine_task_mutex) ;
    _engine_task_store_alloc++ ;
    //STATS_COUNTER_MAX(STATS_ENGINE_TASK_MAX,_engine_task_store_alloc) ;
    ENGINE_EVENT_T* task =0 ;
    task = stack_head(&_engine_task_store) ;
    if (task) {
        DBG_ENGINE_ASSERT (_engine_task_store_cnt,
                    "[err] ---> engine_task_alloc\r\n") ;
        _engine_task_store_cnt-- ;
        stack_remove_head(&_engine_task_store, OFFSETOF(SVC_TASKS_T, next)) ;
        svc_tasks_init_task (&task->task) ;
        svc_tasks_set_flags (&task->task, ENGINE_TASK_HEAP_STORE) ;
        os_mutex_unlock (&_engine_task_mutex) ;

    } else {
        os_mutex_unlock (&_engine_task_mutex) ;
        task = (ENGINE_EVENT_T*)qoraal_malloc (QORAAL_HeapAuxiliary, sizeof(ENGINE_EVENT_T)) ;
        svc_tasks_init_task (&task->task) ;

    }
    if (_engine_task_store_cnt <= 2) os_thread_sleep (8) ;
    return task ;
#endif
}

static inline void
engine_task_free(ENGINE_EVENT_T* task) {
#if !ENGINE_TASK_STORE_CNT
    qoraal_free(QORAAL_HeapAuxiliary, task) ;
#else
    os_mutex_lock (&_engine_task_mutex) ;
    _engine_task_store_alloc-- ;
    //if (_engine_task_store_cnt < ENGINE_TASK_STORE_CNT) {
    if (svc_tasks_get_flags(&task->task) & ENGINE_TASK_HEAP_STORE) {
        DBG_ENGINE_ASSERT (_engine_task_store_cnt<ENGINE_TASK_STORE_CNT,
                    "[err] ---> engine_task_free\r\n") ;
        _engine_task_store_cnt++ ;
        stack_add_head(&_engine_task_store, (plists_t)task, OFFSETOF(SVC_TASKS_T, next)) ;
        os_mutex_unlock (&_engine_task_mutex) ;
    } else {
        os_mutex_unlock (&_engine_task_mutex) ;
        qoraal_free(QORAAL_HeapAuxiliary, task) ;
    }
#endif
}

static inline void
engine_task_init(void) {
#if ENGINE_TASK_STORE_CNT
    if (_engine_task_store_cnt < 0) {
        for (_engine_task_store_cnt=0;
                _engine_task_store_cnt<ENGINE_TASK_STORE_CNT;
                _engine_task_store_cnt++) {
            ENGINE_EVENT_T* task = &_engine_task_store_heap[_engine_task_store_cnt] ;
            svc_tasks_init_task (&task->task) ;
            svc_tasks_set_flags(&task->task, 1) ;
            stack_add_head(&_engine_task_store, (plists_t)task, OFFSETOF(SVC_TASKS_T, next)) ;

        }

    }

#endif
}

static inline void
engine_task_wait(void) {
#if ENGINE_TASK_STORE_CNT
    int i ;
    for (i=0;
            (i<ENGINE_TASK_STORE_CNT) ;
            i++) {

        ENGINE_EVENT_T* task = &_engine_task_store_heap[i] ;
        svc_tasks_cancel_wait (&task->task, 500) ;
    }

    DBG_ENGINE_ASSERT (_engine_task_store_cnt==ENGINE_TASK_STORE_CNT,
                "[err] ---> engine_task_wait %d\r\n") ;

#endif
}

int32_t
engine_port_init (void * arg)
{
    engine_task_init () ;

#if !defined CFG_ENGINE_STRSUB_DISABLE
    strsub_install_handler(0, StrsubToken1, &_engine_strsub, engine_strsub_cb) ;
#endif

    return ENGINE_OK ;
}

int32_t
engine_port_start (void)
{
    if (os_mutex_create (&_engine_task_mutex) != EOK) return EFAIL ;
    if (os_mutex_create (&_engine_mutex) != EOK) return ENGINE_FAIL ;

    return ENGINE_OK ;
}

void
engine_port_stop (void)
{
    os_mutex_delete (&_engine_task_mutex) ;
    os_mutex_delete (&_engine_mutex) ;

    engine_task_wait () ;
}

void*
engine_port_malloc (portheap heap, uint32_t size)
{
    uint32_t * mem = qoraal_malloc(QORAAL_HeapAuxiliary, size + sizeof(uint32_t)) ;
    if (mem) {
        _engine_alloc[heap] += size ;
        if (_engine_alloc_max[heap] < _engine_alloc[heap]) {
            _engine_alloc_max[heap] = _engine_alloc[heap] ;
        }
        *mem = size ;
        return mem + 1 ;

    }

    return 0 ;
}

void
engine_port_free (portheap heap, void* mem)
{
    if (mem) {
        uint32_t * pmem = (uint32_t*)mem - 1 ;
        _engine_alloc[heap] -= *pmem ;
        qoraal_free (QORAAL_HeapAuxiliary, pmem) ;
    }
}

void
engine_log_mem_usage (void)
{
    DBG_ENGINE_LOG (ENGINE_LOG_TYPE_ERROR,
            "port: alloc %u machine bytes (%u max)",
            _engine_alloc[heapMachine], _engine_alloc_max[heapMachine]) ;
    DBG_ENGINE_LOG (ENGINE_LOG_TYPE_ERROR,
            "port: alloc %u parser bytes (%u max)",
            _engine_alloc[heapParser], _engine_alloc_max[heapParser]) ;

}

void
engine_port_lock (void)
{
    os_mutex_lock (&_engine_mutex) ;
}

void
engine_port_unlock (void)
{
    os_mutex_unlock (&_engine_mutex) ;
}

static void
port_event_queue_callback (SVC_TASKS_T * task, uintptr_t parm, uint32_t reason)
{
    ENGINE_EVENT_T*  engine = (ENGINE_EVENT_T*)task ;
    if (reason == SERVICE_CALLBACK_REASON_RUN) {
        engine->complete ((PENGINE_EVENT_T)task, engine->event, engine->event_register, parm) ;

    }

    svc_tasks_complete ((SVC_TASKS_T*)task) ;
    engine_task_free ((ENGINE_EVENT_T*)task) ;
}


PENGINE_EVENT_T
engine_port_event_create (EVENT_TASK_CB complete)
{
    ENGINE_EVENT_T * task = engine_task_alloc () ;

    if (!task) {
        svc_logger_printf ("ENG   : : event allocation failed!!") ;
        return 0 ;

    }
    task->complete = complete ;

    return (PENGINE_EVENT_T)task ;
}

int32_t
engine_port_event_queue (PENGINE_EVENT_T event, uint16_t event_id,
        int32_t reg, uintptr_t parm, int32_t timeout)
{
    if (event) {
        ENGINE_EVENT_T * task = (ENGINE_EVENT_T*)event ;

        if (timeout>= 0) {

            task->event = event_id ;
            task->event_register = reg ;
            if  (svc_tasks_schedule (&task->task, port_event_queue_callback, parm,
                SERVICE_ENGINE_TASK_QUEUE, timeout ? SVC_TASK_MS2TICKS(timeout) : 0) == EOK) {
                return EOK ;

            }

        }

        engine_task_free (task) ;

     }

    return ENGINE_FAIL ;
}

int32_t
engine_port_event_cancel (PENGINE_EVENT_T event)
{
    int32_t expire = svc_task_expire (&event->task) ;
    if (svc_tasks_cancel (&event->task) != EOK) {
        expire = 0 ;
    }

    return expire ? SVC_TASK_TICKS2MS(expire) : 0 ;
}

void
engine_port_log (int inst, const char *format_str, va_list  args)
{
    svc_logger_vlog_state (inst, format_str, args) ;
}

void
engine_port_assert (const char *msg)
{
    qoraal_debug_assert(msg)  ;
}

int32_t
engine_port_variable_write (uint32_t idx, int32_t val)
{
    if (idx >= ENGINE_MAX_VARIABLES) {
        return ENGINE_PARM ;

    }

    _engine_variables[idx] = val ;
    return ENGINE_OK ;
}

int32_t
engine_port_variable_read (uint32_t idx, int32_t * val)
{
    if (idx >= ENGINE_MAX_VARIABLES) {
        return ENGINE_PARM ;

    }

    *val = _engine_variables[idx] ;
    return ENGINE_OK ;
}

static int32_t
_corshell_out(void* ctx, uint32_t out, const char* str)
{
    if (str && (out && out < SVC_SHELL_IN_STD)) {
        DBG_MESSAGE_T (DBG_MESSAGE_SEVERITY_ERROR, 0,
                "ENG  : :  %s", str ? str : "") ;

    }

    return  ENGINE_OK ;
}


int32_t
engine_port_shellcmd (const char* shellcmd)
{
    int32_t res = ENGINE_FAIL ;
    char  *  cmd     ;


    if (shellcmd && strlen(shellcmd)) {

        SVC_SHELL_IF_T  qshell_if ;
        svc_shell_if_init (&qshell_if, 0, _corshell_out, 0) ;

        int len = strlen (shellcmd) ;
        cmd = engine_port_malloc (heapParser, len+1) ;
        if (cmd) {
            strncpy (cmd, shellcmd, len) ;
            cmd[len] = '\0' ;
            svc_shell_script_run (&qshell_if, "", cmd, len) ;
            engine_port_free (heapParser, cmd) ;

        }


    }

    return res ;
}

uint32_t
engine_timestamp (void)
{
    return os_sys_timestamp () ;

}

#if !defined CFG_ENGINE_STRSUB_DISABLE
int32_t
engine_strsub_cb (STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg)
{
    int32_t res = E_INVAL ;
    uint32_t idx ;
    int32_t value = 0 ;
    char strvalue[16] ;

    if (
            isdigit((int)str[0]) &&
            (sscanf(str, "%u", (unsigned int*)&idx) > 0)
        ) {
        res = engine_get_variable (0, idx, &value) ;
        if (res >= 0) {
            res = sprintf (strvalue, "%d", (int)value) ;
            if (res > 0) {
                res = cb (strvalue, res, offset, arg) ;

            }

        }

    }

    return res ;
}
#endif

#if !defined CFG_ENGINE_STRSUB_DISABLE
static int32_t
parse_strsub_cb(STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg)
{
#define SERVICES_STRSUB_BUFFER_LEN          12
    int32_t res = ENGINE_FAIL ;
    char buffer[SERVICES_STRSUB_BUFFER_LEN] ;

    uint32_t idx = 0 ;
    int type = parseInvalid ;
    if (len) {
        if (isdigit((int)str[0])) {
            if (sscanf(str, "%u", (unsigned int*)&idx) <= 0) return res ;

        } else {
            type = ParseGetIdentifierId(str, len, &idx) ;

        }

    }

    if (type == parseVariable) {
        res = snprintf (buffer, SERVICES_STRSUB_BUFFER_LEN, "[%u]", (unsigned int)idx) ;

    } else if (type == parseEvent) {
        res = snprintf (buffer, SERVICES_STRSUB_BUFFER_LEN, "%u", (unsigned int)idx) ;
       
    }
    if (res >= 0) {
        res = cb (buffer, res, offset, arg) ;

    }

    return res ;
}
#endif

const char *
engine_port_sanitize_string (const char * string, uint32_t * plen)
{
#if !defined CFG_ENGINE_STRSUB_DISABLE
    #pragma GCC diagnostic ignored  "-Wmissing-braces"
    STRSUB_INSTANCE_T  strsub_instance = {STRSUB_ESCAPE_TOKEN, STRSUB_HANDLERS_TOKENS, {0}} ;
    STRSUB_HANDLER_T    strsub ;
    strsub_install_handler(&strsub_instance, StrsubToken1, &strsub, parse_strsub_cb) ;
    int32_t dstlen = strsub_parse_get_dst_length (&strsub_instance, string, *plen) ;
    if (dstlen > 0) {
        char * newname = qoraal_malloc(QORAAL_HeapAuxiliary, dstlen) ;
        if (newname) {
            *plen = strsub_parse_string_to (&strsub_instance, string, *plen, newname, dstlen) ;
        }
        return newname ;
    }
#endif
    return 0 ;
}

void
engine_port_release_string (const char * string)
{
#if !defined CFG_ENGINE_STRSUB_DISABLE
    qoraal_free (QORAAL_HeapAuxiliary, (void*)string) ;
#endif
}


