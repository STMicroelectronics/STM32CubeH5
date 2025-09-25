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

/**
* @mainpage \ref navaro
* @defgroup Navaro Embedded Software
* @date 2 Novemebre, 2022
* @authors Natie van Rooyen
*
* */


#ifndef __ENGINE_PORT_H__
#define __ENGINE_PORT_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>



/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Constants                                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef struct ENGINE_EVENT_S * PENGINE_EVENT_T ;
typedef void (*EVENT_TASK_CB) (PENGINE_EVENT_T /*task*/, uint16_t /*event*/, int32_t /*event_register*/, uintptr_t /*parm*/) ;

typedef enum {
    /*
     * State machine memory.
     */
     heapMachine,
     /*
      * Temporary memory used by the parser.
      */
     heapParser
} portheap ;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


    int32_t             engine_port_init (void * arg) ;
    int32_t             engine_port_start (void) ;
    void                engine_port_stop (void) ;

    void                engine_port_lock (void) ;
    void                engine_port_unlock (void) ;

    int32_t             engine_port_variable_write (uint32_t idx, int32_t val) ;
    int32_t             engine_port_variable_read (uint32_t idx, int32_t * val) ;

    void*               engine_port_malloc (portheap heap, uint32_t size) ;
    void                engine_port_free (portheap heap, void* mem) ;
    void                engine_log_mem_usage (void) ;

    PENGINE_EVENT_T     engine_port_event_create (EVENT_TASK_CB complete) ;
    int32_t             engine_port_event_queue (PENGINE_EVENT_T task, uint16_t event, int32_t reg, uintptr_t parm, int32_t timeout) ;
    int32_t             engine_port_event_cancel (PENGINE_EVENT_T event) ;

    void                engine_port_log (int inst, const char *format_str, va_list  args) ;
    void                engine_port_assert (const char *msg) ;
    int32_t             engine_port_shellcmd (const char* shellcmd) ;

    const char *        engine_port_sanitize_string (const char * string, uint32_t * plen) ;
    void                engine_port_release_string (const char * string) ;

    extern uint32_t     engine_timestamp (void) ;


#ifdef __cplusplus
}
#endif

#endif /* __ENGINE_PORT_H__ */
