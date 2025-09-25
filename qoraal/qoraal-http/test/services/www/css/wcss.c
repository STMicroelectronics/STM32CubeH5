
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

#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal-http/wserver.h"
// https://notisrac.github.io/FileToCArray
#define PROGMEM
#define byte char
#include "skeleton.inc"


/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wcss_handler(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    HTTP_HEADER_T css_headers[] = {
            {"Cache-Control",  "public, max-age=604800"} ,
    };

    if (method == HTTP_HEADER_METHOD_GET) {

        if (strcmp(endpoint, "css/default.css") == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_CSS,
                    css_headers, sizeof(css_headers)/sizeof(css_headers[0]),
                    skeleton, sizeof(skeleton)) ;
        }
        else {
            httpserver_write_response (user, WSERVER_RESP_CODE_400, HTTP_SERVER_CONTENT_TYPE_HTML,
                    0, 0, WSERVER_RESP_CONTENT_400, strlen(WSERVER_RESP_CONTENT_400)) ;

        }

    } else {
        httpserver_write_response (user, WSERVER_RESP_CODE_400, HTTP_SERVER_CONTENT_TYPE_HTML,
                0, 0, WSERVER_RESP_CONTENT_400, strlen(WSERVER_RESP_CONTENT_400)) ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
}



