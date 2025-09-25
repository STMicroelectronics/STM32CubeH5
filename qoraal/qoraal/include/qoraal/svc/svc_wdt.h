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


#ifndef __SVC_WDT_H__
#define __SVC_WDT_H__

#include <stdint.h>
#include <stdbool.h>
#include "qoraal/config.h"
#include "qoraal/os.h"

#define DBG_MESSAGE_SVC_WDT(severity, fmt_str, ...)    DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_SVC_WDT                               DBG_ASSERT_T


/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

typedef enum SVC_WDT_TIMEOUTS_E {

    TIMEOUT_10_SEC=0,
    TIMEOUT_30_SEC,
    TIMEOUT_60_SEC,

    TIMEOUT_LAST



 } SVC_WDT_TIMEOUTS_T ;

 /*===========================================================================*/
 /* Module data structures and types.                                         */
 /*===========================================================================*/

typedef void (*SVC_WDY_KICK_CB_T)(void) ;



typedef struct SVC_WDT_HANDLE_S {
    struct SVC_EVENTS_HANDLER_S * next ;
    p_thread_t      thread ;
    uintptr_t       id ;
    uint32_t        flags ;
} SVC_WDT_HANDLE_T ;


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t                 svc_wdt_init (void) ;
    int32_t                 svc_wdt_start (void) ;
    void                    svc_wdt_stop (void) ;

    void                    svc_wdt_register (SVC_WDT_HANDLE_T * handler, SVC_WDT_TIMEOUTS_T id) ;
    void                    svc_wdt_unregister (SVC_WDT_HANDLE_T * handler, SVC_WDT_TIMEOUTS_T id) ;

    void                    svc_wdt_activate (SVC_WDT_HANDLE_T * handler) ;
    void                    svc_wdt_set_id (SVC_WDT_HANDLE_T * handler, uintptr_t id) ;
    void                    svc_wdt_deactivate (SVC_WDT_HANDLE_T * handler) ;
    void                    svc_wdt_handler_kick (SVC_WDT_HANDLE_T * handler) ;

    void                    svc_wdt_kick (void) ;

#ifdef __cplusplus
}
#endif


#endif /* __SVC_EVENTS_H__ */
/** @} */
