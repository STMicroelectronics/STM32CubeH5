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


#ifndef __WSERVER_H__
#define __WSERVER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "qoraal-http/httpserver.h"


/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Pre-compile time settings.                                                */
/*===========================================================================*/


/**
 * @name    WServer Responses
 * @{
 */
#if 1
#define WSERVER_RESP_CONTENT_400    "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>400 Bad Request</title></head><body><h1>Oops! Bad Request</h1><p>It seems your request was a bit, well, scrambled. Maybe double-check and give it another shot? 🥚</p><p><a href=\"/\">Back to Safety</a></p></body></html>"
#define WSERVER_RESP_CODE_400 400

#define WSERVER_RESP_CONTENT_401 "<html><head><title>401 Unauthorized</title></head><body><h1>401 Unauthorized</h1><p>Please log in to access this page.</p><a href=\"/\">Home</a></body></html>"
#define WSERVER_RESP_CODE_401 401

#define WSERVER_RESP_CONTENT_404 "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>404 Not Found... Oops!</title><style>body{font-family:Arial,sans-serif;color:#333;text-align:center;padding:50px;background-color:#f9f9f9;}h1{font-size:48px;margin-bottom:20px;}p{font-size:18px;line-height:1.6;}a{color:#007BFF;text-decoration:none;font-weight:bold;}a:hover{text-decoration:underline;}.joke{margin-top:40px;font-style:italic;}</style></head><body><h1>Uh-oh! Page Not Found</h1><p>It looks like you took a wrong turn at Albuquerque. The requested URL <code>/hrll</code> was not found on this server.</p><p>Maybe it ran away? Maybe it’s hiding? Either way, it’s not here. 🤷‍♂️</p><p><a href=\"/\">Go Home</a> and pretend this never happened. Or, try another path and hope for the best.</p><div class=\"joke\"><p>In the meantime, here’s a joke:</p><p><em>Why was the web page so sad?</em><br>Because it had too many broken links!</p></div></body></html>"
#define WSERVER_RESP_CODE_404 404

#define WSERVER_RESP_CONTENT_500 "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>500 Internal Server Error</title><style>body{font-family:Arial,sans-serif;color:#333;text-align:center;padding:50px;background-color:#fff3f3;}h1{font-size:48px;color:#e74c3c;margin-bottom:20px;}p{font-size:18px;line-height:1.6;}a{color:#007BFF;text-decoration:none;font-weight:bold;}a:hover{text-decoration:underline;}</style></head><body><h1>Oh No! Server Meltdown</h1><p>Looks like something went terribly wrong on our end. Our server might just be taking a coffee break ☕, or maybe it’s staging a silent protest. Either way, we’ll try to fix it soon!</p><p><a href=\"/\">Take Me Home</a> and let’s pretend this never happened.</p></body></html>"
#define WSERVER_RESP_CODE_500 500
#else

#define WSERVER_RESP_CONTENT_400 "<!DOCTYPE html><html><head><title>400 Bad Request.</title></head><body><h1>400 Bad Request.</h1></body></html>"
#define WSERVER_RESP_CODE_400 400

#define WSERVER_RESP_CONTENT_401 "<!DOCTYPE html><html><head><title>401 Unauthorized.</title></head><body><h1>>401 Unauthorized.</h1></body></html>"
#define WSERVER_RESP_CODE_401 401

#define WSERVER_RESP_CONTENT_404 "<!DOCTYPE html><html><head><title>404 Not Found.</title></head><body><h1>404 Not Found.</h1></body></html>"
#define WSERVER_RESP_CODE_404 404

#define WSERVER_RESP_CONTENT_500 "<!DOCTYPE html><html><head><title>500 Internal Server Error.</title></head><body><h1>500 Internal Server Error.</h1></body></html>"
#define WSERVER_RESP_CODE_500 500

#endif
/** @} */



/**
 * @name    Timeout for request after client connected. The socket will be
 *          closed if no request was received after this timeout.
 * @{
 */
#define WSERVER_KEEPALIVE_TIMEOUT                               285000
#define WSERVER_NO_KEEPALIVE_TIMEOUT                            4000

/** @} */

/**
 * @name    Timeout for request after client connected. The socket will be
 *          closed if no request was received after this timeout.
 * @{
 */
//#define WSERVER_COMMAND_KEEPALIVE_TIMEOUT                             10000
/** @} */

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef struct HTTPSERVER_INST_S   HTTPSERVER_INST_T ;      

/**
 * @brief Authentication handler.
 */
typedef uint32_t (*WSERVER_AUTHENTICATE)(const char * /*user*/, const char * /*passwd*/) ;

/**
 * @brief Embedded web server.
 */
typedef int32_t (*WSERVER_CONTENT) (HTTP_USER_T * /*user*/, uint32_t /*method*/, char* /*endpoint*/) ;
typedef const char* (*WSERVER_CTRL) (HTTP_USER_T * /*user*/, uint32_t /*method*/, char* /*endpoint*/, uint32_t /*type*/) ;
typedef int32_t (*WSERVER_FRAMEWORK) (HTTP_USER_T * /*user*/, uint32_t /*method*/, char* /*endpoint*/, WSERVER_CTRL /*ctrl*/) ;

typedef struct WSERVER_HANDLER_S    {

    const WSERVER_FRAMEWORK*        headers ;       // zero terminated array
    const WSERVER_FRAMEWORK*        footers ;       // zero terminated array
    WSERVER_CTRL                    ctrl ;
    WSERVER_CONTENT                 content ;
    const char*                     endpoint ;
    uint16_t                        endpoint_match_len ; // zero for exact match
    uint16_t                        endpoint_flags ; // 
    uint32_t                        access ; // 

} WSERVER_HANDLERS_T ;
/** @} */

#define WSERVER_CTRL_START                      0
#define WSERVER_CTRL_STOP                       1
#define WSERVER_CTRL_METADATA_HEADERS           2
#define WSERVER_CTRL_METADATA_HEADING           3

#define WSERVER_ENDPOINT_ACCESS_OPEN            0
#define WSERVER_ENDPOINT_ACCESS_USER            (1<<0)
#define WSERVER_ENDPOINT_ACCESS_ADMIN           (1<<1)

#define WSERVER_ENDPOINT_FLAGS_DEFAULT          (1<<0)
#define WSERVER_ENDPOINT_FLAGS_DISABLE_WDT      (1<<1)

#define WSERVER_HANDLERS_START(name) \
        const WSERVER_HANDLERS_T name[] = {
#define WSERVER_FRAMEWORK_HANDLER(ep, handler, access, flags, ctrl, framework) \
        {framework##_headers, framework##_footers, ctrl, handler, ep, sizeof(ep) - 1, flags, access},
#define WSERVER_CTRL_HANDLER(ep, handler, access, flags, ctrl) \
        {0, 0, ctrl, handler, ep, sizeof(ep) - 1, flags, access},
#define WSERVER_HANDLER(ep, handler, access, flags) \
        {0, 0, 0, handler, ep, sizeof(ep) - 1, flags, access},
#define WSERVER_HANDLERS_END() \
        {0, 0, 0, 0, 0, 0, 0, 0} } ;


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

        extern HTTPSERVER_INST_T *      httpserver_wserver_create (uint32_t port, bool ssl, const WSERVER_HANDLERS_T* handlers, WSERVER_AUTHENTICATE authenticate) ;
        extern void                     httpserver_wserver_destroy (HTTPSERVER_INST_T * inst) ;
        extern int32_t                  httpserver_wserver_run (HTTPSERVER_INST_T * inst) ;
        extern void                     httpserver_wserver_stop (HTTPSERVER_INST_T * inst) ;

#ifdef __cplusplus
}
#endif

#endif /* __HTTPSERVER_H__ */

