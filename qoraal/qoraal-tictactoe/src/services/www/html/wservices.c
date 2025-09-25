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
#include "qoraal/svc/svc_services.h"
#include "qoraal-http/wserver.h"
#include "wservices.h"


const char*
wservices_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "Services" ;

    }
    if (type == WSERVER_CTRL_METADATA_HEADERS) {
        if (strchr (endpoint, '?')) {
            return "<meta http-equiv=\"refresh\" content=\"1;url=/services\">";

        }
    }

    return 0 ;
}

int32_t
wservices_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    if (method == HTTP_HEADER_METHOD_GET) {

        SCV_SERVICE_HANDLE h ;

        char* request = strchr (endpoint, '?') ;
        if (request) {
            request++ ;

            DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_INFO, 
                    "service %s", request)

            for (h = svc_service_first(); h!=SVC_SERVICE_INVALID_HANDLE; ) {

                if (strncmp(request, svc_service_name(h), strlen(svc_service_name(h))) == 0) {

                    if (svc_service_status(h) != SVC_SERVICE_STATUS_STOPPED) {
                        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_INFO, 
                                "stopping %s", svc_service_name(h))
                        svc_service_stop (h,0,0) ;

                    } else{
                        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_INFO, 
                                "starting %s", svc_service_name(h))
                        svc_service_start (h, 0, 0, 0) ;

                    }

                    break ;

                }

                h = svc_service_next(h);

            }
        }

        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_INFO, "done")

        httpserver_chunked_append_fmtstr (user,
                    "<table style=\"width: 50%%; box-sizing: border-box;\">\r\n"
                    //"<table width='100%' cellpadding=\"0\" cellspacing=\"0\">\r\n"
                    "<thead><tr><td></td></tr>\r\n"
                    "<tr><td valign=\"top\" > <b> SERVICE </b> </td>"
                    "<td valign=\"top\" > <b> STATUS </b> </td>"
                    "</tr>\r\n") ;


        for (h = svc_service_first(); h!=SVC_SERVICE_INVALID_HANDLE; ) {

            const char * name = svc_service_name(h) ;
            const char * next_state = svc_service_status(h) != SVC_SERVICE_STATUS_STOPPED ? "Stop" : "Start" ;
            const char* next_name = (svc_service_status(h) == SVC_SERVICE_STATUS_STOPPED) ? "[-]" :
                                    (svc_service_status(h) == SVC_SERVICE_STATUS_STARTED) ? "[+]" :
                                                                                            "[ ]";

            httpserver_chunked_append_fmtstr (user,
                    "<tr><td valign=\"top\" >  %s  </td>"
                    "<td> <a href=\"/services?%s=%s\">%s</a> </td>"
                    "</tr>\r\n",
                    name,
                    name,
                    next_state,
                    next_name) ;

            h = svc_service_next(h);


        }


        httpserver_chunked_append_fmtstr (user,
                "</thead></table><br>\r\n"
                "<form action=\"\" method=\"get\"><input type=\"submit\" name=\"refresh\" value=\"Refresh\">\r\n"

        ) ;
        // "<form action=\"log/reset\" method=\"get\"><input type=\"submit\" value=\"Reset\"></form><br>\r\n"


    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;

    }

    return HTTP_SERVER_WSERVER_E_OK ;
}


