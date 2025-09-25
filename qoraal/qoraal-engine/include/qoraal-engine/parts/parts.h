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


#ifndef __PARTS_H__
#define __PARTS_H__


#include <stdint.h>
#include <stddef.h>
#include <limits.h>


extern char __engine_action_base__ ;
extern char __engine_action_end__ ;
extern char __engine_event_base__ ;
extern char __engine_event_end__ ;
extern char __engine_const_base__ ;
extern char __engine_const_end__ ;
extern char __engine_cmd_base__ ;
extern char __engine_cmd_end__ ;


typedef struct ENGINE_S * PENGINE_T ;

typedef int32_t (*PART_CMD_FP)(PENGINE_T /*instance*/, uint32_t /*parm*/) ;
typedef int32_t (*PART_ACTION_FP)(PENGINE_T /*instance*/, uint32_t /*parm*/, uint32_t /*flags*/) ;

#define PART_ACTION_FLAG_EXEC               0
#define PART_ACTION_FLAG_VALIDATE           (1<<0)
#define PART_ACTION_FLAG_INDEXED            (1<<2)
#define PART_ACTION_FLAG_STRING             (1<<3)
#define PART_ACTION_FLAG_VARIABLE           (1<<4)

#define PART_CMD_PARM_STOP                  0
#define PART_CMD_PARM_START                 1

#define ALIGN

typedef struct __attribute__((packed)) PART_ACTION_S {
    PART_ACTION_FP          fp ;
    const char *            name ;
    const char *            desc ;

} PART_ACTION_T ;


typedef struct __attribute__((packed)) PARTS_EVENT_S {
    const char *            name ;
    const char *            desc ;

} PART_EVENT_T ;


typedef struct __attribute__((packed)) PART_CONST_S {
    int16_t                 id ;
    uint16_t                reserved ;
    const char *            name ;
    const char *            desc ;

} PART_CONST_T ;


typedef struct __attribute__((packed)) PART_CMD_S {
    PART_CMD_FP         fp ;
    const char *        name ;
} PART_CMD_T ;



#define ENGINE_ACTION_IMPL(name, desc)      \
    static int32_t action_##name (PENGINE_T, uint32_t, uint32_t) ; \
    const PART_ACTION_T                     \
    __engine_action_##name ALIGN            \
        __attribute__((used, section(".engine.engine_action." #name ), aligned(1))) =        \
    { action_##name,                        \
    #name,                                  \
    desc                                    \
    }

#define ENGINE_EVENT_IMPL(name, desc)       \
    const PART_EVENT_T                      \
    __engine_event_##name ALIGN             \
     __attribute__((used, section(".engine.engine_event." #name), aligned(1))) =      \
    {                                       \
    #name,                                  \
    desc                                    \
    }

#define ENGINE_CONST_IMPL(value, name, desc)        \
    const PART_CONST_T                      \
    __engine_const_##name ALIGN             \
     __attribute__((used, section(".engine.engine_const." #name), aligned(1))) =      \
    { value,                                \
    0,                                      \
    #name,                                  \
    desc                                    \
    }

#define ENGINE_CMD_FP_IMPL(fp)              \
    static int32_t fp (PENGINE_T, uint32_t) ; \
    const PART_CMD_T                        \
    __engine_cmd_##fp ALIGN                 \
    __attribute__((used, section(".engine.engine_cmd." #fp), aligned(1))) =      \
    { fp, #fp                               \
    }



#include "parts_events.h"


#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Functions used by engine to access parts.
     */
    extern int32_t              parts_cmd (PENGINE_T instance, uint32_t cmd) ;
    extern const PART_ACTION_T* parts_get_action (uint16_t action_id) ;
    extern const PART_EVENT_T*  parts_get_event (uint16_t event_id) ;
    extern int32_t              parts_find_event_id (const char* name) ;
    extern PART_ACTION_FP       parts_get_action_fp (uint16_t action_id) ;
    extern const char*          parts_get_action_name (uint16_t action_id) ;
    extern const char*          parts_get_event_name (uint16_t event_id) ;

    /*
     * Helper functions used by parts.
     */
    extern int32_t              parts_valadate_string (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
    extern int32_t              parts_valadate_int (PENGINE_T instance, uint32_t parm, uint32_t flags, int32_t min, int32_t max) ;
    extern const char*          parts_get_string(PENGINE_T instance, uint32_t parm, uint32_t flags) ;
    extern int32_t              parts_get_int(PENGINE_T instance, uint32_t parm, uint32_t flags, int32_t min, int32_t max) ;


#ifdef __cplusplus
}
#endif


#endif /* __PARTS_H__ */
