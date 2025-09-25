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

/**
 * @file    strsub
 * @brief   String Substitution Utility
 * @details Substrings between an open and a closing delimiter token are replaced
 *          with a new string, for example:
 *
 *              The name of the module is [name].
 *
 *          In this case "[" and "]" is the opening and the closing delimiter
 *          tokens. An installed handler can replace [name] wit a replacement
 *          string. One or more handlers can be installed for a pair of delimiter
 *          tokens.
 *
 *
 *
 * @addtogroup
 * @{
 */


#ifndef __STRSUB_H__
#define __STRSUB_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * Tokens used by the default instance of the module.
 */
#define STRSUB_HANDLERS_TOKENS      {"[]", "%%", "<>"}
#define STRSUB_ESCAPE_TOKEN         '\\'

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

typedef enum strsub_token_e {
    StrsubDull = -2,
    StrsubEsc = -1,
    StrsubToken1,
    StrsubToken2,
    StrsubToken3,
    StrsubLast

} strsub_token_t ;

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef int32_t (*STRSUB_REPLACE_CB)(const char * str, uint32_t len, uint32_t offset, uintptr_t arg) ;
typedef int32_t (*STRSUB_LOOKUP_CB)(STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t offset, uintptr_t arg) ;

typedef struct STRSUB_HANDLER_S {
    struct STRSUB_HANDLER_S *   next  ;
    STRSUB_LOOKUP_CB            cb ;
} STRSUB_HANDLER_T ;

typedef  char * STRSUB_TOKENS[StrsubLast];

typedef struct STRSUB_INSTANCE_S {
    char                        escape ;
    STRSUB_TOKENS               tokens ;
    STRSUB_HANDLER_T *          handlers[StrsubLast] ;
} STRSUB_INSTANCE_T ;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


    /**
     * Replacement handlers can be installed for an instance of the string substitution
     * module. If 'instance' is NULL the default instance is used.
     */
    extern bool         strsub_install_handler (STRSUB_INSTANCE_T * instance, strsub_token_t token, STRSUB_HANDLER_T* handler, STRSUB_LOOKUP_CB cb) ;
    extern void         strsub_uninstall_handler (STRSUB_INSTANCE_T * instance, strsub_token_t token, STRSUB_HANDLER_T* handler) ;

    /**
     * A source string can be parsed to a user allocated destination string using
     * the following functions.
     */
    extern int32_t      strsub_parse_get_dst_length (STRSUB_INSTANCE_T * instance, const char * str, uint32_t len) ;
    extern uint32_t     strsub_parse_string_to (STRSUB_INSTANCE_T * instance, const char * str, uint32_t len, char* dst, uint32_t dst_len) ;

    /**
     * This is the raw parser function.
     */
    extern uint32_t     strsub_parse (STRSUB_INSTANCE_T * instance, STRSUB_REPLACE_CB cb, const char * str, size_t len, uint32_t * tokens, uintptr_t arg) ;

#ifdef __cplusplus
}
#endif

#endif /* __STRSUB_H__ */

/** @} */
