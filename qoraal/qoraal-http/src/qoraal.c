
#include "qoraal/qoraal.h"
#include "qoraal-http/qoraal.h"

extern void    keep_httpcmds (void) ;

#include "qoraal-http/httpclient.h"
#include "qoraal-http/httpserver.h"
#include "qoraal-http/httpwebapi.h"


void qoraal_http_keep_server(void) ;
void qoraal_http_keep_client(void) ;
#if HTTP_WEBSOCK
void qoraal_http_keep_websocket(void) ;
#endif
void qoraal_http_keep_webapi(void) ;


int32_t qoraal_http_init_default ()
{
    keep_httpcmds () ;
    return EOK ;
}

#if HTTP_WEBSOCK
void
qoraal_http_keep_websocket(void)
{
    httpclient_websock_get (0, 0, 0) ;
    httpclient_websock_read_response (0, 0) ;
    httpclient_websock_read (0, 0, 0) ;
    httpclient_websock_free (0, 0) ;
    httpclient_websock_write_text (0,  0, 0) ;
    httpclient_websock_write_binary (0,  0, 0) ;
    httpclient_websock_ping (0) ;
    httpclient_websock_initiate_close (0,  0) ;

}
#endif

void
qoraal_http_keep_client(void)
{
    httpclient_init (0, 0) ;
    httpclient_connect (0, 0, 0) ;
    httpclient_set_hostname (0, 0) ;
    httpclient_is_connected (0) ;
    httpclient_close (0) ;

    httpclient_get (0, 0, 0) ;
    httpclient_post (0, 0, 0, 0, 0) ;
    httpclient_post_chunked (0, 0, 0, 0, 0) ;
    httpclient_post_stream (0, 0, 0, 0, 0, 0) ;
    httpclient_read_response (0, 0, 0, 0) ;
    httpclient_free_response (0, 0) ;

}

void
qoraal_http_keep_server(void)
{
	httpserver_init(0);
	httpserver_close(0);
	httpserver_user_init(0);
	httpserver_listen(0);
	httpserver_select(0, 0);
	httpserver_user_accept(0, 0, 0);
	httpserver_user_ssl_accept(0, 0);
	httpserver_user_select(0, 0);
	httpserver_user_close(0);
	httpserver_read_request_ex(0, 0, 0, 0);
	httpserver_get_authorization_header(0);
	httpserver_read_content_ex(0, 0, 0);
	httpserver_read_all_content_ex(0, 0, 0);
	httpserver_free_request(0);
	httpserver_wait_close(0, 0);
	httpserver_write_response(0, 0, 0, 0, 0, 0, 0);
	httpserver_chunked_response(0, 0, 0, 0, 0);
	httpserver_chunked_append_fmtstr(0, 0, 0);
	//httpserver_chunked_vappend_fmtstr(0, 0);
	httpserver_chunked_append_str(0, 0, 0);
	httpserver_chunked_flush(0);
	httpserver_chunked_complete(0);
}

void
qoraal_http_keep_webapi(void)
{
	webapi_init (0, 0) ;
	webapi_inst_add(0) ;
	webapi_add_property(0, 0) ;
	webapi_ep_available(0) ;
	webapi_swagger_yaml()  ;
	webapi_swagger_yaml_free(0) ;
	webapi_generate_simple_json(0) ;
	webapi_simple_json_free(0) ;
	webapi_post(0, 0) ;
}
