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
#if !defined CFG_JSON_DISABLE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "qoraal/qoraal.h"
#include "qoraal/common/lists.h"
#include "qoraal-http/qoraal.h"
#include "qoraal-http/httpwebapi.h"
#include "qoraal-http/json/jsmn_tools.h"
#include "qoraal-http/json/frozen.h"


// Define the type of property (e.g., string, integer)
static linked_t _webapi_inst_list ;
static QORAAL_HEAP _webapi_heap = QORAAL_HeapAuxiliary ;
static const char * _webapi_root = "api" ;

int32_t webapi_init (const char * root, QORAAL_HEAP heap)
{
    _webapi_root = root ;
    _webapi_heap = heap ;
    linked_init (&_webapi_inst_list) ;
    return EOK ;
}

int32_t webapi_inst_add (WEBAPI_INST_T * inst)
{
    linked_init (&inst->props_list) ;
    linked_add_tail (&_webapi_inst_list, inst, OFFSETOF(WEBAPI_INST_T, next)) ;
    return EOK ;
}

WEBAPI_INST_T *  webapi_inst_get (const char * ep)
{
    WEBAPI_INST_T* current = linked_head(&_webapi_inst_list);
    while (current != NULL) {
        if (strcmp(ep, current->ep) == 0) return current ;
        current = linked_next(current, OFFSETOF(WEBAPI_INST_T, next));

    }

    return 0 ;
}

bool webapi_ep_available(const char * ep)
{
    return webapi_inst_get (ep) ? true : false ;
}

// Function to create a new ServiceProperty and return a pointer to it
int32_t webapi_add_property(WEBAPI_INST_T *inst, WEBAPI_PROP_T *prop) 
{
    linked_add_tail(&inst->props_list, prop, OFFSETOF(WEBAPI_PROP_T, next));

    return EOK;
}

// Helper function to generate or calculate the length of the Swagger YAML
static size_t swagger_yaml(char* buffer, size_t len) {
    size_t total_length = 0;

    // Static YAML header
    total_length += snprintf(buffer ? buffer : NULL, len,
        "openapi: 3.0.0\n"
        "info:\n"
        "  title: API Documentation\n"
        "  version: '1.0'\n"
        "paths:\n");

    // Iterate over all instances
    WEBAPI_INST_T* inst = (WEBAPI_INST_T*) linked_head(&_webapi_inst_list);
    while (inst != NULL) {
        // For each instance, generate the path
        total_length += snprintf(buffer ? buffer + total_length : NULL, buffer ? len - total_length : 0,
            "  %s/%s:\n"
            "    get:\n"
            "      summary: Get all properties of %s\n"
            "      responses:\n"
            "        '200':\n"
            "          description: Successful response\n"
            "          content:\n"
            "            application/json:\n"
            "              schema:\n"
            "                type: object\n"
            "                properties:\n",
            _webapi_root, inst->ep, inst->title);

        // Iterate through the properties of the instance
        WEBAPI_PROP_T* current = (WEBAPI_PROP_T*) linked_head(&inst->props_list);
        while (current != NULL) {
            const char* type_str = (current->type == PROPERTY_TYPE_STRING) ? "string" :
                                   (current->type == PROPERTY_TYPE_INTEGER) ? "integer" :
                                   (current->type == PROPERTY_TYPE_BOOLEAN) ? "boolean" : "unknown";

            // Property details
            total_length += snprintf(buffer ? buffer + total_length : NULL, 
                                     buffer ? len - total_length : 0,
                "                  %s:\n"
                "                    type: %s\n"
                "                    description: %s\n",
                current->name,
                type_str,
                current->description
            );

            current = linked_next(current, OFFSETOF(WEBAPI_PROP_T, next));
        }

        // Move to the next instance
        inst = linked_next(inst, OFFSETOF(WEBAPI_INST_T, next));
    }

    return total_length;
}

// Function to generate Swagger YAML for all properties
char* webapi_swagger_yaml(void) {
    // First pass: Get the required buffer size by calling swagger_yaml() with NULL
    size_t yaml_length = swagger_yaml(NULL, 0);

    // Allocate the buffer
    char* yaml_buffer = (char*) qoraal_malloc(_webapi_heap, yaml_length + 1);  // +1 for null terminator
    if (yaml_buffer == NULL) {
        return NULL;  // Handle memory allocation failure
    }

    // Second pass: Generate the YAML by calling swagger_yaml() with the allocated buffer
    swagger_yaml(yaml_buffer, yaml_length+1);

    return yaml_buffer;  // Return the generated YAML string
}


void webapi_swagger_yaml_free(char * buffer)
{
    qoraal_free(_webapi_heap, buffer);
}


static int json_printer_null(struct json_out *out, const char *buf, size_t len) {
    out->u.buf.len += len ;
    return len;
}

#define JSON_OUT_NULL() \
  {                            \
    json_printer_null, {        \
      { 0, 0, 0 }          \
    }                          \
  }

// Helper function to generate simple key-value JSON with correct types
static size_t generate_simple_json(WEBAPI_INST_T *inst, struct json_out *out) {
    size_t total_length = 0;

    // Start the JSON object
    total_length += json_printf(out, "{");

    if (inst) {
        // Iterate through the linked list of properties
        WEBAPI_PROP_T *current = (WEBAPI_PROP_T *)linked_head(&inst->props_list);

        while (current != NULL) {
            // Get the value using the callback
            total_length += json_printf(out, "\"%s\": ", current->name);

            int32_t res = EFAIL ;

            if (current->get_callback != NULL) {
                uint32_t get_buffer[WEBAPI_GET_BUFFER_MAX/sizeof(uint32_t)] ;
                int int_value;
                bool bool_value;
                res = current->get_callback((void*)get_buffer);

                if (res >= EOK) {
                    // Handle value formatting based on property type
                    switch (current->type) {
                        case PROPERTY_TYPE_STRING:
                            total_length += json_printf(out, "%Q", (const char *)get_buffer);  // Print as JSON string
                            break;
                        case PROPERTY_TYPE_INTEGER:
                            memcpy(&int_value, get_buffer, sizeof(int));
                            total_length += json_printf(out, "%d", int_value);  // Print as integer
                            break;
                        case PROPERTY_TYPE_BOOLEAN:
                            memcpy(&bool_value, get_buffer, sizeof(bool));
                            total_length += json_printf(out, "%B", bool_value);  // Print as boolean (true/false)
                            break;
                        default:
                            total_length += json_printf(out, "%Q", 0);  // Print  null
                            break;
                    }
                }
            } 
            
            if (res < EOK) {
                total_length += json_printf(out, "%Q", 0);  // Print  null

            }

            // Print the property as a key-value pair in JSON format

            current = linked_next(current, OFFSETOF(WEBAPI_PROP_T, next));
            if (current) {
                total_length += json_printf(out, ",");

            }
        }

    }

    // Close the JSON object
    total_length += json_printf(out, "}");

    return total_length;
}

// Function to generate simple key-value JSON for all properties
char *webapi_generate_simple_json(const char * ep) 
{
    WEBAPI_INST_T *inst = webapi_inst_get (ep) ;

    // First pass: calculate the required buffer size
    struct json_out out_size = JSON_OUT_NULL();  // No buffer, just calculating the size
    size_t json_length = generate_simple_json(inst, &out_size);

    // Allocate the buffer
    char *json_buffer = (char *)qoraal_malloc(_webapi_heap, json_length + 1);  // +1 for null terminator
    if (json_buffer == NULL) {
        return NULL;  // Handle memory allocation failure
    }

    // Second pass: generate the JSON content
    struct json_out out_buffer = JSON_OUT_BUF(json_buffer, json_length + 1);
    generate_simple_json(inst, &out_buffer);

    return json_buffer;  // Return the generated JSON string
}

// Function to free the JSON buffer
void webapi_simple_json_free(char *buffer)
{
	qoraal_free(_webapi_heap, buffer);
}

int32_t webapi_post(const char * ep, const char *json) 
{
    int32_t res = EOK ;
    jsmn_parser parser;
    jsmntok_t *tokens;
    int32_t len = strlen(json); 

    if (len == 0) {
        return E_PARM;
    }
    
    WEBAPI_INST_T *inst = webapi_inst_get (ep) ;

    if (!inst) {
        return E_NOTFOUND ;

    }

    jsmn_init(&parser);

    // Get the number of tokens
    int token_num = jsmn_parse(&parser, json, len, NULL, 0);

    if (token_num <= 0) {
        return E_PARM;
    }

    tokens = qoraal_malloc(_webapi_heap, token_num * sizeof(jsmntok_t));
    if (!tokens) {    	
        return E_NOMEM;
    }

    jsmn_init(&parser);

    // Parse the JSON
    token_num = jsmn_parse(&parser, json, len, tokens, token_num);

    if (tokens[0].type != JSMN_OBJECT) {
        qoraal_free(_webapi_heap, tokens);
        return E_PARM;
    }

    jsmntok_t *key = jsmn_get_next(tokens, 0);
    jsmntok_t *val = key + 1;

    while (key && val) {
        if (
            (key->type != JSMN_STRING) ||
            ((val->type != JSMN_STRING) && (val->type != JSMN_PRIMITIVE))
        ) {
            // Only accept key-value pairs
        	qoraal_free(_webapi_heap, tokens);
            return E_PARM;
        }

        // Copy the string value
        char temp_str[WEBAPI_GET_BUFFER_MAX];
        jsmn_copy_str(json, key, temp_str, sizeof(temp_str));

        // Look for the property in the list
        WEBAPI_PROP_T *prop = (WEBAPI_PROP_T *)linked_head(&inst->props_list);
        while (prop) {
            // Match the property name with the JSON key
            if (strcmp(prop->name, temp_str) == 0) {
                // Found the matching property, now set its value
                jsmn_copy_str(json, val, temp_str, sizeof(temp_str));
                if (prop->set_callback) {
                    switch (prop->type) {
                        case PROPERTY_TYPE_STRING: {
                            res = prop->set_callback((void *)temp_str);
                            break;
                        }
                        case PROPERTY_TYPE_INTEGER: {
                            // Convert the JSON value to integer and call the setter
                            int temp_int = atoi(temp_str);
                            res = prop->set_callback(&temp_int);
                            break;
                        }
                        case PROPERTY_TYPE_BOOLEAN: {
                            // Convert to boolean (true/false)
                            bool temp_bool = (strncmp(temp_str, "true", 4) == 0);
                            res = prop->set_callback(&temp_bool);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            if (res < 0) break ;

            // Move to the next property
            prop = linked_next(prop, OFFSETOF(WEBAPI_PROP_T, next));
        }

        // Move to the next key-value pair
        key = jsmn_get_next(tokens, key);
        val = key + 1;
    }

    qoraal_free(_webapi_heap, tokens);
    return res < 0 ? res : EOK ;
}

#endif /* CFG_JSON_DISABLE */


