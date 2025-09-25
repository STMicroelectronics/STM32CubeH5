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


#ifndef __QORAAL_LOGIT_H__
#define __QORAAL_LOGIT_H__

#include <stdint.h>
#include "qoraal/common/rtclib.h"

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/
typedef struct  {
    RTCLIB_DATE_T   date ;
    RTCLIB_TIME_T   time ;
    uint16_t        cnt ;
    uint16_t        id ;
    union {
    uint16_t        type ;
    struct {
        uint8_t     severity ;
        uint8_t     facillity ;
    } ;
    } ;
    uint16_t        len ;
    char    msg [] ;
} QORAAL_LOG_MSG_T ;

typedef struct QORAAL_LOG_IT_S * PQORAAL_LOG_IT_T;
typedef int32_t (*QORAAL_LOG_IT_PREV_T)(struct QORAAL_LOG_IT_S *) ;
typedef int32_t (*QORAAL_LOG_IT_GET_T)(struct QORAAL_LOG_IT_S *, QORAAL_LOG_MSG_T *, uint32_t len) ;

typedef struct QORAAL_LOG_IT_S {
    QORAAL_LOG_IT_PREV_T prev ;
    QORAAL_LOG_IT_GET_T get ;
} QORAAL_LOG_IT_T ;

#endif /* __QORAAL_LOGIT_H__ */
