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

#include "qoraal/config.h"
#if !defined CFG_COMMON_STRSUB_DISABLE

#include <string.h>
#include "qoraal/common/strsub.h"
#include "qoraal/errordef.h"

#pragma GCC diagnostic ignored  "-Wmissing-braces"
static STRSUB_INSTANCE_T  _strsub_instance = {STRSUB_ESCAPE_TOKEN,
                                                STRSUB_HANDLERS_TOKENS, {0}} ;


bool
strsub_install_handler (STRSUB_INSTANCE_T * instance, strsub_token_t token,
        				STRSUB_HANDLER_T* handler, STRSUB_LOOKUP_CB cb)
{
    if (instance == 0) instance = &_strsub_instance ;


    if ((token >= 0) &&
            ((unsigned int)token < StrsubLast)) {

        STRSUB_HANDLER_T * l =  instance->handlers[token] ;
        for (  ; (l!=0)  ; ) {
            if (l == handler) return true ;
            l = l->next;

        }

        handler->cb = cb ;
        handler->next = instance->handlers[token] ;
        instance->handlers[token] = handler ;

        return true ;
    }

    return false ;
}

void
strsub_uninstall_handler (STRSUB_INSTANCE_T * instance, strsub_token_t token,
							STRSUB_HANDLER_T* handler)
{
	if (instance == 0) instance = &_strsub_instance ;

	STRSUB_HANDLER_T * l =  instance->handlers[token] ;
	STRSUB_HANDLER_T * prev = 0 ;

    for (  ; (l!=0) && (l!=handler) ; ) {

        prev = l ;
        l = l->next;

    }

    if ((l == handler) && prev) {
            prev->next = l->next ;

    } else if (l == handler) {
    	instance->handlers[token] = l->next ;
    }

    return  ;
}

uint32_t
strsub_parse (STRSUB_INSTANCE_T * instance, STRSUB_REPLACE_CB cb,
        const char * buffer, size_t len, uint32_t *tokens, uintptr_t arg)
{
    const char *p, *start_of_word = buffer ;
    strsub_token_t state = StrsubDull;
    int32_t offset = 0 ;
    int32_t res = 0 ;
    int c;
    uint32_t i  ;

    if (instance == 0) instance = &_strsub_instance ; /* default instance */
    if (tokens) *tokens = 0 ;

    for (p = buffer;
            res >= 0 && *p != '\0' && ((unsigned int)(p - buffer) < len) ;
            p++) {
        c = (unsigned char) *p;
        switch (state) {
        case StrsubDull:

            if (instance->escape && (c == instance->escape)) {
                state = StrsubEsc ;
                continue ;

            } else {
                for (i= 0; i < StrsubLast; i++) {
                    if (
                            instance->tokens[i] &&
                            instance->handlers[i] &&
                            (c == instance->tokens[i][0])
                        ) {
                        /* open token found */
                        state = i ;
                        start_of_word = p + 1;

                        break ;

                    }

                }

                if (state > StrsubDull) {
                    continue ;
                }
            }

            /* emit one char at a time */
            if ((res = cb (p, 1, offset, arg)) > 0 ){
                offset += res ;

            }
            break;

        case StrsubEsc:

            /* emit the escaped char */
            if ((res = cb (p, 1, offset, arg)) > 0 ){
                    offset += res ;
            }
            state = StrsubDull ;
            break ;

        case StrsubToken1:
        case StrsubToken2:
        case StrsubToken3:

            if (c == instance->tokens[state][1]) {

                /* closing token found */
                STRSUB_HANDLER_T* start ;
                for ( start = instance->handlers[state] ;
                        (start!=0)
                            ; ) {

                        if ((res = start->cb (cb, start_of_word,
                                        p - start_of_word, offset, arg)) >= 0) {
                            /* handler send replacement text to cb function. Advance
                               the read pointer past the closing delimiter token
                               and continue */
                            offset += res ;
                            if (tokens) (*tokens)++ ;
                            break ;

                        }
                        start = start->next ;

                    }
                    if (!start) {
                        /* No handler found: leave as is including tokens */
                        res = cb (start_of_word-1, p - start_of_word + 2, offset, arg) ;
                        if (res > 0) {
                            offset += res ;
                        }

                    }
                    state = StrsubDull;

            }
            continue;

        default:
            state = StrsubDull;
            continue ;

        }

    }

    if (res >= 0) {
        /* add a terminating zero */
        res = cb ("\0", 1, offset, arg) ;

    }

    return offset ;
}

/* Following is an implementation of the strsub module to replace delimited strings. */

typedef struct STRSUB_TO_CB_S {
    char *      buffer ;
    uint32_t    len ;
} STRSUB_TO_CB_T ;

static int32_t
strsub_cb(const char * str, uint32_t len, uint32_t offset, uintptr_t arg)
{
    STRSUB_TO_CB_T * parg = (STRSUB_TO_CB_T*)arg ;

    if (offset >= parg->len - 1) {
        return -1 ;

    }

    if (len + offset >= parg->len - 1) {
        len = parg->len - offset - 1;

    }
    if (parg->buffer) {
        memcpy (&(parg->buffer[offset]), str, len) ;

    }


    return len ;
}

int32_t
strsub_parse_get_dst_length (STRSUB_INSTANCE_T * instance,
                                const char * str, uint32_t len)
{
    STRSUB_TO_CB_T arg = {0};
    arg.len = ~0 ;
    uint32_t tokens = 0 ;

    if (str == 0) return 0 ;
    if (!len) len = strlen(str) ;

    /* NULL buffer will get the length only */
    int32_t dstlen = strsub_parse (instance, strsub_cb, str, len, &tokens, (uintptr_t)&arg) ;
    if (dstlen < 0) {
        return EFAIL ;

    }
    if (!tokens) {
        return E_NOTFOUND ;

    }

    return dstlen + 1 ;
}

uint32_t
strsub_parse_string_to (STRSUB_INSTANCE_T * instance, const char * str,
                        uint32_t len, char* dst, uint32_t dst_len)
{
    STRSUB_TO_CB_T arg = {0} ;
    arg.buffer = dst ;
    arg.len = dst_len ;
    arg.buffer[0] = '\0' ;

    if (str == 0) return 0 ;
    if (!len) len = strlen(str) ;

    uint32_t offset = strsub_parse (instance, strsub_cb, str, len, 0, (uintptr_t)&arg) ;

    return offset ;
}

#endif /* CFG_COMMON_STRSUB_DISABLE */
