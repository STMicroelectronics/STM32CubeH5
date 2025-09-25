
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
#include <stdlib.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal-http/wserver.h"
#include "qoraal-http/httpwebapi.h"
#include "wwebapi.h"

static bool _upgrade_init = false ;

static const char  status_fail[] =              "{ \"status\": \"fail\" }" ;

static char _upgrade_url[WEBAPI_GET_BUFFER_MAX] = "https://navaro.nl/upgrade.php" ;
static bool _upgrade_start = false ;

/*===========================================================================*/
/* Upgrade API declarations.                                                    */
/*===========================================================================*/

static int32_t
upgrade_start_get(void* val)
{
    *(bool*)val = _upgrade_start ;
    return sizeof(bool) ;
}

static int32_t
upgrade_start_set(void* val)
{
	_upgrade_start = *(bool*)val ;
    return sizeof(bool) ;
}

static int32_t
upgrade_url_get(void* val)
{
    return snprintf ((char*)val, WEBAPI_GET_BUFFER_MAX, "%s", _upgrade_url) ;
}

static int32_t
upgrade_url_set(void* val)
{
    return snprintf (_upgrade_url, WEBAPI_GET_BUFFER_MAX, "%s", (const char*)val) ;
}

static int32_t
upgrade_status_get(void* val)
{
    int32_t status = 0 ;
    *(int32_t*)val = status ;
    return sizeof(int32_t) ;
}

static int32_t
upgrade_version_get(void* val)
{
    snprintf ((char*)val, WEBAPI_GET_BUFFER_MAX - 1,
        "%d.%d.%d", 1, 0, 0) ;
 
    return strlen (val) ;
}


static WEBAPI_INST_DECL(_wupgrade_api, "UPGRADE API", "1.0", "upgrade");
static WEBAPI_PROP_DECL(_wupgrade_prop_start, "start", PROPERTY_TYPE_BOOLEAN, "start upgrade", upgrade_start_get, upgrade_start_set) ;
static WEBAPI_PROP_DECL(_wupgrade_prop_url, "url", PROPERTY_TYPE_STRING, "upgrade-config url", upgrade_url_get, upgrade_url_set) ;
static WEBAPI_PROP_DECL(_wupgrade_prop_status, "status", PROPERTY_TYPE_INTEGER, "system status", upgrade_status_get, 0) ;
static WEBAPI_PROP_DECL(_wupgrade_prop_version, "version", PROPERTY_TYPE_STRING, "current version", upgrade_version_get, 0) ;


/*===========================================================================*/
/* server API                                                                */
/*===========================================================================*/

const char*
wwebapi_metadata (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type)
{
    if (type == WSERVER_CTRL_METADATA_HEADING) {
        return "UPGRADE API" ;
    }

    return 0 ;
}
    
int32_t
write_response (HTTP_USER_T *user, char * ep)
{
    char * json = webapi_generate_simple_json (ep) ;

    if (!json) {
        return httpserver_write_response (user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_HTML,
            0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;

    }

    int32_t res = httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_JSON,
            0, 0, json, strlen (json)) ;

    webapi_simple_json_free (json) ;
    return  res ; 
}

/**
 * @brief   Handler 
 *
 * @app
 */
int32_t
wwebapi_handler(HTTP_USER_T *user, uint32_t method, char* endpoint)
{            
    int32_t res = HTTP_SERVER_WSERVER_E_OK ;

    if (!_upgrade_init) {
        _upgrade_init = true ;
        webapi_init ("webapi", QORAAL_HeapAuxiliary) ;
        webapi_inst_add (&_wupgrade_api) ;
        webapi_add_property (&_wupgrade_api, &_wupgrade_prop_version) ;
        webapi_add_property (&_wupgrade_api, &_wupgrade_prop_url) ;
        webapi_add_property (&_wupgrade_api, &_wupgrade_prop_start) ;
        webapi_add_property (&_wupgrade_api, &_wupgrade_prop_status) ;

    }

    char* cmd[5]  = {0} ;
    int i ;

    cmd[0] = strchr (endpoint, '/') ;
    for (i=0; i<5; i++) {
        if (cmd[i]) *(cmd[i])++ = 0 ;
        if (cmd[i]) cmd[i+1] = strchr (cmd[i], '/') ;
        if (cmd[i+1] == 0) break ;

    }

    if (!cmd[0]) {
        return httpserver_write_response (user, WSERVER_RESP_CODE_400, HTTP_SERVER_CONTENT_TYPE_HTML,
                0, 0, WSERVER_RESP_CONTENT_400, strlen(WSERVER_RESP_CONTENT_400)) ;

    }

    if (method == HTTP_HEADER_METHOD_GET) {


        if ((strcmp(cmd[0], "docs") == 0) || (strcmp(cmd[0], "swagger") == 0)) {

            char * buffer = webapi_swagger_yaml () ;

            if (!buffer) {
                return httpserver_write_response (user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_HTML,
                    0, 0, WSERVER_RESP_CONTENT_500, strlen(WSERVER_RESP_CONTENT_500)) ;

            }

            res = httpserver_write_response (user, 200, HTTP_SERVER_CONTENT_TYPE_JSON,
                    0, 0, buffer, strlen (buffer)) ;
            webapi_swagger_yaml_free (buffer) ;
            return  res ; 

        } else if (webapi_ep_available(cmd[0])) {

            return write_response (user, cmd[0]) ;
        }


    } else {

        if (webapi_ep_available(cmd[0])) {
            char * content ;
            int32_t len = httpserver_read_all_content_ex (user, 1000, &content) ;

            if (len <= 0) {
                httpserver_write_response (user, WSERVER_RESP_CODE_400, HTTP_SERVER_CONTENT_TYPE_HTML,
                    0, 0, WSERVER_RESP_CONTENT_400, strlen(WSERVER_RESP_CONTENT_400)) ;
                return HTTP_SERVER_E_CONTENT ;

            }

            if (webapi_post(cmd[0], content) == EOK) {

                 return write_response (user, cmd[0]) ;

            } else {
                return httpserver_write_response (user, WSERVER_RESP_CODE_500, HTTP_SERVER_CONTENT_TYPE_JSON,
                    0, 0, status_fail, sizeof(status_fail) - 1) ;

            }


        }
    }

    return httpserver_write_response (user, WSERVER_RESP_CODE_400, HTTP_SERVER_CONTENT_TYPE_HTML,
             0, 0, WSERVER_RESP_CONTENT_400, strlen(WSERVER_RESP_CONTENT_400)) ;

}


