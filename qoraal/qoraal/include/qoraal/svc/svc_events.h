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

/**
 * @file    svc_events.h
 * @brief   Events dispatcher in high priority thread from any context.
 * @details
 *
 * @addtogroup
 * @{
 */

#ifndef __SVC_EVENTS_H__
#define __SVC_EVENTS_H__

#include <stdint.h>
#include "qoraal/config.h"

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

typedef uint32_t SVC_EVENTS_T;
typedef enum {
   SVC_EVENTS_TASK,
   SVC_EVENTS_USER,
   SVC_EVENTS_LAST = 31
 } SVC_EVENTS ;


 /*===========================================================================*/
 /* Module data structures and types.                                         */
 /*===========================================================================*/

#define SVC_EVENTS_ALL        ((1<<SVC_EVENTS_LAST) - 1)

typedef void (*SVC_EVENTS_CALLBACK_T) (SVC_EVENTS_T /*id*/, void * ctx) ;
typedef struct SVC_EVENTS_HANDLER_S {
    struct SVC_EVENTS_HANDLER_S * next ;
    SVC_EVENTS_CALLBACK_T fp ;
    void * ctx ;
} SVC_EVENTS_HANDLER_T ;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t                 svc_events_init (void) ;
    int32_t                 svc_events_start (void) ;
    void                    svc_events_stop (void) ;

    void                    svc_events_register (SVC_EVENTS_T id,  SVC_EVENTS_HANDLER_T * handler, SVC_EVENTS_CALLBACK_T fp, void * ctx) ;
    void                    svc_events_unregister (SVC_EVENTS_T id,  SVC_EVENTS_HANDLER_T * handler) ;

    void                    svc_events_signal_isr (SVC_EVENTS_T id) ;
    void                    svc_events_signal (SVC_EVENTS_T id) ;

    void                    svc_events_handle (void) ;

#ifdef __cplusplus
}
#endif

#endif /* __SVC_EVENTS_H__ */
/** @} */
