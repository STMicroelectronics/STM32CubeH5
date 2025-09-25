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

#ifndef __MBEDTLSUTILS_H__
#define __MBEDTLSUTILS_H__

#include <stdint.h>
#include "mbedtls/ssl.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#define DBG_MESSAGE_MBEDTLS(severity, fmt_str, ...)         DBG_MESSAGE_T(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_MBEDTLS                                  DBG_ASSERT_T

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

#define MBEDTLSUTILS_DEFAULT_CERT \
    "-----BEGIN CERTIFICATE-----\r\n" \
    "MIIB/jCCAaUCCQC17+kdZwy75jAJBgcqhkjOPQQBMIGGMQswCQYDVQQGEwJFVTEN\r\n" \
    "MAsGA1UECAwEbm9uZTENMAsGA1UEBwwEbm9uZTENMAsGA1UECgwEbm9uZTEWMBQG\r\n" \
    "A1UECwwNSVQgRGVwYXJ0bWVudDESMBAGA1UEAwwJbmF2YXJvLm5sMR4wHAYJKoZI\r\n" \
    "hvcNAQkBFg9uYXRpZUBuYXZhcm8ubmwwIBcNMjQxMjIzMDczNzQyWhgPMjA3NDEy\r\n" \
    "MTEwNzM3NDJaMIGGMQswCQYDVQQGEwJFVTENMAsGA1UECAwEbm9uZTENMAsGA1UE\r\n" \
    "BwwEbm9uZTENMAsGA1UECgwEbm9uZTEWMBQGA1UECwwNSVQgRGVwYXJ0bWVudDES\r\n" \
    "MBAGA1UEAwwJbmF2YXJvLm5sMR4wHAYJKoZIhvcNAQkBFg9uYXRpZUBuYXZhcm8u\r\n" \
    "bmwwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATDOGvNfAbmXGO055xN3DdO5Z1R\r\n" \
    "JI7ulyw/7kj1DHYgd8ETMN+KbwNxEU10eiY6YTwVv9PJ2S9FzFxr6dNVERvPMAkG\r\n" \
    "ByqGSM49BAEDSAAwRQIgVdyPqpkxpxlXfvwzDQLpJadjy/3YHMP3vSKV4EGr11gC\r\n" \
    "IQDeWguEP3NEPt8jySHvF98pLal7kN4TaY9oMsBURlF61A==\r\n" \
    "-----END CERTIFICATE-----\r\n"

#define MBEDTLSUTILS_DEFAULT_KEY \
    "-----BEGIN EC PARAMETERS-----\r\n" \
    "BggqhkjOPQMBBw==\r\n" \
    "-----END EC PARAMETERS-----\r\n" \
    "-----BEGIN EC PRIVATE KEY-----\r\n" \
    "MHcCAQEEIH0LpvN2YIRjQKMEhMdNs3UPVe0uiUwAO/r/FtySWebAoAoGCCqGSM49\r\n" \
    "AwEHoUQDQgAEwzhrzXwG5lxjtOecTdw3TuWdUSSO7pcsP+5I9Qx2IHfBEzDfim8D\r\n" \
    "cRFNdHomOmE8Fb/TydkvRcxca+nTVREbzw==\r\n" \
    "-----END EC PRIVATE KEY-----\r\n"

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


    int32_t                 mbedtlsutils_init (void) ;
    int32_t                 mbedtlsutils_start (void) ;

    mbedtls_ssl_config*     mbedtlsutils_get_client_config (void)    ;
    void                    mbedtls_release_client_config (void) ;
    mbedtls_ssl_config*     mbedtlsutils_get_server_config (void)    ;
    void                    mbedtls_release_server_config (void) ;

    int                     mbedtls_net_send (void *ctx, const unsigned char *buf, size_t len) ;
    int                     mbedtls_net_recv (void *ctx, unsigned char *buf, size_t len) ;
    int                     mbedtls_net_recv_timeout (void *ctx, unsigned char *buf, size_t len, uint32_t timeout) ;


#ifdef __cplusplus
}
#endif

#endif /* __MBEDTLSUTILS_H__ */
