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
 * @file        wserver.h
 * @author      Natie van Rooyen <natie@navaro.nl>
 * @date        January 1, 2015
 * @version     0.0.0.1 (alpha)
 *
 * @section DESCRIPTION
 *
 * Simple web server for provisioning.
 */

#ifndef __WSERVER_INST_H__
#define __WSERVER_INST_H__


#include <stdint.h>
#include "qoraal-http/wserver.h"
#include "qoraal/svc/svc_services.h"
#include "../services.h"

/*===========================================================================*/
/* Client constants.                                                         */
/*===========================================================================*/

/**
 * @name    Error Codes
 * @{
 */
#define HTTP_WSERVER_E_OK                                EOK

/**
 * @name    Debug Level
 * @{
 */
#define DBG_MESSAGE_WWW(severity, fmt_str, ...)     DBG_MESSAGE_T(SVC_LOGGER_TYPE(severity,0), QORAAL_SERVICE_WWW, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_WWW                              DBG_ASSERT_T
/** @} */

/** @} */

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    HTML text
 * @{
 */
#define WSERVER_TITLE_TEXT      "Qoraal"

/** @} */


#define WSERVER_CHECK_SEND(x)   if ((x) < HTTP_WSERVER_E_OK) return -2

/** @} */





#define WSERVER_HTML_404    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>404 - Page Not Found</title></style></head><body><h1>404 - Page Not Found</h1><p>The page you're looking for is on a secret mission...</p></body></html>"

/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    extern int32_t          wserver_service_ctrl (uint32_t code, uintptr_t arg) ;
    extern int32_t          wserver_service_run (uintptr_t arg) ;

#ifdef __cplusplus
}
#endif

#endif /* __WSERVER_INST_H__ */

