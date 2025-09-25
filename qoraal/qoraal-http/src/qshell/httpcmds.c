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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal-http/qoraal.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-http/httpdwnld.h"
#include "qoraal-http/httpparse.h"


SVC_SHELL_CMD_DECL("wsource", qshell_wsource, "<url>");
#ifdef CFG_OS_POSIX
SVC_SHELL_CMD_DECL("wget", qshell_wget, "<url>");
#endif


static int32_t
qshell_wsource (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res ;
    HTTPDWNLD_MEM_T script ;

    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS ;
        
    }

    httpdwnld_mem_init (&script) ;
    script.grow = 8*1024 ;
    script.heap = QORAAL_HeapAuxiliary ;

    res = httpdwnld_mem_download (argv[1], &script, 4000) ;
    if (res < 0) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "ERROR: script downloading %s failed with %d!\r\n",
                script.dwnld.name, res) ;
        qoraal_free(script.heap, script.mem) ;

    } else  {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "script download %d bytes for %s\r\n",
                res, script.dwnld.name) ;

        svc_shell_script_clear_last_error (pif) ;
        res = svc_shell_script_run (pif, script.dwnld.name, script.mem, script.offset) ;

        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "script %s complete with %d\r\n",
                script.dwnld.name, res) ;

        qoraal_free(script.heap, script.mem) ;

    }

    return res >= EOK ? SVC_SHELL_CMD_E_OK : res ;

}

#ifdef CFG_OS_POSIX
int resolve_hostname(const char *hostname, uint32_t *ip) {
    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM; // TCP connection

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        return HTTP_CLIENT_E_HOST; // Failed to resolve
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    *ip = addr->sin_addr.s_addr;

    freeaddrinfo(res);
    return EOK;
}


int32_t qshell_wget(SVC_SHELL_IF_T *pif, char **argv, int argc) 
{
    HTTP_CLIENT_T client;
    int32_t status;
    uint8_t *response;
    int32_t res;
    uint32_t ip;
    struct sockaddr_in addr;
    int https, port;
    char *host, *path, *credentials;
    FILE *file = NULL;

    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS;
    }

    // Parse the URL
    res = httpparse_url_parse(argv[1], &https, &port, &host, &path, &credentials);
    if (res != EOK) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "Failed to parse URL: %s\n", argv[1]);
        return res;
    }

    // Extract the filename
    const char *filename = "index.html" ;
    if (path) {
        filename = strrchr(path, '/');
        filename = (filename && *(filename + 1)) ? filename + 1 : path; 
    }

    // Open file for writing
    file = fopen(filename, "wb");
    if (!file) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "Failed to open file: %s\n", filename);
        return -1;
    }

    // Resolve hostname
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (resolve_hostname(host, &ip) != EOK) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "HTTP  : : resolving %s failed!\n", host);
        fclose(file);
        return HTTP_CLIENT_E_HOST;
    }
    addr.sin_addr.s_addr = ip;

    // initialise the client
    httpclient_init (&client, 0) ;
    // for name-based virtual hosting
    httpclient_set_hostname (&client, host) ; 
    // Connect to the server

    void * pssl_config = 0 ;
#if !defined CFG_HTTPCLIENT_TLS_DISABLE    
    if (https) {
        pssl_config = mbedtlsutils_get_client_config () ;
        if (!pssl_config) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD, 
                        "ssl config failed!") ;
            return HTTP_CLIENT_E_SSL_CONNECT ;

        }

    }
#endif

    res = httpclient_connect(&client, &addr, pssl_config);
    if (res != HTTP_CLIENT_E_OK) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "Failed to connect to server\n");
        fclose(file);
        httpclient_close(&client);
        return res;
    }

    // Send GET request
    res = httpclient_get(&client, path, credentials);
    if (res < 0) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, "GET %s failed\n", path);
        fclose(file);
        httpclient_close(&client);
        return res;
    }

    // Read response and headers
    res = httpclient_read_response_ex(&client, 5000, &status);
    if (res < 0 || status / 100 != 2) {
        svc_shell_print(pif, SVC_SHELL_OUT_STD, 
                "Failed to read response status %d result %d\n", status, res);
        if (res < 0) {
            fclose(file);
            httpclient_close(&client);
            return res;
        }
    }

    // Read response body and write to file
    while ((res = httpclient_read_next_ex(&client, 5000, &response)) > 0) {
        fwrite(response, 1, res, file);
    }

    // Clean up
    fclose(file);
    httpclient_close(&client);

    svc_shell_print(pif, SVC_SHELL_OUT_STD, "Download complete: %s\n", filename);
    
    return res >= EOK ? SVC_SHELL_CMD_E_OK : res;
}
#endif


void
keep_httpcmds (void)
{
    (void)qshell_wsource ; 
#ifdef CFG_OS_POSIX
    (void)qshell_wget ;
#endif
}
