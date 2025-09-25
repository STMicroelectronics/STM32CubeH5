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
#include <stdlib.h>
#include "qoraal/qoraal.h"
#include "qoraal-http/wserver.h"
#include "../../services.h"
#include "../parts/html.h"

static HTML_EMIT_T  _wengine_emit ;


const char*
wengine_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    static int started = 0 ;
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "Engine" ;
    }
    else if (type == WSERVER_CTRL_START){
        if (!started) {
            html_emit_create (&_wengine_emit) ;
            started = 1 ;
        }
    }
    else if (type == WSERVER_CTRL_STOP){
        html_emit_delete (&_wengine_emit) ;
        started = 0 ;
    }
    
    return 0 ;
}


/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wengine_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    int32_t res = HTTP_SERVER_WSERVER_E_OK ;

    char* cmd[5]  = {0} ;
    int i ;
    uint16_t event = 0 ;
    uint16_t parm = 0 ;

    cmd[0] = strchr (endpoint, '/') ;
    for (i=0; i<5; i++) {
        if (cmd[i]) *(cmd[i])++ = 0 ;
        if (cmd[i]) cmd[i+1] = strchr (cmd[i], '/') ;
        if (cmd[i+1] == 0) break ;

    }

    if (method == HTTP_HEADER_METHOD_GET) {

        if (html_emit_lock (&_wengine_emit, 4000) != EOK) {
            return httpserver_write_response (user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_HTML,
                0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;

        }

        if (cmd[1]) event = (uint16_t)atoi(cmd[1]) ;
        if (cmd[2]) parm = (uint16_t)atoi(cmd[2]) ;
        res = html_emit_wait (cmd[0], event, parm, user, 120000) ;
        html_emit_unlock (&_wengine_emit) ;
        if (res == E_NOTFOUND) {  
            res =  httpserver_write_response (user, WSERVER_RESP_CODE_404, HTTP_SERVER_CONTENT_TYPE_HTML,
                0, 0, WSERVER_RESP_CONTENT_404, strlen(WSERVER_RESP_CONTENT_404)) ;

        } else if (res != EOK) {
            res =  httpserver_write_response (user, WSERVER_RESP_CODE_404, HTTP_SERVER_CONTENT_TYPE_HTML,
                0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;

        }
        
        return res ;

    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
 
}


