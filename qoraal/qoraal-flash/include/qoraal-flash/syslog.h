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



#ifndef __LOG_H__
#define __LOG_H__


#include <stdint.h>
#include <stdarg.h>
#include "qoraal/svc/svc_logger.h"
#include "qoraal/common/rtclib.h"
#include "qoraal/common/logit.h"
#include "qoraal-flash/nvram/nlog2.h"

#define DBG_MESSAGE_SYSLOG                                DBG_MESSAGE_T_LOG
#define DBG_ASSERT_SYSLOG                                 DBG_ASSERT_T

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

                    



#define SYSLOG_SEVERITY_NEVER               (SVC_LOGGER_SEVERITY_NEVER)
#define SYSLOG_SEVERITY_ASSERT              (SVC_LOGGER_SEVERITY_ASSERT)
#define SYSLOG_SEVERITY_ERROR               (SVC_LOGGER_SEVERITY_ERROR)
#define SYSLOG_SEVERITY_WARNING             (SVC_LOGGER_SEVERITY_WARNING)
#define SYSLOG_SEVERITY_REPORT              (SVC_LOGGER_SEVERITY_REPORT)
#define SYSLOG_SEVERITY_LOG                 (SVC_LOGGER_SEVERITY_LOG)
#define SYSLOG_SEVERITY_INFO                (SVC_LOGGER_SEVERITY_INFO)
#define SYSLOG_SEVERITY_DEBUG               (SVC_LOGGER_SEVERITY_DEBUG)


#define SYSLOGLOG_MAX_MSG_SIZE               (200)

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

#define SYSLOG_ITERATOR_T                   NLOG2_ITERATOR_T

#define SYSLOG_LOG_MAX                      2
typedef struct {
    NLOG2_T  log[SYSLOG_LOG_MAX] ;    
} SYSLOG_INSTANCE_T ;

#define SYSLOG_INST_DECL(name, start, info_sector_count, info_sector_size, assert_sector_count, assert_sector_size) \
    static SYSLOG_INSTANCE_T name = { \
        {  \
            NLOG2_LOG_DATA(start, info_sector_count, info_sector_size), \
            NLOG2_LOG_DATA(start+info_sector_count*info_sector_size, assert_sector_count, assert_sector_size) \
        }  \
    };

#define SYSLOG_INFO_LOG                     0
#define SYSLOG_ASSERT_LOG                   1

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t             syslog_init (SYSLOG_INSTANCE_T * inst);
    int32_t             syslog_start (void);
    int32_t             syslog_stop (void) ;
    int32_t             syslog_reset (uint32_t idx) ;
    void                syslog_append (uint32_t idx, uint16_t facillity, uint16_t severity, const char* msg) ;
    void                syslog_append_fmtstr (uint32_t idx, uint16_t facillity, uint16_t severity, const char* format, ...) ;
    void                syslog_vappend_fmtstr (int32_t idx, int16_t facillity, int16_t severity, const char* format, va_list   args) ;

    int32_t             syslog_iterator_init (uint32_t idx, uint16_t severity, SYSLOG_ITERATOR_T *it) ;
    int32_t             syslog_iterator_prev (SYSLOG_ITERATOR_T *it) ;
    int32_t             syslog_iterator_next (SYSLOG_ITERATOR_T *it) ;
    int32_t             syslog_iterator_read (SYSLOG_ITERATOR_T *it, QORAAL_LOG_MSG_T *msg, uint32_t len) ;

    QORAAL_LOG_IT_T *   syslog_platform_it_create (uint32_t idx) ;
    void                syslog_platform_it_destroy (QORAAL_LOG_IT_T * it) ;

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
