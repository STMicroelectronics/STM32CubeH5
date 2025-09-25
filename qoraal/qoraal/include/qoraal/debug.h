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


#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>
#include <stdarg.h>
#include "qoraal/config.h"
#include "qoraal/svc/svc_logger.h"

#define DBG_STATIC_ASSERT(condition) typedef char DBG_STATIC_ASSERT__LINE__[ (condition) ? 1 : -1];

#define DBG_MESSAGE_SEVERITY_NEVER                          (SVC_LOGGER_SEVERITY_NEVER)
#define DBG_MESSAGE_SEVERITY_ASSERT                         (SVC_LOGGER_SEVERITY_ASSERT)
#define DBG_MESSAGE_SEVERITY_ERROR                          (SVC_LOGGER_SEVERITY_ERROR)
#define DBG_MESSAGE_SEVERITY_WARNING                        (SVC_LOGGER_SEVERITY_WARNING)
#define DBG_MESSAGE_SEVERITY_REPORT                         (SVC_LOGGER_SEVERITY_REPORT)
#define DBG_MESSAGE_SEVERITY_LOG                            (SVC_LOGGER_SEVERITY_LOG)
#define DBG_MESSAGE_SEVERITY_INFO                           (SVC_LOGGER_SEVERITY_INFO)
#define DBG_MESSAGE_SEVERITY_DEBUG                          (SVC_LOGGER_SEVERITY_DEBUG)

#define DBG_MESSAGE_SEVERITY_MASK                           (SVC_LOGGER_SEVERITY_MASK)

#define DBG_MESSAGE_FLAG_NO_TIMESTAMP                       (SVC_LOGGER_FLAGS_NO_TIMESTAMP)
#define DBG_MESSAGE_FLAG_NO_FORMATTING                      (SVC_LOGGER_FLAGS_NO_FORMATTING)
#define DBG_MESSAGE_FLAG_PROGRESS                           (SVC_LOGGER_FLAGS_NO_TIMESTAMP|SVC_LOGGER_FLAGS_NO_FORMATTING|SVC_LOGGER_FLAGS_PROGRESS)

#define DBG_MESSAGE_GET_TYPE(severity, flags)               (SVC_LOGGER_TYPE(severity, flags))
#define DBG_MESSAGE_GET_SEVERITY(type)                      (SVC_LOGGER_GET_SEVERITY(type))

#ifdef NDEBUG

#define DBG_MESSAGE_T(type, fmt_str, ...)
#define DBG_MESSAGE_T_ERROR(type, fmt_str, ...)
#define DBG_MESSAGE_T_WARNING(type, fmt_str, ...)
#define DBG_MESSAGE_T_REPORT(type, fmt_str, ...)
#define DBG_MESSAGE_T_LOG(type, fmt_str, ...)
#define DBG_MESSAGE_T_DEBUG(type, fmt_str, ...)
#define DBG_MESSAGE_T_ASSERT(type, facility, fmt_str, ...)  {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_ASSERT)   svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_ASSERT_T(cond, fmt_str, ...) 					(void) (cond)
#define DBG_ASSERT_ISR_T(cond, fmt_str, ...)				(void) (cond)
#define DBG_CHECK_T(cond, ret, fmtstr, ...)                 { if (!(cond)) { return ret ; } }
#define DBG_CHECKV_T(cond, fmtstr, ...)                     { if (!(cond)) { return  ; } }

#else

#define DBG_ASSERT_T(cond, fmt_str, ...)                    {  if (!(cond)) { debug_assert(fmt_str, ##__VA_ARGS__)  ; }}
#define DBG_ASSERT_ISR_T(cond, fmt_str, ...)                {  if (!(cond)) { debug_assert(fmt_str, ##__VA_ARGS__)  ; }}

#if !defined CFG_DEBUG_SVC_LOGGER_DISABLE

#define DBG_MESSAGE_T(type, facility, fmt_str, ...)         {  svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_ERROR(type, facility, fmt_str, ...)   {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_ERROR)    svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_WARNING(type, facility, fmt_str, ...) {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_WARNING)  svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_REPORT(type, facility, fmt_str, ...)  {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_REPORT)   svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_LOG(type, facility, fmt_str, ...)     {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_LOG)      svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_DEBUG(type, facility, fmt_str, ...)   {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_DEBUG)    svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_ASSERT(type, facility, fmt_str, ...)  {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_ASSERT)   svc_logger_type_log(type, facility, fmt_str, ##__VA_ARGS__) ; }

#ifdef NDEBUG
#define DBG_CHECK_T(cond, ret, fmtstr, ...)                 { if (!(cond)) { return ret ; } }
#define DBG_CHECKV_T(cond, fmtstr, ...)                     { if (!(cond)) { return  ; } }
#else
#define DBG_CHECK_T(cond, ret, fmt_str, ...)                {  if (!(cond)) { svc_logger_type_log(DBG_MESSAGE_SEVERITY_ERROR , 0, fmt_str, ##__VA_ARGS__ ) ; return ret ; }}
#define DBG_CHECKV_T(cond, fmt_str, ...)                    {  if (!(cond)) { svc_logger_type_log(DBG_MESSAGE_SEVERITY_ERROR , 0, fmt_str, ##__VA_ARGS__ ) ; }}
#endif

#else

#define DBG_MESSAGE_T(type, facility, fmt_str, ...)         {  debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_ERROR(type, facility, fmt_str, ...)   {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_ERROR)    debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_WARNING(type, facility, fmt_str, ...) {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_WARNING)  debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_REPORT(type, facility, fmt_str, ...)  {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_REPORT)   debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_LOG(type, facility, fmt_str, ...)     {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_LOG)      debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_DEBUG(type, facility, fmt_str, ...)   {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_DEBUG)    debug_printf(fmt_str, ##__VA_ARGS__) ; }
#define DBG_MESSAGE_T_ASSERT(type, facility, fmt_str, ...)  {  if (DBG_MESSAGE_GET_SEVERITY((type)) <= DBG_MESSAGE_SEVERITY_ASSERT)   debug_printf(fmt_str, ##__VA_ARGS__) ; }

#ifdef NDEBUG
#define DBG_CHECK_T(cond, ret, fmtstr, ...)                 { if (!(cond)) { return ret ; } }
#define DBG_CHECKV_T(cond, fmtstr, ...)                     { if (!(cond)) { return  ; } }
#else
#define DBG_CHECK_T(cond, ret, fmt_str, ...)                {  if (!(cond)) {  debug_printf(fmt_str, ##__VA_ARGS__ ) ; return ret ; }}
#define DBG_CHECKV_T(cond, fmt_str, ...)                    {  if (!(cond)) {  debug_printf(fmt_str, ##__VA_ARGS__ ) ; }}
#endif

#endif

#endif

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    extern int              debug_printf (const char *format, ...) ;
    extern void             debug_assert (const char *format, ...)  ;

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H__ */
