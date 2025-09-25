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


#include "qoraal/qoraal.h"
#include "qoraal-http/config.h"
#include "qoraal-http/qoraal.h"
#include "qoraal-http/httpdwnld.h"
#include "qoraal-http/httpclient.h"
#include "qoraal-http/httpparse.h"
#include "qoraal-http/network.h"
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
#include "qoraal-http/mbedtls/mbedtlsutils.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>




void
httpdwnld_init (HTTPDWNLD_T * dwnld)
{
    memset (dwnld, 0, sizeof(HTTPDWNLD_T)) ;
    dwnld->mss = 0 ; // registry_get ("net.mss", 0) ;
}

void
httpdwnld_mem_init (HTTPDWNLD_MEM_T * dwnld)
{
    memset (dwnld, 0, sizeof(HTTPDWNLD_MEM_T)) ;
    httpdwnld_init (&dwnld->dwnld) ;
}

#if !defined CFG_LITTLEFS_DISABLE
void
httpdwnld_fs_init (HTTPDWNLD_FS_T * dwnld, lfs_t *   drive, const char * path)
{
    memset (dwnld, 0, sizeof(HTTPDWNLD_FS_T)) ;
    dwnld->drive = drive ;
    dwnld->path = path ;
    httpdwnld_init (&dwnld->dwnld) ;

}
#endif

void
httpdwnld_cancel (HTTPDWNLD_T * dwnld)
{
    dwnld->cancel = 1 ;
}

uint32_t
httpdwnld_bytes (HTTPDWNLD_T * dwnld)
{
    return dwnld->bytes ;
}

/**
 * @brief   httpdwnld_download
 * @details Download a complete file using the callback to add downloaded chunks.

 *
 * @param[in] hostname
 * @param[in] ipv4
 * @param[in] port
 * @param[in] ssl
 * @param[in] mss                       "Maximum Segment Size"
 * @param[in] url
 * @param[in] credentials
 * @param[in] timeout
 * @param[in] cb
 * @param[in] parm
 *
 * @return                              Number of bytes in read or < 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Error occurred during select, possible timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_HEADER         Error parsing HTTP header.
 * @retval HTTP_CLIENT_E_CONTENT        Error reading content, unexpected length.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 * @retval HTTP_CLIENT_E_HOST           Failed to resolve host name.
 * @retval HTTP_CLIENT_E_ERROR          socket error.
 * @retval HTTP_CLIENT_E_SSL_CONNECT    SSL connect error.
 * @retval HTTP_CLIENT_E_SSL_TRUST      SSL certificate error.
 *
 *
    int             https ;
    int             port  ;
    char *          host;
    char *          name  ;
    char *          credentials ;
    char *          postdata ;
    uint32_t        postlen ;
 *
 *
 * @http
 */
int32_t
httpdwnld_download (HTTPDWNLD_T * dwnld,
        uint32_t timeout, HTTP_CLIENT_DOWNLOAD_CB cb, uintptr_t parm)
{
    HTTP_CLIENT_T client ;
    struct sockaddr_in addr  ;
    // int i = 0 ;
    int32_t res ;
    int32_t status ;
    int32_t read = 0 ;
    uint8_t* response ;
    uint32_t ip ;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)dwnld->port);
#if QORAAL_CFG_USE_LWIP
    addr.sin_len = sizeof(addr);
#endif

    if (gethostbyname_timeout (dwnld->host, &ip, 6) != EOK) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                                   "HTTP  : : resolving %s failed!", dwnld->host) ;
        return HTTP_CLIENT_E_HOST ;

    }
    addr.sin_addr.s_addr = /*ntohl*/(ip) ;

    httpclient_init (&client, dwnld->mss) ;

    if (dwnld->host) {
        httpclient_set_hostname (&client, dwnld->host) ;

    }

    void * pssl_config = 0 ;
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    if (dwnld->https) {
        pssl_config = mbedtlsutils_get_client_config () ;
        if (!pssl_config) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                        "HTTP  : : mbedtlsutils_get_client_config failed!") ;
            return HTTP_CLIENT_E_SSL_CONNECT ;

        }

    }
#endif
    res = httpclient_connect (&client, &addr, pssl_config) ;

    if (res != HTTP_CLIENT_E_OK) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                    "HTTP  : : httpclient_connect failed!");
        httpclient_close (&client) ;
        return res ;

    }

    res = cb (1, 0, 0, parm) ;
    if (res != EOK) {
        httpclient_close (&client) ;
        return res ;
    }


    do {



        if (dwnld->postlen) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                        "HTTP  : : POST %s %s", dwnld->host, dwnld->https ? "usinng SSL" : "");
            res = httpclient_post_stream(&client, dwnld->name, dwnld->stream, dwnld->parm, dwnld->postlen, dwnld->credentials) ;

        } else {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                        "HTTP  : : GET %s %s", dwnld->host, dwnld->https ? "usinng SSL" : "");
            res = httpclient_get(&client, dwnld->name, dwnld->credentials) ;

        }

        if ((res > HTTP_CLIENT_E_OK)) {

            res = httpclient_read_response_ex (&client, timeout, &status) ;

            if (status/100 != 2) {
                res = status ? HTTP_CLIENT_E_CONTENT : HTTP_CLIENT_E_CONNECTION ;

            }

            if (res >= 0) {
                res = cb (status, 0, 0, parm) ;

            }

            if (res == EOK) {

                while (1) {
                    res = httpclient_read_next_ex (&client, timeout, &response) ;
                    if (res <= 0) {
                        break ;

                    }
                    read += res ;

                    res = cb (0, response, res, parm) ;
                    if (res != EOK) {
                        break;

                    }

                }

            } else {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                                           "HTTP  : : httpclient_read_response_ex failed %d status %d!",
                                           res, status) ;

            }

            httpclient_read_complete_ex (&client) ;

        } else {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                                       "HTTP  : : httpclient_get failed %d!", res) ;

        }

    } while (0) ;

    httpclient_close (&client) ;

    return res  < 0 ? res : read ;
}


int32_t
httpdwnld_url_parse (char* url, int *https, int *port, char** host,
        char** name, char** credentials)
{
    int32_t res ;
    static char _last_host[128]  ;
    static int  _last_port = 0 ;

    if ((res = httpparse_url_parse (url, https, port, host, name, credentials)) != EOK) {
        return res ;

    }

    if (strlen(*host) > 0) {
        strncpy (_last_host, *host, 128-1) ;
        _last_host[128-1] = '\0';

    }
    else {
        *host = (char*)_last_host ;

    }

    if (*port) {
        _last_port = *port  ;
    } else {
        if (_last_port) {
            *port = _last_port ;

        } else if (*https) {
            *port = 443 ;

        } else {
            *port = 80 ;

        }

    }

    return res ;
}


int32_t
httpdwnld_mem_download_cb (int32_t status, uint8_t * buffer, uint32_t len, uintptr_t parm)
{
    HTTPDWNLD_MEM_T * mem = (HTTPDWNLD_MEM_T *) parm ;
    int32_t res = EOK ;
    int i ;
    char* tmp ;

    if (status/100 == 2) { // connected an open
        return EOK ;

    }

    if (len) {

        mem->dwnld.bytes += len ;
        i = 0 ;
        while (mem->capacity + i < mem->offset + len) {
            i += mem->grow ? mem->grow : 2*1024 ;

        }

        if (i) {

            tmp = HTTP_CLIENT_MALLOC (mem->capacity + i) ;
            if (!tmp) {
                DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: memory failed from heap 0x%x for %d bytes after %d bytes",
                    mem->heap, i, mem->capacity);
                return E_NOMEM ;

            }
            if (mem->mem) {
                memcpy(tmp, mem->mem, mem->offset) ;
                HTTP_CLIENT_FREE (mem->mem) ;

            }

            mem->mem = tmp ;
            mem->capacity += i ;

            }

        if (mem->capacity >= mem->offset + len) {
            memcpy (&mem->mem[mem->offset], buffer, len) ;
            mem->offset += len ;
        } else {
            return E_NOMEM ;

        }

    }

    return  mem->dwnld.cancel ? E_CANCELED : res ;
}

int32_t
httpdwnld_mem_download (char* url, HTTPDWNLD_MEM_T* mem, uint32_t timeout)
{
    int32_t res ;

    HTTPDWNLD_T * dwmld = &mem->dwnld ;

    res = httpdwnld_url_parse (url, &dwmld->https, &dwmld->port, &dwmld->host, &dwmld->name, &dwmld->credentials) ;

    if (res < 0) {
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_LOG,
            "HTTP  : : invalid url") ;
        return res ;

    }

    res = httpdwnld_download (dwmld, timeout, httpdwnld_mem_download_cb, (uintptr_t) mem) ;

    DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_REPORT,
        "HTTP  : : download complete return %d bytes", res);

    return res  ;
}

int32_t
httpdwnld_mem_post (char* url, HTTP_STREAM_NEXT_T stream, uint32_t parm, uint32_t len, HTTPDWNLD_MEM_T* mem, uint32_t timeout)
{
    int32_t res ;

    HTTPDWNLD_T * dwnld = &mem->dwnld ;
    dwnld->stream = stream ;
    dwnld->parm = parm ;
    dwnld->postlen = len ;

    res = httpdwnld_url_parse (url, &dwnld->https, &dwnld->port, &dwnld->host, &dwnld->name, &dwnld->credentials) ;

    if (res < 0) {
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_LOG,
            "HTTP  : : invalid url") ;
        return res ;

    }

    res = httpdwnld_download (dwnld, timeout, httpdwnld_mem_download_cb, (uintptr_t) mem) ;


    DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_LOG,
        "HTTP  : : '%s' complete %d", dwnld->name, res);


    return res  ;

}

#if !defined CFG_LITTLEFS_DISABLE
int32_t
httpdwnld_fs_download_cb (int32_t status, uint8_t * buffer, uint32_t len, uintptr_t parm)
{
    HTTPDWNLD_FS_T * fs = (HTTPDWNLD_FS_T *) parm ;
    int32_t res = EOK ;
    //int i ;
    //char* tmp ;
    lfs_ssize_t writelen ;
    //UINT bw ;

    if (status/100 == 2) { // connected an open

        if (fs->drive && fs->path) {
            res = lfs_file_open(fs->drive, &fs->f, fs->path, LFS_O_CREAT|LFS_O_TRUNC|LFS_O_WRONLY);
            if (res == LFS_ERR_OK) {
                fs->opened = 1 ;

            } else {
                DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  : : fail open '%s' with %d", fs->path, res);
                res = E_FILE ;

            }
        } else {
            // fs->opened = 1 ;
        }

    } else if (status/100 > 0) {
        res = E_NOTFOUND ;

    }

    if (len) {
        if (fs->capacity && (fs->capacity < fs->dwnld.bytes)) {
            return E_FULL ;

        }

        //res = f_write (&fs->f, buffer, len, &bw);
        if (fs->opened) {
            writelen = lfs_file_write(fs->drive, &fs->f,
                buffer, len);
        } else {
            writelen = len ;
        }

        if (writelen < 0) {
            DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_ERROR,
                "HTTP  : : fail writing '%s' with %d after %d bytes",
                fs->path, writelen, fs->dwnld.bytes);
            res = EFAIL ;
        } else {

            fs->dwnld.bytes += writelen ;
            if (!(fs->dwnld.cnt%25)) {
                DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_REPORT|DBG_MESSAGE_FLAG_PROGRESS,
                        "HTTP  : : '%s' %d bytes downloaded...\r\n", fs->dwnld.name, fs->dwnld.bytes);

            }
            fs->dwnld.cnt++ ;

        }




    }

    return  fs->dwnld.cancel ? E_CANCELED : res ;
}

int32_t
httpdwnld_fs_download (char* url, HTTPDWNLD_FS_T* fs, uint32_t timeout)
{
    int32_t res ;

    HTTPDWNLD_T * dwmld = &fs->dwnld ;

   res = httpdwnld_url_parse (url, &dwmld->https, &dwmld->port, &dwmld->host, &dwmld->name, 0) ;

    if (res < 0) {
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_LOG,
            "HTTP  : : invalid url") ;
        return res ;

    }

    res = httpdwnld_download (dwmld, timeout, httpdwnld_fs_download_cb, (uintptr_t) fs) ;

    if (fs->opened) {
        lfs_file_close(fs->drive, &fs->f);

    }

    if (res < 0) {
//      lfs_remove(fs_drives_get(fs->drive), fs->filename);
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_ERROR,
            "HTTP  : : downloading '%s' failed %d", dwmld->name, res);

    } else {
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_REPORT,
            "HTTP  : : downloading '%s' completed", dwmld->name);


    }

    return res  ;
}
#endif

int32_t
httpdwnld_test_download_cb (int32_t status, uint8_t * buffer, uint32_t len, uintptr_t parm)
{
    int32_t res = EOK ;
    HTTPDWNLD_T * dwnld = (HTTPDWNLD_T *) parm ;

    if (status/100 == 2) { // connected an open

        return EOK ;
    }

    if (len) {

        dwnld->bytes += len ;
        if (!(dwnld->cnt%25)) {
            DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_REPORT|DBG_MESSAGE_FLAG_PROGRESS,
                    "HTTP  : : '%s' %d bytes downloaded...\r\n", dwnld->name, dwnld->bytes);

        }
        dwnld->cnt++ ;
#if 0
        uint32_t r = nhal_rand31();
        os_thread_sleep(r%100) ;
#endif

    }

    return  dwnld->cancel ? E_CANCELED : res ;

}

int32_t
httpdwnld_test_download (char* url, HTTPDWNLD_T* dwmld, uint32_t timeout)
{
    int32_t res ;

   res = httpdwnld_url_parse (url, &dwmld->https, &dwmld->port, &dwmld->host, &dwmld->name, &dwmld->credentials) ;

    if (res < 0) {
        DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_LOG,
            "HTTP  : : invalid url") ;
        return res ;

    }

    res = httpdwnld_download (dwmld, timeout, httpdwnld_test_download_cb, (uintptr_t) dwmld) ;


    DBG_MESSAGE_HTTPDWNLD (DBG_MESSAGE_SEVERITY_REPORT,
        "HTTP  : : '%s' complete return %d bytes", dwmld->name, res);


    return res  ;

}




