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



#ifndef __HTTPPARSE_H__
#define __HTTPPARSE_H__

#include <stdint.h>
#include <stddef.h>
//#include "errordef.h"
#define DBG_MESSAGE_HTTP_PARSE(severity, fmt_str, ...)      DBG_MESSAGE_T_LOG(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_HTTP_PARSE                               DBG_ASSERT_T


/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

#define HTTP_HEADER_KEY_CONTENT_TYPE            "Content-Type"
#define HTTP_HEADER_KEY_CONTENT_LENGTH          "Content-Length"
#define HTTP_HEADER_KEY_TRANSFER_ENCODING       "Transfer-Encoding"
//#define HTTP_HEADER_KEY_SERVER                    "Server"
#define HTTP_HEADER_KEY_SERVER                  "Allow"
#define HTTP_HEADER_KEY_CONNECTION              "Connection"
#define HTTP_HEADER_KEY_AUTHENTICATE            "WWW-Authenticate" // WWW-Authenticate: Basic realm="private area"
#define HTTP_HEADER_KEY_AUTHORIZATION           "Authorization" // WWW-Authenticate: Basic realm="private area"
#define HTTP_HEADER_KEY_CONTENT_DISPOSITION     "Content-Disposition"
#define HTTP_HEADER_KEY_FILE                    "file"
#define HTTP_HEADER_KEY_FILENAME                "filename"
#define HTTP_HEADER_KEY_WEBSOCK_UPGRADE         "Upgrade"
#define HTTP_HEADER_KEY_WEBSOCK_ACCEPT          "Sec-WebSocket-Accept"
#define HTTP_HEADER_KEY_WEBSOCK_PROT            "Sec-WebSocket-Protocol"





#define HTTP_HEADER_METHOD_GET                  1
#define HTTP_HEADER_METHOD_HEAD                 2
#define HTTP_HEADER_METHOD_POST                 3
#define HTTP_HEADER_METHOD_PUT                  4
#define HTTP_HEADER_METHOD_DELETE               5
#define HTTP_HEADER_METHOD_CONNECT              6
#define HTTP_HEADER_METHOD_OPTIONS              7
#define HTTP_HEADER_METHOD_TRACE                8
#define HTTP_HEADER_METHOD_UNDEFINED            -1

/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/


typedef struct HTTP_HEADER_S {
    const char*  key ;
    const char* value ;
}HTTP_HEADER_T;

#define HTTP_HEADER_DECL(name, key) const HTTP_HEADER_T name = { key, 0 } ;


#define HTTP_REQUEST_HEADER     "%s /%s HTTP/1.1"
#define HTTP_PARSE_REQUEST_METHODS  \
    { HTTP_HEADER_METHOD_GET, 3, "GET" }, \
    { HTTP_HEADER_METHOD_HEAD, 4, "HEAD" }, \
    { HTTP_HEADER_METHOD_POST, 4, "POST" }, \
    { HTTP_HEADER_METHOD_PUT, 3, "PUT" }, \
    { HTTP_HEADER_METHOD_DELETE, 6, "DELETE" }, \
    { HTTP_HEADER_METHOD_CONNECT, 7, "CONNECT" }

//  { HTTP_HEADER_METHOD_OPTIONS, 7, "OPTIONS" },
//  { HTTP_HEADER_METHOD_TRACE, 5, "TRACE" }

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    extern char *   strnstr(const char *s, const char *find, size_t slen) ;
    extern char *   strntrim(const char *p, size_t n) ;
    extern char *   strnchr(const char *p, char c, size_t n) ;
    extern int      strnicmp(const char *pStr1, const char *pStr2, size_t Count) ;

    extern char *   memstr(char *haystack, char *needle, int size) ;

    extern void     urldecode(char *dst, const char *src, uint32_t dstlen) ;
    extern int      urlencode( char *pstr, char * buf, int len ) ;
    extern int      htmlencode( char *pstr, char * buf, int len ) ;

    #define BASE64_OUTPUT_LENGTH(input_length)      (4 * ((input_length + 2) / 3))

    extern char*    base64_encode(const char *data, size_t input_length, char* encoded_data, size_t output_length) ;
    extern int      base64_decode(unsigned char *dest, const unsigned char *src, int srclen) ;

    extern char*    httpparse_headers(char* data, int len, HTTP_HEADER_T* headers, int count) ;
    extern int32_t  httpparse_response(char* data, int len, HTTP_HEADER_T* headers, int headers_count, char** content) ;
    extern int32_t  httpparse_request(char* data, int len, HTTP_HEADER_T* headers, int count, char** endpoint, char** content) ;
    extern uint32_t  httpparse_content(char* data, int len, HTTP_HEADER_T* headers, int headers_count, char** content, int32_t *content_length) ;

    extern  int32_t httpparse_append_headers (char* data, int len, const HTTP_HEADER_T* headers, int headers_count, int last) ;
    extern char*    httpparse_get_multipart_separator ( const HTTP_HEADER_T* headers, int headers_count) ;

    extern int32_t  httpparse_url_parse (char* url, int *https, int *port, char** host, char** endpoint, char** credentials) ;


#ifdef __cplusplus
}
#endif

#endif /* __HTTPPARSE_H__ */
