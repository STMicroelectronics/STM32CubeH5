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


#include "qoraal-http/config.h"
#include "qoraal-http/qoraal.h"
#include "qoraal/qoraal.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#if QORAAL_CFG_USE_LWIP
#define DNS_USE_NETCONN_GETHOSTBYNAME           1

#if !DNS_USE_NETCONN_GETHOSTBYNAME
typedef struct DNS_RESOLVE_S {
    p_sem_t *     sem ;
    ip_addr_t           addr ;
    int32_t             res ;
} DNS_RESOLVE_T ;

static void
network_dns_found_callback(const char *name, const ip_addr_t *ipaddr,
                           void *callback_arg)
{
    DNS_RESOLVE_T * dns = (DNS_RESOLVE_T*)callback_arg ;
    if ((ipaddr) && (ipaddr->addr)) {
        dns->addr.addr = ipaddr->addr;
        dns->res =  EOK ;

    } else {
      dns->res =  E_NOTFOUND ;

    }

    os_sem_signal (dns->sem) ;
}
#endif

int32_t
gethostbyname_timeout (const char* hostname, uint32_t *ip4_address,
                         uint32_t timeout)
{
    if (!hostname || !ip4_address) {
        return E_PARM; // Invalid arguments
    }

#if !DNS_USE_NETCONN_GETHOSTBYNAME
    err_t err ;
    p_sem_t sem ;
    DNS_RESOLVE_T dns = {&sem, {0}, EFAIL} ;

    if (os_sem_create (&sem, 0) != EOK) {
        return EFAIL ;

    }

    err = dns_gethostbyname(hostname, &dns.addr,
                                  network_dns_found_callback, &dns) ;
    switch(err){
    case ERR_OK:
        // numeric or cached, returned in resolved
        dns.res = EOK ;
        break;

    case ERR_INPROGRESS:
        // need to ask, will return data via callback
        os_sem_wait (&sem) ;
        break;

    default:
        // bad arguments in function call
        break;

    }

    os_sem_delete (&sem) ;

    DBG_MESSAGE_NETWORK(DBG_MESSAGE_SEVERITY_INFO,"NET   : : gethostfromname %s returned %s with %d",
            hostname, ip4addr_ntoa(&dns.addr), dns.res) ;

    *ip4_address = dns.addr.addr ;

    return dns.res ;

#else
    struct netconn *conn;
    ip_addr_t addr;
    int32_t res = E_CONNECTION;

    // Create a new connection identifier.
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL) {
        // Perform DNS lookup
        res = netconn_gethostbyname(hostname, &addr);

        if (res == ERR_OK) {
            // Convert the IP address to a string
            res = EOK ;
            *ip4_address = addr.addr ;

        } else {
            res = E_NOTFOUND ;
        }

        // Delete the connection identifier
        netconn_delete(conn);
    } 

    return res ;
#endif
}
#else


int32_t
gethostbyname_timeout (const char* hostname, uint32_t *ip4_address,
                         uint32_t timeout)
{

    struct addrinfo hints, *res = NULL;
    int ret;

    if (!hostname || !ip4_address) {
        return -1; // Invalid arguments
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    ret = getaddrinfo(hostname, NULL, &hints, &res);
    if (ret != 0) {
        return -1; // DNS resolution failed
    }

    // Assume we found at least one result (res will be non-NULL on success)
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    *ip4_address = ipv4->sin_addr.s_addr;

    freeaddrinfo(res);

    return 0; // Success

}

#endif