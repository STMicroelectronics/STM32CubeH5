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



#ifndef __HTML_H__
#define __HTML_H__


#include <stdint.h>
#include <stdbool.h>
#include "qoraal/qoraal.h"
#include "qoraal-http/wserver.h"

/*===========================================================================*/
/* Client constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/

typedef void (*HTML_EMIT_CB)(void* /*ctx*/, const char* /*html*/, uint32_t /*len*/) ;

typedef struct HTML_EMIT_S {
    HTTP_USER_T *           user ;
    int32_t                 response ;
    p_sem_t                 lock ;
    p_event_t               complete ;
} HTML_EMIT_T ;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t     html_emit_create (HTML_EMIT_T* emit) ;
    void        html_emit_delete (HTML_EMIT_T* emit) ;
    int32_t     html_emit_lock (HTML_EMIT_T* emit, uint32_t timeout) ;
    void        html_emit_unlock (HTML_EMIT_T* emit) ;

    int32_t     html_emit_wait (const char * ep, uint16_t event, uint16_t parm, HTTP_USER_T * user, uint32_t timeout) ;

#ifdef __cplusplus
}
#endif

#endif /* __WRTLOG_H__ */

