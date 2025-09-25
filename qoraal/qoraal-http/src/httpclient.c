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


#include <string.h>
#include <stdio.h>
#include "qoraal/qoraal.h"
#include "qoraal-http/config.h"
#include "qoraal-http/qoraal.h"
#include "qoraal-http/httpclient.h"
#include "qoraal-http/httpparse.h"
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
#include "qoraal-http/mbedtls/mbedtlsutils.h"
#endif

#define HTTP_GET_REQ            "GET /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "\r\n"

#define HTTP_GET_REQ_BASIC      "GET /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "Authorization: Basic %s\r\n" \
                                "\r\n"

#define HTTP_POST_REQ           "POST /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Content-Type: text/plain\r\n" \
                                "X-Content-Type-Options: nosniff" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "Content-Length: %d\r\n" \
                                "\r\n"

#define HTTP_POST_CHUNKED_REQ   "POST /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Content-Type: text/plain\r\n" \
                                "X-Content-Type-Options: nosniff" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "Transfer-Encoding: chunked\r\n" \
                                "\r\n"

#define HTTP_POST_REQ_BASIC     "POST /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Content-Type: text/plain\r\n" \
                                "X-Content-Type-Options: nosniff" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "Authorization: Basic %s\r\n" \
                                "Content-Length: %d\r\n" \
                                "\r\n"

#define HTTP_POST_CHUNKED_REQ_BASIC     "POST /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Content-Type: text/plain\r\n" \
                                "X-Content-Type-Options: nosniff" \
                                "Accept: */*\r\n" \
                                "Connection: keep-alive\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                "Authorization: Basic %s\r\n" \
                                "Transfer-Encoding: chunked\r\n" \
                                "\r\n"

#define HTTP_WEBSOCKET_REQ      "GET /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Sec-WebSocket-Key: %s\r\n" \
                                "Upgrade: websocket\r\n" \
                                "Connection: Upgrade\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                /*"Origin: http://example.com\r\n"*/ \
                                "Sec-WebSocket-Protocol: " HTTP_CLIENT_WEBSOCKET_PROTOCOL "\r\n" \
                                "Sec-WebSocket-Version: 13\r\n" \
                                "\r\n"

#define HTTP_WEBSOCKET_REQ_BASIC "GET /%s HTTP/1.1\r\n" \
                                "Host: %s\r\n" \
                                "Sec-WebSocket-Key: %s\r\n" \
                                "Upgrade: websocket\r\n" \
                                "Connection: Upgrade\r\n" \
                                "User-Agent: " HTTP_CLIENT_AGENT_NAME "\r\n" \
                                /*"Origin: http://example.com\r\n"*/ \
                                "Sec-WebSocket-Protocol: " HTTP_CLIENT_WEBSOCKET_PROTOCOL "\r\n" \
                                "Sec-WebSocket-Version: 13\r\n" \
                                "Authorization: Basic %s\r\n" \
                                "\r\n"


/**
 * @brief   httpclient_init
 * @details The instance 'client' to be used in all calls from this API is initialised to zero.
 *
 * @param[in] client
 *
 * @return                          The function status.
 * @retval HTTP_CLIENT_E_OK         client was initialised
 *
 * @http
 */
int32_t
httpclient_init (HTTP_CLIENT_T* client, int32_t mss)
{
    memset (client, 0, sizeof (HTTP_CLIENT_T)) ;
    client->mss = mss ;
    client->socket = -1 ;

    return HTTP_CLIENT_E_OK ;
}

/**
 * @brief   httpclient_connect
 * @details Connect a TCP/IP socket to the address. If SSL is required, the
 *          SSL connection is also initialised and the handshaking performed.
 *
 * @param[in] client
 * @param[in] addr
 * @param[in] use_ssl
 *
 * @return                              The function status.
 * @retval HTTP_CLIENT_E_OK             client was connected.
 * @retval HTTP_CLIENT_E_ERROR          socket error.
 * @retval HTTP_CLIENT_E_SSL_CONNECT    SSL connect error.
 * @retval HTTP_CLIENT_E_SSL_TRUST      SSL certificate error.
 *
 * @http
 */
int32_t
httpclient_connect (HTTP_CLIENT_T* client, const struct sockaddr_in* addr, void * ssl_config)
{
    int32_t status ;
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    mbedtls_ssl_config * pssl_config = (mbedtls_ssl_config *)ssl_config ;
#endif

    DBG_CHECK_HTTP_CLIENT (client->socket == -1, EFAIL,
                    "HTTP  :E: httpclient_connect unexpected\r\n!") ;

    client->socket = socket (AF_INET, SOCK_STREAM, 0) ;

    if (client->socket < 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                    "HTTP  :E: failed to create socket");
        client->socket = -1 ;
        return HTTP_CLIENT_E_ERROR;
    }

#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    if (pssl_config) {
        if (!client->ssl) {
            client->ssl = HTTP_CLIENT_MALLOC(sizeof(mbedtls_ssl_context)) ;
            if (!client->ssl) {
                return E_NOMEM ;

            }
            mbedtls_ssl_init((mbedtls_ssl_context *)client->ssl) ;
            if (mbedtls_ssl_setup( (mbedtls_ssl_context *)client->ssl, pssl_config) != 0) {
                HTTP_CLIENT_FREE(client->ssl) ;
                client->ssl = 0 ;
                return E_NOMEM ;

            }
        }
        mbedtls_ssl_set_bio( (mbedtls_ssl_context *)client->ssl, (void*)client->socket,
                mbedtls_net_send, mbedtls_net_recv, 0 /*mbedtls_net_recv_timeout*/ )  ;

        mbedtls_ssl_set_hostname ((mbedtls_ssl_context *)client->ssl, client->hostname) ;

    }
#endif

    inet_ntop (AF_INET, &addr->sin_addr, client->host, MAX_IPv4_STR_LEN) ;

    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : connect to %s:%d (%d) using %s",
                    client->host, (int)ntohs((uint16_t)addr->sin_port), 
                    client->socket, ssl_config ? "SSL" : "NO SSL");

#if 0
    if (client->mss > 0) {
        int mss  = client->mss ;
        status = setsockopt(client->socket, SOL_SOCKET, TCP_MAXSEG, (uint8_t*)&mss, sizeof(int) );
        if (status != A_OK) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
                            ("HTTP  :W: failed setting TCP_MAXSEG")) ;

        }
    }
#endif

    status = connect (client->socket, (const struct sockaddr *) addr, sizeof(struct sockaddr_in));
    if (status != 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
                    "HTTP  :W: t_connect to %s:%d failed %d!",
                    client->host,(int)ntohs((uint16_t)addr->sin_port), status);
        closesocket(client->socket);
        return HTTP_CLIENT_E_CONNECTION ;

    }
#if 0
    unsigned long flags = 1;
    status = ioctlsocket (client->socket, FIONBIO, &flags) ;
    if (status != 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
                    "HTTP  :W: setting FIONBIOfailed %d!", status);
        closesocket(client->socket);
        return HTTP_CLIENT_E_CONNECTION;
    }
#endif
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    if (client->ssl) {
        do {
            struct fd_set   fdread;
            struct fd_set   fdex;
            struct timeval tv;

            status = mbedtls_ssl_handshake((mbedtls_ssl_context *)client->ssl);

            if (status == MBEDTLS_ERR_SSL_WANT_READ ||
                status == MBEDTLS_ERR_SSL_WANT_WRITE ) {

                FD_ZERO(&fdread) ;
                FD_ZERO(&fdex) ;
                FD_SET(client->socket, &fdread);
                FD_SET(client->socket, &fdex);
                tv.tv_sec = 8 ;
                tv.tv_usec = 0 ;

                int received = select(client->socket+1, &fdread, 0, &fdex, &tv) ;
                (void) received ;

                if (!FD_ISSET(client->socket, &fdread)) {
                       DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                                    "HTTP  : : read select failed with %d", received);
                        return HTTP_CLIENT_E_CONNECTION ;

                }
                else if (FD_ISSET(client->socket, &fdex)) {
                    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                                "HTTP  : : read exception on listening socket...");
                    return HTTP_CLIENT_E_CONNECTION ;

                }
            }

            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                "ongoing mbedtls_ssl_handshake() returned -0x%04X", -status);

        } while(status == MBEDTLS_ERR_SSL_WANT_READ ||
                status == MBEDTLS_ERR_SSL_WANT_WRITE ||
                status == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS ||
                status == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS);

        if (status < 0) {
            if (status != MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE) {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
                    "mbedtls_ssl_handshake() returned -0x%04X", -status);

            }
            return HTTP_CLIENT_E_CONNECTION ;

        }

    }
#endif
    return EOK ;

}

/**
 * @brief   httpclient_set_hostname
 * @details Set the hostname to be used in the http request headers. Is not set
 *          the IP address is used.
 * @notes   Typically used for virtual servers.
 *
 * @param[in] client
 * @param[in] hostname

 *
 * @return                              The function status.
 * @retval HTTP_CLIENT_E_OK
 *
 * @http
 */
int32_t
httpclient_set_hostname (HTTP_CLIENT_T* client, const char* hostname)
{
    strncpy (client->hostname, hostname, MAX_HOSTNAME_LEN-1) ;
    client->hostname[MAX_HOSTNAME_LEN-1] = '\0';

    return HTTP_CLIENT_E_OK ;
}

/**
 * @brief   httpclient_is_connected
 * @details Simply do a _t_select() with 0 timeout to see if the socket was closed..
 *
 * @param[in] client
 *
 * @return                              1 if the connection is valid, zero otherwise.
 *
 * @http
 */
int32_t
httpclient_is_connected (HTTP_CLIENT_T* client)
{
     fd_set   fdread;
    struct timeval tv;

    if (client->socket < 0) {
        return 0 ;
    }
    FD_ZERO(&fdread) ;
    FD_SET(client->socket, &fdread);
    tv.tv_sec = 0 ;
    tv.tv_usec = 0 ;

    if (select(client->socket+1, &fdread, 0, 0, &tv) < 0) {
        return 0 ;
    }

    return 1 ;
}


/**
 * @brief   httpclient_close
 * @details Closes connection including SSL if required and frees accosted with 'client'
 *
 * @param[in] client
 *
 * @http
 */
void
httpclient_close (HTTP_CLIENT_T* client)
{
    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : closesocket sock 0x%x", client->socket) ;

#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    if (client->ssl) {
        mbedtls_ssl_close_notify ((mbedtls_ssl_context *)client->ssl) ;
    }
#endif
    if (client->socket>=0) {
        // close (client->socket) ;
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
        if (client->ssl) {
            mbedtls_ssl_free ((mbedtls_ssl_context *)client->ssl) ;
            HTTP_CLIENT_FREE(client->ssl) ;
            client->ssl = 0 ;

        }
#endif
        closesocket (client->socket) ;
        client->socket = -1 ;

    }
}


/**
 * @brief httpclient_write
 *
 * @param[in] client
 * @param[in] buffer
 * @param[in] length
 *
 * @return                              Number of bytes sent.
 * @retval HTTP_CLIENT_E_ERROR          error occured during send.
 *
 * @http
 */
static int32_t
httpclient_write (HTTP_CLIENT_T* client, uint8_t* buffer, uint32_t length, uint32_t timeout)
{
    int32_t sent_bytes = 0;
    uint32_t total = 0 ;

    setsockopt (client->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    while (length) {
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
        if (client->ssl) {
            sent_bytes = mbedtls_ssl_write ((mbedtls_ssl_context *)client->ssl, (unsigned char*)&buffer[total], length) ;
        } else
#endif
        {
            sent_bytes = send (client->socket, (unsigned char*)&buffer[total], length, 0);

        }

        if (sent_bytes <= 0) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "httpserver_write : ERROR: socket 0x%x sent_bytes %d", client->socket, sent_bytes);
            return HTTP_CLIENT_E_CONNECTION ;

        } else {
            total += sent_bytes ;
            if (length >= (unsigned int)sent_bytes) length -= sent_bytes ;
            else break ;

        }

    }

    return total ;
}

/**
 * @brief httpclient_write
 *
 * @param[in] client
 * @param[in] buffer
 * @param[in] length
 *
 * @return                              Number of bytes sent.
 * @retval HTTP_CLIENT_E_ERROR          error occured during send.
 *
 * @http
 */
static int32_t
httpclient_write_chunked (HTTP_CLIENT_T* client, uint8_t* buffer, uint32_t length, uint32_t timeout)
{
    int32_t sent_bytes = 0;
    unsigned int i ;
    uint32_t total = 0 ;
    char chunkhead[16] ;

    setsockopt (client->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    i = snprintf (chunkhead, 16, "%x\r\n", (unsigned int)length) ;
    send (client->socket, chunkhead, i, 0);

    while (length) {

#if !defined CFG_HTTPCLIENT_TLS_DISABLE
        if (client->ssl) {
            sent_bytes = mbedtls_ssl_write ((mbedtls_ssl_context *)client->ssl,
            		(unsigned char*)&buffer[total], length) ;
        } else
#endif
        {
            sent_bytes = send (client->socket, (unsigned char*)&buffer[total], length, 0);
        }

        if (sent_bytes <= 0) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "httpserver_write : ERROR: socket 0x%x sent_bytes %d",
					client->socket, sent_bytes);
            return HTTP_CLIENT_E_CONNECTION ;

        } else {
            total += sent_bytes ;
            if (length >= (unsigned int)sent_bytes) length -= sent_bytes ;
            else break ;

        }
    }
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
        if (client->ssl) {
            sent_bytes = mbedtls_ssl_write ((mbedtls_ssl_context *)client->ssl,
            			(const unsigned char *)"\r\n", 2) ;
        } else
#endif
    {
        send (client->socket, "\r\n", 2, 0);
    }
    return total ;
}

static int32_t
socket_send(uint32_t ctx, const void* data, int32_t len)
{
    return send ((int)ctx, data, len, 0);
}

#if !defined CFG_HTTPCLIENT_TLS_DISABLE
static int32_t
ssl_socket_send(uint32_t ctx, const  void* data, int32_t len)
{
    return mbedtls_ssl_write ((mbedtls_ssl_context *)ctx, data, len);
}
#endif

/**
 * @brief httpclient_write
 *
 * @param[in] client
 * @param[in] buffer
 * @param[in] length
 *
 * @return                              Number of bytes sent.
 * @retval HTTP_CLIENT_E_ERROR          error occured during send.
 *
 * @http
 */
static int32_t
httpclient_write_stream (HTTP_CLIENT_T* client, HTTP_STREAM_NEXT_T stream, uint32_t parm, int32_t len, uint32_t timeout)
{
    int32_t sent_bytes = 0;
    int32_t total = 0 ;

    setsockopt (client->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

    do {
#if !defined CFG_HTTPCLIENT_TLS_DISABLE
        if (client->ssl) {
            sent_bytes = stream((HTTP_STREAM_WRITE)ssl_socket_send,
                    (uint32_t) client->ssl, parm, len-total) ;

        } else
#endif
        {
            sent_bytes = stream((HTTP_STREAM_WRITE)socket_send,
                    (uint32_t) client->socket, parm, len-total) ;
 
         }

        if (sent_bytes <= 0) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "httpserver_write : ERROR: socket 0x%x sent_bytes %d", 
                    client->socket, sent_bytes);
            return HTTP_CLIENT_E_CONNECTION ;

        } else {
            total += sent_bytes ;

        }
    } while (total < len) ;

    return total ;
}

static int32_t
httpclient_wait_read(HTTP_CLIENT_T* client, uint32_t timeout)
{
    int32_t res ;
    fd_set   fdread;
    fd_set   fdex;
    struct timeval tv;

    FD_ZERO(&fdread) ;
    FD_SET(client->socket, &fdread);
    FD_ZERO(&fdex) ;
    FD_SET(client->socket, &fdex);
    tv.tv_sec = timeout/1000 ;
    tv.tv_usec = (timeout % 1000) * 1000 ;

    res = select(client->socket+1, &fdread, 0, &fdex, &tv) ;
    (void) res ;

    if (FD_ISSET(client->socket, &fdread)) {
        return EOK ;

    }
    if (FD_ISSET(client->socket, &fdex)) {
           DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                        "HTTP  : : read select failed with exception");
            return HTTP_CLIENT_E_CONNECTION ;

    }

    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                    "HTTP  : : read select failed with %d after %d", res, timeout);

    return HTTP_CLIENT_E_ERROR ;
}

/**
 * @brief httpclient_read
 *
 * @param[in] client
 * @param[in] buffer
 * @param[in] length
 *
 * @return                              Number of bytes read.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during read.
 *
 * @http
 */
static int32_t
httpclient_read (HTTP_CLIENT_T* client, void* buffer, uint32_t length, uint32_t timeout)
{
    int32_t received = EOK ;

#if !defined CFG_HTTPCLIENT_TLS_DISABLE
    if (client->ssl) {
        if (!mbedtls_ssl_get_bytes_avail ((mbedtls_ssl_context *)client->ssl)) {
            received = httpclient_wait_read (client, timeout) ;
        }
        while (received == EOK) {
            received = mbedtls_ssl_read ((mbedtls_ssl_context *)client->ssl, buffer, length) ;
            if (received == MBEDTLS_ERR_SSL_WANT_READ) {
                received = httpclient_wait_read (client, timeout) ;
            } else {
                break ;
            }

        }
    } else
#endif
    {
        received = httpclient_wait_read (client, timeout) ;
        if (received == EOK) {
            received = recv (client->socket, buffer, length, 0);

        }

    }

    if(received <= 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                "HTTP  : : httpclient_read  %d (-0x%x)", received, -received);

    }

    return received ;
}

/**
 * @brief   httpclient_get
 * @details Starts a HTTP GET request. The response should be read with httpclient_read_response.
 * @note    Buffer including the HTTP is limited to a size of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH (1280).
 *          Fixed: Fix the buffer size limitation.
 *
 * @param[in] client
 * @param[in] endpoint
 * @param[in] credentials               username:password
 *
 * @return                              The number of bytes sent or if < 0 it indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_get (HTTP_CLIENT_T* client, const char* endpoint, const char *credentials)
{
    int32_t         sent_bytes = 0;
    int32_t         count ;
    char*           buffer = 0;
    char* host ;

    DBG_CHECK_HTTP_CLIENT (client->socket>=0, EFAIL,
                    "HTTP  :E: unexpected - socket not valid!") ;

    if(!httpclient_is_connected(client)) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                    "HTTP  : : socket closed!");
        return HTTP_CLIENT_E_CONNECTION ;

    }

    buffer = HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH);
    if(buffer == NULL) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: Memory allocation failed");
        return HTTP_CLIENT_E_MEMORY ;
    }

    if (client->hostname[0]) {
        host = client->hostname ;

    } else {
        host = client->host ;

    }

    if (credentials == 0) {
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH,
                            HTTP_GET_REQ, endpoint ? endpoint : "", host);

    } else {
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH,
                            HTTP_GET_REQ_BASIC, endpoint ? endpoint : "", host,
                            base64_encode(credentials, strlen(credentials),
                            buffer + HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH - BASE64_OUTPUT_LENGTH(strlen(credentials)),
                            BASE64_OUTPUT_LENGTH(strlen(credentials))));

    }

    if (count >= HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH to small for HTTP headers!");
        HTTP_CLIENT_FREE (buffer) ;
        return HTTP_CLIENT_E_LENGTH ;

    }

    sent_bytes = httpclient_write(client, (unsigned char*)buffer , count, 8000);
    HTTP_CLIENT_FREE (buffer) ;

    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : send %d bytes", sent_bytes);

    return sent_bytes ;
}


/**
 * @brief   httpclient_post
 * @details Starts a HTTP POST request. The response should be read with httpclient_read_response().
 * @note    Buffer including the HTTP is limited to a size of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH (1280).
 *          Fixed: Fix the buffer size limitation.
 *
 * @param[in] client
 * @param[in] endpoint
 * @param[in] reponse
 *
 * @return                              The number of bytes sent or if < 0 it indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_post_start (HTTP_CLIENT_T* client, const char* endpoint, int32_t len, const char *credentials, uint32_t chunked)
{
    int32_t         sent_bytes = 0;
    int32_t         count ;
    char*           buffer = 0;
    char* host ;

    DBG_CHECK_HTTP_CLIENT (client->socket>=0, EFAIL,
                    "HTTP  :E: unexpected - socket not valid!") ;

    if(!httpclient_is_connected(client)) {
         DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_LOG,
                    "HTTP  : : socket closed!");
        return HTTP_CLIENT_E_CONNECTION ;

    }

    buffer = HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH);
    if(buffer == NULL) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: Memory allocation failed");
        return HTTP_CLIENT_E_MEMORY ;

    }

    if (client->hostname[0]) {
        host = client->hostname ;

    } else {
        host = client->host ;

    }

    if (credentials == 0) {
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH,
                            chunked ? HTTP_POST_CHUNKED_REQ : HTTP_POST_REQ,
                            endpoint ? endpoint : "", host, (int)len);

    } else {
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH,
                            chunked ? HTTP_POST_CHUNKED_REQ_BASIC : HTTP_POST_REQ_BASIC,
                            endpoint ? endpoint : "", host,
                            base64_encode(credentials, strlen(credentials),
                            buffer + HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH - BASE64_OUTPUT_LENGTH(strlen(credentials)),
                            (int)BASE64_OUTPUT_LENGTH(strlen(credentials))),
                            (int)len);

    }

    if (count <= 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: header error");
        HTTP_CLIENT_FREE (buffer) ;
        return HTTP_CLIENT_E_HEADER ;

    }

    if (count >= HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH to small for HTTP headers!");
        HTTP_CLIENT_FREE (buffer) ;
        return HTTP_CLIENT_E_LENGTH ;

    }

    sent_bytes = httpclient_write(client, (unsigned char*)buffer, count, 8000);
    HTTP_CLIENT_FREE (buffer) ;
    if (sent_bytes < count) {
        return HTTP_CLIENT_E_CONNECTION ;

    }

    return sent_bytes ;
}


/**
 * @brief   httpclient_post
 * @details Starts a HTTP POST request. The response should be read with httpclient_read_response().
 * @note    Buffer including the HTTP is limited to a size of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH (1280).
 *          Fixed: Fix the buffer size limitation.
 *
 * @param[in] client
 * @param[in] endpoint
 * @param[in] reponse
 *
 * @return                              The number of bytes sent or if < 0 it indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_post (HTTP_CLIENT_T* client, const char* endpoint, const char* request, int32_t len, const char *credentials)
{
    int32_t status = httpclient_post_start (client, endpoint, len, credentials, 0) ;

    if (status >= 0) {
        status = httpclient_write(client, (unsigned char*)request, len, 8000);

    }

    return status ;
}

/**
 * @brief   httpclient_post
 * @details Starts a HTTP POST request. The response should be read with httpclient_read_response().
 * @note    Buffer including the HTTP is limited to a size of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH (1280).
 *          Fixed: Fix the buffer size limitation.
 *
 * @param[in] client
 * @param[in] endpoint
 * @param[in] reponse
 *
 * @return                              The number of bytes sent or if < 0 it indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_post_chunked (HTTP_CLIENT_T* client, const char* endpoint, const char* request, int32_t len, const char *credentials)
{
    int32_t status = httpclient_post_start (client, endpoint, len, credentials, 1) ;

    if ((status >= 0) && (len > 0)) {
        status = httpclient_write_chunked(client, (unsigned char*)request, len, 8000);

    }
    status = httpclient_write_chunked(client, (unsigned char*)0, 0, 8000);

    return status ;
}


/**
 * @brief   httpclient_post_stream
 * @details Starts a HTTP POST request. The response should be read with httpclient_read_response().
 * @note    Buffer including the HTTP is limited to a size of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH (1280).
 *          Fixed: Fix the buffer size limitation.
 *
 * @param[in] client
 * @param[in] endpoint
 * @param[in] reponse
 *
 * @return                              The number of bytes sent or if < 0 it indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_post_stream (HTTP_CLIENT_T* client, const char* endpoint, HTTP_STREAM_NEXT_T stream, uint32_t parm, int32_t len, const char *credentials)
{
    int32_t status = httpclient_post_start (client, endpoint, len, credentials, 0) ;

    if (status >= 0) {
        status = httpclient_write_stream(client, stream, parm, len, 8000) ;

    }

    return status ;
}

/**
 * @brief   httpclient_free_response
 *
 * @param[in] client
 * @param[out] response
 *
 *
 * @http
 */
void
httpclient_free_response (HTTP_CLIENT_T* client, void* response)
{
    if (response) {
        HTTP_CLIENT_FREE (response) ;
        
    }
}

/**
 * @brief   httpclient_read_response
 * @details Reads the response from a get or a post.
 *          In case of an error there might be data in the response. In this
 *          case, if the response is not 0 upon return it should always be
 *          freed by the function httpclient_free_response().
 * @note
 *
 * @param[in] client
 * @param[in] timeout
 * @param[out] response
 * @param[out] status                   HTTP status, like 200 for OK
 *
 * @return                              Number of bytes in response or < 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Error occurred during select, possible timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_HEADER         Error parsing HTTP header.
 * @retval HTTP_CLIENT_E_CONTENT        Error reading content, unexpected length.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_read_response (HTTP_CLIENT_T* client, uint32_t timeout, char** response, int32_t* status)
{
    int32_t         received ;
    char*           content ;
    char*           payload ;
    unsigned int    offset = 0 ;
    int             chunks = 1 ;
    unsigned int    content_length = 0 ;
    char*           header ;
    HTTP_HEADER_T headers[] = {
            {HTTP_HEADER_KEY_CONTENT_TYPE, 0},
            {HTTP_HEADER_KEY_CONTENT_LENGTH, 0},
            {HTTP_HEADER_KEY_TRANSFER_ENCODING, 0},
            {HTTP_HEADER_KEY_AUTHENTICATE, 0},
    } ;

    DBG_CHECK_HTTP_CLIENT (client->socket>=0, EFAIL,
                    "HTTP  :E: unexpected - socket not valid!") ;
    DBG_CHECK_HTTP_CLIENT (response, EFAIL,
                    "HTTP  :E: unexpected - response not valid!") ;
    DBG_CHECK_HTTP_CLIENT (status, EFAIL,
                    "HTTP  :E: unexpected - status not valid!") ;

    *response = 0 ;
    *status = 0 ;

    header = (char*)HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_RECV_HEADER_LENGTH) ;
    if (header == 0) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
            "HTTP  : : out of memory ");
        return HTTP_CLIENT_E_MEMORY ;

    }

    do {

        received = httpclient_read(client, &header[offset], HTTP_CLIENT_MAX_RECV_HEADER_LENGTH - offset, timeout); //1550 is RX size
        if(received <= 0) {
            HTTP_CLIENT_FREE(header) ;
            return HTTP_CLIENT_E_CONNECTION ;
        }
        client->total_read += received ;

        offset += received ;
        if (strnstr(header, "\r\n\r\n", offset)) break ;

        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : incomplete header received %d bytes",offset);

        if (offset >= HTTP_CLIENT_MAX_RECV_HEADER_LENGTH) {
            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                        "HTTP  :E: HTTP_CLIENT_MAX_RECV_HEADER_LENGTH to small for HTTP headers!");
            HTTP_CLIENT_FREE(header) ;
            return HTTP_CLIENT_E_LENGTH ;

        }

    } while (offset < HTTP_CLIENT_MAX_RECV_HEADER_LENGTH) ;

    *status = httpparse_response(header, offset, headers, sizeof(headers)/sizeof(headers[0]), &content) ;
    if (*status <= 0) {
        HTTP_CLIENT_FREE(header) ;
        return HTTP_CLIENT_E_HEADER ;

    }

    if (content) {

        unsigned int left = offset - (content - header) ;
        content_length = httpparse_content(content, left, headers, sizeof(headers)/sizeof(headers[0]), &payload, 0) ;

        if (content_length) {

            *response = (char*)HTTP_CLIENT_MALLOC(content_length + 1) ;

            if (*response == 0) {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "hHTTP  :E: out of memory ");
                received = HTTP_CLIENT_E_MEMORY ;
                //zero_copy_free (buffer) ;
                HTTP_CLIENT_FREE(header) ;

            } else {
                left = offset - (payload - header) ;
                offset = left >= content_length ? content_length : left ;

                memcpy (*response, payload, offset) ;
                HTTP_CLIENT_FREE(header) ;

                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : total %d read %d and %d bytes left to read",
                    received, offset, content_length);

                while (offset < content_length) {

                    received = httpclient_read(client, &(*response)[offset], /*1550*//*1452*/ content_length, timeout); //1550 is RX size
                    if (received <= 0) {
                        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                                "HTTP  : : read error %d (%d chunks read %d out of %d)",
                                        received, chunks, offset, content_length);
                        break ;
                    }
                    client->total_read += received ;

                    chunks++ ;
                    if (received + offset > content_length) {
                        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                                "HTTP  :E: read %d bytes, expected %d",
                                received, content_length-offset);
                        // we ignore trailing HTTP end-of-message characters
                        // we ignore extra chunks, if any, in this implementation.
                        break ;
                    }
                    offset += received ;
                    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                        "HTTP  : : read %d and %d bytes left to read",
                                received, content_length-offset);

                }

            }

        } else {
            HTTP_CLIENT_FREE(header) ;

        }

    } else {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
            "HTTP  :W: incomplete header received %d bytes",
                    received);

        HTTP_CLIENT_FREE(header) ;

        received = HTTP_CLIENT_E_HEADER ;

    }

    return received > 0 ? content_length : received ;
}

/**
 * @brief   httpclient_read_response_ex
 * @details Reads the response from a get or a pot.
 * @note    If the response has content it should be read with calls to httpclient_read_next_ex.
 *
 * @param[in] client
 * @param[in] timeout
 * @param[out] status                   HTTP status, like 200 for OK
 *
 * @return                              Number of bytes in response or < 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Error occurred during select, possible timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_HEADER         Error parsing HTTP header.
 * @retval HTTP_CLIENT_E_CONTENT        Error reading content, unexpected length.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_read_response_ex (HTTP_CLIENT_T* client, uint32_t timeout, int32_t* status)
{
#define READ_RESPONSE_PEEK          0
    int32_t         received ;
#if READ_RESPONSE_PEEK
    int32_t         received_left = 0 ;
#endif
    uint8_t*        content ;
    int             offset = 0 ;
    HTTP_HEADER_T headers[] = {
            {HTTP_HEADER_KEY_CONTENT_TYPE, 0},
            {HTTP_HEADER_KEY_CONTENT_LENGTH, 0},
            {HTTP_HEADER_KEY_TRANSFER_ENCODING, 0},
            {HTTP_HEADER_KEY_AUTHENTICATE, 0},
    } ;

    DBG_CHECK_HTTP_CLIENT (client->socket>=0, EFAIL,
                    "HTTP  :E: unexpected - socket not valid.!") ;
    DBG_CHECK_HTTP_CLIENT (status, EFAIL,
                    "HTTP  :E: unexpected - status not valid.!") ;
    DBG_CHECK_HTTP_CLIENT (client->read_buffer == 0, EFAIL,
                    "HTTP  :E: unexpected - client->read_buffer.!") ;

    *status = 0 ;
    client->payload_length = 0 ;
    client->content_length = 0 ;
    client->chunked = 0 ;
    client->chunk_length = 0 ;
    client->read = 0 ;
    client->read_buffer = (unsigned char*)HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH + HTTP_CLIENT_MAX_RECV_BUFFER_PADDING) ;
    if (client->read_buffer == 0) {
        return HTTP_CLIENT_E_MEMORY ;
    }

    do {

        received = httpclient_read(client, &client->read_buffer[offset], HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH - offset, timeout); //1550 is RX size
        if(received <= 0) {

            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                    "HTTP  : : httpclient_read_response_ex  %d",
                    received);

            HTTP_CLIENT_FREE(client->read_buffer) ;
            client->read_buffer = 0 ;
            return received ;
        }

        client->total_read += received ;
        offset += received ;
        if (strnstr((char*)client->read_buffer, "\r\n\r\n", offset)) {
            break ;
        }
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
                    "HTTP  : : incomplete header received %d bytes",
                    offset);

        if (offset >= HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH) {

            DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
                    "HTTP  :E: HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH to small for HTTP headers!");

            HTTP_CLIENT_FREE(client->read_buffer) ;
            client->read_buffer = 0 ;
            return HTTP_CLIENT_E_LENGTH ;
        }

    } while (1) ;


    *status = httpparse_response((char*)client->read_buffer, offset, headers,
            sizeof(headers)/sizeof(headers[0]), (char**)&content) ;
    if ((*status == 0) || (content == 0)) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_WARNING,
                    "HTTP  :W: incomplete header received %d bytes",
                    offset);
        HTTP_CLIENT_FREE(client->read_buffer) ;
        client->read_buffer = 0 ;
        return HTTP_CLIENT_E_HEADER ;

    }

    if (headers[2].value && (strncmp(headers[2].value, "chunked", 7) == 0)) {
        client->chunked = 1 ;

    } else {
        if (!headers[1].value || !sscanf(headers[1].value, "%u", (unsigned int*)&client->content_length)) {
            HTTP_CLIENT_FREE(client->read_buffer) ;
            client->read_buffer = 0 ;
            return HTTP_CLIENT_E_HEADER ;

        }

    }

    client->payload_length = offset - (content - client->read_buffer) ;
    memmove (client->read_buffer, content, client->payload_length) ;

    return HTTP_CLIENT_E_OK ;
}

/**
 * @brief   httpclient_read_next_ex
 * @details Reads the content from a get or a pot.
 * @note    recv data should be freed with httpclient_free_response. When all data read
 *          httpclient_read_complete_ex() should be called.
 *
 * @param[in] client
 * @param[in] timeout
 * @param[out] recv                 Pointer to received data
 *
 * @return                              Number of bytes in content OR 0 for no more available OR < 0 for error.
 * @retval HTTP_CLIENT_E_ERROR          Error occurred during select, possible timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_HEADER         Error parsing HTTP header.
 * @retval HTTP_CLIENT_E_CONTENT        Error reading content, unexpected length.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t
httpclient_read_next_ex (HTTP_CLIENT_T* client, uint32_t timeout, uint8_t** recv)
{
    int32_t received ;

    if (!client->chunked) {

        if (client->read >= client->content_length) {
            return 0 ;
        }

        if (!client->payload_length) {
            client->payload_length = httpclient_read(client, client->read_buffer, HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH, timeout); //1550 is RX size
            if (client->payload_length <= 0) {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                        "HTTP  : : httpclient_read_next_ex %d",
                        client->payload_length);
                return HTTP_CLIENT_E_ERROR ;
                
            }

        }

        received = client->payload_length ;

        client->total_read += client->payload_length ;
        client->read += client->payload_length  ;
        client->payload_length = 0 ;
        *recv = client->read_buffer ;

    } else {

        client->chunked++ ;

        if (client->read) {
            client->payload_length -= client->read ;
            memmove (client->read_buffer, &client->read_buffer[client->read], client->payload_length) ;

            if (!client->chunk_length) {
                // remove training \r\n at end of chunk
                while (client->payload_length < 2) {
                    received = httpclient_read(client, &client->read_buffer[client->payload_length],
                            HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH - client->payload_length, timeout); //1550 is RX size
                    if (received <= 0) {
                        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                                "HTTP  : : httpclient_read_next_ex failed with %d",
                                        received);
                        return -1 ;

                    }
                    client->payload_length += received ;

                }

                client->payload_length -= 2 ;
                memmove (client->read_buffer, &client->read_buffer[2], client->payload_length) ;

            }

        }

        if (!client->chunk_length) {

            char *p = strnchr ((char*)client->read_buffer,'\n', client->payload_length)  ;
            while (!p) {
                received = httpclient_read(client, &client->read_buffer[client->payload_length],
                        HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH - client->payload_length, timeout); //1550 is RX size

                if (received <= 0) {
                    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                            "HTTP  : : httpclient_read_next_ex received %d",
                                    received);
                    return HTTP_CLIENT_E_ERROR ;

                }
                client->payload_length += received ;

                p = strnchr ((char*)client->read_buffer,'\n', client->payload_length)  ;
                if(client->payload_length > 12) {
                    break ;
                }

            }


            if (!p || sscanf((char*)client->read_buffer, "%x\r\n", (unsigned int*)&client->chunk_length) == 0) {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                        "HTTP  : : httpclient_read_next_ex chunk not found");
                return -1 ;

            }
            if (client->chunk_length == 0) {
                return 0 ;
            }

            p++ ;
            uint32_t offset = (uint32_t) ((uintptr_t)p - (uintptr_t)client->read_buffer) ;
            client->payload_length -= offset ;
            memmove (client->read_buffer, &client->read_buffer[offset], client->payload_length) ;

        }


        if (!client->payload_length) {
            client->payload_length = httpclient_read(client, client->read_buffer, HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH, timeout); //1550 is RX size
            if (client->payload_length <= 0) {
                DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_REPORT,
                        "HTTP  : : httpclient_read_next_ex payload_length %d",
                        client->payload_length);
                return HTTP_CLIENT_E_ERROR ;

            }

        }

        if (client->payload_length < client->chunk_length) {
            client->read = client->payload_length ;

        } else {
            client->read = client->chunk_length ;
            
        }

        client->total_read += client->read ;
        *recv = client->read_buffer ;
        client->chunk_length -= client->read ;

        received = client->read ;

    }

    if (received > HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH) {
        DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_ERROR,
            "HTTP  :E: read %d", received);

    }

    return received ;
}

/**
 * @brief   httpclient_read_complete_ex
 * @details Called at the end of a get ot post.
 *
 * @param[in] client
 *
 * @return                              EOK.
 *
 * @http
 */
int32_t
httpclient_read_complete_ex (HTTP_CLIENT_T* client)
{
    DBG_MESSAGE_HTTP_CLIENT (DBG_MESSAGE_SEVERITY_INFO,
        "HTTP  : : read complete %d bytes in %d chunks",
                client->read, client->chunked);


    if (client->read_buffer) {
        HTTP_CLIENT_FREE(client->read_buffer) ;
        client->read_buffer = 0 ;
    }

    return EOK ;
}

#if HTTP_WEBSOCK

/**
 * @brief 	httpclient_websock_get
 * @details	Starts a HTTP GET request to open a websocket.
 * @note	The response should be read with httpclient_websock_read_response.
 *
 * @param[in] client					http client instance
 * @param[in] endpoint					url
 * @param[in] credentials				username:password
 *
 * @return 								The number of bytes sent or < 0 for error codes.
 * @retval HTTP_CLIENT_E_ERROR         	Error occurred during send.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t httpclient_websock_get(HTTP_CLIENT_T* client, const char* endpoint, const char* credentials)
{
    int32_t sent_bytes = 0;
    int32_t count;
    char* buffer = 0;
    char* host;
    char key[BASE64_OUTPUT_LENGTH(16) + 1];

    DBG_CHECK_HTTP_CLIENT(client->socket, EFAIL, "httpclient_websock_get : CHECK : unexpected - socket not valid.\r\n!");
    DBG_CHECK_HTTP_CLIENT(endpoint, EFAIL, "httpclient_websock_get : CHECK : unexpected - endpoint not valid.\r\n!");

    buffer = HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH);
    if (buffer == NULL)
    {
        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_ERROR, "HTTP  :E: Memory allocation failed\r\n");
        return HTTP_CLIENT_E_MEMORY;
    }

    /*
     * If routed by the server use the httpclient_set_hostname()
     * to set the specific hostname before calling this function.
     */
    if (client->hostname[0])
    {
        host = client->hostname;
    }
    else
    {
        host = client->host;
    }

    /*
     * Create the Sec-WebSocket-Key for this connection
     */
    for (count = 0; count < 4; count++)
    {
        client->key[count] = HTTP_CLIENT_RAND();
    }
    base64_encode((char*)client->key, 16, key, BASE64_OUTPUT_LENGTH(16));
    key[BASE64_OUTPUT_LENGTH(16)] = '\0';

    if (credentials == 0)
    {
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH, HTTP_WEBSOCKET_REQ, endpoint, host, key);
    }
    else
    {
        int credentials_len = strlen(credentials);
        int enc_credentials_len = BASE64_OUTPUT_LENGTH(credentials_len);
        char enc_credentials[enc_credentials_len + 1];
        base64_encode(credentials, credentials_len, enc_credentials, enc_credentials_len);
        enc_credentials[enc_credentials_len] = '\0';  // add string terminator to encoded credentials
        count = snprintf((char*)buffer, HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH, HTTP_WEBSOCKET_REQ_BASIC, endpoint, host,
                          key, enc_credentials);
    }

    /*
     * If the header was too big just return an error.
     */
    if (count >= HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH)
    {
        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_ERROR,
                                "HTTP  :E: HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH to small for HTTP headers!\r\n");
        HTTP_CLIENT_FREE(buffer);
        return HTTP_CLIENT_E_LENGTH;
    }

    /*
     * Send the request to the server.
     */
    sent_bytes = httpclient_write(client, (unsigned char*)buffer, count, 2000);
    HTTP_CLIENT_FREE(buffer);

    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_INFO, "HTTP  : : send %d bytes\r\n", sent_bytes);

    return sent_bytes;
}

/**
 * @brief 	httpclient_read_response
 * @details Reads the response after a httpclient_websock_get().
 * @note	If the connection was successfully upgraded to a websocket a HTTP status
 *			code of 101 should be returned.
 *
 * @param[in] client					http client instance.
 * @param[in] timeout					Timeout in ms to wait for the response.
 *
 * @return 								The HTTP status code or < 0 for error codes.
 * @retval HTTP_CLIENT_E_ERROR         	Error occurred during select, possible timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Socket was closed.
 * @retval HTTP_CLIENT_E_HEADER         Error parsing HTTP header.
 * @retval HTTP_CLIENT_E_CONTENT        Error reading content, unexpected length.
 * @retval HTTP_CLIENT_E_MEMORY         Failed to allocate a receive buffer.
 *
 * @http
 */
int32_t httpclient_websock_read_response(HTTP_CLIENT_T* client, uint32_t timeout)
{
    int32_t received;
    char* content;
    int rx_bytes = 0;
    int status;
    char* header;
    HTTP_HEADER_T headers[] = {
        {HTTP_HEADER_KEY_CONTENT_TYPE, 0},    {HTTP_HEADER_KEY_CONTENT_LENGTH, 0}, {HTTP_HEADER_KEY_CONNECTION, 0},
        {HTTP_HEADER_KEY_WEBSOCK_UPGRADE, 0}, {HTTP_HEADER_KEY_WEBSOCK_ACCEPT, 0}, {HTTP_HEADER_KEY_WEBSOCK_PROT, 0},
        //{HTTP_HEADER_KEY_TRANSFER_ENCODING, 0},
        //{HTTP_HEADER_KEY_AUTHENTICATE, 0},
    };

    DBG_CHECK_HTTP_CLIENT(client->socket, EFAIL,
                          "httpclient_websock_read_response : CHECK : unexpected - socket not valid.\r\n!");

    /*
     * Preallocate a buffer to receive the header in.
     */
    header = (char*)HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH);
    if (header == 0)
    {
        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_INFO, "HTTP  : : out of memory \r\n");
        return HTTP_CLIENT_E_MEMORY;
    }

    /*
     * Read the complete header up including the "\r\n\r\n" marking the start of the content if any.
     */
    do
    {

        received = httpclient_read(client, (void*)&header[rx_bytes], HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH - rx_bytes,
                                   timeout);  // 1550 is RX size
        if (received <= 0)
        {
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_WARNING, "HTTP  :E: httpclient_read failed %d\r\n", received);
            HTTP_CLIENT_FREE(header);
            return received;
        }
        client->total_read += received;

        rx_bytes += received;
        if (strnstr(header, "\r\n\r\n", rx_bytes)) break;

        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : incomplete header received %d bytes\r\n",
                                rx_bytes);

        if (rx_bytes >= HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH)
        {
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_ERROR,
                                    "HTTP  :E: HTTP_CLIENT_MAX_HEADER_LENGTH to small for HTTP headers!\r\n");
            HTTP_CLIENT_FREE(header);
            return HTTP_CLIENT_E_LENGTH;
        }

    } while (rx_bytes < HTTP_CLIENT_MAX_RECV_BUFFER_LENGTH);

    /*
     * Parse the received headers.
     */
    status = httpparse_response(header, rx_bytes, headers, sizeof(headers) / sizeof(headers[0]), &content);

    /*
     * Process the returned status
     */
    if (status <= 0)
    {
        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_ERROR, "HTTP  :E: status code %d\r\n", status);
        status = HTTP_CLIENT_E_HEADER;
    }
    else if (status != 101)
    {
        // ToDo: This if clause (for status != 101) is not tested

        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_WARNING, "HTTP  :W: error status code %d\r\n", status);
        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_INFO, "hHTTP  : : : %s\r\n", content);

        /*
         * This is a failure. Read the content (if any) to empty the socket.
         */
        /*
   if (headers[1].value) { // content-length
       uint32_t length ;

               if (sscanf(headers[1].value, "%u", (unsigned int*)&length)) {
                       received = httpclient_read(client, (void**)&buffer, length); // peek to find the length of the
   header while ((received > 0) && (length > 0)) { length -= received ; zero_copy_free(buffer) ; received =
   httpclient_read(client, (void**)&buffer, 0); // peek to find the length of the header
                       }

               }

   }
       */
    }
    else
    {

        DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT,
                                "HTTP  : : httpclient_websock_read_response status code %d\r\n", status);

        // for debugging purposes print out all the received headers.
        unsigned int i;
        for (i = 0; i < sizeof(headers) / sizeof(headers[0]); i++)
        {
            if (headers[i].value)
            {
                DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_INFO,
                                        "HTTP  : : httpclient_websock_read_response header %s: %s\r\n", headers[i].key,
                                        headers[i].value ? headers[i].value : "");
            }
        }

        /*
         * Check required header fields as specified in RFC6455
         */
        if (!headers[2].value || strnicmp(headers[2].value, "Upgrade", 7))
        {  // Connection header
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG,
                                    "HTTP  :E: httpclient_websock_read_response connection header invalid!");
            status = HTTP_CLIENT_E_HEADER;
        }
        if (!headers[3].value || strnicmp(headers[3].value, "websocket", 9))
        {  // Upgrade header
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG,
                                    "HTTP  :E: httpclient_websock_read_response upgrade header invalid!");
            status = HTTP_CLIENT_E_HEADER;
        }
        if (!headers[4].value)
        {  // Sec-WebSocket-Accept header
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  :E: Sec-WebSocket-Accept header missing!");
            status = HTTP_CLIENT_E_HEADER;
        }
        else
        {
            // ToDo: Validate correct SHA-1 hash in Sec-WebSocket-Accept header
        }
        if (!headers[5].value)
        {  // Sec-WebSocket-Protocol header
            // RFC 6455 spec says this is an optional response from the server
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_WARNING, "HTTP  :E: Sec-WebSocket-Protocol header missing!");
            /////// ignore			status =   HTTP_CLIENT_E_HEADER ;
        }
        else
        {
            // ToDo: Validate that HTTP_CLIENT_WEBSOCKET_PROTOCOL is returned.
        }
    }

    HTTP_CLIENT_FREE(header);

    return status;
}

/**
 * @brief 	websock_decode
 * @details Decodes websocket frame received from server.
 *
 * @param[in] client					http client instance
 * @param[out] opcode					Opcode of websocket frame.
 * @param[out] fin						FIN flag of websocket frame.
 * @param[in] timeout					Timeout im ms to read from socket.
 *
 * @return 								Number of bytes in payload or < 0 indicates an
 * error.
 * @retval HTTP_CLIENT_E_ERROR         	Socket timeout.
 * @retval HTTP_CLIENT_E_CONNECTION     Remote connection closed.
 * @retval HTTP_CLIENT_E_LENGTH         Buffer or payload length error.
 * @retval HTTP_CLIENT_E_HEADER     	Protocol error.
 *
 * @websockets
 */
static int32_t websock_decode(HTTP_CLIENT_T* client, uint8_t* opcode, uint8_t* fin, uint32_t timeout)
{
    int32_t status;
    uint32_t payload_length = 0;
    uint8_t header[6] = {0};

#    define WEBSOCK_FIN(buffer) ((buffer[0] >> 7) & 0x1)
#    define WEBSOCK_OPCODE(buffer) (buffer[0] & 0xF)
#    define WEBSOCK_MASK(buffer) ((buffer[1] >> 7) & 0x1)
#    define WEBSOCK_PAYLOAD_LENGTH(buffer) (buffer[1] & 0x7F)

    *opcode = 0;
    *fin = 0;

    // status = t_recv (athwifi_get_handle(), client->socket, &pbuffer, 0, 0);
    status = httpclient_read(client, (void*)header, 2, timeout);  //

    if (status <= 0)
    {
        return status;
    }

    if (WEBSOCK_MASK(header))
    {
        /*
         * RFC Section 5.1: "A server MUST NOT mask any frames that it sends to the client."
         */
        //	httpclient_read(client, (void*)&pbuffer, -1);
        return HTTP_CLIENT_E_HEADER;
    }

    *opcode = WEBSOCK_OPCODE(header);
    *fin = WEBSOCK_FIN(header);
    payload_length = WEBSOCK_PAYLOAD_LENGTH(header);

    if (payload_length == 126)
    {
        uint16_t tmp;
        status = httpclient_read(client, (void*)header, 2, timeout);
        if (status > 0)
        {
            memcpy(&tmp, &header[0], 2);
            payload_length = ntohs(tmp);
        }
    }
    else if (payload_length == 127)
    {
        status = httpclient_read(client, (void*)header, 4, timeout);
        if (status > 0)
        {
            memcpy(&payload_length, &header[0], 4);
            payload_length = ntohl(payload_length);
        }
    }
    else
    {
        // status = httpclient_read(client, (void*)&header, 2) ;
    }

    return payload_length;
}

/**
 * @brief 	websock_write
 * @details Write the frame header followed by the payload to the websocket.
 *
 * @param[in] client					http client instance.
 * @param[in] opcode					Opcode for frame.
 * @param[in] payload					payload.
 * @param[in] length					payload length.
 *
 * @return 								Number of bytes in payload or <= 0 indicates an
 * error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 * @retval HTTP_CLIENT_E_MEMORY     	Out of memory.
 *
 * @websockets
 */
static int32_t websock_write(HTTP_CLIENT_T* client, uint8_t opcode, const char* payload, uint32_t length)
{
    char* buffer;

    // pre-allocate maximum send buffer
    buffer = HTTP_CLIENT_MALLOC(HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH);
    if (buffer)
    {
        uint32_t offset = 0;
        uint32_t mask;
        uint32_t i;
        int32_t res = 0;

        // Set the opcode (text or binary) and the FIN bit.
        buffer[offset++] = (1 << 7) | opcode;

        // Write the length and mask bit to the header.
        if (length < 126)
        {
            buffer[offset++] = (length & 0x7F) | (1 << 7);
        }
        else if (length < (uint16_t)-1)
        {
            uint16_t tmp = htons(length);
            buffer[offset++] = 126 | (1 << 7);
            memcpy(&buffer[offset], &tmp, 2);
            ;
            offset += 2;
        }
        else
        {
            uint32_t tmp = htons(length);
            buffer[offset++] = 127 | (1 << 7);
            memcpy(&buffer[offset], &tmp, 4);
            ;
            offset += 4;
        }

        // Write the mask to the header
        do
        {
            mask = HTTP_CLIENT_RAND();
        } while (mask == 0);
        memcpy(&buffer[offset], &mask, 4);
        offset += 4;
        // encode the payload with the mask.
        for (i = 0; i < length; i++)
        {
            if (offset >= HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH)
            {
                // send in chunks of HTTP_CLIENT_MAX_XMIT_BUFFER_LENGTH
                if ((res = httpclient_write(client, (unsigned char*)buffer, offset, 100)) <= 0)
                {
                    break;
                }
                offset = 0;
            }
            buffer[offset++] = payload[i] ^ (((char*)&mask)[i % 4]);
        }

        if (offset && (res >= 0)) res = httpclient_write(client, (unsigned char*)buffer, offset, 100);
        HTTP_CLIENT_FREE(buffer);

        return res > 0 ? (int32_t)i : res;
    }

    return HTTP_CLIENT_E_MEMORY;
}

/**
 * @brief 	httpclient_websock_read
 * @details Read one frame from the websocket.
 * @note	If timeout is zzero the function will poll for data with t_select_ver1(). If there is
 *			data on the socket it will continue to read the header. If the complete header is
 *			not available it will use the timeout HTTP_CLIENT_READMORE_TIMEOUT to read the data
 *			from the socket.
 *
 * @param[in] client					http client instance
 * @param[in] buffer					Pointer to frame received.
 * @param[in] timeout					Timeout wait for frame from server.
 *
 * @return 								Number of bytes in payload or <= 0 indicates an
 *error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 * @retval HTTP_CLIENT_E_MEMORY     	Out of memory.
 * @retval HTTP_CLIENT_E_HEADER     	Unexpected opcode received.
 *
 * @websockets
 */
int32_t httpclient_websock_read(HTTP_CLIENT_T* client, char** buffer, uint32_t timeout)
{
    // char* read ;
    int32_t read_len;
    uint8_t opcode;
    uint8_t fin;

    *buffer = 0;

    int32_t payload_length = websock_decode(client, &opcode, &fin, timeout);

    if (payload_length == E_TIMEOUT)
    {
        payload_length = 0;
    }

    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT, "HTTP  : : httpclient_websock_read payload length %d\r\n",
                            payload_length);

    if (payload_length > 0)
    {

        uint32_t offset = 0;

        if (payload_length > 0)
        {
            *buffer = (char*)HTTP_CLIENT_MALLOC(payload_length);
            if (*buffer == 0)
            {
                return HTTP_CLIENT_E_MEMORY;
            }

            // read the complete payload
            while (offset < (uint32_t)payload_length)
            {

                read_len = httpclient_read(client, (void*)&(*buffer)[offset], payload_length - offset, timeout);
                if (read_len <= 0)
                {
                    HTTP_CLIENT_FREE(*buffer);
                    return read_len;
                }

                offset += read_len;
            }
        }

        switch (opcode)
        {
        case 0:  // continuation
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_read header continuation");
            break;

        case 1:  // text
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_INFO, "HTTP  : : websock_read header text");
            break;

        case 2:  // binary
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_read header binary");
            break;

        /*
         * Control Frames:
         */
        case 8:  // connection closesocket
        {
            uint16_t code = 1000;

            if (payload_length >= 2)
            {

                memcpy(&code, *buffer, 2);
                code = ntohs(code);
                DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG,
                                        "HTTP  : : websock_read header connection closed with status %d\r\n", code);
            }
            else
            {
                DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT, "HTTP  : : websock_read header connection closed");
            }
            client->status |= HTTP_CLIENT_STATUS_CLOSE_SERVER;
            if (!(client->status & HTTP_CLIENT_STATUS_CLOSE_CLIENT))
            {
                // if the closesocket was initiated from the serve, we send the expected closesocket response
                // and wait for the server to closesocket the socket.
                httpclient_websock_initiate_close(client, code);

            }
            // return A_SOCK_INVALID so the user can shutdown the socket.
            payload_length = -2;  // A_SOCK_INVALID ;
            HTTP_CLIENT_FREE(*buffer);
        }
        break;

        case 9:  // ping
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_read header ping");
            // ToDo: Implement ping and continue to receive next frame.
            payload_length = 0;
            HTTP_CLIENT_FREE(*buffer);
            break;

        case 10:  // pong
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_read header pong");
            payload_length = 0;
            HTTP_CLIENT_FREE(*buffer);
            break;

        default:  // unexpected
            DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_read header unexpected opcode %d\r\n",
                                    (int)opcode);
            payload_length = HTTP_CLIENT_E_HEADER;
            HTTP_CLIENT_FREE(*buffer);
            break;
        }
    }

    return payload_length;
}

/**
 * @brief 	httpclient_websock_free
 * @details Free a frame received by a call to httpclient_websock_read().
 * @note
 *
 * @param[in] client					http client instance
 * @param[in] buffer					Pointer to frame.
 *
 * @return 								void.
 *
 * @websockets
 */
void httpclient_websock_free(HTTP_CLIENT_T* client, char* buffer)
{
    if (buffer)
    {
        HTTP_CLIENT_FREE(buffer);
    }
}

/**
 * @brief 	httpclient_websock_write_text
 * @details Write a websocket frame and mark it as text.
 * @note
 *
 * @param[in] client					http client instance.
 * @param[in] payload					Payload of frame.
 * @param[in] length					Length of payload.
 *
 * @return 								Number of bytes send or <= 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 *
 * @websockets
 */
int32_t httpclient_websock_write_text(HTTP_CLIENT_T* client, const char* payload, uint32_t length)
{
    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT, "HTTP  : : websock_write_text payload length %d\r\n", length);

    return websock_write(client, 1, payload, length);
}

/**
 * @brief 	httpclient_websock_write_binary
 * @details Write a websocket frame and mark it as binary.
 * @note
 *
 * @param[in] client					http client instance.
 * @param[in] payload					Payload of frame.
 * @param[in] length					Length of payload.
 *
 * @return 								Number of bytes send or <= 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 *
 * @websockets
 */
int32_t httpclient_websock_write_binary(HTTP_CLIENT_T* client, const char* payload, uint32_t length)
{
    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT, "HTTP  : : websock_write_binary payload length %d\r\n",
                            length);

    return websock_write(client, 2, payload, length);
}

/**
 * @brief 	httpclient_websock_ping
 * @details Send a websocket ping.
 * @note
 *
 * @param[in] client					http client instance.
 *
 * @return 								Number of bytes send or <= 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 *
 * @websockets
 */
int32_t httpclient_websock_ping(HTTP_CLIENT_T* client)
{
    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_REPORT, "HTTP  : : websock_ping payload length %d\r\n", 0);

    return websock_write(client, 9, 0, 0);
}

/**
 * @brief 	httpclient_websock_close
 * @details Initiate the closesocket handshake for the websocket.
 * @note	The client should call httpclient_websock_read to read the servers closesocket response.
 *
 * @param[in] client					http client instance.
 *
 * @return 								Number of bytes send or <= 0 indicates an error.
 * @retval HTTP_CLIENT_E_ERROR          Socket error.
 * @retval HTTP_CLIENT_E_CONNECTION     Connection closed by server.
 *
 * @websockets
 */
int32_t httpclient_websock_initiate_close(HTTP_CLIENT_T* client, uint16_t code)
{
    DBG_MESSAGE_HTTP_CLIENT(DBG_MESSAGE_SEVERITY_LOG, "HTTP  : : websock_close closing connection with %d\r\n", code);

    code = htons(code);
    client->status |= HTTP_CLIENT_STATUS_CLOSE_CLIENT;
    return websock_write(client, 8, (char*)&code, 2);
}

#endif /* HTTP_WEBSOCK */


