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
 * @file    nlog.h
 * @brief   nlog service h-file.
 * @details Defines nlog service interface.
 *
 * @addtogroup template
 * @details Defines the storage service interface.
 * @{
 */

#ifndef __NLOG2_H__
#define __NLOG2_H__


#include <stdint.h>
#include <stdarg.h>
#include "qoraal/common/rtclib.h"



#define DBG_MESSAGE_NLOG(severity, fmt_str, ...)            DBG_MESSAGE_T_REPORT(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_NLOG                 DBG_ASSERT_T

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/

#define NLOG_FACILITY_DEBUG             0
#define NLOG_FACILITY_SYSTEM            1
#define NLOG_FACILITY_EVENT             2
#define NLOG_FACILITY_ENGINE            4
#define NLOG_FACILITY_VBAT              5
#define NLOG_FACILITY_CONNECTIVITY      5
#define NLOG_FACILITY_UPDATES           6
#define NLOG_FACILITY_SERVICES          7
#define NLOG_FACILITY_SECONDARY         98
#define NLOG_FACILITY_TEST              99

#define NLOG_SEVERITY_NONE              (0x00)
#define NLOG_SEVERITY_ASSERT            (0x01)
#define NLOG_SEVERITY_ERROR             (0x02)
#define NLOG_SEVERITY_WARNING           (0x03)
#define NLOG_SEVERITY_REPORT            (0x04)
#define NLOG_SEVERITY_LOG               (0x05)
#define NLOG_SEVERITY_INFO              (0x06)


#define NLOG_INFO                       0
#define NLOG_ASSERT                     1

#define NLOG2_MAX_MESSAGE_SIZE          (2000)

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

#pragma pack(1)
typedef struct NLOG2_LOG_HEADER_S {
    uint32_t                this ;
    uint32_t                next ;
    uint32_t                previous ;
    uint32_t                flags ;
    uint32_t                record_type ;
    uint32_t                user_type ;
    uint32_t                id ;
    uint32_t                len ;

} NLOG2_LOG_HEADER_T ;
#pragma pack()

//typedef struct NLOG2_S * NLOG2_T ;
typedef struct NLOG2_ITERATOR_S {
    struct NLOG2_S *        plog ;
    uint16_t                sector ;
    uint32_t                type ;
    NLOG2_LOG_HEADER_T      header ;
} NLOG2_ITERATOR_T ;


typedef struct NLOG2_S {
    uint32_t                startaddr ;
    uint32_t                sectorcount ;
    uint32_t                sectorsize ;
    uint32_t                id ;
    NLOG2_ITERATOR_T        it ;
} NLOG2_T ;


#pragma pack(1)
typedef struct NLOG2_LOG_INFO_S {
    uint32_t                number ;
    RTCLIB_DATE_T           date ;
    RTCLIB_TIME_T           time ;
    uint16_t                facillity ;
    uint16_t                severity ;
    char                    msg[] ;
} NLOG2_LOG_INFO_T ;
#pragma pack()

#define NLOG2_LOG_DATA(start, count, sectorsize)        {start, count, sectorsize}
#define NLOG2_LOG_DECL(name, start, count, sectorsize)  NLOG2_T     name = {start, count, sectorsize}


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t             nlog2_init (NLOG2_T * plog);
    int32_t             nlog2_reset (NLOG2_T * plog) ;
    uint32_t            nlog2_get_id (NLOG2_T * plog) ;

    int32_t             nlog2_append (NLOG2_T * plog, uint32_t type, const char* data, uint32_t len) ;

    int32_t             nlog2_iterator_init (NLOG2_T * plog, uint32_t type, NLOG2_ITERATOR_T *it) ;
    int32_t             nlog2_iterator_prev (NLOG2_ITERATOR_T *it) ;
    int32_t             nlog2_iterator_next (NLOG2_ITERATOR_T *it) ;
    int32_t             nlog2_iterator_read (NLOG2_ITERATOR_T *it, char* data, int32_t len) ;
    uint32_t            nlog2_iterator_id (NLOG2_ITERATOR_T *it) ;

#ifdef __cplusplus
}
#endif


#endif /* __NLOG2_H__ */
