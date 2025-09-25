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
#include "qoraal/common/strsub.h"
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"
#include "html.h"


#define USE_MUTEX           1
static HTML_EMIT_T *        _html_emit  = 0 ;
#if USE_MUTEX
static p_mutex_t            _html_mutex = 0 ;
#endif
uint32_t                    _html_event_mask = 0 ;

#if USE_MUTEX
#define MUTEX_LOCK()    os_mutex_lock (&_html_mutex) ;  
#define MUTEX_UNLOCK()  os_mutex_unlock (&_html_mutex) ;
#else
#define MUTEX_LOCK()
#define MUTEX_UNLOCK()
#endif

/*===========================================================================*/
/* part local functions.                                                */
/*===========================================================================*/

#define HTML_RESPONSE_TYPE_HTML                       0
#define HTML_RESPONSE_TYPE_TEXT                       1
#define HTML_RESPONSE_TYPE_CSS                        2
#define HTML_RESPONSE_TYPE_JSON                       3


/**
 * @brief   Initializes actions for part
 *
 */
ENGINE_ACTION_IMPL  (html_response,     "Startes a response of param TEXT, HTML, CSS or JSON.") ;
ENGINE_ACTION_IMPL  (html_emit,         "Emits html text.") ;
ENGINE_ACTION_IMPL  (html_subst_emit,   "Emits html text after string substetution.") ;
ENGINE_ACTION_IMPL  (html_ready,        "Completes html text / ready to render.") ;


/**
 * @brief   Initialises events for part
 *
 */
ENGINE_EVENT_IMPL   ( _html_render,     "Render content with html_emit and finally html_ready again.") ;
ENGINE_EVENT_IMPL   ( _html_click,      "Click event.") ;

/**
 * @brief   Initialises constants for part
 *
 */
ENGINE_CONST_IMPL(HTML_RESPONSE_TYPE_HTML,  HTML,           "text/html");
ENGINE_CONST_IMPL(HTML_RESPONSE_TYPE_TEXT,  TEXT,           "text");
ENGINE_CONST_IMPL(HTML_RESPONSE_TYPE_CSS,   CSS,            "text/css");
ENGINE_CONST_IMPL(HTML_RESPONSE_TYPE_JSON,  JSON,           "application/json");


/**
 * @brief   part declaration.
 *
 */
ENGINE_CMD_FP_IMPL (part_html_cmd) ;


int32_t 
html_emit_create (HTML_EMIT_T* emit)
{
    if (os_event_create (&emit->complete) != EOK) {
        html_emit_delete (emit) ;
        return EFAIL ;
    }
    if (os_sem_create (&emit->lock, 1) != EOK) {
        html_emit_delete (emit) ;
        return EFAIL ;
    }

     return EOK ;
}

void 
html_emit_delete (HTML_EMIT_T* emit)
{
    if (emit->complete) os_event_delete (&emit->complete) ;
    if (emit->lock) os_sem_delete (&emit->lock) ;
    emit->complete = 0 ;
    emit->lock = 0 ;
    emit->user = 0 ;
}

int32_t 
html_emit_lock (HTML_EMIT_T* emit, uint32_t timeout)
{
    if (!_html_mutex) return E_UNEXP ;
    int32_t res = os_sem_wait_timeout (&emit->lock, OS_MS2TICKS(timeout)) ;
    if (res == EOK) {
        _html_emit = emit ;
    }

    return res ;
}

void 
html_emit_unlock (HTML_EMIT_T* emit)
{
    _html_emit = 0 ;
    os_sem_signal (&emit->lock) ;
}

int32_t 
html_emit_wait (const char * ep, uint16_t event, uint16_t parm, HTTP_USER_T * user, uint32_t timeout)
{
    if (!_html_emit || !_html_mutex) return E_UNEXP ;
    if (!user) return E_PARM ;

    uint32_t mask ;
    int32_t idx ;
    if (ep == 0) idx = 0 ;
    else idx = engine_statemachine_idx (ep) ;
    if (idx < 0) return idx ;
    MUTEX_LOCK () ;
    mask = _html_event_mask & (1 << idx) ;
    MUTEX_UNLOCK () ;

    if (!mask) return E_NOTFOUND ;

    _html_emit->response = -1 ;
    _html_emit->user = user ;

    if (event) {
        engine_queue_masked_event (mask, event, parm) ;
    }

    MUTEX_LOCK () ;
    int32_t res = engine_queue_masked_event (mask, ENGINE_EVENT_ID_GET(_html_render), 0) ;
    if (res == EOK) {
        _html_event_mask &= ~mask ;
    }
    MUTEX_UNLOCK () ;

    if (res == EOK) {
        os_event_wait_timeout (&_html_emit->complete, 1, mask, 0, OS_MS2TICKS(timeout)) ;
        if (_html_emit->response >= 0) {
            httpserver_chunked_complete (_html_emit->user) ;

        }

    } else {
        DBG_ENGINE_LOG(ENGINE_LOG_TYPE_ERROR,
            "error: failed %d to queue event %d\n", res, ENGINE_EVENT_ID_GET(_html_render)) ;

    }

    return EOK ;
}



/**
 * @brief   part_toaster_cmd
 * @param[in] instance      Statemachine instance.
 * @param[in] start         Start/stop.
 */
int32_t
part_html_cmd (PENGINE_T instance, uint32_t start)
{
#if USE_MUTEX
    if (!instance) {
        if (start == PART_CMD_PARM_START) {
            return os_mutex_create (&_html_mutex) ;
        } else {        
            os_mutex_delete (&_html_mutex) ;
            _html_mutex = 0 ;
        }
    }   
#endif
    return ENGINE_OK ;
}

/**
 * @brief   emmits html text
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_html_response (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parm <= HTML_RESPONSE_TYPE_JSON ? EOK : EFAIL  ;

    }

    if (!_html_emit) {
        DBG_ENGINE_LOG(ENGINE_LOG_TYPE_ERROR,
            "error: html_response unexpected!") ;
            return EFAIL ;
       
    }

    const   HTTP_HEADER_T headers[]   = { {"Cache-Control", "no-cache"} };
    const HTTP_HEADER_T * pheaders = 0 ;
    int32_t res = ENGINE_FAIL ;
    const char * resp = HTTP_SERVER_CONTENT_TYPE_TEXT  ;
    if (parm == HTML_RESPONSE_TYPE_HTML)    {
        resp = HTTP_SERVER_CONTENT_TYPE_HTML ;
        pheaders = headers ;
    }
    if (parm == HTML_RESPONSE_TYPE_CSS)     {
        resp = HTTP_SERVER_CONTENT_TYPE_CSS ;
    }
    if (parm == HTML_RESPONSE_TYPE_JSON)    {
        resp = HTTP_SERVER_CONTENT_TYPE_JSON ;
    }

    HTTP_USER_T * user =  0 ;
    MUTEX_LOCK () ;
    if (_html_emit && _html_emit->user) {
        _html_emit->response = parm ;
        user = _html_emit->user ;
    }
    MUTEX_UNLOCK () ;
    if (user) {
        res = httpserver_chunked_response (user, 200, 
                resp, pheaders, sizeof(headers)/sizeof(headers[0])) ;

    }

    return res ;
}

/**
 * @brief   emmits html text
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
static int32_t
html_emit_text (PENGINE_T instance, uint32_t parm, uint32_t flags, bool strsub)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_string (instance, parm, flags) ;

    }

    HTTP_USER_T * user =  0 ;
    MUTEX_LOCK () ;
    if (!_html_emit || _html_emit->response < 0) {
        MUTEX_UNLOCK () ;
        DBG_ENGINE_LOG(ENGINE_LOG_TYPE_ERROR,
            "error: html_emit unexpected!") ;
        return EFAIL ;
       
    }

    user = _html_emit->user ;
    MUTEX_UNLOCK () ;


    if (user) {
        uint16_t len ;
        const char* str = 0 ;
        if (flags & PART_ACTION_FLAG_STRING) {
            str = engine_get_string (instance, parm, &len) ;

        }

        if (str) {
            char * newstr =  0 ;
            if (strsub) {
                int32_t dstlen = strsub_parse_get_dst_length (0, str, len) ;
                if (dstlen > 0) {
                    newstr = qoraal_malloc(QORAAL_HeapAuxiliary, dstlen) ;
                    if (newstr) {
                        len = strsub_parse_string_to (0, str, len, newstr, dstlen) ;
                        str = newstr ;

                    }

                }

            }

            httpserver_chunked_append_str (user, str, len) ;

            if (newstr) qoraal_free(QORAAL_HeapAuxiliary, newstr) ;

        }

    }

    return ENGINE_OK ;
}

int32_t
action_html_emit (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return html_emit_text (instance, parm, flags, false) ;
}

int32_t
action_html_subst_emit (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return html_emit_text (instance, parm, flags, true) ;
}


/**
 * @brief   emmits html text
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_html_ready (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return EOK ;

    }

    MUTEX_LOCK () ;
    uint32_t mask = engine_get_mask (instance) ;
    _html_event_mask |= mask ;
    if (_html_emit && _html_emit->complete) {
       os_event_signal (&_html_emit->complete, mask) ;
    }
    MUTEX_UNLOCK () ;

    return EOK ;
}
