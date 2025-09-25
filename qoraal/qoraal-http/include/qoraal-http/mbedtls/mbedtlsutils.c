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

#include "../config.h"
#if !defined CFG_HTTPSERVER_TLS_DISABLE || !defined CFG_HTTPCLIENT_TLS_DISABLE
#include <time.h>
#include "mbedtlsutils.h"
#include "threading_alt.h"
#include "qoraal/qoraal.h"
#include "qoraal-http/qoraal.h"
#include "qoraal/common/rtclib.h"
#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif
#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif


#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl_ticket.h"
#include "mbedtls/ssl_cache.h"
#include "mbedtls/oid.h"

#include "psa/crypto.h"



/*===========================================================================*/
/* local variables.                                                    */
/*===========================================================================*/

static mbedtls_entropy_context          _ssl_entropy;
static mbedtls_ctr_drbg_context         _ssl_ctr_drbg;
#if defined(MBEDTLS_SSL_CACHE_C)
static mbedtls_ssl_cache_context        _ssl_cache;
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
static mbedtls_ssl_ticket_context       _ssl_ticket_ctx;
#endif

static mbedtls_ssl_config 				_ssl_client_config_conf = {0} ;
static mbedtls_ssl_config 			*	_ssl_client_config_conf_inst = 0 ;
static mbedtls_ssl_config 				_ssl_server_config_conf = {0} ;
static mbedtls_ssl_config 			*	_ssl_server_config_conf_inst = 0 ;

static mbedtls_x509_crt 				_ssl_srvcert ;
static mbedtls_pk_context 				_ssl_pkey ;

static OS_MUTEX_DECL(_mbedtls_mutex) ;


/*===========================================================================*/
/* function declarations.                                                    */
/*===========================================================================*/

static int
mbedtls_tick_entropy_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    uint32_t timer = os_sys_ticks () ;
    
    if( len >= sizeof(uint32_t)) len = sizeof(uint32_t) ;
    memcpy( output, &timer, len );
    *olen = len;

    DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_INFO,  
            "TLS   : : mbedtls_tick_entropy_poll %d ", timer );

    return( 0 );
}

static int
mbedtls_hw_entropy_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    *olen = len ;
    while (len > 4) {
        uint32_t random = qoraal_rand () ;
        memcpy (output, &random, 4) ;
        output += 4 ;
        len -= 4 ;
    }

    DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_INFO,  
            "TLS   : : mbedtls_hw_entropy_poll %d bytes", *olen );

    return( 0 );
}

static void 
mbedtls_debug_cb ( void *ctx, int level, const char *file, int line,
                      const char *str )
{
    DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
            "TLS   : : %s:%d: %s",file,line,str) ;
}

static mbedtls_time_t
mbedtls_time_cb ( mbedtls_time_t* time )
{
    return rtc_time () ;
}

#if defined(MBEDTLS_THREADING_ALT)
static void
mutex_init( mbedtls_threading_mutex_t * mtx)
{
    os_mutex_create (mtx) ;
}
static void
mutex_free( mbedtls_threading_mutex_t * mtx)
{
    os_mutex_delete (mtx) ;
}
static int
mutex_lock( mbedtls_threading_mutex_t * mtx)
{
    if (!*mtx) {
        return -1 ;
    }
    os_mutex_lock (mtx) ;
    return 0 ;
}
static int
mutex_unlock( mbedtls_threading_mutex_t * mtx)
{
    if (!*mtx) {
        return -1 ;
    }
    os_mutex_unlock (mtx) ;
    return 0 ;
}
#endif

#if defined CFG_MBEDTLS_PLATFORM_INIT_ENABLE
static void*
mbedtls_calloc_cb(size_t size, size_t bytes)
{
    void* mem = qoraal_malloc(QORAAL_HeapAuxiliary, bytes*size) ;
    if (mem) memset (mem, 0, bytes*size) ;

    return mem ;
}

static void
mbedtls_free_cb(void* mem)
{
    qoraal_free(QORAAL_HeapAuxiliary, mem) ;
}
#endif

static int32_t 
mbedtls_load_certificates( const char * pem_cert, const char * pem_key)
{
	int32_t res  = 0 ;

    mbedtls_x509_crt_init( &_ssl_srvcert );
    mbedtls_pk_init( &_ssl_pkey );

	do {

		res = mbedtls_x509_crt_parse(&_ssl_srvcert, (const unsigned char *) pem_cert,
							strlen (pem_cert) + 1 );
		if( res != 0 ) {
			DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_x509_crt_parse returned %04X", -res );
			break;
		}

		res =  mbedtls_pk_parse_key(&_ssl_pkey, (const unsigned char *) pem_key,
							strlen(pem_key)+1, NULL, 0,
							mbedtls_ctr_drbg_random, &_ssl_ctr_drbg);
		if( res != 0 ) {
			mbedtls_x509_crt_free (&_ssl_srvcert) ;
			DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_pk_parse_key returned %04X", -res );
			break;
		}


	} while (0) ;


	return res ;
}

int32_t
mbedtlsutils_init (void)
{
    os_mutex_init (&_mbedtls_mutex) ;
    return 0 ;
}

int32_t
mbedtlsutils_start (void)
{
    int32_t ret = 0;
    const char *pers = "Qoraal" ;

    psa_crypto_init() ;

#if defined CFG_MBEDTLS_PLATFORM_INIT_ENABLE
    mbedtls_platform_set_calloc_free( mbedtls_calloc_cb,
            mbedtls_free_cb ) ;
#endif

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt(mutex_init, mutex_free, mutex_lock, mutex_unlock );
#endif

    mbedtls_entropy_init( &_ssl_entropy );
    mbedtls_entropy_add_source( &_ssl_entropy, mbedtls_hw_entropy_poll, NULL,
                                12, MBEDTLS_ENTROPY_SOURCE_STRONG );
    mbedtls_entropy_add_source( &_ssl_entropy, mbedtls_tick_entropy_poll, NULL,
                                4, MBEDTLS_ENTROPY_SOURCE_WEAK );

    mbedtls_ctr_drbg_init( &_ssl_ctr_drbg );

#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_init( &_ssl_cache );
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    mbedtls_ssl_ticket_init( &_ssl_ticket_ctx );
#endif

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( 1 );
#endif

    mbedtls_platform_set_time (mbedtls_time_cb) ;

#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_set_max_entries( &_ssl_cache, 5 );
#endif

    do {
        if( ( ret = mbedtls_ctr_drbg_seed( &_ssl_ctr_drbg, mbedtls_entropy_func, &_ssl_entropy,
                                   (const unsigned char *) pers,
                                   strlen( pers ) ) ) != 0 ) {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_ctr_drbg_seed returned %d", ret );
            break ;

        }


#if defined(MBEDTLS_SSL_SESSION_TICKETS)
        if( ( ret = mbedtls_ssl_ticket_setup( &_ssl_ticket_ctx,
                        mbedtls_ctr_drbg_random, &_ssl_ctr_drbg,
                        MBEDTLS_CIPHER_AES_256_GCM,
                        86400 ) ) != 0 )
        {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_ssl_ticket_setup returned %d", ret );
            break ;
        }
#endif

    } while(0);

    if (ret < 0) {
#if defined(MBEDTLS_SSL_CACHE_C)
        mbedtls_ssl_cache_free( &_ssl_cache );
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
        mbedtls_ssl_ticket_free (&_ssl_ticket_ctx) ;
#endif
        mbedtls_ctr_drbg_free (&_ssl_ctr_drbg) ;
#if defined(MBEDTLS_THREADING_ALT)
        mbedtls_threading_free_alt () ;
#endif

    }

    return ret ;
}

int32_t
mbedtls_client_inst_init (mbedtls_ssl_config * ssl_config)
{
    int32_t ret = 0;
    mbedtls_ssl_config * pconf = ssl_config ;

    mbedtls_ssl_config_init( pconf );

    do {

        DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : Setting up the SSL data ..." );

        if( ( ret = mbedtls_ssl_config_defaults( pconf,
                        MBEDTLS_SSL_IS_CLIENT,
                        MBEDTLS_SSL_TRANSPORT_STREAM,
                        MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
        {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_ssl_config_defaults returned %d", ret );
            break;
        }

        mbedtls_ssl_conf_authmode(pconf, MBEDTLS_SSL_VERIFY_OPTIONAL);
        mbedtls_ssl_conf_rng( pconf, mbedtls_ctr_drbg_random, &_ssl_ctr_drbg );
        mbedtls_ssl_conf_dbg( pconf, mbedtls_debug_cb, NULL );

#if defined(MBEDTLS_SSL_CACHE_C)
        mbedtls_ssl_conf_session_cache( pconf, &_ssl_cache,
                mbedtls_ssl_cache_get,
                mbedtls_ssl_cache_set ) ;
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
        mbedtls_ssl_conf_session_tickets_cb( pconf,
                mbedtls_ssl_ticket_write,
                mbedtls_ssl_ticket_parse,
                &_ssl_ticket_ctx );
#endif

    } while (0) ;

    if (ret < 0) {
        mbedtls_ssl_config_free (pconf) ;

    }

    return ret ;
}

int32_t
mbedtls_server_inst_init (mbedtls_ssl_config * ssl_config)
{
    int32_t ret = 0;
    mbedtls_ssl_config * pconf = ssl_config ;

    mbedtls_ssl_config_init( pconf );

    do {

        DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG, 
                "TLS   : : Setting up the SSL data ..." );

        if( ( ret = mbedtls_ssl_config_defaults( pconf,
                        MBEDTLS_SSL_IS_SERVER,
                        MBEDTLS_SSL_TRANSPORT_STREAM,
                        MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
        {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  
                "TLS   : : failed! mbedtls_ssl_config_defaults returned %d", ret );
             break;
        }

        mbedtls_ssl_conf_authmode(pconf, MBEDTLS_SSL_VERIFY_NONE);
        mbedtls_ssl_conf_rng( pconf, mbedtls_ctr_drbg_random, &_ssl_ctr_drbg );
        mbedtls_ssl_conf_dbg( pconf, mbedtls_debug_cb, NULL );

#if defined(MBEDTLS_SSL_CACHE_C)
        mbedtls_ssl_conf_session_cache( pconf, &_ssl_cache,
                mbedtls_ssl_cache_get,
                mbedtls_ssl_cache_set ) ;
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
        mbedtls_ssl_conf_session_tickets_cb( pconf,
                mbedtls_ssl_ticket_write,
                mbedtls_ssl_ticket_parse,
                &_ssl_ticket_ctx );
#endif

    } while (0) ;

    if (ret < 0) {
        mbedtls_ssl_config_free (pconf) ;

    }

    return ret ;
}


mbedtls_ssl_config*
mbedtlsutils_get_client_config (void)
{
    os_mutex_lock (&_mbedtls_mutex) ;
    if (!_ssl_client_config_conf_inst) {
        if (mbedtls_client_inst_init (&_ssl_client_config_conf) == 0) {
            _ssl_client_config_conf_inst = &_ssl_client_config_conf ;
        }
    }
    os_mutex_unlock (&_mbedtls_mutex) ;
	return _ssl_client_config_conf_inst ;
}

void
mbedtls_release_client_config (void)
{
    os_mutex_lock (&_mbedtls_mutex) ;
    if (_ssl_client_config_conf_inst) {
        mbedtls_ssl_config_free (_ssl_client_config_conf_inst) ;
        _ssl_client_config_conf_inst = 0 ;
    }
    os_mutex_unlock (&_mbedtls_mutex) ;
}

mbedtls_ssl_config*
mbedtlsutils_get_server_config (void)
{
    int32_t ret = 0 ;
    os_mutex_lock (&_mbedtls_mutex) ;
    if (!_ssl_server_config_conf_inst) {
        ret = mbedtls_server_inst_init (&_ssl_server_config_conf) ;
        if (ret == 0) {
            _ssl_server_config_conf_inst = &_ssl_server_config_conf ;
            ret = mbedtls_load_certificates (MBEDTLSUTILS_DEFAULT_CERT, MBEDTLSUTILS_DEFAULT_KEY) ;

            if (ret == 0) {
                mbedtls_ssl_conf_ca_chain( &_ssl_server_config_conf , _ssl_srvcert.next, NULL );
                if( ( ret = mbedtls_ssl_conf_own_cert( &_ssl_server_config_conf, &_ssl_srvcert, &_ssl_pkey) ) != 0 )
                {
                    DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  "TLS   : : failed! mbedtls_ssl_conf_own_cert returned %d", ret );

                } 
            }

        }
    }
    
    if (ret != 0) {
        mbedtls_release_server_config () ;
        _ssl_server_config_conf_inst = 0 ;
        
    }
    os_mutex_unlock (&_mbedtls_mutex) ;

	return _ssl_server_config_conf_inst ;
}

void
mbedtls_release_server_config (void)
{
    if (_ssl_server_config_conf_inst) {
        mbedtls_x509_crt_free (&_ssl_srvcert) ;
        mbedtls_pk_free (&_ssl_pkey) ;
        mbedtls_ssl_config_free (_ssl_server_config_conf_inst) ;
        _ssl_server_config_conf_inst = 0 ;
    }
}

#if defined QORAAL_CFG_USE_LWIP
int 
mbedtls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout)
{
    int ret = 0 ;
    int fd = (int)ctx;
    struct fd_set readSet;
    struct fd_set exceptSet;
    struct timeval tv;

    if( fd < 0 ) {
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    while (1) {

        FD_ZERO(&readSet);
        FD_SET(fd, &readSet);
        FD_ZERO(&exceptSet);
        FD_SET(fd, &exceptSet);

        tv.tv_sec = timeout/1000 ; // TIMEOUT_PER_IDLE_SELECT_SEC;
        tv.tv_usec = timeout%1000 ;

        lwip_select(fd+1, &readSet, NULL, &exceptSet, &tv) ;

        if (FD_ISSET(fd, &readSet)) {
            break ;
        }
        if (FD_ISSET(fd, &exceptSet)) {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_LOG,  "TLS   : : mbedtls_net_recv_timeout read except...") ;
            ret = MBEDTLS_ERR_SSL_CONN_EOF ;
            break ;
        }

        DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_INFO,  "TLS   : : mbedtls_net_recv_timeout read timeout...") ;
        ret = MBEDTLS_ERR_SSL_TIMEOUT ;
        break ;


    }

    if (!ret) {
        ret = (int) lwip_recv( fd, buf, len, MSG_DONTWAIT );
        if (ret <= 0) {
            ret = MBEDTLS_ERR_SSL_CONN_EOF ;
        }
    }

    return( ret );

}

int 
mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len)
{
    int ret = 0 ;
    int fd = (int)ctx;

    if( fd < 0 ) {
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) lwip_recv( fd, buf, len, MSG_DONTWAIT);
    if (ret <= 0) {
        if (errno == EWOULDBLOCK) {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_INFO,  "mbedtls_net_recv WANT_READ") ;
            return MBEDTLS_ERR_SSL_WANT_READ ;
        }
        else {
            DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_REPORT,  "TLS   : : mbedtls_net_recv CONN_EOF (%d)", ret) ;
            return ret == 0 ? MBEDTLS_ERR_NET_RECV_FAILED : MBEDTLS_ERR_NET_CONN_RESET ;
        }

    } else {
        DBG_MESSAGE_MBEDTLS(DBG_MESSAGE_SEVERITY_INFO,  "TLS   : : mbedtls_net_recv read %d!", ret) ;

    }

    return( ret );
}

int 
mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = (int)ctx;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) lwip_write( fd, buf, len );

    if( ret < 0 )
    {
        //if( net_would_block( ctx ) != 0 )
            return( MBEDTLS_ERR_SSL_CONN_EOF );

        //return( MBEDTLS_ERR_NET_SEND_FAILED );
    }

    return( ret );
}
#endif

#endif /* CFG_HTTPCLIENT_TLS_DISABLE */
