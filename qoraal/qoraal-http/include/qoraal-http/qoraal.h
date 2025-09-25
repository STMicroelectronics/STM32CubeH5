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


#ifndef __QORAAL_HTTP_H__
#define __QORAAL_HTTP_H__

#include <stdint.h>
#include <string.h>
#include "config.h"


#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#elif __has_include(<lwip/inet.h>) || (defined QORAAL_CFG_USE_LWIP && QORAAL_CFG_USE_LWIP)
    // Looks like LWIP is present
	#include <lwip/opt.h>
	#include <lwip/def.h>
	#include <lwip/mem.h>
	#include <lwip/pbuf.h>
	#include <lwip/sys.h>
	#include <lwip/stats.h>
	#include <lwip/snmp.h>
	#include <lwip/tcpip.h>
	#include <netif/etharp.h>
	#include <lwip/netifapi.h>
	#include <lwip/dns.h>

    #include <lwip/inet.h>
    #include <lwip/sockets.h>
    #include <lwip/netdb.h>
    #ifdef QORAAL_CFG_USE_LWIP
    #undef QORAAL_CFG_USE_LWIP
    #endif
    #define QORAAL_CFG_USE_LWIP            1
#elif __has_include("nxd_bsd.h") || (defined QORAAL_CFG_USE_LWIP && QORAAL_CFG_USE_LWIP)

#include "nxd_bsd.h"
#define closesocket soc_close
#define ioctlsocket  ioctl

#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <netdb.h>

    #define closesocket close
    #define ioctlsocket  ioctl

#endif




/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

int32_t qoraal_http_init_default () ;
 
#ifdef __cplusplus
}
#endif

#endif /* __QORAAL_HTTP_H__ */
