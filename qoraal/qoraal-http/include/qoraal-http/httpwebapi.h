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

#ifndef __HTTPWWEBSERVICE_H__
#define __HTTPWWEBSERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "qoraal/qoraal.h"
#include "qoraal/common/lists.h"

#define DBG_MESSAGE_HTTPWEBSERVICE(severity, fmt_str, ...)           DBG_MESSAGE_T_LOG(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_HTTPWEBSERVICE                    DBG_ASSERT


/*===========================================================================*/
/* Client constants.                                                         */
/*===========================================================================*/

#define WEBAPI_GET_BUFFER_MAX               128

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/


/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/



typedef struct WEBAPI_INST_S {
    struct WEBAPI_INST_S * next ;
    const char * title ;
    const char * version ;
    const char * ep ;

    linked_t props_list ;
    
} WEBAPI_INST_T ;


#define WEBAPI_INST_DECL(name, title, version, ep)    \
    WEBAPI_INST_T  name = {                                 \
        0,                                                  \
        title,                                              \
        version,                                            \
        ep,                                                 \
        {0,0}   \
    }

typedef enum {
    PROPERTY_TYPE_STRING,
    PROPERTY_TYPE_INTEGER,
    PROPERTY_TYPE_BOOLEAN
} WEBAPI_PROP_TYPE;


// Update the WEBAPI_PROP_T structure to include access type
typedef struct WEBAPI_PROP_S {
    const char * name;  // Property name (e.g., "state")
    WEBAPI_PROP_TYPE type;  // Data type of the property (string, integer, boolean)
    const char * description;  // Description for Swagger documentation
    int32_t (*get_callback)(void*);  // Callback function for GET requests
    int32_t (*set_callback)(void*);  // Callback function for POST/PUT requests
    struct WEBAPI_PROP_S* next;  // Pointer to next property (for linked list structure)
} WEBAPI_PROP_T;

#define WEBAPI_PROP_DECL(name, prop, type, description, get, set)    \
    WEBAPI_PROP_T  name = {                                 \
        prop,                                               \
        type,                                               \
        description,                                        \
        get,                                                \
        set,                                                \
        0                                                   \
    }


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t webapi_init (const char * root, QORAAL_HEAP heap) ;

    int32_t webapi_inst_add(WEBAPI_INST_T * inst) ;
    int32_t webapi_add_property(WEBAPI_INST_T *inst, WEBAPI_PROP_T *prop) ;

    bool webapi_ep_available(const char * ep) ;

    char* webapi_swagger_yaml(void)  ;
    void webapi_swagger_yaml_free(char * buffer) ;

    char *webapi_generate_simple_json(const char * ep) ;
    void webapi_simple_json_free(char *buffer) ;

    int32_t webapi_post(const char * ep, const char *json) ;

#ifdef __cplusplus
}
#endif


#endif /* __HTTPWWEBSERVICE_H__ */
