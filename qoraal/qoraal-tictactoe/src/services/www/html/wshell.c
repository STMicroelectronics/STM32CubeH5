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
#include "qoraal/svc/svc_shell.h"
#include "qoraal/common/rtclib.h"
#include "qoraal-http/wserver.h"
#include "../wserver_inst.h"
#include "wshell.h"



const char*
wshell_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "QShell" ;
    }

    return 0 ;
}


static int32_t
wshell_qshell_out(void* ctx, uint32_t out, const char* str)
{
    HTTP_USER_T *user = (HTTP_USER_T *)ctx ;

    if (out == SVC_SHELL_OUT_ERR) {
        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_INFO,
                "WWW   : : SHELL: %s", str ? str : "") ;


    } else if (out == SVC_SHELL_IN_STD) {
        return -1 ;
        
    }

    if (str) {
        httpserver_chunked_append_str (user, str, strlen(str)) ;
    }

    return EOK ;
}

int32_t
wshell_handler_result (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    if (method == HTTP_HEADER_METHOD_GET) {

        char* dst = "version" ;
        char *last, *next, *end ;
        end = strchr (endpoint, '?') ;
        bool run = false ;

       while (end++) {
            last = end ;
            next = strchr (last, '=')  ;
            if (!next)  break ;
            *next++ = '\0' ;
            end = strchr (next, '&')  ;
            if (end) *end = '\0' ;


            if ((strcmp(last, "script") == 0) && next) {
                dst = next;
                urldecode (dst, next, 0) ;

                run = true ;
                break ;
            }

       }


        httpserver_chunked_append_fmtstr (user,
            "<tr><td><br><form method=\"get\" action=\"shell\">\r\n"
            "<textarea name=\"script\" rows=\"3\" cols=\"62\">"
            "%s"
            "</textarea><br>"
            "<input type=\"submit\" name=\"run\" value=\"Run\" style=\"float: left;\" />\r\n" \
            "</form>\r\n" \
            "\r\n<br>", dst) ;

        if (run) {

            SVC_SHELL_IF_T shellif ;
            char * script = qoraal_malloc(QORAAL_HeapAuxiliary, strlen(dst)+1);
            if (script) {
                strcpy(script, dst) ;
                httpserver_chunked_append_fmtstr (user, "<pre>\r\n");
                svc_shell_if_init (&shellif, (void*)user, wshell_qshell_out, 0) ;
                svc_shell_script_run (&shellif, "WShell", script, strlen(script)) ;
                httpserver_chunked_append_fmtstr (user, "</pre>\r\n");
                qoraal_free(QORAAL_HeapAuxiliary, script) ;

            } else {
                return HTTP_SERVER_E_MEMORY ;

            }

        }





    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;

    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

int32_t
wshell_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    return wshell_handler_result (user, method, endpoint) ;
}

