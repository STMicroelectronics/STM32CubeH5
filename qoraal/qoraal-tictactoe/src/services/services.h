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


#ifndef __SERVICES_H__
#define __SERVICES_H__

#include "qoraal/svc/svc_services.h"
#include "qoraal-engine/starter.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#define ENGINE_STARTUP_MACHINE      "tictactoe.e"

#define DBG_MESSAGE_SERVICES(severity, fmt_str, ...)   DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)


typedef enum  {
    QORAAL_SERVICE_SHELL = SVC_SERVICES_USER,
    QORAAL_SERVICE_WWW,
    QORAAL_SERVICE_ENGINE
} QORAAL_SERVICES ;

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t      system_service_ctrl (uint32_t code, uintptr_t arg) ;
    extern int32_t      system_service_run (uintptr_t arg) ;

    extern int32_t      engine_service_ctrl (uint32_t code, uintptr_t arg) ;
    extern int32_t      engine_service_run (uintptr_t arg) ;

    extern int32_t      engine_machine_start (const char *filename, void* ctx, STARTER_OUT_FP log_cb, bool start, bool verbose) ;
    extern void         engine_machine_stop (void) ;

#ifdef __cplusplus
}
#endif

#endif /* __SERVICES_H__ */
