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
#include "html/wsystem.h"
#include "html/wservices.h"
#include "html/wabout.h"
#include "html/wshell.h"
#include "html/wnlog.h"
#include "html/wrtlog.h"
#include "html/wengine.h"
#include "css/wcss.h"
#include "image/wimage.h"
#include "wserver_inst.h"


static int32_t      wserver_init (uintptr_t arg) ;
static int32_t      wserver_start (uintptr_t port) ;
static int32_t      wserver_stop (uintptr_t arg) ;

static HTTPSERVER_INST_T *  _wserver_inst = 0 ;

/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wserver_header_start(HTTP_USER_T * user, uint32_t method, char* endpoint, WSERVER_CTRL ctrl)
{
    static const char head_content_1[] =
            "<!DOCTYPE HTML PUBLIC>"
            "<head>\r\n"
            "<meta content=\"text/html;charset=utf-8\" http-equiv=\"Content-Type\">"
            "<meta content=\"utf-8\" http-equiv=\"encoding\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n"
            "<link rel=\"icon\" href=\"data:;base64,=\">"
            ;

    static const char head_content_2[] =
            "<link rel=\"stylesheet\" href=\"/css/default.css\">\r\n"
            "\r\n</head>\r\n<body style=\"background-color: #F5F5DC;\">\r\n" ;

    int32_t res ;

    const char * headers = 0;
    if (ctrl) {
        headers = ctrl (user, method, endpoint, WSERVER_CTRL_METADATA_HEADERS) ;
    }

    do {
        if ((res = httpserver_chunked_response (user, 200, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0) < HTTP_SERVER_E_OK)) {
            break ;
        }
        if ((res = httpserver_chunked_append_str (user, head_content_1, sizeof(head_content_1) - 1) < HTTP_SERVER_E_OK)) {
            break ;
        }
        if (headers) {
            if ((res = httpserver_chunked_append_str (user, headers, strlen(headers)) < HTTP_SERVER_E_OK)) {
                break ;
            }
        }

        if ((res = httpserver_chunked_append_str (user, head_content_2, sizeof(head_content_2) - 1) < HTTP_SERVER_E_OK)) {
            break ;
        }

    } while (0) ;

    return res ;
}



int32_t
wserver_footer_end(HTTP_USER_T * user, uint32_t method, char* endpoint, WSERVER_CTRL ctrl)
{
    static const char footer_content[] =
            "\r\n</body>\r\n</html>\r\n\r\n" ;
    int32_t res ;

    do {
        if ((res = httpserver_chunked_append_str (user, footer_content, sizeof(footer_content) - 1) < HTTP_SERVER_E_OK)) {
            break ;
        }
        if ((res = httpserver_chunked_complete (user)) < HTTP_SERVER_E_OK) {
            break ;
        }
    } while (0) ;

    return  res ;

}

int32_t
wserver_handler_framework_start(HTTP_USER_T * user, uint32_t method, char* endpoint, WSERVER_CTRL ctrl)
{
    static const char framework_content_1[] =
            "<div style=\"max-width:1536x; width:90%\" class=\"container\" >\r\n"
            "   <div class=\"row\">\r\n"
            "   <div class=\"two columns\">\r\n"
            "       <a href=\"/\"><img src=\"/image/sicon.png\" align=\"top\" style=\"width:100%; \"></a>"
            "   </div><div class=\"eight columns\">\r\n"
            "   <H4 align=\"center\"><B>&nbsp;" WSERVER_TITLE_TEXT ;

    static const char framework_content_2[] =
            "</B></H4>\r\n"
            "   </div>\r\n"
            "   </div>\r\n"
            "   <div class=\"row\"><br></div>\r\n"
            "   <div class=\"row\">\r\n"
            "   <div class=\"two columns\">\r\n"
            "       <table><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/system\" >System</A><br>\r\n"
            "       </tr><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/engine\" >Engine</A><br>\r\n"
            "       </tr><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/services\" >Services</A><br>\r\n"
            "       </tr><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/log\" >Log</A><br>\r\n"
            "       </tr><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/shell\" >Shell</A><br>\r\n"
            "       </tr><tr>\r\n"
            "           <A style=\"width:100%\" class=\"button button-primary\" href=\"/about\" >About</A><br>\r\n"
            "       </tr></table>\r\n"
            "   </div>\r\n"
            "   <div class=\"one columns\">\r\n"
            "   </div>\r\n"
            "   <div class=\"nine columns\">\r\n" ;

    int32_t res ;
    const char * heading = 0;
    if (ctrl) {
        heading = ctrl (user, method, endpoint, WSERVER_CTRL_METADATA_HEADING) ;
    }


    (void) endpoint ;
    do {
        if ((res = httpserver_chunked_append_str (user, framework_content_1, sizeof(framework_content_1) - 1)) < HTTP_SERVER_E_OK) {
            break ;
        }
        if (heading) {
            if ((res = httpserver_chunked_append_fmtstr (user, "&nbsp;-&nbsp;%s", heading)) < HTTP_SERVER_E_OK) {
                break ;
            }
        }
        if ((res = httpserver_chunked_append_str (user, framework_content_2, sizeof(framework_content_2) - 1)) < HTTP_SERVER_E_OK) {
            break ;
        }
    } while(0) ;

    return res ;
}

int32_t
wserver_handler_framework_end(HTTP_USER_T * user, uint32_t method, char* endpoint, WSERVER_CTRL ctrl)
{
    static const char framework_content[] =
            "\r\n"
            "   </div>\r\n"
            "   </div>\r\n"
            "   <div class=\"row\">\r\n"
            "       <div class=\"twelve columns\">\r\n"
            "           <br><p STYLE=\"text-align: right;\">"
            "<img src=\"/image/sicon.png\" height=\"16\" width=\"16\">&nbsp;&nbsp;"
            WSERVER_TITLE_TEXT "</p>\r\n"
            "       </div>\r\n"
            "   </div>\r\n"
            "</div>\r\n" ;

    return httpserver_chunked_append_str (user, framework_content, sizeof(framework_content) - 1) ;
}


const char*
windex_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "Configuration" ;
    }
    return 0 ;
}


int32_t
windex_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    static const char home_content[] =
            "<br><p align=\"center\" style=\"color:grey\">"
            "<b>" WSERVER_TITLE_TEXT "</b><br>"
            "Use the menu to configure this " WSERVER_TITLE_TEXT "."
            "</p>" ;


    int32_t res ;

    if (method == HTTP_HEADER_METHOD_GET) {

        res = httpserver_chunked_append_str (user, home_content, sizeof(home_content) - 1) ;

    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return res  ;
}

/**
 * @brief       wserver_service_ctrl
 * @details
 * @note
 *
 * @param[in] code
 * @param[in] arg
 *
 * @return              status
 *
 * @power
 */
int32_t     wserver_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = E_NOIMPL ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        res = wserver_init (arg) ;
        break ;

    case SVC_SERVICE_CTRL_START:
        res = wserver_start(arg) ;
        break ;

    case SVC_SERVICE_CTRL_HALT:
    case SVC_SERVICE_CTRL_STOP:
        res = wserver_stop(arg) ;
        break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        break ;

    }

    return res ;
}


int32_t wserver_service_run (uintptr_t arg)
{

    int32_t status = httpserver_wserver_run (_wserver_inst) ;
    httpserver_wserver_destroy (_wserver_inst) ;

    return status ;
}

int32_t
wserver_init (uintptr_t arg)
{
    return EOK ;
}

uint32_t
wserver_authenticate (const char * user, const char * passwd)
{
    return WSERVER_ENDPOINT_ACCESS_ADMIN ;
}

int32_t
wserver_start (uintptr_t arg)
{
    uint32_t port = 8080 ; //registry_get ("www.port", 80) ;
    bool ssl = false ; // registry_get ("www.ssl", false) ;

    static const WSERVER_FRAMEWORK wserver_std_headers[] = {
            wserver_header_start,
            wserver_handler_framework_start,
            0
    };

    static const WSERVER_FRAMEWORK wserver_std_footers[] = {
            wserver_handler_framework_end,
            wserver_footer_end,
            0
    };

    static WSERVER_HANDLERS_START(handlers)
    WSERVER_CTRL_HANDLER         ("",        wengine_handler,           WSERVER_ENDPOINT_ACCESS_OPEN,   0, wengine_ctrl)
    WSERVER_CTRL_HANDLER         ("engine",  wengine_handler,           WSERVER_ENDPOINT_ACCESS_OPEN,   0, wengine_ctrl)
    WSERVER_HANDLER              ("rtlog",   wrtlog_handler,            WSERVER_ENDPOINT_ACCESS_OPEN,   WSERVER_ENDPOINT_FLAGS_DISABLE_WDT)
    WSERVER_HANDLER              ("memlog",  wnlog_memlog_handler,      WSERVER_ENDPOINT_ACCESS_OPEN,   0)
    WSERVER_HANDLER              ("about2",  wserver_handler_about2,    WSERVER_ENDPOINT_ACCESS_ADMIN,  0)
    WSERVER_HANDLER              ("about3",  wserver_handler_about3,    WSERVER_ENDPOINT_ACCESS_OPEN,   0)
    WSERVER_HANDLER              ("image",   wimage_handler,            WSERVER_ENDPOINT_ACCESS_OPEN,   0)
    WSERVER_HANDLER              ("css",     wcss_handler,              WSERVER_ENDPOINT_ACCESS_OPEN,   0)
    WSERVER_FRAMEWORK_HANDLER    ("index",   windex_handler,            WSERVER_ENDPOINT_ACCESS_OPEN,   0, windex_ctrl,    wserver_std)
    WSERVER_FRAMEWORK_HANDLER    ("about",   wabout_handler,            WSERVER_ENDPOINT_ACCESS_OPEN,   0, wabout_ctrl,    wserver_std)
    WSERVER_FRAMEWORK_HANDLER    ("system",  wsystem_handler,           WSERVER_ENDPOINT_ACCESS_OPEN,   0, wsystem_ctrl,   wserver_std)
    WSERVER_FRAMEWORK_HANDLER    ("log",     wnlog_handler,             WSERVER_ENDPOINT_ACCESS_OPEN,   0, wnlog_ctrl,     wserver_std)
    WSERVER_FRAMEWORK_HANDLER    ("services", wservices_handler,        WSERVER_ENDPOINT_ACCESS_ADMIN,  0, wservices_ctrl, wserver_std)
    WSERVER_FRAMEWORK_HANDLER    ("shell",   wshell_handler,            WSERVER_ENDPOINT_ACCESS_ADMIN,  0, wshell_ctrl,    wserver_std)
    WSERVER_HANDLERS_END()

    _wserver_inst = httpserver_wserver_create (port, ssl, handlers, wserver_authenticate) ;
    return _wserver_inst ? EOK : EFAIL ;
}

int32_t
wserver_stop (uintptr_t arg)
{
    if (_wserver_inst) {
        httpserver_wserver_stop (_wserver_inst) ;
    }

    return EOK ;
}


