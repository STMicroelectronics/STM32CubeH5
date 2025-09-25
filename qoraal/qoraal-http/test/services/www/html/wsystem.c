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
#include "qoraal/common/rtclib.h"
#include "qoraal-http/wserver.h"
#include "wsystem.h"
#include "../wserver_inst.h"


const char*
wsystem_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "System" ;
    }
    if (type == WSERVER_CTRL_METADATA_HEADERS) {
        char* groupname = strchr (endpoint, '?') ;
        if (groupname) {
            groupname++ ;
        }

        if (groupname && strncmp(groupname, "reset", 5) == 0) {

                return "<meta http-equiv=\"refresh\" content=\"5; url=/\">" ;

        }
    }

    return 0 ;
}

/**
 * @brief   Handler
 *
 * @app
 */
int32_t
wsystem_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{

    if (method == HTTP_HEADER_METHOD_GET) {

        char* cmd[5]  = {0} ;
        cmd[0] = strchr (endpoint, '/') ;
        for (int i=0; i<5; i++) {
            if (cmd[i]) *(cmd[i])++ = 0 ;
            if (cmd[i]) cmd[i+1] = strchr (cmd[i], '/') ;
            if (cmd[i+1] == 0) break ;

        }


        WSERVER_CHECK_SEND(httpserver_chunked_append_fmtstr (user,
                    "<table class=\"u-full-width\">\r\n"
                    "<tbody>\r\n")) ;

        WSERVER_CHECK_SEND(httpserver_chunked_append_fmtstr (user,
                    "<tr><td><br><b>Version Information<b></td></tr>"
                    "<tr><td>Vesrion</td>"
                    "<td>v%.2d.%.2d%c B%.5d</td></tr>"
                    "<tr><td>Build Name</td>"
                    "<td>%s</td></tr>",
                    0, 0, 'a' + 1, 99,
                    "Qoraal HTTP")) ;

    	RTCLIB_DATE_T   date ;
    	RTCLIB_TIME_T   time ;
        rtc_localtime (rtc_time(), &date, &time) ;
        WSERVER_CHECK_SEND(httpserver_chunked_append_fmtstr (user,
                    "<tr><td><br><b>Local Time<b></td></tr>"
                    "<tr><td>Date</td>"
                    "<td>%.4d-%.2d-%.2d</td></tr>"
                    "<tr><td>Time</td><td>%.2d:%.2d:%.2d</td></tr>\r\n",
                    (int32_t)date.year, (int32_t)date.month, (int32_t)date.day,
                    (int32_t)time.hour, (int32_t)time.minute, (int32_t)time.second)) ;

        uint32_t hours, minutes, seconds ;
        seconds = OS_TICKS2S(os_sys_ticks() ) ;
        minutes = seconds / 60 ;
        seconds = seconds % 60 ;
        hours = minutes / 60 ;
        minutes = minutes % 60 ;
        WSERVER_CHECK_SEND(httpserver_chunked_append_fmtstr (user,
                    "<tr><td>Up Time</td>"
                "<td> %d hours %d minutes %d seconds</td></tr>\r\n",
                hours, minutes, seconds)) ;

        WSERVER_CHECK_SEND(httpserver_chunked_append_fmtstr (user,
                "</tbody></table><br>\r\n"
             )) ;
        WSERVER_CHECK_SEND(httpserver_chunked_append_str (user,
                "<A style=\"width:100%\" class=\"button button-primary\" href=\"system/shutdown\" >Shutdown</A>\r\n", 0
            )) ;

        if (cmd[0] && strcmp(cmd[0], "shutdown") == 0) {
            svc_service_stop_timeout (svc_service_get(QORAAL_SERVICE_WWW), 0) ;

        }
        
    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;

    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

