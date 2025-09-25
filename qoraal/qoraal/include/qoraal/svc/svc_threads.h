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


#ifndef __SVC_THREADS_H__
#define __SVC_THREADS_H__

#include <stdint.h>
#include <stdbool.h>
#include "qoraal/errordef.h"
#include "qoraal/os.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/
#define DBG_MESSAGE_SVC_THREADS(severity, fmt_str, ...)    DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_SVC_THREADS                               DBG_ASSERT_T

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

struct SVC_THREADS_S ;
typedef void (*SVC_THREADS_COMPLETE_CALLBACK_T)(struct SVC_THREADS_S * /*service_thread*/, void* /*arg*/) ;

typedef struct SVC_THREADS_S {
    struct SVC_THREADS_S *          next ;
    p_thread_t                      thread ;
    p_thread_function_t             pf ;        
    void*                           arg ;
    SVC_THREADS_COMPLETE_CALLBACK_T complete ;
} SVC_THREADS_T;

#define _SVC_THREADS_DATA/*(name)*/    {0,0,0,0,0}
#define SVC_THREADS_DECL(name)   SVC_THREADS_T name =  _SVC_THREADS_DATA/*(name)*/ 

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t         svc_threads_init (void) ;
    int32_t         svc_threads_start (void) ;

    int32_t         svc_threads_create (SVC_THREADS_T* thread, SVC_THREADS_COMPLETE_CALLBACK_T complete,
                            size_t stack_size, uint32_t prio, p_thread_function_t pf,
                            void *arg, const char* name) ;

    bool            svc_threads_is_active (SVC_THREADS_T* service_thread) ;

    uint32_t        svc_threads_count (void) ;

#ifdef __cplusplus
}
#endif


#endif /* __SERVICES_H__ */
