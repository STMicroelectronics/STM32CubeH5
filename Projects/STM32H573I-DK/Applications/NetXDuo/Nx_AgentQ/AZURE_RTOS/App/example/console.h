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

/**
 * @file        terminal.h
 * @brief       Qoraal POSIX Terminal Service
 * @details     This file implements a POSIX-based console service for the  
 *              Qoraal framework. It provides an interface to the Qoraal 
 *              shell, handling user input, executing commands, logging 
 *              messages, and managing the service lifecycle.  
 *
 * @author      Navaro
 * @version    
 * @date        
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "qoraal/svc/svc_services.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t      console_service_ctrl (uint32_t code, uintptr_t arg) ;
    extern int32_t      console_service_run (uintptr_t arg) ;
    extern void         console_wait_for_exit (SVC_SERVICES_T  id) ;

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_H__ */
