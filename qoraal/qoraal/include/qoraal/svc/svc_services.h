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


#ifndef __SVC_SERVICES_H__
#define __SVC_SERVICES_H__

#include <stdint.h>
#include "svc_threads.h"

#define DBG_MESSAGE_SVC_SERVICES(severity, fmt_str, ...)    DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_SVC_SERVICES                             DBG_ASSERT_T

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

#define SVC_SERVICE_STATUS_DISABLED         -1
#define SVC_SERVICE_STATUS_STOPPED          0
#define SVC_SERVICE_STATUS_STOPPING         1
#define SVC_SERVICE_STATUS_STARTING         2
#define SVC_SERVICE_STATUS_STARTED          3
#define SVC_SERVICE_STATUS_RESIDENT         4

#define SVC_SERVICE_FLAGS_AUTOSTART         1

typedef int SVC_SERVICES_T ;
typedef enum  {
   SVC_SERVICES_INVALID = 0,
   SVC_SERVICES_USER,

 } SVC_SERVICES ;

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef uint32_t                        SCV_SERVICE_HANDLE ;

#define SVC_SERVICE_INVALID_HANDLE      0xFFFFFFFF

typedef int32_t (*SVC_SERVICE_RUN_FP)(uintptr_t /*parm*/) ;
typedef int32_t (*SVC_SERVICE_CTRL_FP)(uint32_t /*code*/, uintptr_t /*parm*/) ;
typedef int32_t (*SVC_SERVICE_COMPLETE_CB)(SVC_SERVICES_T /* id */, uintptr_t /*cb_parm*/) ;

#define SVC_SERVICE_CTRL_OFFSET         1000
#define SVC_SERVICE_CTRL_USER           2000

#define SVC_SERVICE_CTRL_INIT           (1+SVC_SERVICE_CTRL_OFFSET)
#define SVC_SERVICE_CTRL_START          (2+SVC_SERVICE_CTRL_OFFSET)
#define SVC_SERVICE_CTRL_STOP           (3+SVC_SERVICE_CTRL_OFFSET)
#define SVC_SERVICE_CTRL_STATUS         (4+SVC_SERVICE_CTRL_OFFSET)
#define SVC_SERVICE_CTRL_HALT           (5+SVC_SERVICE_CTRL_OFFSET)


#define SVC_SERVICE_FLAGS_AUTOSTART             1
#define SVC_SERVICE_FLAGS_DBGSTART              2

typedef struct SVC_SERVICE_S {

    SVC_THREADS_T               thd ;
    SVC_SERVICE_COMPLETE_CB     cb ;
    uintptr_t                   cb_parm ;

    SVC_SERVICE_RUN_FP          run ;
    SVC_SERVICE_CTRL_FP         ctrl ;

    uintptr_t                   parm ;
    uint16_t                    stack ;
    uint16_t                    prio ;

    int32_t                     status ;
    int32_t                     exit_status ;

    SVC_SERVICES_T              service ;
    uint32_t                    flags ;
    const char*                 name ;

} SVC_SERVICE_T ;


#define SVC_SERVICE_LIST_START(name) SVC_SERVICE_T name[] = {
#define SVC_SERVICE_LIST_END() \
        {_SVC_THREADS_DATA, 0, 0, 0, 0, 0, 0, 0, \
        0, 0, SVC_SERVICES_INVALID, 0, 0} };

#define SVC_SERVICE_RUN_DECL(name, fprun, fpctrl, parm, stack, prio, service, flags) \
        {_SVC_THREADS_DATA, 0, 0, fprun, fpctrl, parm, stack, prio, \
        SVC_SERVICE_STATUS_STOPPED, EOK, service, flags, name},

#define SVC_SERVICE_DECL(name, fpctrl, parm, service, flags) \
        {_SVC_THREADS_DATA, 0, 0, 0, fpctrl, parm, 0, 0, \
        SVC_SERVICE_STATUS_STOPPED, EOK, service, flags, name},

typedef struct CORAL_SERVICE_S * PSVC_SERVICE_T ;

typedef void (*SVC_SERVICE_CALLBACK_T) (SVC_SERVICES_T /* id */, int32_t /*status*/, uintptr_t) ;
typedef struct SVC_SERVICE_HANDLER_S {
    struct SVC_SERVICE_HANDLER_S * next ;
    SVC_SERVICE_CALLBACK_T fp ;
    uintptr_t parm ;
} SVC_SERVICE_HANDLER_T ;


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t                  svc_service_services_init (SVC_SERVICE_T * list) ;
    extern int32_t                  svc_service_services_start (uint32_t startflags) ;
    extern void                     svc_service_register_handler (SVC_SERVICE_HANDLER_T * handler, SVC_SERVICE_CALLBACK_T fp, uintptr_t parm) ;
    extern void                     svc_service_unregister_handler (SVC_SERVICE_HANDLER_T * handler) ;

    extern SCV_SERVICE_HANDLE       svc_service_get (SVC_SERVICES_T id) ;
    extern SCV_SERVICE_HANDLE       svc_service_get_by_name (const char * name) ;
    extern SCV_SERVICE_HANDLE       svc_service_first (void) ;
    extern SCV_SERVICE_HANDLE       svc_service_next (SCV_SERVICE_HANDLE handle) ;
    extern SVC_SERVICES_T           svc_service_service (SCV_SERVICE_HANDLE handle) ;

    extern int32_t                  svc_service_start (SCV_SERVICE_HANDLE handle, uintptr_t arg, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm) ;
    extern int32_t                  svc_service_start_timeout (SCV_SERVICE_HANDLE handle, uintptr_t arg, uint32_t timeout) ;
    extern int32_t                  svc_service_stop (SCV_SERVICE_HANDLE handle, SVC_SERVICE_COMPLETE_CB cb, uintptr_t cb_parm) ;
    extern int32_t                  svc_service_stop_timeout (SCV_SERVICE_HANDLE handle, uint32_t timeout) ;
    extern int32_t                  svc_service_ctrl (SCV_SERVICE_HANDLE handle, uint32_t cmd, uintptr_t parm) ;
    extern void                     svc_service_ctrl_broadcast (uint32_t cmd, uintptr_t parm) ;

    extern int32_t                  svc_service_status (SCV_SERVICE_HANDLE handle) ;
    extern uint32_t                 svc_service_get_arg (SCV_SERVICE_HANDLE handle) ;
    extern uint32_t                 svc_service_get_flags (SCV_SERVICE_HANDLE handle) ;
    extern int32_t                  svc_service_exit_status (SCV_SERVICE_HANDLE handle) ;
    extern const char*              svc_service_name (SCV_SERVICE_HANDLE handle) ;
    extern const char*              svc_service_status_name (SCV_SERVICE_HANDLE handle) ;
    extern const char*              svc_service_status_description (SCV_SERVICE_HANDLE handle) ;

    void                            svc_service_system_halt (void) ;

#ifdef __cplusplus
}
#endif


#endif /* __SVC_SERVICES_H__ */
/** @} */
