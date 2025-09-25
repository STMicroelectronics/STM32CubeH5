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


#ifndef __REGISTRY_H__
#define __REGISTRY_H__

#include <stdint.h>
#include <string.h>
#include "qoraal-flash/nvram/nvol3.h"


#define DBG_MESSAGE_REGISTRY(severity, fmt_str, ...)        DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_REGISTRY                                 DBG_ASSERT_T
#define DBG_CHECK_REGISTRY                                  DBG_ASSERT_T

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

typedef NVOL3_INSTANCE_T REGISTRY_INSTANCE_T;

#define REGISTRY_INST_DECL(name, start_addr, sector_size, key_size, data_size, hashsize)  \
        NVOL3_INSTANCE_DECL(name, \
            start_addr, \
            start_addr + sector_size, \
            sector_size, \
            key_size, \
            DICTIONARY_KEYSPEC_STRING, \
            hashsize /*hashsize*/, \
            sizeof(uint16_t)*2 + data_size, \
            4 /*local_size*/, \
            0 /*tallie*/, \
            NVOL3_SECTOR_VERSION /*version*/) ;


#define REGISTRY_TYPE_NONE                  ((uint16_t)-1)
#define REGISTRY_TYPE_STRING                0
#define REGISTRY_TYPE_INT                   1
#define REGISTRY_TYPE_ENUM                  2
#define REGISTRY_TYPE_BLOB                  3

#define REGISTRY_GET_TYPE(type)             (type & 0xFF)
#define REGISTRY_GET_ENUM_TYPE(type)        ((type>>8) & 0xFF)
#define REGISTRY_TYPE(type, enum_type)      (type | (enum_type<<8))

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

#if 1
typedef const char* REGISTRY_KEY_T ;
#else
typedef uint16_t STRTAB_KEY_T ;
#endif


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    int32_t     registry_init(REGISTRY_INSTANCE_T * inst) ;
    int32_t     registry_start(void) ;
    void        registry_stop(void) ;
    int32_t     registry_erase(void) ;
    int32_t     registry_repair (void) ;

    int32_t     registry_value_type (REGISTRY_KEY_T id) ;
    uint32_t    registry_value_valid (REGISTRY_KEY_T id, uint16_t type) ;
    int32_t     registry_value_delete (REGISTRY_KEY_T id) ;
    int32_t     registry_value_length (REGISTRY_KEY_T id) ;
    int32_t     registry_value_get (REGISTRY_KEY_T id, char* value, uint16_t* type, unsigned int length) ;
    int32_t     registry_value_set (REGISTRY_KEY_T id, const char* value, uint16_t type,  unsigned int length) ;

    int32_t     registry_int32_get (REGISTRY_KEY_T id, int32_t* value) ;
    int32_t     registry_int32_set (REGISTRY_KEY_T id, int32_t value) ;
    int32_t     registry_get (REGISTRY_KEY_T id, int32_t default_value) ;

    int32_t     registry_string_length (REGISTRY_KEY_T id) ;
    int32_t     registry_string_get (REGISTRY_KEY_T id, char* value, unsigned int length ) ;
    int32_t     registry_string_set (REGISTRY_KEY_T id, const char* value ) ;

    int32_t     registry_set_strval (REGISTRY_KEY_T id, const char * value, uint16_t type) ;
    int32_t     registry_get_strval (REGISTRY_KEY_T id, char * value, uint32_t length, uint16_t * type) ;

    int32_t     registry_first (REGISTRY_KEY_T* key, char* value, uint16_t* type, int length) ;
    int32_t     registry_next (REGISTRY_KEY_T* key, char* value, uint16_t* type, int length) ;

    void        registry_log_status (void) ;

    static inline uint32_t  registry_blob_valid (REGISTRY_KEY_T id) { return registry_value_valid(id, REGISTRY_TYPE_BLOB) ; }
    int32_t     registry_blob_value_set (REGISTRY_KEY_T id, const uint8_t * blob) ;
    int32_t     registry_blob_value_get (REGISTRY_KEY_T id, uint8_t * blob, uint32_t length) ;


#ifdef __cplusplus
}
#endif

#endif /* __REGISTRY_REGISTRY_H__ */
