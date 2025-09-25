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
#include "qoraal/svc/svc_logger.h"
#include "qoraal/common/mlog.h"
#include "qoraal-http/wserver.h"
#include "wnlog.h"




//#define WNLOG_BUFFER_SIZE (4096 + 384)
#define WNLOG_BUFFER_SIZE (2048)
#define LOG_MSG_SIZE    (sizeof(QORAAL_LOG_MSG_T) + 200)


const char*
wnlog_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "Log" ;
    }
    return 0 ;
}

int32_t
wnlog_handler_get (HTTP_USER_T *user, uint32_t method, char* endpoint,
                   QORAAL_LOG_IT_T * it, const char * log_name)
{

    (void)endpoint ;

    if (method == HTTP_HEADER_METHOD_GET) {
#define LOG_COUNT       30
        int32_t back = LOG_COUNT ;
        //int32_t count = 25 ;
        int history = 0 ;

        char *last, *next, *end ;
        //uint32_t* paddr = 0 ;

        char * buffer ;
        char* pseverity ;
        int len = 0 ;
        unsigned int severity = 6 ;

        int32_t bnext = 0 ;

        end = strchr (endpoint, '?') ;

        while (end++) {
            last = end ;
            next = strchr (last, '=')  ;
            if (!next)  break ;
            *next++ = '\0' ;
            end = strchr (next, '&')  ;
            if (end) *end = '\0' ;

            if (strcmp(last, "back") == 0) {
                bnext=1 ;
            }
            if (strcmp(last, "next") == 0) {
                bnext=-1 ;
            }
            else if (strcmp(last, "refresh") == 0) {
                bnext=0 ;
            }
            else if (strcmp(last, "history") == 0) {
                sscanf (next, "%d", &history) ;
            }
            else if (strcmp(last, "severity") == 0) {
                pseverity = next ;
                if (pseverity) {
                    severity = (unsigned int) atoi(pseverity) ;
                }
            }


        }


        if (bnext) {
            history  +=  LOG_COUNT*bnext  ;
        }
        if (!bnext) {
            history = 0 ;
        }
        if (history<0) {
            history = 0 ;
        }

        buffer = qoraal_malloc(QORAAL_HeapAuxiliary, WNLOG_BUFFER_SIZE) ;
        QORAAL_LOG_MSG_T *  msg =  qoraal_malloc(QORAAL_HeapAuxiliary, LOG_MSG_SIZE) ;


        if (buffer && msg) {

            httpserver_chunked_append_fmtstr (user,
                    "<h4>%s log:</h4>", log_name) ;

            httpserver_chunked_append_fmtstr (user,
                    "<table class=\"u-full-width\">\r\n"
                    "<thread><tr>"
                    "    <th>ID</th>"
                    "    <th>Date</th>"
                    "   <th>Time</th>"
                    "   <th>Message</th>"
                    "</tr></thread><tbody>\r\n") ;
            back = history ;

            while (back-- && (it->prev(it) == EOK)) ;

            back = LOG_COUNT ;
            //len = 0 ;
            while (back-- && (it->get (it, msg, LOG_MSG_SIZE) >= EOK)) {

                if (msg->severity <= severity) {

                len += snprintf (&buffer[len], WNLOG_BUFFER_SIZE - len,
                //httpserver_chunked_append_fmtstr (user,
                        "<tr><td>%.6u.%d&nbsp;&nbsp;</td>"
                        "<td>%.2d/%.2d/%.4d&nbsp;&nbsp;</td>"
                        "<td>%.2d:%.2d:%.2d&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                        "<td>%s<br></td></tr>\r\n",
                        (unsigned int)msg->id, (int)msg->severity,
                        (int)msg->date.month, (int)msg->date.day, (int)msg->date.year,
                        (int)msg->time.hour, (int)msg->time.minute, (int)msg->time.second,
                        (char*)msg->msg) ;

                if (len >= WNLOG_BUFFER_SIZE - 384) {
                    if (httpserver_chunked_append_str (user, buffer, len) < 0) {
                        qoraal_free(QORAAL_HeapAuxiliary, buffer) ;
                        return HTTP_SERVER_E_CONNECTION ;
                    }
                    len = 0 ;
                }

                }


                if (it->prev(it) != EOK) break ;
            }


            if (len) {
                if (httpserver_chunked_append_str (user, buffer, len) < 0) {
                    qoraal_free(QORAAL_HeapAuxiliary, buffer) ;
                    return HTTP_SERVER_E_CONNECTION ;

                }
            }

            len = snprintf (buffer, WNLOG_BUFFER_SIZE ,
            //httpserver_chunked_append_fmtstr (user,
                    "</tbody></table>\r\n<br>"
                    "<form action=\"\" method=\"get\">"
                    "<select name=\"severity\" >"
                    "<option %s value=1>1 - Assert</option>\r\n"
                    "<option %s value=2>2 - Error</option>\r\n"
                    "<option %s value=3>3 - Warning</option>\r\n"
                    "<option %s value=4>4 - Report</option>\r\n"
                    "<option %s value=5>5 - Log</option>\r\n"
                    "<option %s value=6>6 - Info</option>\r\n"
                    "</select>&nbsp;&nbsp;&nbsp;"
                    "<input type=\"submit\" name=\"refresh\" value=\"Refresh\">"
                    "&nbsp;&nbsp;&nbsp;<input type=\"submit\" name=\"back\" value=\"&lt; Back\" >"
                    "&nbsp;<input type=\"submit\" name=\"next\" value=\"Next &gt;\" >"
                    "<input type=\"hidden\" name=\"history\" value=\"%d\">"
                    "</form>\r\n",
                    severity == 1 ? "selected" : "",
                    severity == 2 ? "selected" : "",
                    severity == 3 ? "selected" : "",
                    severity == 4 ? "selected" : "",
                    severity == 5 ? "selected" : "",
                    severity == 6 ? "selected" : "",
                    history) ;
            // "<form action=\"log/reset\" method=\"get\"><input type=\"submit\" value=\"Reset\"></form><br>\r\n"

            len = httpserver_chunked_append_str (user, buffer, len) ;

            return len > 0 ? HTTP_SERVER_WSERVER_E_OK : len ;
        }

        qoraal_free(QORAAL_HeapAuxiliary, buffer) ;
        qoraal_free(QORAAL_HeapAuxiliary, msg) ;


    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;

    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

#if 1
int32_t
wmemlog_handler_download (HTTP_USER_T *user, uint32_t method, char* endpoint, int log, int type)
{
    //ATH_GET_SCAN list ;
    //int i, j ;

    (void)endpoint ;

    if (method == HTTP_HEADER_METHOD_GET) {
#define LOG_COUNT       30
        //NLOG_LOG_T  msg ;

        void* it = mlog_itertor_last (log, type)  ;

        char * buffer = qoraal_malloc(QORAAL_HeapAuxiliary, WNLOG_BUFFER_SIZE) ;
        int32_t len = 0 ;


        if (buffer) {

                while (it) {

                    QORAAL_LOG_MSG_T* msg =  mlog_itertor_get (log, it) ;

                    if (msg == 0) break ;

                    len += snprintf (&buffer[len], WNLOG_BUFFER_SIZE - len,
                    //httpserver_chunked_append_fmtstr (user,
                            "%.4x\t"
                            "%.4d-%.2d-%.2d\t"
                            "%.2d:%.2d:%.2d\t"
                            "%s\r\n",
                            msg->id,
                            msg->date.year, msg->date.month, msg->date.day,
                            msg->time.hour, msg->time.minute, msg->time.second,
                            msg->msg) ;

                    mlog_itertor_release (log, it) ;


                    if (len >= WNLOG_BUFFER_SIZE - 384) {
                        if (httpserver_chunked_append_str (user, buffer, len) < 0) {
                            qoraal_free(QORAAL_HeapAuxiliary, buffer) ;
                            return HTTP_SERVER_E_CONNECTION ;
                        }
                        len = 0 ;
                    }


                    it = mlog_itertor_next (log, it, type)  ;
                }

                if (len) {
                    if (httpserver_chunked_append_str (user, buffer, len) < 0) {
                        qoraal_free(QORAAL_HeapAuxiliary, buffer) ;
                        return HTTP_SERVER_E_CONNECTION ;

                    }
                }


        }

    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;

    }

    return HTTP_SERVER_WSERVER_E_OK ;
}
#endif


int32_t
wnlog_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    QORAAL_LOG_IT_T * it = 0 ;
    int32_t res = HTTP_SERVER_WSERVER_E_OK ;
    int syslog = 1 ;
    const char * name = "" ;

#if 0
    if (strncmp(endpoint, "log/1", 5) == 0) {
        it = syslog_platform_it_create (1) ;
        name = "Debug" ;

    }
    else 
#endif
    if (strncmp(endpoint, "log/2", 5) == 0) {
        it = mlog_platform_it_create (MLOG_DBG) ;
        syslog = 0 ;
        name = "Memory" ;

    }
    else if (strncmp(endpoint, "log/3", 5) == 0) {
        it = mlog_platform_it_create (MLOG_ASSERT) ;
        syslog = 0 ;
        name = "Assert" ;
    }



    httpserver_chunked_append_fmtstr (user,
        "<A style=\"width:110%\" class=\"button\" href=\"/log/0\" >System</A>\r\n"
        "<A style=\"width:110%\" class=\"button\" href=\"/log/1\" >Assert</A>\r\n"
        "<A style=\"width:110%\" class=\"button\" href=\"/log/2\" >Memory</A>\r\n"
        );

    if (it) {
        res = wnlog_handler_get (user, method, endpoint, it, name) ;
        //if (syslog) syslog_platform_it_destroy (it) ;
#if 1  
        mlog_platform_it_destroy (it) ;
#endif
    }

return res ;
}

int32_t
wnlog_memlog_handler (HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    int32_t res ;

    if ((res = httpserver_chunked_response (user, 200, HTTP_SERVER_CONTENT_TYPE_TEXT, 0, 0) < HTTP_SERVER_E_OK)) {
        return res ;
    }
    httpserver_chunked_append_fmtstr (user,
            "MEM LOG\r\n\r\ttype\tid\tdate\ttime\tmessage\r\n") ;
#if CFG_PLATFORM_MEMLOG
    if ((res = wmemlog_handler_download (user, method, endpoint, MLOG_DBG, 0 ) < HTTP_SERVER_E_OK)) {
        return res ;
    }
#endif
    return httpserver_chunked_complete (user) ;

}
