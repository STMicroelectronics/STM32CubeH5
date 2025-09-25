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



#ifndef __MACHINE_H__
#define __MACHINE_H__

#include <stdint.h>
#include <stdbool.h>
#include "collection.h"
#include "parse.h"
#include "qoraal-engine/engine.h"

/*===========================================================================*/
/* Macros and defines.                                                                */
/*===========================================================================*/
#define MACHINE_ALIGN_TYPE			uint32_t
#define MACHINE_ALIGNED_SIZE(x)   	((((x + (sizeof(MACHINE_ALIGN_TYPE)))-((MACHINE_ALIGN_TYPE) 1))/(sizeof(MACHINE_ALIGN_TYPE))) * (sizeof(MACHINE_ALIGN_TYPE)))


/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/


/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    STATEMACHINE_T*         machine_create (const char* name, uint16_t state_count, uint16_t state_entries) ;
    STATEMACHINE_STATE_T*   machine_next_state (STATEMACHINE_T* statemachine, STATEMACHINE_STATE_T* state, uint16_t idx, uint16_t super_idx) ;
    bool                    machine_state_name (STATEMACHINE_T* statemachine, STATEMACHINE_STATE_T* state, char* name) ;
    void                    machine_state_default_idx (STATEMACHINE_STATE_T* state, uint16_t idx ) ;
    void                    machine_state_super_idx (STATEMACHINE_STATE_T* state, uint16_t idx ) ;
    bool                    machine_start_state (STATEMACHINE_T* statemachine, uint16_t idx) ;
    bool                    machine_state_add_entry (STATEMACHINE_STATE_T* state, STATE_DATA_T value ) ;
    bool                    machine_state_add_exit (STATEMACHINE_STATE_T* state, STATE_DATA_T value ) ;
    bool                    machine_state_add_event (STATEMACHINE_STATE_T* state, STATE_DATA_T value ) ;
    bool                    machine_state_add_action (STATEMACHINE_STATE_T* state, STATE_DATA_T event , STATE_DATA_T action ) ;
    bool                    machine_state_add_deferred (STATEMACHINE_STATE_T* state, STATE_DATA_T value ) ;
    void                    machine_destroy (const STATEMACHINE_T* statemachine) ;

    STRINGTABLE_T*          machine_stringtable_create(struct collection * dict) ;
    void                    machine_stringtable_destroy(STRINGTABLE_T* stringtable) ;

    int32_t                 machine_validate(const STATEMACHINE_T* statemachine, const STRINGTABLE_T* stringtable, PARSE_LOG_IF * logif)  ;
    int32_t                 machine_stringtable_validate(const STRINGTABLE_T* strings, PARSE_LOG_IF * logif) ;


#ifdef __cplusplus
}
#endif


#endif /* __MACHINE_H__ */
