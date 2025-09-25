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

/*
 * registry.c
 *
 *  Created on: 3 May 2015
 *      Author: natie
 */

#include "qoraal-flash/config.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "qoraal/common/strsub.h"
#include "qoraal-flash/nvram/nvol3.h"
#include "qoraal-flash/registry.h"


REGISTRY_INSTANCE_T *  _registry_inst = 0 ;
static p_mutex_t        _registry_mutex ;
NVOL3_RECORD_T *        _registry_scratch_value = 0 ;

#if !defined CFG_REGISTRY_STRSUB_DISABLE
static int32_t registry_strsub_cb(STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg) ;
static STRSUB_HANDLER_T _registry_strsub ;
#endif

extern void     keep_registrycmds (void) ;

static void 
scratch_set_key(REGISTRY_KEY_T key, uint32_t len, uint16_t type) {
    size_t key_size = _registry_inst->config->key_size;

    // Copy the key
    if (!len) {
        strncpy((char*)_registry_scratch_value->key_and_data, key, key_size);
    } else {
        memset ((char*)_registry_scratch_value->key_and_data, 0, key_size);
        memcpy ((char*)_registry_scratch_value->key_and_data, key, len);
    }

    // Set reserved to 0
    uint16_t* pres = (uint16_t*)&_registry_scratch_value->key_and_data[key_size];
    *pres = 0;

    // Set the type
    uint16_t* ptype = (uint16_t*)&_registry_scratch_value->key_and_data[key_size + sizeof(uint16_t)];
    *ptype = type;
}

static inline NVOL3_RECORD_T*
scratch_get (void) 
{
    return _registry_scratch_value ;
}

char *
scratch_get_key (void) 
{
    return (char*)_registry_scratch_value->key_and_data ;
}

uint16_t
scratch_get_type (void) 
{
    size_t key_size = _registry_inst->config->key_size;
    uint16_t* ptype = (uint16_t*)&_registry_scratch_value->key_and_data[key_size + sizeof(uint16_t)];
    return *ptype;
}

int32_t
scratch_key_type_len (void) 
{
    return _registry_inst->config->key_size + 2 * sizeof(uint16_t);
}

int32_t
scratch_value_len (void) 
{
    return _registry_inst->config->record_size - scratch_key_type_len() - sizeof(NVOL3_RECORD_HEAD_T) ;
}

char *
scratch_get_value (void)
{
    size_t key_size = _registry_inst->config->key_size;
    return (char*)&_registry_scratch_value->key_and_data[key_size + 2 * sizeof(uint16_t)];
}

int32_t
registry_init(REGISTRY_INSTANCE_T * inst)
{
#if !defined CFG_REGISTRY_STRSUB_DISABLE
    strsub_install_handler(0, StrsubToken1, &_registry_strsub, registry_strsub_cb) ;
#endif

    keep_registrycmds () ;

    _registry_inst = inst ;
    
    return EOK ;
}

int32_t
registry_start(void)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t status = 0 ;

    _registry_scratch_value = NVOL3_MALLOC(_registry_inst->config->record_size) ;
    if (!_registry_scratch_value) return E_NOMEM ;

    if (os_mutex_create (&_registry_mutex) != EOK) {
        NVOL3_FREE (_registry_scratch_value) ;
        _registry_scratch_value = 0 ;
        return EFAIL ;
    }

    os_mutex_lock (&_registry_mutex) ;

    if (nvol3_validate(_registry_inst) != EOK) {
        DBG_MESSAGE_REGISTRY( DBG_MESSAGE_SEVERITY_REPORT, 
                "REG   : : resetting _registry_inst")
        status = nvol3_reset (_registry_inst) ;

    } else {
        status = nvol3_load (_registry_inst) ;

    }

    os_mutex_unlock (&_registry_mutex) ;

    return status ;
}

void
registry_stop(void)
{
    if (!_registry_inst) return ;
    os_mutex_lock (&_registry_mutex) ;
    nvol3_unload (_registry_inst) ;
    _registry_inst = 0 ;
    os_mutex_unlock (&_registry_mutex) ;
    NVOL3_FREE (_registry_scratch_value) ;
    _registry_scratch_value = 0 ;
    os_mutex_delete (&_registry_mutex) ;
}

int32_t
registry_erase(void)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t status = 0 ;
    os_mutex_lock (&_registry_mutex) ;
    status = nvol3_reset (_registry_inst) ;
    os_mutex_unlock (&_registry_mutex) ;

    return status ;
}

int32_t
registry_repair (void)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t status = 0 ;
    os_mutex_lock (&_registry_mutex) ;
    status = nvol3_repair (_registry_inst) ;
    os_mutex_unlock (&_registry_mutex) ;

    return status ;
}

int32_t
registry_value_delete (REGISTRY_KEY_T id)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res = EFAIL;
    DBG_CHECK_T(id, E_PARM, "registry_value_delete id") ;
    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, REGISTRY_TYPE_INT) ;
    res = nvol3_record_delete(_registry_inst, scratch_get()) ;
    os_mutex_unlock (&_registry_mutex) ;

    return res ;
}

int32_t
registry_value_type (REGISTRY_KEY_T id)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res = EFAIL;
    DBG_CHECK_T(id, E_PARM, "registry_value_type id") ;
    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, 0) ;
    if (nvol3_record_get(_registry_inst, scratch_get()) > scratch_key_type_len ()) {
        res = scratch_get_type () ;
    }
    os_mutex_unlock (&_registry_mutex) ;

    return res ;


}

uint32_t
registry_value_valid (REGISTRY_KEY_T id, uint16_t type)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res = 0;
    DBG_CHECK_T(id, E_PARM, "registry_value_valid id") ;
    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, type) ;
    if (nvol3_record_get(_registry_inst, scratch_get()) > scratch_key_type_len ()) {
        res = REGISTRY_GET_TYPE(scratch_get_type ()) == (type)  ;
    }
    os_mutex_unlock (&_registry_mutex) ;

    return res ;

}

int32_t
registry_value_length (REGISTRY_KEY_T id)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res ;
    DBG_CHECK_T(id, E_PARM, "registry_value_length id") ;
    os_mutex_lock (&_registry_mutex) ;
    scratch_set_key (id, 0, 0) ;
    res = nvol3_record_key_and_data_length  (_registry_inst, scratch_get_key ()) ;
    if (res > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;
    } else {
        res = 0 ;
    }

    os_mutex_unlock (&_registry_mutex) ;

    return res ;
}

int32_t
registry_value_get(REGISTRY_KEY_T id, char* value, uint16_t* type, unsigned int length)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res ;
    DBG_CHECK_T(id, E_PARM, "registry_value_get id") ;
    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, 0) ;
    memset(value, 0, length) ;
    if ((res = nvol3_record_get(_registry_inst, scratch_get())) > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;
        if (type) *type = scratch_get_type () ;
        if (value && (length > 0)) {
            res = (int)length <= res ? (int)length : res ;
            memcpy(value, scratch_get_value (), res) ;
        }
    } else if (res >= 0) {
        res = E_INVAL ;

    }
    else if (res == E_NOTFOUND) {
        DBG_MESSAGE_REGISTRY( DBG_MESSAGE_SEVERITY_INFO,
                "REG   : : registry id '%s' not found!", id)

    }

    os_mutex_unlock (&_registry_mutex) ;

    return res ;
}

int32_t
registry_value_set(REGISTRY_KEY_T id, const char* value, uint16_t type, unsigned int length)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res ;
    DBG_CHECK_T(value, E_PARM, "registry_value_set val") ;
    DBG_CHECK_T(id, E_PARM, "registry_value_set id") ;
    if (length > scratch_value_len()) return E_PARM ;
    os_mutex_lock (&_registry_mutex) ;
    scratch_set_key (id, 0, type) ;
    memcpy(scratch_get_value (), value, length) ;

    res =  nvol3_record_set (_registry_inst, scratch_get(), length + scratch_key_type_len ()) ;

    os_mutex_unlock (&_registry_mutex) ;

    return res ;

}

int32_t registry_int32_get (REGISTRY_KEY_T id, int32_t* value) {
    return registry_value_get(id, (char*)value, 0, sizeof(uint32_t)) ;
}
int32_t registry_int32_set (REGISTRY_KEY_T id, int32_t value) {
    return registry_value_set(id, (const char*)&value, REGISTRY_TYPE_INT, sizeof(uint32_t)) ;
}
int32_t registry_string_length (REGISTRY_KEY_T id) {
    return registry_value_length(id) ;
}
int32_t registry_string_get (REGISTRY_KEY_T id, char* value, unsigned int length ) {
    return registry_value_get(id, value, 0, length) ;
}
int32_t registry_string_set (REGISTRY_KEY_T id, const char* value ) {
    return registry_value_set(id, value, REGISTRY_TYPE_STRING, strlen(value)+1) ;
}
int32_t registry_enum_get_value (REGISTRY_KEY_T id, int32_t* value) {
    return registry_value_get(id, (char*)value, 0, sizeof(uint32_t)) ;
}


static NVOL3_ITERATOR_T _registry_it ;

static int32_t
reg_cmp (REGISTRY_KEY_T first, REGISTRY_KEY_T second)
{
    return strcmp (first, second) ;
}

int32_t
registry_first (REGISTRY_KEY_T* key, char* value, uint16_t* type, int length)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res ;
    DBG_CHECK_T(value, E_PARM, "registry_first val") ;
    DBG_CHECK_T(key, E_PARM, "registry_first id") ;
    os_mutex_lock (&_registry_mutex) ;
    if ((res = nvol3_record_first (_registry_inst,
            scratch_get(), &_registry_it, reg_cmp)) > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;
        if (res < length) {
            length = res ;
        }
        memcpy(value, (char*)scratch_get_value (), length) ;
        *key = nvol3_record_key (_registry_inst, &_registry_it)  ;
        *type = scratch_get_type () ;

    } else if (res >= 0) {
        res = E_INVAL ;

    }

    os_mutex_unlock (&_registry_mutex) ;
    return res ;
}

int32_t
registry_next (REGISTRY_KEY_T* key, char* value, uint16_t* type, int length)
{
    if (!_registry_inst) return E_UNEXP ;
    int32_t res ;
    DBG_CHECK_T(value, E_PARM, "registry_next val") ;
    DBG_CHECK_T(key, E_PARM, "registry_next id") ;
    os_mutex_lock (&_registry_mutex) ;
    if ((res = nvol3_record_next (_registry_inst, scratch_get(), &_registry_it)) > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;
        if (res < length) {
            length = res ;
        }
        memcpy(value, (char*)scratch_get_value (), length) ;
        *key = nvol3_record_key (_registry_inst, &_registry_it)  ;
        *type = scratch_get_type () ;

    } else if (res >= 0) {
        res = E_INVAL ;

    }

    os_mutex_unlock (&_registry_mutex) ;
    return res ;
}


int32_t
registry_get (REGISTRY_KEY_T id, int32_t default_value)
{
    int32_t value ;
    int32_t res = registry_int32_get (id, &value);

    return res > 0 ?  value : default_value ;
}


int32_t
get_int_from_str(const char * value, int32_t* intval)
{
    if ((value[0] == '0') && (value[1] == 'x')) {
        if (sscanf(value, "0x%x", (unsigned int*)intval) < 0) {
            return E_VALIDATION ;

        }

    }
    else {
        if (sscanf (value, "%i", (int*)intval) < 1) {
            return E_VALIDATION ;

        }

    }

    return EOK ;
}

int32_t
_set_strval (REGISTRY_KEY_T id, uint16_t type, const char * value)
{
    int32_t res = EFAIL ;
    int32_t intval ;

    if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_STRING) {
        return registry_string_set (id, value) ;

    } else if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_INT) {
        if ((res = get_int_from_str(value, &intval)) == EOK) {
            res = registry_int32_set (id, intval) ;

        }

    } else if (REGISTRY_GET_TYPE(type) == REGISTRY_TYPE_BLOB) {
        res = registry_blob_value_set (id, (const uint8_t *)value) ;

    }

    return res ;
}

int32_t
registry_set_strval (REGISTRY_KEY_T id, const char * value, uint16_t type)
{
    int32_t res = registry_value_type (id) ;

    if ((res >= 0) &&
             ((type == REGISTRY_TYPE_NONE) || (res == type))
        ){
        // update existing
        res = _set_strval (id, res, value) ;


    } else if (res >= 0) {
        // type not the same as existing
        res = E_PARM ;

    } else {
        // create new
        res = _set_strval (id, type, (const char*)value) ;


    }

    return res ;

}

int32_t
registry_get_strval (REGISTRY_KEY_T id, char * value, uint32_t length, uint16_t * type)
{
    int32_t res = registry_value_get (id, value, type, length) ;
    if (res < 0) {
        return res ;
    }

    if (REGISTRY_GET_TYPE(*type) == REGISTRY_TYPE_STRING) {


    } else  {
        int32_t intval ;
        memcpy (&intval, value, sizeof(int32_t));
        res = snprintf (value, length, "%d", (int)intval) ;


    }

    return res ;
}


int32_t
registry_blob_value_set (REGISTRY_KEY_T id, const uint8_t * blob)
{
    int32_t res ;
    int i , cnt ;
    unsigned int val ;
    DBG_CHECK_T(blob, E_PARM, "registry_blob_value_set val") ;
    DBG_CHECK_T(id, E_PARM, "registry_blob_value_set id") ;

    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, REGISTRY_TYPE_BLOB) ;
    for (i=0, cnt=0; cnt<scratch_value_len(); cnt++) {
        while (blob[i] && isspace((int)blob[i])) {
            i++ ;
        }
        if (!blob[i]) break ;
        res = sscanf((const char *)&blob[i], "%x", &val) ;
        if (res != 1) {
            break ;
        }
        scratch_get_value ()[cnt] = (char)val ;
        while (blob[i] && !isspace((int)blob[i])) {
            i++ ;
        }

    }

    res =  nvol3_record_set (_registry_inst, scratch_get(), cnt + scratch_key_type_len ()) ;

    os_mutex_unlock (&_registry_mutex) ;

    return res ;

}

int32_t
registry_blob_value_get (REGISTRY_KEY_T id, uint8_t * blob, uint32_t length)
{
    int32_t res ;
    uint32_t len = 0 ;
    int i = 0 ;
    DBG_CHECK_T(blob, E_PARM, "registry_blob_value_set val") ;
    DBG_CHECK_T(id, E_PARM, "registry_blob_value_set id") ;
    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (id, 0, 0) ;
    memset(blob, 0, length) ;
    if ((res = nvol3_record_get(_registry_inst, scratch_get())) > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;
        while ((len + 4 < length) && (i<res)) {
            unsigned int val = ((uint8_t*)scratch_get_value ())[i++] ;
            len += sprintf ((char*)&blob[len], "%02x ",
                    val) ;

        }

    } else if (res >= 0) {
        res = E_INVAL ;

    }

    os_mutex_unlock (&_registry_mutex) ;

    return len ? len - 1 : 0 ;

}

void
registry_log_status (void)
{
    nvol3_entry_log_status (_registry_inst, 1) ;

}

#if !defined CFG_REGISTRY_STRSUB_DISABLE
int32_t
registry_strsub_cb(STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg)
{
    int32_t res ;

    if (isdigit((int)str[0])) {
        return E_INVALID ;
    }

    os_mutex_lock (&_registry_mutex) ;

    scratch_set_key (str, len, 0) ;

    if ((res = nvol3_record_get(_registry_inst, scratch_get())) > scratch_key_type_len ()) {
        res -= scratch_key_type_len () ;

        if (REGISTRY_GET_TYPE(scratch_get_type ()) == REGISTRY_TYPE_STRING) {
            res = strlen(scratch_get_value ()) ;

        } else  {
            int32_t ival = *((int32_t*)scratch_get_value ());
            res = sprintf (scratch_get_value (), "%d", (int)ival) ;

        }

        res = cb (scratch_get_value (), res, offset, arg) ;

    } else if (res >= 0) {
        res = E_INVAL ;

    }

    os_mutex_unlock (&_registry_mutex) ;

    return res ;
}
#endif


