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

/**
 * @name    HTML text
 * @{
 */
#define WSERVER_ABOUT_TEXT      "Qoraal About"

const char*
wabout_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "About" ;
    }
    if (type == WSERVER_CTRL_METADATA_HEADERS) {

        char* groupname = strchr (endpoint, '/') ;
        if (groupname++) {
            if (strcmp(groupname, "refresh") == 0) {
                return "<meta http-equiv=\"refresh\" content=\"2; \">" ;
            }

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
wabout_handler_about(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    static const char about_start[] = \
            "<H1>" WSERVER_ABOUT_TEXT "</H1>\r\n"
            "<H2>" WSERVER_ABOUT_TEXT "</H2>\r\n"
            "<H3>" WSERVER_ABOUT_TEXT "</H3>\r\n"
            "<H4>" WSERVER_ABOUT_TEXT "</H4>\r\n"
            "<H5>" WSERVER_ABOUT_TEXT "</H5>\r\n"
            "<H6>" WSERVER_ABOUT_TEXT "</H6>\r\n"
            ;


    if (method == HTTP_HEADER_METHOD_GET) {
            httpserver_chunked_append_str (user, about_start, sizeof(about_start) - 1) ;
    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

#if 1
int32_t
wserver_handler_about2(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
#define ABOUT_CONTENT_SIZE          2048
    static int i = 0 ;
    static const char about_start[] = \
            "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n" \
            "<html><head>\r\n" \
            "<meta http-equiv=\"refresh\" content=\"2; \">"
            "<title>" WSERVER_ABOUT_TEXT " About</title>\r\n" \
            "</head><body>\r\n" \
            "<H2>" WSERVER_ABOUT_TEXT  " %d <H2>" ;
    static const char about_end[] = \
            "\r\n</body></html>\r\n" ;
    static const char about_content[] = \
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n" ;

    if (method == HTTP_HEADER_METHOD_GET) {

        if (ABOUT_CONTENT_SIZE >= sizeof(about_content)) {
            char * send = qoraal_malloc(QORAAL_HeapAuxiliary, ABOUT_CONTENT_SIZE) ;
            if (send) {
                uint32_t len = 0 ;
                sprintf (send, about_start, i++) ;
                len = strlen(send);
                while ((unsigned int)len <= ABOUT_CONTENT_SIZE - sizeof(about_content)) {
                strcpy (&send[len], about_content) ;
                len += sizeof(about_content) - 1 ;
                }
                strncpy (&send[len], about_end, ABOUT_CONTENT_SIZE - len) ;
                len = strlen(send) ;



                httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0,
                        send, len) ;

                qoraal_free(QORAAL_HeapAuxiliary, send) ;

            } else {
                return HTTP_SERVER_E_MEMORY ;

            }
        }

    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
}
#endif

/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wserver_handler_about3(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    static const char about_start[] = \
            "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n" \
            "<html><head>\r\n" \
            "</head><body>\r\n" \
            "<H2>" WSERVER_ABOUT_TEXT  " %d <H2><br>" ;
    static const char about_end[] = \
            "\r\n</body></html>\r\n" ;

    static const char about_content[] = \
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890<br>\r\n"
            ;
    //static int i = sizeof(about_content) - 1;


    if (method == HTTP_HEADER_METHOD_GET) {

        httpserver_chunked_response (user, 200, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0) ;
        httpserver_chunked_append_fmtstr (user, about_start) ;
        int j ;
        for (j=0; j<100; j++) {
             uint32_t len = sizeof(about_content) - 1 ;
             if (httpserver_chunked_append_str (user, about_content, len) < 0) break ;
        }


        httpserver_chunked_append_fmtstr (user, about_end) ;
        return httpserver_chunked_complete (user) ;
        
    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

/**
 * @brief   Handler 
 *
 * @app
 */
#if defined DEBUG && DEBUG
int32_t
wabout_handler_post(HTTP_USER_T *user, uint32_t method, char* endpoint, char* content, int len)
{
    (void) endpoint ;
    (void) content ;
    (void) len ;


    if (method == HTTP_HEADER_METHOD_POST) {


        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_LOG, "WSERVER HTTP POST REQEST:\r\n")  ;
        DBG_MESSAGE_WWW (DBG_MESSAGE_SEVERITY_LOG, content) ;


    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;

}
#endif

/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wabout_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{


        return wabout_handler_about (user, method, endpoint) ;



}


