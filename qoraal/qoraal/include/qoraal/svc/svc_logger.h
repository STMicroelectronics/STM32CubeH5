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


#ifndef __SVC_LOGGER_H__
#define __SVC_LOGGER_H__

#include <stdint.h>
#include <stdarg.h>
#include "qoraal/svc/svc_tasks.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#ifndef SVC_LOGGER_APPEND_CRLF
#define SVC_LOGGER_APPEND_CRLF                      0
#endif
#ifndef SVC_LOGGER_APPEND_TIMESTAMP
#define SVC_LOGGER_APPEND_TIMESTAMP                 1
#endif
#ifndef SVC_LOGGER_MAX_QUEUE_SIZE
#define SVC_LOGGER_MAX_QUEUE_SIZE                   16
#endif


/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

#define SVC_LOGGER_SEVERITY_NEVER                   (0x00)
#define SVC_LOGGER_SEVERITY_ASSERT                  (0x01)
#define SVC_LOGGER_SEVERITY_ERROR                   (0x02)
#define SVC_LOGGER_SEVERITY_WARNING                 (0x03)
#define SVC_LOGGER_SEVERITY_REPORT                  (0x04)
#define SVC_LOGGER_SEVERITY_LOG                     (0x05)
#define SVC_LOGGER_SEVERITY_INFO                    (0x06)
#define SVC_LOGGER_SEVERITY_DEBUG                   (0x07)

#define SVC_LOGGER_FLAGS_NO_FORMATTING              (0x01<<4)
#define SVC_LOGGER_FLAGS_NO_TIMESTAMP               (0x01<<5)
#define SVC_LOGGER_FLAGS_PROGRESS                   (0x01<<6)

#define SVC_LOGGER_SEVERITY_MASK                    (0x0F)
#define SVC_LOGGER_FLAGS_MASK                       (0xF0)
#define SVC_LOGGER_MASK                             ((LOGGERT_MASK_T)-1)

#define SVC_LOGGER_GET_SEVERITY(type)               (type & SVC_LOGGER_SEVERITY_MASK)
#define SVC_LOGGER_GET_FLAGS(type)                  (type & SVC_LOGGER_FLAGS_MASK)

#define SVC_LOGGER_SET_SEVERITY(type, severity)     do { \
                                                    type &= ~SVC_LOGGER_SEVERITY_MASK; \
                                                    type |= (severity) ; \
                                                    } while (0)
#define SVC_LOGGER_SET_FLAGS(type, flags)           do { \
                                                    type &= ~SVC_LOGGER_FLAGS_MASK; \
                                                    type |= (flags) ; \
                                                    } while (0)

#define SVC_LOGGER_TYPE(severity, flags)            ((severity) | (flags))

#define SVC_LOGGER_FACILITY_MASK(facility)          ((LOGGERT_MASK_T)1<<facility)

#define SVC_LOGGER_WOULD_LOG(filter, type, facility)  \
                        (   \
                        (SVC_LOGGER_GET_SEVERITY(type) <= SVC_LOGGER_GET_SEVERITY(filter.type)) && \
                        (!facility || (SVC_LOGGER_FACILITY_MASK(facility) & filter.mask)) \
                        )

/*
 * For a log channel, two filters are provided to filter facilities to a severity.
 */
#define SVC_LOGGER_FILTER_CNT                       2

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

typedef uint64_t    LOGGERT_MASK_T ;
typedef uint8_t     LOGGER_TYPE_T ;

typedef void (*LOGGER_CHANNEL_FP)(void* channel, LOGGER_TYPE_T /*type*/, uint8_t /*facility*/, const char* /*msg*/) ;

#pragma pack(1)
typedef struct LOGGGER_CHANNEL_FILTER_S {
    LOGGERT_MASK_T              mask ;
    LOGGER_TYPE_T              type ;
} LOGGGER_CHANNEL_FILTER_T ;
#pragma pack()

typedef struct LOGGER_CHANNEL_S {
    struct LOGGER_CHANNEL_S *   next ;
    LOGGER_CHANNEL_FP           fp ;
    LOGGGER_CHANNEL_FILTER_T    filter[SVC_LOGGER_FILTER_CNT] ;
    void*                       user ;
} LOGGER_CHANNEL_T ;



/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t          svc_logger_init (SVC_TASK_PRIO_T  prio) ;
    extern int32_t          svc_logger_start (void) ;

    extern uint32_t         svc_logger_would_log (LOGGER_TYPE_T type, uint8_t facility) ;
    extern int32_t          svc_logger_type_log (LOGGER_TYPE_T type, uint8_t facility, const char *str, ...) ;
    extern int32_t          svc_logger_type_vlog (LOGGER_TYPE_T type, uint8_t facility, const char *format_str, va_list args) ;
    extern int32_t          svc_logger_type_mem (LOGGER_TYPE_T type, uint8_t facility, const char* mem, uint32_t size, const char * head, const char * tail) ;
    
    extern int32_t          svc_logger_printf (const char *format_str, ...) ;
    extern int32_t          svc_logger_vprintf (const char *format_str, va_list args) ;
    extern int32_t          svc_logger_put (const char *str, uint32_t len) ;
    extern int32_t          svc_logger_vlog_state (int inst, const char *format_str, va_list args) ;

    extern void             svc_logger_set_mem_filter (LOGGGER_CHANNEL_FILTER_T filter) ;

    extern void             svc_logger_channel_add (LOGGER_CHANNEL_T * channel) ;
    extern void             svc_logger_channel_remove (LOGGER_CHANNEL_T * channel) ;

    extern int32_t          svc_logger_wait (uint32_t timeout) ;
    extern int32_t          svc_logger_wait_all (uint32_t timeout) ;

    extern const char *     svs_logger_severity_str (LOGGER_TYPE_T type) ;

    extern LOGGGER_CHANNEL_FILTER_T svs_logger_get_filter (void) ;

#ifdef __cplusplus
}
#endif


#endif /* __SVC_LOGGER_H__ */
