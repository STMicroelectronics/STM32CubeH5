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

/*
 * memlog.h
 *
 *  Created on: 30 Jun 2015
 *      Author: natie
 */

#ifndef __MLOG_H__
#define __MLOG_H__

#include <stdint.h>
#include <stdbool.h>
#include "rtclib.h"
#include "logit.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#define MLOG_LOGS_BUFFERSIZE_MIN            512
#define MLOG_LOGS_MSG_SIZE_MAX              256

typedef enum {
    MLOG_DBG = 0,
    MLOG_ASSERT
} MLOG_TYPE_T ;


/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

    int32_t         mlog_init (uint32_t * log_buffer, uint32_t log_size, uint32_t *  assert_buffer, uint32_t assert_size) ;
    bool            mlog_started (void) ;
    int32_t         mlog_reset (MLOG_TYPE_T log) ;

    QORAAL_LOG_MSG_T* mlog_get (MLOG_TYPE_T log, uint16_t idx) ;

    int32_t         mlog_dbg (uint16_t type, uint16_t id, const char* msg, ...) ;
    int32_t         mlog_log (int16_t facillity,  int16_t severity, const char* msg, ...) ;
    int32_t         mlog_assert (const char* msg, ...) ;

    int32_t         mlog_total (uint16_t log) ;
    int32_t         mlog_count (uint16_t log, uint16_t type) ;
    void*           mlog_itertor_first (MLOG_TYPE_T log, uint16_t type) ;
    void*           mlog_itertor_last (MLOG_TYPE_T log, uint16_t type) ;
    void*           mlog_itertor_prev (MLOG_TYPE_T log, void * iterator, uint16_t type) ;
    void*           mlog_itertor_next (MLOG_TYPE_T log, void * iterator, uint16_t type) ;
    QORAAL_LOG_MSG_T*  mlog_itertor_get (MLOG_TYPE_T log, void * it) ;
    void            mlog_itertor_release (MLOG_TYPE_T log, void * it) ;

    QORAAL_LOG_IT_T * mlog_platform_it_create (MLOG_TYPE_T log) ;
    void            mlog_platform_it_destroy (QORAAL_LOG_IT_T * it) ;


#ifdef __cplusplus
}
#endif

#endif /* __MLOG_H__ */
