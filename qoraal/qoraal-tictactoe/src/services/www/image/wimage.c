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

/*
 * wcss.c
 *
 *  Created on: 11 May 2015
 *      Author: natie
 */

#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/common/rtclib.h"
#include "qoraal-http/wserver.h"
#include "wimage.h"
#define PROGMEM
#include "logo.inc"

/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wimage_handler(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    HTTP_HEADER_T image_headers[]   = {
            {"Cache-Control",  "public, max-age=3600"} ,
            //{"Cache-Control",  "max-age=10, public"} ,
    };

    if (method == HTTP_HEADER_METHOD_GET) {


       // char* filename = strchr (endpoint, '/') ;


        /*
        if (strcmp(endpoint, "image/icon") == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    image_icon(), image_icon_size()) ;
        }
        else
        */
        if (strncmp(endpoint, "image/sicon", 11) == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    logo, sizeof(logo)) ;
        }
        /*
        else if (strcmp(endpoint, "image/ssicon") == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    image_ssicon(), image_ssicon_size()) ;
        }
        else if (strcmp(endpoint, "image/icon") == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    image_sicon(), image_sicon_size()) ;
        }
        else if (strncmp(endpoint, "favicon", 7) == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    image_ssicon(), image_ssicon_size()) ;
        }
        else if (strncmp(endpoint, "image/favicon", 13) == 0) {
            httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_IMAGE,
                    image_headers, sizeof(image_headers)/sizeof(image_headers[0]),
                    image_ssicon(), image_ssicon_size()) ;
        }
        */
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

