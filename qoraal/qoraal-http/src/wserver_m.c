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

/**
 * @brief Multi-threaded implementation for the wserver.
 */



#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal-http/qoraal.h"
#include "qoraal-http/wserver.h"

/**
 * @brief Represents an HTTP server thread handling a client connection.
 */
typedef struct HTTPSERVER_THREAD_S {
    SVC_THREADS_T               thread;          /**< Thread handle or context */
    HTTPSERVER_INST_T *         inst;            /**< Pointer to the HTTP server instance */
    HTTP_USER_T                 user;            /**< User session or connection information */
    uint32_t                    authenticated;   /**< Authentication state or permissions */
} HTTPSERVER_THREAD_T ;

/**
 * @brief Represents an HTTP server instance with configuration and state.
 */
typedef struct HTTPSERVER_INST_S {
    int                         server_sock ;      /**< Socket the server listens on */
    uint32_t                    port;              /**< Port number the server listens on */
    bool                        ssl;               /**< Flag indicating if SSL/TLS is enabled */
    const WSERVER_HANDLERS_T*   handlers;          /**< Array of request handlers */
    int32_t                     close;             /**< Flag indicating if the server should close */
    p_sem_t                     count_sem;         /**< Semaphore to limit concurrent connections */
    WSERVER_AUTHENTICATE        authenticate;      /**< Function pointer for authentication */
} HTTPSERVER_INST_T ;

#define HTTPSERVER_USER_ACCEPT_IN_THREAD            1
#define HTTPSERVER_USE_KEEPALIVE                    1
#define HTTPSERVER_USER_MAX                         96
#ifndef CFG_UTILS_HTTP_SERVER_THREADS
#define CFG_UTILS_HTTP_SERVER_THREADS               6
#endif

/**
 * @brief   Authenticates a client request in the given thread.
 *          This function checks if the requested handler requires authentication and verifies the client credentials.
 *          If authentication fails, it responds with a 401 Unauthorized status.
 *
 * @param[in] inst      Pointer to the HTTP server instance.
 * @param[in] handler   Pointer to the handler for the requested endpoint.
 * @param[in] thread    Pointer to the thread handling the client connection.
 *
 * @return EOK on successful authentication, E_AUTH if authentication fails, or EFAIL on error.
 */
static int32_t 
wserver_thread_authenticate (HTTPSERVER_INST_T * inst, const WSERVER_HANDLERS_T* handler, HTTPSERVER_THREAD_T* thread)
{
    bool resp401 = false ;

    if (    inst->authenticate && 
            handler->access &&
            !(thread->authenticated & handler->access)
        ) {
        resp401 = true ;
        unsigned char* in = (unsigned char*)httpserver_get_authorization_header(&thread->user);
        if (in) {
            const char* expected_scheme = "Basic ";
            size_t scheme_len = strlen(expected_scheme);

            // Check if the header starts with "Basic "
            if (strncmp((char*)in, expected_scheme, scheme_len) == 0) {
                char name[HTTPSERVER_USER_MAX] ;
                in += scheme_len;  // Advance the pointer past "Basic "
                int len = strlen((char*)in);

                // Proceed only if there's data after "Basic "
                if (len > 0 && len < BASE64_OUTPUT_LENGTH(sizeof(name) - 1)) {
                    char* pwd;

                    base64_decode((unsigned char*)name, in, len);
                    pwd = strchr(name, ':');

                    if (pwd) {
                        *pwd++ = '\0';
                        thread->authenticated = inst->authenticate(name, pwd);
                        resp401 = (thread->authenticated & handler->access) ? false : true;
                    }

                    if (resp401 && strlen(name)) {
                        DBG_MESSAGE_HTTP_SERVER(
                            DBG_MESSAGE_SEVERITY_REPORT,
                            "WSERV : : authenticate '%s' failed", name);
                    }
                }
            } else {
                // ToDo: Handle cases where the scheme is not "Basic "
                resp401 = true;

            }

        }

    }

    if (resp401) {
        int32_t res ;
        HTTP_HEADER_T headers_authenticate[]  = {
                {HTTP_HEADER_KEY_AUTHENTICATE, "Basic realm=\"private area\""}};

        if ((res = httpserver_write_response (&thread->user, WSERVER_RESP_CODE_401, HTTP_SERVER_CONTENT_TYPE_HTML,
                headers_authenticate, 1, WSERVER_RESP_CONTENT_401, strlen(WSERVER_RESP_CONTENT_401))) < HTTP_SERVER_E_OK) {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_REPORT,
                    "WSERV :E: write WSERVER_RESP_CODE_401 failed %d (1).\r\n", res);
            return EFAIL ;

        }
        if (httpserver_free_request (&thread->user) < 0) {
            return EFAIL ;

        }

        return E_AUTH ;

    }

    return EOK ;
}

/**
 * @brief   Thread function to handle client connections.
 *          This function runs in a separate thread to handle HTTP requests from a client.
 *          It reads requests, processes them using the registered handlers, and sends responses.
 *
 * @param[in] arg   Pointer to the HTTPSERVER_THREAD_T structure for this thread.
 */
static void 
_wserver_thread (void *arg)
{
    char * endpoint = 0 ;
    int32_t len ;
    int32_t method ;
    int32_t timeout = WSERVER_KEEPALIVE_TIMEOUT  ;
    HTTPSERVER_THREAD_T*    thread = (HTTPSERVER_THREAD_T* )arg ;
    HTTPSERVER_INST_T *     inst = thread->inst ;

    SVC_WDT_HANDLE_T hwdt ;
    svc_wdt_register (&hwdt, TIMEOUT_60_SEC) ;
    svc_wdt_activate (&hwdt) ;


    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
            "WSERV : : start socket 0x%x.\r\n", thread->user.socket);

#if HTTPSERVER_USER_ACCEPT_IN_THREAD
    if (!inst->ssl || httpserver_user_ssl_accept (&thread->user, 4000) == HTTP_SERVER_E_OK)
#endif
    {

    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
            "WSERV : : start with socket 0x%x.\r\n", thread->user.socket);

    while (!inst->close) {


        svc_wdt_handler_kick (&hwdt) ;


        len = httpserver_read_request_ex (&thread->user, 500,
                        &endpoint, &method) ;

        if (inst->close) {
            break ;

        }
        if (len == -1) {
            timeout -= 500 ;
            if (timeout <= 0) {
                DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_LOG,
                        "WSERV : : connection timeout sock 0x%x.\r\n", thread->user.socket);
                break ;

            }
            if (os_sem_count(&inst->count_sem) <= 2)  {
                DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_LOG,
                    "WSERV : : timeout close thread limit.\r\n");
                break ;

            }
            continue ;

        }
        if (len < 0) {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_LOG,
                    "WSERV : : invalid request %d socket 0x%x.\r\n", len, thread->user.socket);
            break ;

        }
        if (endpoint == 0 ) {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : invalid endpoint %d socket 0x%x.\r\n", len, thread->user.socket);
            break;

        }

        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : request endpoint '%s' socket 0x%x.\r\n", endpoint, thread->user.socket);

        timeout = WSERVER_KEEPALIVE_TIMEOUT ;

#if HTTPSERVER_USE_KEEPALIVE

        if (
                !thread->user.headers[3].value ||
                strnicmp(thread->user.headers[3].value, "Keep-Alive", 10) != 0
            ) {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : connection close no keep-alive connection (%s).\r\n",
                            thread->user.headers[3].value ? thread->user.headers[3].value : "null") ;
            timeout = WSERVER_NO_KEEPALIVE_TIMEOUT ;

        } else {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : connection Keep-Alive threads available %d.\r\n",
                            os_sem_count(&inst->count_sem));

        }
#else
        timeout = WSERVER_NO_KEEPALIVE_TIMEOUT ;
#endif

        const WSERVER_HANDLERS_T* handler = 0 ;
        const WSERVER_HANDLERS_T* default_handler = 0 ;

        for (int i=0; inst->handlers[i].endpoint; i++) {

            handler = &inst->handlers[i] ;
            if (
                    (handler->endpoint_match_len &&
                    (strncmp(handler->endpoint, endpoint, handler->endpoint_match_len) == 0)) ||
                    (strcmp(handler->endpoint, endpoint) == 0)
                ) {
                break ;

            }
            if (handler->endpoint_flags & WSERVER_ENDPOINT_FLAGS_DEFAULT) {
                default_handler = handler ;

            }
            handler = default_handler ;

        }

        if (handler) {

            if (wserver_thread_authenticate(inst, handler, thread) != EOK) {
                continue ;

            }

            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : req '%s'.", handler->endpoint);

            int32_t res = HTTP_SERVER_E_OK ;
            if (handler->headers) {
                const WSERVER_FRAMEWORK* framework = handler->headers ;
                while (*framework) {
                    if ((res = (*framework)(&thread->user, method, endpoint, handler->ctrl)) < HTTP_SERVER_E_OK) {
                        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_ERROR,
                                "WSERV :E: headers for %s failed %d (2).\r\n", endpoint, res);
                        break ;

                    }
                    framework++ ;

                }
                if (res < EOK) {
                    break ;

                }
            }

            if (handler->content) {

                if (handler->endpoint_flags & WSERVER_ENDPOINT_FLAGS_DISABLE_WDT) svc_wdt_deactivate (&hwdt) ;
                res = handler->content (&thread->user, method, endpoint) ;
                if (handler->endpoint_flags & WSERVER_ENDPOINT_FLAGS_DISABLE_WDT) svc_wdt_activate (&hwdt) ;

                if (res < HTTP_SERVER_E_OK) {
                    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_WARNING,
                            "WSERV :E: content for %s failed %d (3).\r\n", endpoint ? endpoint : "", res);
                    break ;

                }
            }

            if (handler->footers) {
                const WSERVER_FRAMEWORK* framework = handler->footers ;
                while (*framework) {
                    if ((res = (*framework)(&thread->user, method, endpoint, handler->ctrl)) < HTTP_SERVER_E_OK) {
                        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_ERROR,
                                "WSERV :E:  footers for %s failed %d (4).\r\n", endpoint, res);
                        break ;

                    }
                    framework++ ;

                }
                if (res < EOK) {
                    break ;

                }

            }

            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : handler complete 0x%x.\r\n", &thread->user);

        } else {
            int32_t res ;
            if ((res = httpserver_write_response (&thread->user, WSERVER_RESP_CODE_404, HTTP_SERVER_CONTENT_TYPE_HTML,
                    0, 0, WSERVER_RESP_CONTENT_404, strlen(WSERVER_RESP_CONTENT_404))) < HTTP_SERVER_E_OK) {
                DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_ERROR,
                        "WSERV :E: write %s WSERVER_RESP_CODE_404 failed %d (5).\r\n", endpoint, res);
                break ;

            }

        }

        if (httpserver_free_request (&thread->user) < 0) {
            break ;

        }

    }

    }

    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
            "WSERV : : complete socket 0x%x read %d write %d.\r\n", thread->user.socket, thread->user.read, thread->user.write);

    httpserver_user_close (&thread->user) ;
    os_sem_signal(&inst->count_sem) ;


    svc_wdt_unregister (&hwdt, TIMEOUT_60_SEC) ;


    return  ;
}

/**
 * @brief Callback function invoked when a client handling thread completes.
 *
 * Cleans up resources associated with the thread after it has finished executing.
 *
 * @param[in] service_thread  Pointer to the service thread structure.
 * @param[in] arg             Pointer to the HTTPSERVER_THREAD_T structure.
 * @param[in] reason          Reason code indicating why the callback was invoked.
 */
void _wserver_thread_complete(SVC_THREADS_T * service_thread, void* arg)
{
#ifndef NDEBUG
    HTTPSERVER_THREAD_T*    thread = (HTTPSERVER_THREAD_T* )arg ;
    HTTPSERVER_INST_T *     inst = thread->inst ;
    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
        "WSERV : : complete (%d)\r\n", os_sem_count(&inst->count_sem)) ;
#endif
    HTTP_SERVER_FREE (service_thread) ;

}


/**
 * @brief   Creates and initializes an HTTP server instance.
 *          Allocates and sets up an HTTPSERVER_INST_T structure with the specified parameters.
 *
 * @param[in] port          Port number the server will listen on.
 * @param[in] ssl           Boolean flag indicating if SSL/TLS should be used.
 * @param[in] handlers      Pointer to an array of request handlers.
 * @param[in] authenticate  Function pointer for handling authentication.
 *
 * @return Pointer to the created HTTPSERVER_INST_T instance, or NULL on failure.
 */
HTTPSERVER_INST_T *
httpserver_wserver_create (uint32_t port, bool ssl, const WSERVER_HANDLERS_T* handlers, WSERVER_AUTHENTICATE authenticate)
{
    HTTPSERVER_INST_T * inst = HTTP_SERVER_MALLOC(sizeof(HTTPSERVER_INST_T)) ;
    if (inst) {
        memset (inst, 0, sizeof(HTTPSERVER_INST_T)) ;
        if (os_sem_create (&inst->count_sem, CFG_UTILS_HTTP_SERVER_THREADS+1) != EOK) {
            HTTP_SERVER_FREE (inst) ;
            return 0 ;
            
        }
        inst->server_sock = -1 ;
        inst->authenticate = authenticate ;
        inst->port = port ;
        inst->ssl = ssl ;
        inst->handlers = handlers ;
        inst->authenticate = authenticate ;

    } 
    return inst ;
}

/**
 * @brief   Destroys and cleans up an HTTP server instance.
 *          Frees resources associated with the server instance.
 *
 * @param[in] inst  Pointer to the HTTP server instance to destroy.
 */
void
httpserver_wserver_destroy (HTTPSERVER_INST_T * inst)
{
    os_sem_delete (&inst->count_sem) ;
    HTTP_SERVER_FREE (inst) ;
}

/**
 * @brief   Runs the HTTP server, accepting and handling client connections.
 *          Enters the main server loop, accepting connections and spawning threads to handle them.
 *
 * @param[in] inst  Pointer to the HTTP server instance.
 *
 * @return HTTP_SERVER_WSERVER_E_OK on server close, or an error code on failure.
 */
int32_t
httpserver_wserver_run (HTTPSERVER_INST_T * inst)
{
    int32_t res ;
    uint32_t i ;

    inst->close = 0 ;
 
    inst->server_sock = httpserver_init (inst->port) ;
    if (inst->server_sock < 0) {
        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_WARNING,
                "WSERV :W: http_server init failed.\r\n");
        return EFAIL ;

    }

    res = httpserver_listen (inst->server_sock) ;
    if (res != HTTP_SERVER_E_OK) {
        httpserver_close (inst->server_sock) ;
        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_WARNING,
                "WSERV :W: http_server listening failed, socket closed.\r\n");
        return res ;

    }

    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_LOG,
            "WSERV : : web server running on port %d%s.\r\n",
            inst->port, inst->ssl ? " with SSL." : " without SSL!!") ;

    for (i=0; inst->handlers[i].endpoint; i++) {
        if ( inst->handlers[i].ctrl)  {
            inst->handlers[i].ctrl (0, 0, 0, WSERVER_CTRL_START) ;
        }
    }

    while (!inst->close) {

        HTTPSERVER_THREAD_T* thread ;

        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                "WSERV : : http_server listening*******\r\n");

        thread = (HTTPSERVER_THREAD_T*)HTTP_SERVER_MALLOC (sizeof(HTTPSERVER_THREAD_T)) ;
        if (thread) {

            memset (thread, 0, sizeof(HTTPSERVER_THREAD_T)) ;
            httpserver_user_init (&thread->user) ;

            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                    "WSERV : : waiting for connections...\r\n");


            do {
                res = httpserver_select (inst->server_sock, 1000) ;
                
                if (res >= 0) {
                    res = httpserver_user_accept (inst->server_sock, &thread->user, 4000) ;
                    if (res == HTTP_SERVER_E_OK) {
                        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                                "WSERV : : incoming connection %d socket 0x%x\r\n",
                                CFG_UTILS_HTTP_SERVER_THREADS - os_sem_count(&inst->count_sem) + 1, thread->user.socket);
                        break ;

                    }

                } 

            } while ((res == HTTP_SERVER_E_ERROR)  && !inst->close) ;

            if (res == HTTP_SERVER_E_CONNECTION) {
                DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_ERROR,
                        "WSERV :E: HTTP_SERVER_E_CONNECTION!\r\n");
                HTTP_SERVER_FREE (thread) ;
                break ;

            }
        } else {
            DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_ERROR,
                    "WSERV :E: HTTP_SERVER_E_MEMORY\r\n");
            res = HTTP_SERVER_E_MEMORY ;
            break ;

        }

        if ((res == HTTP_SERVER_E_OK) && !inst->close) {

            if (os_sem_wait_timeout(&inst->count_sem, OS_MS2TICKS(2000)) == EOK) {

                //svc_system_speed (SYSTEM_SVC_SPEED_SLOW, SYSTEM_SVC_SPEED_REQUESTOR_HTTP) ;

                thread->inst = inst ;
                if (
#if !HTTPSERVER_USER_ACCEPT_IN_THREAD                    
                    (httpserver_user_ssl_accept (&thread->user, 4000) != HTTP_SERVER_E_OK) ||
#endif
                    (svc_threads_create ((SVC_THREADS_T*)thread, _wserver_thread_complete,
                        HTTP_SERVER_USER_THREAD_SIZE, OS_THREAD_PRIO_4, _wserver_thread, (void*)thread, "wuser") != EOK)) {

                    os_sem_signal(&inst->count_sem) ;
                    if (os_sem_count(&inst->count_sem) >= CFG_UTILS_HTTP_SERVER_THREADS +1) {
                        //svc_system_speed (SYSTEM_SVC_SPEED_IDLE, SYSTEM_SVC_SPEED_REQUESTOR_HTTP) ;
                    }

                    //httpserver_write_response (&thread->user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;
                    httpserver_user_close (&thread->user) ;
                    HTTP_SERVER_FREE (thread) ;

                } else {
                    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_INFO,
                            "WSERV : : start thread (%d)...\r\n", CFG_UTILS_HTTP_SERVER_THREADS - os_sem_count(&inst->count_sem) + 1);

                }


            } else {
                    // httpserver_write_response (&thread->user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_HTML, 0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;
                    httpserver_user_close (&thread->user) ;
                    HTTP_SERVER_FREE (thread) ;

            }

        } else {
            HTTP_SERVER_FREE (thread) ;
        }

    }
    httpserver_close (inst->server_sock) ;
    inst->server_sock = -1 ;

    if (os_sem_count(&inst->count_sem) < CFG_UTILS_HTTP_SERVER_THREADS +1) {
        DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_REPORT,
                "WSERV : : waiting for threads to exit...\r\n");

    }

    for (i=0; i<50; i++) {
        if (os_sem_count(&inst->count_sem) >= CFG_UTILS_HTTP_SERVER_THREADS + 1) {
            break;
        }
        os_thread_sleep (100) ;
    }
    //svc_system_speed (SYSTEM_SVC_SPEED_IDLE, SYSTEM_SVC_SPEED_REQUESTOR_HTTP) ;

    for (i=0; inst->handlers[i].endpoint; i++) {
        if ( inst->handlers[i].ctrl)  {
            inst->handlers[i].ctrl (0, 0, 0, WSERVER_CTRL_STOP) ;
        }
    }

    DBG_MESSAGE_HTTP_SERVER (DBG_MESSAGE_SEVERITY_LOG,
            "WSERV : : exit...\r\n");

    return inst->close ? HTTP_SERVER_WSERVER_E_OK : res ;
}

/**
 * @brief   Signals the HTTP server to stop running.
 *          Sets the close flag in the server instance to terminate the main server loop.
 *
 * @param[in] inst  Pointer to the HTTP server instance to stop.
 */
void
httpserver_wserver_stop(HTTPSERVER_INST_T * inst)
{
    inst->close = 1 ;
    //svc_system_speed (SYSTEM_SVC_SPEED_IDLE, SYSTEM_SVC_SPEED_REQUESTOR_HTTP) ;
}

