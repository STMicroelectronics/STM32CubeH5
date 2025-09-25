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


#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include "qoraal-http/qoraal.h"
#include "qoraal-http/httpparse.h"

/*===========================================================================*/
/* Client constants.                                                         */
/*===========================================================================*/

/**
 * @name    Error Codes httpserver
 * @{
 */
#define HTTP_SERVER_E_OK                                     0
#define HTTP_SERVER_E_ERROR                                 -1 /* A_ERROR */
#define HTTP_SERVER_E_CONNECTION                            -2 /* A_INVALID */
#define HTTP_SERVER_E_HEADER                                -3
#define HTTP_SERVER_E_CONTENT                               -4
#define HTTP_SERVER_E_LENGTH                                -5
#define HTTP_SERVER_E_MEMORY                                -6
#define HTTP_SERVER_WSERVER_E_OK                            HTTP_SERVER_E_OK
#define HTTP_SERVER_WSERVER_E_METHOD                        HTTP_SERVER_E_ERROR
/** @} */

/**
 * @name    HTTP Methods
 * @{
 */
#define HTTP_HEADER_METHOD_GET                              1
#define HTTP_HEADER_METHOD_HEAD                             2
#define HTTP_HEADER_METHOD_POST                             3
#define HTTP_HEADER_METHOD_PUT                              4
#define HTTP_HEADER_METHOD_DELETE                           5
#define HTTP_HEADER_METHOD_CONNECT                          6
#define HTTP_HEADER_METHOD_OPTIONS                          7
#define HTTP_HEADER_METHOD_TRACE                            8
#define HTTP_HEADER_METHOD_UNDEFINED                        -1
/** @} */


/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    Debug Level
 * @{
 */
#define DBG_MESSAGE_HTTP_SERVER(severity, fmt_str, ...)     DBG_MESSAGE_T_LOG(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_HTTP_SERVER                              DBG_ASSERT_T
#define DBG_CHECK_HTTP_SERVER                               DBG_ASSERT_T
/** @} */

/**
 * @name    Max Buffer Sizes
 * @{
 */
#define HTTP_SERVER_MAX_XMIT_CONTENT_LENGTH                 (1024*4) /* MAX_ETHERNET_FRAME_SIZE */ // (MAX_ETHERNET_FRAME_SIZE )// 1280 // IPV4_FRAGMENTATION_THRESHOLD
#define MAX_IPv4_STR_LEN                                    32 /* a.b.c.d:port */
#define HTTP_SERVER_MAX_CHUNK_LENGTH                        HTTP_SERVER_MAX_XMIT_CONTENT_LENGTH
/** @} */

/**
 * @name    Content Types
 * @{
 */
#define HTTP_SERVER_CONTENT_TYPE_WAVE                       "audio/wav"
#define HTTP_SERVER_CONTENT_TYPE_TEXT                       "text"
#define HTTP_SERVER_CONTENT_TYPE_HTML                       "text/html"
#define HTTP_SERVER_CONTENT_TYPE_CSS                        "text/css"
#define HTTP_SERVER_CONTENT_TYPE_JSON                       "application/json"
#define HTTP_SERVER_CONTENT_TYPE_IMAGE                      "image/png"
#define HTTP_SERVER_CONTENT_TYPE_ICON                       "image/x-icon"
#define HTTP_SERVER_CONTENT_TYPE_EVENT_STREAM               "text/event-stream"
/** @} */

/**
 * @name    Content Types
 * @{
 */
#define HTTP_SERVER_NAME                                    "Qoraal"
/** @} */


/**
 * @name    Listen backlog: If a connection request arrives while the queue is
 *          full, error code ECONNREFUSED is returned to the client.
 * @{
 */
#define HTTP_SERVER_LISTEN_BACKLOG                          (2)
/** @} */

/**
 * @name    Malloc
 * @{
 */
#define HTTP_SERVER_MALLOC(size)                            qoraal_malloc (QORAAL_HeapAuxiliary, size) ;
#define HTTP_SERVER_FREE(mem)                               qoraal_free (QORAAL_HeapAuxiliary, mem) ;
#define HTTP_SERVER_USER_THREAD_SIZE                        12000
/** @} */


/**
 * @name    close_wait: Time in micro-seconds for TCP sockets before closing.
 * @details         Can be used if linger time is not zero.
 * @{
 */
//#define WSERVER_CLOSE_WAIT_TIME                           (60)
/** @} */


/**
 * @name    Timeout for request after client connected. The socket will be
 *          closed if no request was received after this timeout.
 * @{
 */
#define HTTP_SERVER_KEEPALIVE_HEADER                            "timeout=120"
/** @} */

/**
 * @name    Timeout for request after client connected. The socket will be
 *          closed if no request was received after this timeout.
 * @{
 */
//#define WSERVER_COMMAND_KEEPALIVE_TIMEOUT                             10000
/** @} */

#define HTTP_SERVER_USER_SCRATCH                            512

/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/
/**
 * @brief User Instance.
 */
typedef struct HTTP_USER_S {
    int32_t                 socket ;
    struct sockaddr_in      address ;
    uint32_t                timeout ;

    char*                   rw_buffer ;

    int32_t                 write_idx ;
    int32_t                 send_bytes ;
    uint32_t                read ;
    uint32_t                write ;
    HTTP_HEADER_T           headers[8] ;

    char*                   endpoint ;

    char*                   payload ;
    uint32_t                payload_length ;
    int32_t                 content_length ;

    char*                   content ;
    
#if !defined CFG_HTTPSERVER_TLS_DISABLE
    void*                  ssl ;
#endif

} HTTP_USER_T ;
/** @} */


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

extern int          httpserver_init (uint16_t port) ;
extern int32_t      httpserver_close (int server_sock) ;
extern int32_t      httpserver_user_init (HTTP_USER_T* user) ;

extern int32_t      httpserver_listen (int server_sock) ;
extern int32_t      httpserver_select (int server_sock, uint32_t timeout) ;
extern int32_t      httpserver_user_accept (int server_sock, HTTP_USER_T* user, uint32_t timeout) ;
extern int32_t      httpserver_user_ssl_accept (HTTP_USER_T* user, uint32_t timeout) ;
extern int32_t      httpserver_user_select (HTTP_USER_T* user, uint32_t timeout) ;
extern int32_t      httpserver_user_close (HTTP_USER_T* user) ;

extern int32_t      httpserver_read_request_ex (HTTP_USER_T* user, uint32_t timeout, char** endpoint, int32_t* method) ;
extern const char*  httpserver_get_authorization_header (HTTP_USER_T* user) ;
extern int32_t      httpserver_read_content_ex (HTTP_USER_T* user, uint32_t timeout, char** request) ;
extern int32_t      httpserver_read_all_content_ex (HTTP_USER_T* user, uint32_t timeout, char** request) ;
extern int32_t      httpserver_free_request (HTTP_USER_T* user) ;
extern int32_t      httpserver_wait_close (HTTP_USER_T* user, uint32_t timeout) ;
extern int32_t      httpserver_write_response (HTTP_USER_T* user, uint32_t result, const char* content_type, HTTP_HEADER_T* headers, uint32_t headers_count, const char* content, uint32_t length) ;

extern int32_t      httpserver_chunked_response (HTTP_USER_T* user, uint32_t result, const char* content_type, const HTTP_HEADER_T* headers, uint32_t headers_count) ;
extern int32_t      httpserver_chunked_append_fmtstr (HTTP_USER_T* user, const char* format_str, ...) ;
extern int32_t      httpserver_chunked_vappend_fmtstr (HTTP_USER_T* user, const char* format_str,  va_list  args) ;
extern int32_t      httpserver_chunked_append_str (HTTP_USER_T* user, const char* str, uint32_t len) ;
extern int32_t      httpserver_chunked_flush (HTTP_USER_T* user) ;
extern int32_t      httpserver_chunked_complete (HTTP_USER_T* user) ;



#ifdef __cplusplus
}
#endif

#endif /* __HTTPSERVER_H__ */

