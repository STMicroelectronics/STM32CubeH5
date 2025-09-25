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
 * @file jsmn_tools.h
 *
 * @brief   An Auxiliary Header for JSMN Library Enabling Enhanced JSON
 *          Structure Enumeration
 *
 * This header file, designed to complement the jsmn.h library, is tailored
 * for use in conjunction with a specially defined JSMN_PARENT_CNT during the
 * compilation of jsmn.h. This subtle customisation allows for refined and
 * advanced JSON structure traversal, making it possible to iteratively explore
 * nested sub-objects within the JSON hierarchy.
 *
 */

#ifndef __JSNM_TOOLS_H__
#define __JSNM_TOOLS_H__

#define JSMN_HEADER
#define JSMN_PARENT_LINKS
#include "jsmn.h"


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JSMN_PRINT(js, t)       t->type | (JSMN_STRING|JSMN_PRIMITIVE) ? t->end - t->start : 0, \
                                t->type | (JSMN_STRING|JSMN_PRIMITIVE) ? js + t->start : ""


/**
 * Structure for representing a key-value pair in a JSON object.
 *
 * This structure is used to associate a JSON object key (a string) with its
 * corresponding value (a jsmntok_t token). It is employed to store and manage
 * key-value pairs extracted from a JSON object during parsing.
 *
 * @param key   A pointer to the key string.
 * @param value A pointer to the jsmntok_t token representing the value.
 */
typedef struct jsmn_key_val_s {
    const char * key ;
    jsmntok_t * value ;
} jsmn_key_val_t ;

/**
 * Enumerate and extract key-value pairs from a JSON object.
 *
 * This function takes a JSON object represented by a jsmntok_t pointer
 * and extracts its key-value pairs. It then updates an array of jsmn_key_val_t
 * structures, associating the keys with their corresponding values.
 *
 * This function will not traverse nested structures or arrays.
 *
 *
 * @param js          A pointer to the JSON string.
 * @param t           A pointer to the JSON object token.
 * @param count       The number of tokens in the JSON object.
 * @param out         An array of jsmn_key_val_t structures to store the key-value pairs.
 * @param out_count   The size of the 'out' array.
 *
 * @return 0 if successful, -1 if an error occurs (e.g., unexpected JSON structure).
 */
int jsmn_object_enum (const char *js, jsmntok_t *t, size_t count,
                            jsmn_key_val_t * out, size_t out_count) ;

/**
 * Get the next JSON token in a sequence.
 *
 * This function takes the current token pointer 't' and returns a pointer
 * to the next token in a JSON token sequence. If 't' is NULL, it returns
 * the first token in the sequence.
 *
 * This function will not traverse nested structures or arrays.
 *
 * @param first A pointer to the first token in the JSON token sequence.
 * @param t     A pointer to the current token.
 *
 * @return A pointer to the next token in the sequence or NULL if there are no more tokens.
 */
jsmntok_t * jsmn_get_next(jsmntok_t *first, jsmntok_t *t);


int jsmn_eq(const char *json, jsmntok_t *tok, const char *s);

int jsmn_copy_str (const char *js, jsmntok_t *t, char * sz, size_t size);

unsigned int jsmn_get_hex (const char *js, jsmntok_t *t) ;

unsigned int jsmn_object_is_null(const char *js, jsmntok_t* t) ;

#ifdef __cplusplus
}
#endif

#endif /* __JSNM_TOOLS_H__ */
