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

#include "qoraal-engine/config.h"
#include <string.h>

#include "machine.h"
#include "collection.h"
#include "parse.h"

#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"
#include "../port/port.h"


#define STATEMACHINE_FLAGS_APP_HEAP             (1<<0)

#define MACHINE_LOG(logif, message, ...)        if (logif && logif->Log) logif->Log (message, ##__VA_ARGS__)
#define MACHINE_REPORT(logif, message, ...)     if (logif && logif->Report) logif->Report (message, ##__VA_ARGS__)
#define MACHINE_ERROR(logif, message, ...)      if (logif && logif->Error) logif->Error (message, ##__VA_ARGS__)


STATEMACHINE_T*
machine_create (const char* name, uint16_t state_count, uint16_t state_entries)
{
    STATEMACHINE_T* machine ;
     uint32_t size = sizeof (STATEMACHINE_T) +
                    sizeof (STATEMACHINE_STATE_T *) * state_count +
                    sizeof (STATEMACHINE_STATE_T) * state_count +
                    sizeof(STATE_DATA_T) * state_entries
                    ;


    machine = ( STATEMACHINE_T*)engine_port_malloc (heapMachine, size) ;
    if (machine) {
        memset (machine, 0, size) ;
        machine->size = size ;
        machine->magic = STATEMACHINE_MAGIC ;
        machine->flags  = STATEMACHINE_FLAGS_APP_HEAP ;
        machine->count = state_count ;
        strncpy ((char*)machine->name, name, STATEMACHINE_NAME_SIZE-1) ;


    }

    return machine ;
}

bool
machine_start_state (STATEMACHINE_T* statemachine, uint16_t idx)
{
    if (idx > statemachine->count) {
        statemachine->start_idx = 0 ;
        return  0 ;

    }

    statemachine->start_idx = idx ;
    return 1 ;
}

STATEMACHINE_STATE_T*
machine_next_state (STATEMACHINE_T* statemachine, STATEMACHINE_STATE_T* state,
                uint16_t idx, uint16_t super_idx)
{
    STATEMACHINE_STATE_T* next_state  ;

    if (!state) {
        next_state =  (STATEMACHINE_STATE_T*)&statemachine->states_offset[statemachine->count];
        next_state->idx = 0 ;

    }  else {
        uint16_t i = state->idx + 1 ;
        next_state = (STATEMACHINE_STATE_T*) ((uintptr_t)state + state->size ) ;
        next_state->idx = i ;

    }

    DBG_ENGINE_CHECK (idx == next_state->idx, 0, "machine_next_state idx") ;
    DBG_ENGINE_CHECK (idx < statemachine->count, 0, "machine_next_state count") ;
    DBG_ENGINE_CHECK (!(next_state->entry || next_state->action ||
            next_state->exit || next_state->deferred ||
            next_state->events || next_state->size ||
            next_state->magic || next_state->def_idx ||
            next_state->super_idx), 0, "machine_next_state corrupt" ) ;

    next_state->size = sizeof(STATEMACHINE_STATE_T) ;
    next_state->magic = STATEMACHINE_MAGIC ;
    next_state->def_idx = STATEMACHINE_INVALID_STATE ;
    next_state->super_idx = super_idx ;
    SET_STATEMACHINE_STATE(statemachine, idx, next_state) ;
    return next_state ;
}

bool
machine_state_name (STATEMACHINE_T* statemachine, STATEMACHINE_STATE_T* state,
        char* name)
{
    if (state && name) {
        int i ;

        strncpy ((char*)state->name, name, STATEMACHINE_STATE_NAME_SIZE-1) ;

        for (i=0; i<statemachine->count; i++) {
            STATEMACHINE_STATE_T * next_state = GET_STATEMACHINE_STATE_REF(statemachine, i) ;
            if (state->idx != i) {
                if (
                        (strlen((const char*)next_state->name) == strlen(name)) &&
                        (strcmp((const char*)next_state->name, name) == 0)
                    ){

                    return  0 ;

                }

            }

        }

    } else {
        return 0 ;

    }

    return 1 ;
}

void
machine_state_default_idx (STATEMACHINE_STATE_T* state, uint16_t idx)
{
    if (state) {
        state->def_idx = idx ;

    }
}

void
machine_state_super_idx (STATEMACHINE_STATE_T* state, uint16_t idx)
{
    if (state) {
        state->super_idx = idx ;

    }
}

void
_shift_data(STATEMACHINE_STATE_T* state, uint32_t start, uint32_t count)
{
    uint32_t* pend = (uint32_t*)&state->data[start + count] ;
    while (count) {
        *pend = *(pend-1) ;
        pend-- ;
        count-- ;

    }
}

bool
machine_state_add_event (STATEMACHINE_STATE_T* state, STATE_DATA_T value)
{
    if (!state) return 0 ;
    uint32_t start =  state->events++ ;
    uint32_t count =  state->deferred + state->entry + state->exit + state->action ;
    if (count) _shift_data(state, start, count) ;
    state->data[start] = value ;
    state->size += sizeof(STATE_DATA_T) ;
    return 1 ;
}

bool
machine_state_add_deferred (STATEMACHINE_STATE_T* state, STATE_DATA_T value )
{
    if (!state) return 0 ;
    uint32_t start =  state->events + state->deferred++ ;
    uint32_t count =   state->entry + state->exit + state->action ;
    if (count)  _shift_data(state, start, count) ;
    state->data[start] = value ;
    state->size += sizeof(STATE_DATA_T) ;
    return 1 ;
}

bool
machine_state_add_entry (STATEMACHINE_STATE_T* state, STATE_DATA_T value )
{
    if (!state) return 0 ;
    uint32_t start = state->events + state->deferred + state->entry++ ;
    uint32_t count = state->exit  + state->action ;
    if (count) _shift_data(state, start, count) ;
    state->data[start] = value ;
    state->size += sizeof(STATE_DATA_T) ;
    return 1 ;
}

bool
machine_state_add_exit (STATEMACHINE_STATE_T* state, STATE_DATA_T value )
{
    if (!state) return 0 ;
    uint32_t start = state->events + state->deferred + state->entry + state->exit++ ;
    uint32_t count =   state->action ;
    if (count) _shift_data(state, start, count) ;
    state->data[start] = value ;
    state->size += sizeof(STATE_DATA_T) ;
    return 1 ;
}

bool
machine_state_add_action (STATEMACHINE_STATE_T* state, STATE_DATA_T event, STATE_DATA_T action)
{
    if (!state) return 0 ;
    uint32_t start = state->events + state->deferred + state->entry +
            state->exit  + state->action;
    state->action += 2 ;
    state->data[start] = event ;
    state->data[start+1] = action ;
    state->size += 2*sizeof(STATE_DATA_T) ;
    return 1 ;
}


void
machine_destroy (const STATEMACHINE_T* statemachine)
{
    if (statemachine && (statemachine->flags & STATEMACHINE_FLAGS_APP_HEAP)) {
        engine_port_free (heapMachine, (void*)statemachine) ;

    }
}

STRINGTABLE_T*
machine_stringtable_create(struct collection * dict)
{
    struct collection_it it ;
    struct clist * p ;
    uint32_t count = 0 ;
    uint32_t total_len = sizeof(STRINGTABLE_T) ;
    uint32_t idx ;
    STATEMACHINE_STRING_T * pstring ;

    STRINGTABLE_T *table ;

    if (!dict) {
        return  0 ;
    }
    for (p = collection_it_first (dict, &it) ; p;  ) {
        count++ ;
        const char * key = collection_get_key (dict, p) ;
        total_len += MACHINE_ALIGNED_SIZE(sizeof (STATEMACHINE_STRING_T) + strlen(key) + 1) ;
        p = collection_it_next (dict, &it) ;

    }
    // offset table
    total_len += sizeof(STATEMACHINE_STRING_T*) * count ;

    table = (STRINGTABLE_T*)engine_port_malloc(heapMachine, total_len) ;
    if (!table) {
        return 0 ;
    }
    memset (table, 0,total_len) ;

    table->size = total_len ;
    table->count = count ;
    table->magic = STATEMACHINE_MAGIC ;
    table->flags = STATEMACHINE_FLAGS_APP_HEAP ;

        
    pstring = (STATEMACHINE_STRING_T*) &table->strings_offset[count] ;

    for (idx =0 ; idx <count; idx++) {

        for (p = collection_it_first (dict, &it) ; p;  ) {
                
             if (*(unsigned int*)collection_get_value (dict, p) == idx) {
                SET_STATEMACHINE_STRINGTABLE(table, idx, pstring) ;
                const char * key = collection_get_key (dict, p) ;
                pstring->len = strlen(key) + 1 ;
                strcpy((char*)pstring->value, key) ;
                pstring->id = ((unsigned int*)collection_get_value (dict, p))[1] ;

                pstring = (STATEMACHINE_STRING_T*)  ((uintptr_t)pstring +
                        MACHINE_ALIGNED_SIZE(sizeof (STATEMACHINE_STRING_T) + (pstring->len))) ;

                break ;

            }


            p = collection_it_next (dict, &it) ;
        }
          
        
    }

    return table ;
}

void
machine_stringtable_destroy(STRINGTABLE_T* stringtable)
{
    if (stringtable && (stringtable->flags & STATEMACHINE_FLAGS_APP_HEAP)) {
        engine_port_free(heapMachine, stringtable) ;

    }
}

int32_t
machine_state_validate(const STATEMACHINE_T* statemachine,
        const STRINGTABLE_T* stringtable, STATEMACHINE_STATE_T* state,
        PARSE_LOG_IF * logif)
{
    int i ;
    int j = 0 ;
    MACHINE_LOG(logif, "\tState (%s): %d - %s: size %d, entries %d",
                &state->magic, state->idx, state->name, state->size,
                state->events + state->deferred + state->entry + state->exit) ;
    MACHINE_LOG(logif, "\t\tdefault state: %s\r\n",
            (state->def_idx == STATEMACHINE_INVALID_STATE ?
            "(none)" : (char*)GET_STATEMACHINE_STATE_REF(statemachine, state->def_idx)->name)) ;
    MACHINE_LOG(logif, "\t\tsuper state: %s\r\n",
            (state->super_idx == STATEMACHINE_INVALID_STATE ?
            "(none)" : (char*)GET_STATEMACHINE_STATE_REF(statemachine, state->super_idx)->name)) ;

    if (state->def_idx != STATEMACHINE_INVALID_STATE) {
        i = STATEMACHINE_SUPER_STATE_MAX ;
        STATEMACHINE_STATE_T* defstate = GET_STATEMACHINE_STATE_REF(statemachine, state->def_idx) ;
        while (defstate && (defstate->def_idx != STATEMACHINE_INVALID_STATE) && --i) {
            if (state->idx == defstate->idx) {
                MACHINE_ERROR(logif, "state %s default state circular reference!",
                        state->name) ;
                return ENGINE_FAIL ;

            }
            defstate = GET_STATEMACHINE_STATE_REF(statemachine, defstate->def_idx) ;

        }
        if (i == 0) {
            MACHINE_ERROR(logif, "state %s max default state exceeded!",
                    state->name) ;
            return ENGINE_FAIL ;

        }
    }
    if (state->super_idx != STATEMACHINE_INVALID_STATE) {
        i = STATEMACHINE_SUPER_STATE_MAX ;
        STATEMACHINE_STATE_T* superstate = GET_STATEMACHINE_STATE_REF(statemachine, state->super_idx) ;
        while (superstate && (superstate->super_idx != STATEMACHINE_INVALID_STATE) && --i) {
            if (state->idx == superstate->idx) {
                MACHINE_ERROR(logif, "state %s super state circular reference!",
                        state->name) ;
                return ENGINE_FAIL ;

            }
            superstate = GET_STATEMACHINE_STATE_REF(statemachine, superstate->super_idx) ;

        }
        if (i == 0) {
            MACHINE_ERROR(logif, "state %s max super states exceeded!",
                    state->name) ;
            return ENGINE_FAIL ;
        }
    }

    for (i=0; i<state->events; i++,j++) {

        if ((state->data[j].id & STATES_EVENT_ID_MASK) < STATES_EVENT_DECL_START) {
            const PART_EVENT_T* event = parts_get_event (state->data[j].id) ;
            if (!event) {
                MACHINE_ERROR(logif, "%s state %s event 0x%.4x validation failed!",
                        statemachine->name, state->name, state->data[j].id) ;
                return ENGINE_FAIL ;

            }
            MACHINE_LOG(logif, "\t\tevent: %s -> %s",
                        event->name, GET_STATEMACHINE_STATE_REF(statemachine, state->data[j].param)->name) ;


        } else {

            if (state->data[j].param < statemachine->size/sizeof(STATE_DATA_T)) {
                MACHINE_LOG(logif, "\t\tevent: 0x%.4x -> %s",
                            state->data[j].id, GET_STATEMACHINE_STATE_REF(statemachine, state->data[j].param)->name) ;

            } else {
                MACHINE_LOG(logif, "\t\tevent: 0x%.4x -> %x",
                            state->data[j].id, state->data[j].param) ;

            }
        }
    }


    for (i=0; i<state->deferred; i++, j++) {

        //if ((state->data[j].id & 0xF000) != 0xF000) {
        if ((state->data[j].id & STATES_EVENT_ID_MASK) < STATES_EVENT_DECL_START) {
            const PART_EVENT_T* event = parts_get_event (state->data[j].id) ;
            if (!event) {
                MACHINE_ERROR(logif, "%s state %s defered 0x%.4x validation failed!",
                        statemachine->name, state->name, state->data[j].id) ;
                return ENGINE_FAIL ;

            }
            MACHINE_LOG(logif, "\t\tdefered: %s",
                        event->name) ;

        } else {
            MACHINE_LOG(logif, "\t\tdefered: 0x%.4x",
                        state->data[j].id) ;


        }


    }



    for (i=0; i<state->entry; i++, j++) {
        const PART_ACTION_T* action = parts_get_action (state->data[j].id) ;
        uint32_t flags = 0 ;
        if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_INDEXED ;

        }
        else if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_STRING ;

        }
        else if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_VARIABLE ;

        }
        flags |= PART_ACTION_FLAG_VALIDATE ;

        if (!action) {
            MACHINE_ERROR(logif, "%s state %s entry action 0x%.4x validation failed!",
                    statemachine->name, state->name, state->data[j].id) ;

            return ENGINE_FAIL ;

        }
        if (action->fp(0, (uint32_t)state->data[j].param, flags) != ENGINE_OK) {
             MACHINE_ERROR(logif, "%s state %s entry action %s validation failed for 0x%.4x (0x%x)!",
                    statemachine->name, state->name, action->name, state->data[j].param, flags) ;

            return ENGINE_FAIL ;

        }

        MACHINE_LOG(logif, "\t\tentry: %s -> 0x%x (%d)",
                action->name, (uint32_t)state->data[j].param, (uint32_t)state->data[j].param) ;


    }
    for (i=0; i<state->exit; i++, j++) {
        const PART_ACTION_T* action = parts_get_action (state->data[j].id) ;
        uint32_t flags = 0 ;
        if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_INDEXED ;

        }
        else if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_STRING ;

        }
        else if ((state->data[j].id & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_VARIABLE ;

        }
        flags |= PART_ACTION_FLAG_VALIDATE ;

        if (!action) {
            MACHINE_ERROR(logif, "state %s exit action 0x%.4x validation failed!",
                    state->name, state->data[j].id) ;
            return ENGINE_FAIL ;

        }
        if (action->fp(0, (uint32_t)state->data[j].param, flags) != ENGINE_OK) {
            MACHINE_ERROR(logif, "%s state %s exit action %s validation failed for 0x%.4x (0x%x)!",
                    statemachine->name, state->name, action->name, state->data[j].param, flags) ;
            return ENGINE_FAIL ;

        }

        MACHINE_LOG(logif, "\t\texit: %s -> 0x%x (%d)",
                action->name, (uint32_t)state->data[j].param, (uint32_t)state->data[j].param) ;

    }

    for (i=0; i<state->action; i+=2, j+=2) {
        const PART_ACTION_T* action = parts_get_action (state->data[j+1].id & STATES_ACTION_ID_MASK) ;
        uint32_t flags = 0 ;
        if ((state->data[j+1].id & STATES_ACTION_TYPE_MASK) ==
                STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_INDEXED ;

        }
        else if ((state->data[j+1].id & STATES_ACTION_TYPE_MASK) ==
                STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_STRING ;

        }
        else if ((state->data[j+1].id & STATES_ACTION_TYPE_MASK) ==
                STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET) {
            flags = PART_ACTION_FLAG_VARIABLE ;

        }
        flags |= PART_ACTION_FLAG_VALIDATE ;

        //if ((state->data[j].id & 0xF000) != 0xF000) {
        if ((state->data[j].id & STATES_EVENT_ID_MASK) < STATES_EVENT_DECL_START) {
            const PART_EVENT_T* event = parts_get_event (state->data[j].id) ;
            if (!event) {
                MACHINE_ERROR(logif, "%s state %s action event 0x%.4x validation failed!",
                        statemachine->name, state->name, state->data[j].id) ;
                return ENGINE_FAIL ;
            }
            MACHINE_LOG(logif, "\t\taction event: %s",
                        event->name) ;

        } else {
            MACHINE_LOG(logif, "\t\taction event: 0x%.4x",
                        state->data[j].id) ;


        }


        if (!action) {
            MACHINE_ERROR(logif, "%s state %s action action 0x%.4x validation failed!",
                    statemachine->name, state->name, state->data[j+1].id) ;
            return ENGINE_FAIL ;

        }
        if (action->fp(0, (uint32_t)state->data[j+1].param, flags) != ENGINE_OK) {
            const char * str = "" ;
            if ((flags) & PART_ACTION_FLAG_STRING) {
                uint16_t idx = state->data[j+1].param ;
                if (idx < stringtable->count) {
                    const STATEMACHINE_STRING_T *strt =
                                GET_STATEMACHINE_STRINGTABLE_REF(stringtable, idx) ;
                    if (strt && strt->len) {
                        str = (const char *)strt->value ;

                    }

                }

            }

            MACHINE_ERROR(logif, "%s state %s action action %s validation failed for 0x%.4x %s (0x%x)!",
                statemachine->name, state->name, action->name, state->data[j+1].param,
                str, flags) ;
            return ENGINE_FAIL ;

        }

        MACHINE_LOG(logif, "\t\taction action: %s -> 0x%x (%d)",
            action->name, (uint32_t)state->data[j+1].param, (uint32_t)state->data[j+1].param) ;



    }

    return ENGINE_OK ;
}


int32_t
machine_validate (const STATEMACHINE_T* statemachine,
        const STRINGTABLE_T* stringtable, PARSE_LOG_IF * logif)
{
    int i  ;

    MACHINE_REPORT(logif, "validating statemachine '%s': size %d, states %d",
                statemachine->name,
                statemachine->size, statemachine->count) ;


    if (statemachine->magic != STATEMACHINE_MAGIC) {
        MACHINE_ERROR(logif, "validating statemachine '%s' invalid magic 0x%x",
                statemachine->name, statemachine->magic) ;
        return ENGINE_FAIL ;

    }


    for (i=0; i<statemachine->count; i++) {
        STATEMACHINE_STATE_T* state = GET_STATEMACHINE_STATE_REF(statemachine, i) ;
        if (state->idx != i) {
            MACHINE_ERROR(logif, "validating statemachine '%s' state %d incorect indexed %d!!",
                    statemachine->name, i, state->idx ) ;
            return ENGINE_FAIL ;


        }
        if (machine_state_validate(statemachine, stringtable, state, logif) != ENGINE_OK) {
            MACHINE_ERROR(logif, "validating statemachine '%s' state %s FAIL!!",
                    statemachine->name, state->name) ;
            return ENGINE_FAIL ;

        }
    }

    MACHINE_LOG(logif, "validating statemachine '%s' PASS!",
            statemachine->name) ;

    return ENGINE_OK ;
}


int32_t
machine_stringtable_validate(const STRINGTABLE_T* strings, PARSE_LOG_IF * logif)
{
    uint32_t i ;
    uint32_t total_length = sizeof(STRINGTABLE_T) + sizeof (STATEMACHINE_STRING_T*) * strings->count ;

    MACHINE_REPORT(logif, "validating stringtable: size %d, strings %d",
                strings->size, strings->count) ;



    for (i=0; i<strings->count; i++) {
        const STATEMACHINE_STRING_T *str = GET_STATEMACHINE_STRINGTABLE_REF(strings, i) ;
        total_length += MACHINE_ALIGNED_SIZE(str->len + sizeof(STATEMACHINE_STRING_T)) ;

    }

    i = total_length == strings->size ? ENGINE_OK : ENGINE_FAIL ;
    if (i == ENGINE_OK) {
        MACHINE_LOG(logif, "validating stringtable PASS") ;

    } else {
        MACHINE_ERROR(logif, "validating stringtable FAIL") ;

    }
    return i ;

}




