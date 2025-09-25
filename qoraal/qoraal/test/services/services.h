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


#ifndef __MAIN_H__
#define __MAIN_H__

#include "qoraal/svc/svc_services.h"


/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#define DBG_MESSAGE_SERVICES(severity, fmt_str, ...)   DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)

typedef enum  {
    QORAAL_SERVICE_SYSTEM = SVC_SERVICES_USER,
    QORAAL_SERVICE_DEMO,
    QORAAL_SERVICE_SHELL,
	QORAAL_SERVICE_LAST,
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

    extern int32_t      demo_service_ctrl (uint32_t code, uintptr_t arg) ;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
