/*
    Copyright (C) 2015-2023, Navaro, All Rights Reserved
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


#ifndef __STARTER_H__
#define __STARTER_H__

#include <stdint.h>
#include <stdbool.h>

/*===========================================================================*/
/* Pre-compile time settings.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef enum {
    typeNone,
    typeAction,
    typeEvent,
    typeConstant
} starter_list_t;

typedef int32_t (*STARTER_OUT_FP)(void* /*ctx*/, uint32_t /*out*/,
                            const char* /*str*/);
typedef void (*STARTER_LIST_FP)(void* /*ctx*/, starter_list_t /*type*/,
                            const char * /*name*/, const char* /*description*/) ;

#define STARTER_OUT_OUT_STD         (1)
#define STARTER_OUT_OUT_ERR         (2)



/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Starter public interface.
     */
    int32_t     starter_init (void * arg) ;
    int32_t     starter_start (const char* buffer, uint32_t length,
                                void* ctx, STARTER_OUT_FP log, bool verbose) ;
    int32_t     starter_stop (void) ;

    /*
     * Debug functions.
     */
    int32_t     starter_compile (const char* buffer, uint32_t length,
                                void* ctx,STARTER_OUT_FP log, bool verbose) ;
    int32_t     starter_parts_list (void* ctx, STARTER_LIST_FP log) ;

#ifdef __cplusplus
}
#endif

#endif /* __STARTER_H__ */
