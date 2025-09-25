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
#include "qoraal/svc/svc_logger.h"
#include "qoraal/common/mlog.h"
#include "qoraal-http/wserver.h"
#include "wsystem.h"



const char*
wrtlog_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "RTLog" ;
    }

    return 0 ;
}

static void
logger_cb (void* channel, LOGGER_TYPE_T type, uint8_t facility, const char* msg)
{
    HTTP_USER_T *user = (HTTP_USER_T *) ((LOGGER_CHANNEL_T*)channel)->user ;
    if (user && msg && strlen(msg)) {
        httpserver_chunked_append_fmtstr (user, "data: %s\n\n", msg) ;
        httpserver_chunked_flush (user) ;
    }
}


/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wrtlog_handler(HTTP_USER_T *user, uint32_t method, char* endpoint)
{
    const char html_page[] = "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <title>Log Viewer</title>\n"
                "  <style>\n"
                "    /* Apply a monospaced font like Courier, or fall back to the system's monospace */\n"
                "    html, body {\n"
                "      height: 100%; /* Ensure the body takes full height of the page */\n"
                "      margin: 0;\n"
                "    }\n"
                "    #log-container {\n"
                "      font-family: \"Courier New\", Courier, monospace;\n"
                "      white-space: pre-wrap; /* Ensures logs preserve whitespace and line breaks */\n"
                "      background-color: #f4f4f4; /* Optional: Adds a subtle background */\n"
                "      padding: 10px;\n"
                "      border: 1px solid #ccc;\n"
                "      height: calc(100% - 80px); /* Fit to the page, leaving space for the header (h1) */\n"
                "      overflow-y: auto;  /* Adds scrolling if logs exceed container height */\n"
                "      box-sizing: border-box; /* Ensure padding and borders don't overflow */\n"
                "    }\n"
                "    h1 {\n"
                "      margin: 0;\n"
                "      padding: 20px;\n"
                "      font-size: 24px;\n"
                "      background-color: #eee;\n"
                "      text-align: center;\n"
                "      border-bottom: 1px solid #ccc;\n"
                "    }\n"
                "  </style>\n"                
                "</head>\n"
                "<body>\n"
                "  <h1>Real-time Log</h1>\n"
                "  <div id=\"log-container\"></div> <!-- This is where the logs will go -->\n"
                "\n"
                "  <script>\n"
                "    const eventSource = new EventSource('/rtlog/log'); // SSE endpoint\n"
                "\n"
                "    eventSource.onmessage = function(event) {\n"
                "      const logContainer = document.getElementById('log-container');\n"
                "      const newLog = document.createElement('div'); // Create a new div for each log\n"
                "      newLog.textContent = event.data; // Set the log text\n"
                "      logContainer.appendChild(newLog); // Append the new log to the container\n"
                "\n"
                "      // Scroll to the bottom of the container as new logs come in\n"
                "      logContainer.scrollTop = logContainer.scrollHeight;\n"
                "    };\n"
                "  </script>\n"
                "</body>\n"
                "</html>\n";
            
    int32_t res = HTTP_SERVER_WSERVER_E_OK ;


    char* cmd[5]  = {0} ;
    int i ;

    cmd[0] = strchr (endpoint, '/') ;
    for (i=0; i<5; i++) {
        if (cmd[i]) *(cmd[i])++ = 0 ;
        if (cmd[i]) cmd[i+1] = strchr (cmd[i], '/') ;
        if (cmd[i+1] == 0) break ;

    }

    if (method == HTTP_HEADER_METHOD_GET) {

        if (!cmd[0]) {
            return httpserver_write_response(user, 200, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0, html_page, sizeof(html_page)-1);

        } 
        if (strcmp(cmd[0], "log") == 0) {

            LOGGER_CHANNEL_T         log_channel = {0} ;
            const   HTTP_HEADER_T headers[]   = {
                        {"Cache-Control", "no-cache"}
                    };

            if ((res = httpserver_chunked_response (user, 200, 
                            HTTP_SERVER_CONTENT_TYPE_EVENT_STREAM, headers, 
                            sizeof(headers)/sizeof(headers[0])) < HTTP_SERVER_E_OK)) {
                return res ;
            }

            if (os_sys_timestamp() < 90000) {
                // reconnect after reset, log the boot process
                void* it = mlog_itertor_last (MLOG_DBG, 0)  ;
                while (it) {
                    QORAAL_LOG_MSG_T* msg =  mlog_itertor_get (MLOG_DBG, it) ;
                    if (msg == 0) break ;
                    httpserver_chunked_append_fmtstr (user, "data: %s\n\n", msg->msg) ;
                    mlog_itertor_release (MLOG_DBG, it) ;
                    it = mlog_itertor_next (MLOG_DBG, it, 0)  ;
                    
                }

                httpserver_chunked_flush (user) ;
            }


            log_channel.fp = logger_cb ;
            log_channel.user = (void*) user ;
            log_channel.filter[0].mask = SVC_LOGGER_MASK ;
            log_channel.filter[0].type = SVC_LOGGER_SEVERITY_LOG|SVC_LOGGER_FLAGS_PROGRESS ;

            svc_logger_channel_add (&log_channel) ;
            httpserver_wait_close (user, 900000) ; // keep the socket open for 15 minutes from our side
            svc_logger_channel_remove (&log_channel) ;

            return httpserver_chunked_complete (user) ;

        }

    } else {
        return HTTP_SERVER_WSERVER_E_METHOD ;
    }

    return HTTP_SERVER_WSERVER_E_OK ;
}

