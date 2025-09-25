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



#include <stdio.h>
#include <string.h>
#include "qoraal-engine/config.h"
#include "qoraal-engine/engine.h"
#include "port/port.h"
#include "qoraal-engine/parts/parts.h"
#include "tool/parse.h"


/*===========================================================================*/
/* Macros.                                                                   */
/*===========================================================================*/

#define ENGINE_LOG(instance, type, msg...)  if ((type) & _engine_log_filter)  { engine_log(instance, (type), msg) ; }

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

/**
 * A linked list for deferred events.
 */
typedef struct ENGINE_DEFERED_S {
    struct ENGINE_DEFERED_S *       next ;
    int32_t                         event_register ;
    uint16_t                        event ;
} ENGINE_DEFERED_T;

/**
 * A structure representing an engine instance.
 */
typedef struct ENGINE_S {

    int32_t                         idx ;
    const STATEMACHINE_T*           statemachine ;
    const STATEMACHINE_STATE_T*     current ;
    const STATEMACHINE_STATE_T*     prev[ENGINE_PREVIOUS_STACK] ;
    int32_t                         prev_idx ;
    int32_t                         prev_pin ;
    ENGINE_DEFERED_T *              deferred ;
    uint32_t                        deferred_cnt ;
    int32_t                         reg[ENGINE_REGISTER_COUNT] ;
    int32_t                         stack[ENGINE_ACCUMULATOR_STACK] ;
    int32_t                         stack_idx ;

    TRANSITION_HANDLER_T *          transition_handler ;

    uint32_t                        timer ;
    uint16_t                        action ;

} ENGINE_T,  *PENGINE_T ;

/*===========================================================================*/
/* Local variables.                                                          */
/*===========================================================================*/

static uint16_t                     _engine_log_filter = ENGINE_LOG_FILTER_DEFAULT ;
static uint32_t                     _engine_log_instance = 0xFFFFFFFF ;
static char                         _engine_name[ENGINE_NAME_SIZE] ;
static uint32_t                     _engine_version = 0 ;
static const STRINGTABLE_T *        _engine_stringtable = 0 ;
static ENGINE_T                     _engine_instance[ENGINE_MAX_INSTANCES] ;
static ENGINE_T *                   _engine_active_instance = 0 ;
static uint32_t                     _engine_instance_count = 0 ;
static bool                         _engine_started = false ;

/*===========================================================================*/
/* Local declarations.                                                       */
/*===========================================================================*/

static int32_t      state_transition (PENGINE_T engine, uint16_t next_idx, uint16_t cond) ;
static uint16_t     state_event (PENGINE_T engine, uint16_t event, uint16_t * next_state) ;
static bool         state_deferred_event (PENGINE_T engine, const STATEMACHINE_STATE_T* state, uint16_t event_id) ;
static void         queue_all_deferred (PENGINE_T engine) ;
static bool         state_action (const PENGINE_T engine, uint16_t event_id, const STATEMACHINE_STATE_T* state) ;
static void         log_event(PENGINE_T engine, uint16_t  event_id) ;
static void         log_action(PENGINE_T engine, uint32_t filter, const char* pre, const char* cond, STATES_INTERNAL_T* action) ;
static void         log_function(PENGINE_T engine, uint32_t filter, char* pre, STATES_ACTION_T* action) ;

/**
 * @brief       Return the number of statemachines (engines) loaded.
 * @return      count
 */
uint32_t
engine_statemachine_count (void)
{
    return _engine_instance_count ;
}

/**
 * @brief       Return the name of the engine for the index.
 * @param[in]   idx
 * @return      name OR NULL
 */
const char*
engine_statemachine_name (uint32_t idx)
{
    DBG_ENGINE_CHECK( (idx < _engine_instance_count) ||
            (!_engine_instance[idx].statemachine) , 0,
            "engine_statemachine_name unexpected") ;

    return (const char*)_engine_instance[idx].statemachine->name ;
}

/**
 * @brief       Removes the statemachine added with engine_add_statemachine().
 * @note        Engine must be stopped first.
 * @param[in]   idx
 * @return      statemachie
 */
const STATEMACHINE_T*
engine_remove_statemachine (int idx)
{
    DBG_ENGINE_CHECK(idx < ENGINE_MAX_INSTANCES, 0,
            "engine_remove_statemachine unexpected") ;

    const STATEMACHINE_T* s = _engine_instance[idx].statemachine ;
    _engine_instance[idx].statemachine = 0 ;
    return s ;
}

/**
 * @brief       Gets a reference to a  statemachine added with engine_add_statemachine().
 * @param[in]   idx
 * @return      statemachie
 */
const STATEMACHINE_T*
engine_get_statemachine (int idx)
{
    DBG_ENGINE_CHECK(idx < ENGINE_MAX_INSTANCES, 0,
            "engine_get_statemachine unexpected") ;

    return _engine_instance[idx].statemachine ;
}

/**
 * @brief       Removes the stringtable added with engine_set_stringtable().
 * @note        Engine must be stopped first.
 * @return      stringtable
 */
const STRINGTABLE_T*
engine_remove_stringtable (void)
{
    const STRINGTABLE_T* s = _engine_stringtable ;
    _engine_stringtable = 0 ;
    return s ;
}

/**
 * @brief       Gets a reference to the stringtable added with engine_add_statemachine().
 * @return      stringtable
 */
const STRINGTABLE_T*
engine_get_stringtable (void)
{
    return _engine_stringtable ;
}

/**
 * @brief       Sets the stringtable for all engines.
 * @param[in]   stringtable    stringtable
 * @return      status
 */
int32_t
engine_set_stringtable (const STRINGTABLE_T * stringtable)
{
    _engine_stringtable = stringtable ;
    return ENGINE_OK ;
}

/**
 * @brief       Sets the version for all engines.
 * @param[in]   version
 * @return      status

 */
int32_t
engine_set_version (int32_t version)
{
    _engine_version = version ;
    return ENGINE_OK ;
}

/**
 * @brief       Sets the name for all engines.
 * @param[in]   name
 * @return      status
 */
int32_t
engine_set_name (const char * name)
{
    strncpy (_engine_name, name ? name : "", ENGINE_NAME_SIZE -1) ;
    return ENGINE_OK ;
}


/**
 * @brief       Get the version.
 * @return      version
 */
int32_t
engine_get_version (void)
{
    return _engine_version ;
}

/**
 * @brief       Get the name.
 * @return      name
 */
const char *
engine_get_name (void)
{
    return _engine_name ;
}

/**
 * @brief       Updates the filter for logging.
 * @param[in]   set             ENGINE_LOG_TYPE_xxx bitmask
 * @param[in]   clear           ENGINE_LOG_TYPE_xxx bitmask
 * @return      filter mask
 */
uint32_t
engine_logfilter (uint16_t set, uint16_t clear)
{
    _engine_log_filter &= ~(clear & ~ENGINE_LOG_FILTER_ALWAYS);
    _engine_log_filter |= (set);
    return _engine_log_filter ;
}

/**
 * @brief       Updates the filter for logging.
 * @param[in]   set             bitmask obtained by engine_statemachine_logmask()
 * @param[in]   clear           bitmask obtained by engine_statemachine_logmask()
 * @return      filter mask
 */
uint32_t
engine_loginstance (uint32_t set, uint32_t clear)
{
    _engine_log_instance &= ~(clear);
    _engine_log_instance |= (set);
    return _engine_log_instance ;
}

/**
 * @brief       Return true if the engine and the type would log a message.
 * @param[in]   engine
 * @param[in]   type            ENGINE_LOG_TYPE_xxx bitmask
 * @return      true/false
 */
bool
engine_would_log (PENGINE_T engine, uint32_t type)
{
    return (type & _engine_log_filter) &&
        (!engine || ((1 << engine->idx) & _engine_log_instance));
}

/**
 * @brief       Writes a message to the logging subsystem of the port.
 * @param[in]   engine
 * @param[in]   type            ENGINE_LOG_TYPE_xxx bitmask
 * @param[in]   fmt_str
 */
void
engine_log (PENGINE_T engine, uint32_t type, const char* fmt_str, ...)
{
    va_list         args;
    va_start (args, fmt_str) ;

    if (
            (type == ENGINE_LOG_TYPE_VERBOSE) ||
            ((type & _engine_log_filter) &&
            (!engine || ((1 << engine->idx) & _engine_log_instance)))
        ) {
            engine_port_log (engine ? engine->idx : -1, fmt_str, args) ;

    }

    va_end (args) ;
}


/**
* @brief        Get the index for an engine.
* @param[in]    engine
* @return       index from 0 to ENGINE_MAX_INSTANCES.
*/
int32_t
engine_instance_idx (PENGINE_T engine)
{
    DBG_ENGINE_ASSERT (engine, "engine_instance_idx unexpected!") ;
    return engine->idx ;
}

/**
* @brief        Adds a transition handler for an engine.
*               The handler callback is called on each transition of states of
*               the engines state machine.
* @param[in]    engine
* @param[in]    handler
*/
void
engine_add_transition_handler (PENGINE_T engine, TRANSITION_HANDLER_T * handler)
{
    DBG_ENGINE_ASSERT (engine && handler,
            "engine_add_transition_handler unexpected!") ;

    engine_port_lock () ;

    handler->next = engine->transition_handler ;
    engine->transition_handler = handler ;

    engine_port_unlock () ;
}

/**
* @brief        Removes the transition handler added by engine_add_transition_handler().
* @param[in]    engine
* @param[in]    handler
*/
void
engine_remove_transition_handler (PENGINE_T engine, TRANSITION_HANDLER_T * handler)
{
    TRANSITION_HANDLER_T * previous ;
    TRANSITION_HANDLER_T * start ;

    DBG_ENGINE_ASSERT (engine && handler,
            "engine_add_transition_handler unexpected!") ;

    engine_port_lock () ;

    for ( previous = 0, start = engine->transition_handler ;
            (start!=0) && (start!=handler) ; ) {
        previous = start ;
        start = start->next;

    }

    if (previous == 0) {
        if (start) engine->transition_handler = start->next ;

    } else if (start) {
        previous->next = start->next ;

    }

    engine_port_unlock () ;
}

/**
* @brief        Gets an engine variable.
* @param[in]    engine
* @param[in]    var             index for the variable
* @param[out]   val
* @return       status
*/
int32_t
engine_get_variable (PENGINE_T engine, uint32_t var, int32_t * val)
{
    int res = ENGINE_OK ;

    DBG_ENGINE_CHECK(val, ENGINE_FAIL,
            "engine_get_variable unexpected") ;

    engine_port_lock () ;
    if (var < ENGINE_REGISTER_COUNT) {
        /* First registers are local to each engine. */
        if (!engine) engine = _engine_active_instance ;
        if (!engine) res = ENGINE_FAIL ;
        else *val = engine->reg[var] ;

    } else {
        /* All other registers are global to all engines. */
        res = engine_port_variable_read (var - ENGINE_REGISTER_COUNT, val) ;
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG,
                "[dbg]      var %d get %d", var, *val) ;

    }
    engine_port_unlock () ;

    return res ;
}

/**
* @brief        Sets an engine variable.
* @param[in]    engine
* @param[in]    var             index for the variable
* @param[in]    val
* @return       status
*/
int32_t
engine_set_variable (PENGINE_T engine, uint32_t var, int32_t val)
{
    int32_t res = ENGINE_OK ;

    engine_port_lock () ;
    if (var < ENGINE_REGISTER_COUNT) {
        /* First registers are local to each engine. */
        if (!engine) engine = _engine_active_instance ;
        if (!engine) res = ENGINE_FAIL ;
        else engine->reg[var] = val ;

    } else {
        /* All other registers are global to all engines. */
        res = engine_port_variable_write (var - ENGINE_REGISTER_COUNT, val) ;
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG,
                "[dbg]      var %d set %d", var, val) ;

    }
    engine_port_unlock () ;

    return res ;
}

/**
* @brief        For the instance, push the accumulator on the stack and save the
*               value in the accumulator.
* @param[in]    engine
* @param[in]    value
* @return       status
*/
int32_t
engine_push (PENGINE_T engine, int32_t value)
{
    DBG_ENGINE_ASSERT (engine, "engine_push unexpected!") ;
    engine_port_lock () ;
    engine->stack_idx++ ;
    if (engine->stack_idx >= ENGINE_ACCUMULATOR_STACK) engine->stack_idx = 0 ;
    engine->stack[engine->stack_idx] = engine->reg[ENGINE_VARIABLE_ACCUMULATOR] ;
    engine->reg[ENGINE_VARIABLE_ACCUMULATOR] = value ;
    engine_port_unlock () ;

    return ENGINE_OK ;
}

/**
* @brief        For the instance, swap the accumulator with the bottom of the stack.
* @param[in]    engine
* @return       status
*/
int32_t
engine_swap (PENGINE_T engine)
{
    DBG_ENGINE_ASSERT (engine, "engine_swap unexpected!") ;
    engine_port_lock () ;
    uint32_t tmp  = engine->stack[engine->stack_idx] ;
    engine->stack[engine->stack_idx] = engine->reg[0] ;
    engine->reg[ENGINE_VARIABLE_ACCUMULATOR] = tmp ;
    engine_port_unlock () ;

    return ENGINE_OK ;
}

/**
* @brief        For the instance, pop the stack into the accumulator.
* @param[in]    engine
* @return       status
*/
int32_t
engine_pop (PENGINE_T engine)
{
    DBG_ENGINE_ASSERT (engine, "engine_pop unexpected!") ;
    engine_port_lock () ;
    engine->reg[ENGINE_VARIABLE_ACCUMULATOR]  = engine->stack[engine->stack_idx] ;
    engine->stack[engine->stack_idx] = 0 ;
    engine->stack_idx-- ;
    if (engine->stack_idx < 0) engine->stack_idx = ENGINE_ACCUMULATOR_STACK-1 ;
    engine_port_unlock () ;

    return ENGINE_OK ;
}


/**
 * @brief       Initialises the module.
 * @param[in]   state_machine    state_machine to use
 * @param[in]   start_state_idx    state to transition to
 * @return      status
 */
int32_t
engine_init (void * arg)
{
    uint32_t status = ENGINE_OK ;
    ENGINE_LOG(0, ENGINE_LOG_TYPE_INIT, "[ini] engine_init") ;
    engine_port_init (arg) ;

    return status ;
}

/**
 * @brief       Adds a statemachie.
 * @note        The statemachine will be assigned to the first empty engine.
 * @param[in]   statemachine
 * @return      status
 */
int32_t
engine_add_statemachine (const STATEMACHINE_T *statemachine)
{
    int i ;
    int32_t res = ENGINE_FAIL ;

    if (statemachine->magic != STATEMACHINE_MAGIC) {
        ENGINE_LOG(0, ENGINE_LOG_TYPE_ERROR,
                "[err] engine_statemachine '%s' invalid magic!",
                statemachine->name) ;

    } else {

        for (i=0; i<ENGINE_MAX_INSTANCES; i++) {
            if (_engine_instance[i].statemachine == 0) {
                memset (&_engine_instance[i], 0, sizeof (_engine_instance[i])) ;
                _engine_instance[i].statemachine = statemachine ;
                _engine_instance[i].idx = i ;
                ENGINE_LOG(0, ENGINE_LOG_TYPE_INIT,
                        "[ini] engine_statemachine '%s' loaded", statemachine->name) ;
                res = ENGINE_OK ;
                break ;

            } else if (strncmp((char*)_engine_instance[i].statemachine->name,
                        (char*)statemachine->name, 16) == 0) {
                ENGINE_LOG(0, ENGINE_LOG_TYPE_ERROR,
                        "[err] engine_statemachine failed %s already added!",
                        statemachine->name) ;
                break ;

            }

        }

        if (res != ENGINE_OK) {
            ENGINE_LOG(0, ENGINE_LOG_TYPE_ERROR,
                    "[err] engine_statemachine failed '%s', too many state machines!",
                    statemachine->name) ;
        }

    }

    return res ;
}


/**
 * @brief       Start all statemachines loaded with engine_add_statemachine().
 * @return      status
 */
int32_t
engine_start (void)
{
    uint32_t i ;
    uint32_t status ;

    ENGINE_LOG(0, ENGINE_LOG_TYPE_INIT, "[ini] engine_start") ;

    if (_engine_started) return E_UNEXP ;

    status = engine_port_start () ;
    if (status != EOK) {
        return status ;
    }

    engine_port_lock () ;

    _engine_started = true ;

    for (i=0; i<ENGINE_MAX_INSTANCES; i++) {
        PENGINE_T engine = &_engine_instance[i] ;
        if (!engine->statemachine) {
            _engine_instance_count = i ;
            break ;

        }

    }


    status = parts_cmd (0, PART_CMD_PARM_START) ;
    for (i=0; i<_engine_instance_count; i++) {
        PENGINE_T engine = &_engine_instance[i] ;
        status = parts_cmd (engine, PART_CMD_PARM_START) ;
        if (status != ENGINE_OK) {
            ENGINE_LOG (0, ENGINE_LOG_TYPE_ERROR, "[err] starting subsystems") ;
            break ;

        }

    }

    if (status != ENGINE_OK) {
        parts_cmd (0, PART_CMD_PARM_STOP) ;
        for (; i>0; i--) {
            PENGINE_T engine = &_engine_instance[i-1] ;
            parts_cmd (engine, PART_CMD_PARM_STOP) ;

        }
        _engine_instance_count = 0 ;

    }

    if (status == ENGINE_OK) {
        for (i=0; i<_engine_instance_count; i++) {
            PENGINE_T engine = &_engine_instance[i] ;
            const STATEMACHINE_T *statemachine = engine->statemachine ;

            if (statemachine) {

                uint16_t start_state_idx = 0 ;
                uint16_t event_id = 0 ;
                uint16_t cond = 0 ;

                ENGINE_LOG (0, ENGINE_LOG_TYPE_VALIDATE,
                        "[val] starting statemachine %s", statemachine->name) ;

                if (statemachine->start_idx < statemachine->count) {
                    start_state_idx = statemachine->start_idx ;

                }

                while (start_state_idx != STATEMACHINE_INVALID_STATE) {
                    log_event (engine, event_id) ;
                    state_transition (engine, start_state_idx, cond) ;
                    if (event_id & STATES_EVENT_PREVIOUS_PIN) engine->prev_pin = 1 ;
                    event_id = state_event (engine, STATEMACHINE_STATE_START, &start_state_idx) ;
                    cond = (event_id & STATES_EVENT_COND_MASK) >> STATES_EVENT_COND_OFFSET ;

                }

            }

        }

    }

    engine_port_unlock () ;

    return status ;
}

/**
 * @brief       Stop all statemachines.
 * @return      status
 */
int32_t
engine_stop (void)
{
    int32_t res = ENGINE_FAIL ;
    ENGINE_DEFERED_T* start ;
    uint32_t i ;

    if (!_engine_started) return E_UNEXP ;

    engine_port_lock () ;

    _engine_started = false ;

    if (_engine_instance_count) {
        uint32_t cnt =  _engine_instance_count ;
        _engine_instance_count = 0 ;

        ENGINE_LOG(0, ENGINE_LOG_TYPE_DEBUG, "[dbg] engine_stop") ;


        for (i=0; i<cnt; i++) {
            PENGINE_T engine = &_engine_instance[i] ;

            if (engine->statemachine) {

                while (engine->deferred) {
                    start = engine->deferred ;
                    engine->deferred = start->next ;
                    engine_port_free (heapMachine, start) ;

                }

                /*status = */parts_cmd (engine, PART_CMD_PARM_STOP) ;

            }

        }

        parts_cmd (0, PART_CMD_PARM_STOP) ;

        res = ENGINE_OK ;

    }

    engine_port_unlock () ;

    engine_port_stop () ;

    return res ;
}

/**
 * @brief       Returns the number of statemachies started.
 * @return      count
 */
uint32_t
engine_is_started(void)
{
    return _engine_instance_count ;
}

/**
 * @brief       Dispatch an event to the engine running a statemachine.
 * @param[in]   engine
 * @param[in]   event
 * @return      status
 */
static int32_t
_engine_event (PENGINE_T engine, uint16_t event)
{
    uint16_t idx ;
    uint16_t event_id ;

    log_event (engine, event) ;

    event_id = state_event (engine, event, &idx) ;
    if (idx != STATEMACHINE_INVALID_STATE) {
        uint16_t cond = (event_id & STATES_EVENT_COND_MASK) >> STATES_EVENT_COND_OFFSET ;

        do {
            queue_all_deferred (engine) ;
            if (state_transition (engine, idx, cond) != ENGINE_OK) break ;
            /* lock the PREVIOUS state if the PREVIOUS_PIN flag is set */
            if (event_id & STATES_EVENT_PREVIOUS_PIN) engine->prev_pin = 1 ;
            log_event (engine, STATEMACHINE_STATE_START) ;
            event_id = state_event (engine, STATEMACHINE_STATE_START, &idx) ;
            cond = (event_id & STATES_EVENT_COND_MASK) >> STATES_EVENT_COND_OFFSET ;

            /* _state_start may continue to transition the state machine */
        } while (idx != STATEMACHINE_INVALID_STATE) ;

    }

    return ENGINE_OK ;
}


/**
 * @brief       Dispatch an event to the statemachine running in the engine.
 * @param[in]   engine
 * @param[in]   event
 * @param[in]   event_register
 */
void
engine_event (PENGINE_T engine, uint16_t event, int32_t event_register)
{
    uint32_t i ;

    if (_engine_instance_count) {

        engine_port_lock () ;

        if (engine == 0) {
            for (i=0; i<_engine_instance_count; i++) {
                engine = &_engine_instance[i] ;
                if (engine && engine->statemachine) {
                    engine->reg[ENGINE_VARIABLE_EVENT] = event_register ;
                    _engine_event (engine, event) ;

                }

            }

        } else {
            if (engine->statemachine) {
                engine->reg[ENGINE_VARIABLE_EVENT] = event_register ;
                _engine_event (engine, event) ;

            }

        }

        engine_port_unlock () ;

    }


    return   ;
}

/**
 * @brief       Fire an event to all statemachines in the mask.
 * @param[in]   mask
 * @param[in]   event
 * @param[in]   event_register
 */
void
engine_mask_event (uint32_t mask, uint16_t event_id, int32_t event_register)
{
    int i = 0 ;

    if (_engine_instance_count) {

        engine_port_lock () ;

        while (mask && i < _engine_instance_count) {
            if ((mask & 0x1) && _engine_instance[i].statemachine) {
                _engine_instance[i].reg[ENGINE_VARIABLE_EVENT] = event_register ;
                _engine_event (&_engine_instance[i], event_id) ;

            }
            mask = mask >> 1 ;
            i++ ;

        }

        engine_port_unlock () ;

    }

}

/**
 * @brief       Internal callback used to marshal events onto the port provided
 *              thread to call Engine from.
 * @param[in]   task            allocated with engine_port_event_create() and
 *                              queued with engine_port_event_queue().
 * @param[in]   event_id
 * @param[in]   event_register
 * @param[in]   parm
 */
static void
engine_queue_event_cb (PENGINE_EVENT_T task, uint16_t event_id,
        int32_t event_register, uintptr_t parm)
{
    engine_event ((PENGINE_T)parm, event_id, event_register) ;
}

/**
 * @brief       Internal callback used to marshal events onto the port provided
 *              thread to call Engine from.
 * @param[in]   task            allocated with engine_port_event_create() and
 *                              queued with engine_port_event_queue().
 * @param[in]   event_id
 * @param[in]   event_register
 * @param[in]   parm
 */
void
engine_queue_masked_event_cb (PENGINE_EVENT_T task, uint16_t event_id,
        int32_t event_register, uintptr_t parm)
{
    engine_mask_event (parm, event_id, event_register) ;
}

/**
 * @brief       Get an bitmask for the engine instance.
 * @note        Used with the "mask" functions.
 * @param[in]   engine
 * @return      mask
 */
uint32_t
engine_get_mask (PENGINE_T engine)
{
    if (!engine) {
        return 0 ;

    }

    return (uint32_t)(1<<engine->idx)  ;
}


/**
 * @brief       This function will queue an event with its accosted event
 *              register to the statemachine running in the engine.
 * @param[in]   engine
 * @param[in]   event_id
 * @param[in]   event_register
 * @return      status
 */
int32_t
engine_queue_event (PENGINE_T engine, uint16_t event_id, int32_t event_register)
{
    int32_t status ;
    if (!_engine_instance_count) {
        return ENGINE_FAIL ;

    }

    PENGINE_EVENT_T task = engine_port_event_create (engine_queue_event_cb) ;
    if (!task) {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR,
            "[err] engine_queue_event event %s no memory",
            parts_get_event_name(event_id)) ;
        return ENGINE_NOMEM ;

    }

    ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG,
            "[dbg] engine_queue_event event %s",
            parts_get_event_name(event_id)) ;

    status = engine_port_event_queue (task, event_id, event_register,
            (uintptr_t) engine, 0) ;

    if (status != ENGINE_OK) {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR,
            "[err] statemachine_queue_event failed %d",
            status) ;

    }

    return status ;
}

/**
 * @brief       This function will queue an event with its accosted event
 *              register to all the engines in the mask.
 * @param[in]   mask
 * @param[in]   event_id
 * @param[in]   event_register
 * @return      status
 */
int32_t
engine_queue_masked_event (uint32_t mask, uint16_t event_id, int32_t event_register)
{
    int32_t status ;
    if(!mask) {
        return ENGINE_OK ;

    }
    if (!_engine_instance_count) {
        return ENGINE_FAIL ;

    }

    PENGINE_EVENT_T task = engine_port_event_create (engine_queue_masked_event_cb) ;
    if (!task) {
        ENGINE_LOG (0, ENGINE_LOG_TYPE_ERROR,
            "[err] engine_queue_masked_event event %s no memory",
            parts_get_event_name(event_id)) ;
        return ENGINE_NOMEM ;

    }

    ENGINE_LOG (0, ENGINE_LOG_TYPE_DEBUG,
            "[dbg] engine_queue_masked_event event %s",
            parts_get_event_name(event_id)) ;

    status = engine_port_event_queue (task, event_id, event_register, mask, 0) ;
    if (status != ENGINE_OK) {
        ENGINE_LOG (0, ENGINE_LOG_TYPE_ERROR,
            "[err] engine_queue_masked_event failed %d", status) ;

    }

    return status ;
}

/**
 * @brief       Log formatting function.
 */
static const char*
_log_param (PENGINE_T engine, STATES_ACTION_T* function, char * buffer, uint32_t len)
{
    uint16_t strlen ;
    if ((function->action & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
        int32_t val = 0 ;
        engine_get_variable (engine, function->param, &val) ;
        snprintf (buffer, len, "[%s]", engine_get_string (engine, function->param, &strlen)) ;

    }
    else if ((function->action & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
        snprintf (buffer, len, "'%s'",  engine_get_string (engine, function->param, &strlen)) ;

    }
    else if ((function->action & STATES_ACTION_TYPE_MASK) == STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET) {
        int32_t val = 0 ;
        engine_get_variable (engine, function->param, &val) ;
        snprintf (buffer, len, "[%d] %d", function->param, (int)val ) ;

    } else {
        snprintf (buffer, len, "%d", function->param) ;

    }
    buffer [len-1] = '\0' ;
    return buffer ;
}

/**
 * @brief       Log formatting function.
 */
const char*
_log_cond (PENGINE_T engine, STATES_INTERNAL_T* internal, const char* cond, char * buffer, uint32_t len)
{
    int32_t cond_val ;
    const char * term =  (internal->event & STATES_INTERNAL_EVENT_TERMINATE) ?
                    " - terminating" : "";

    if (!cond) return "" ;

    if (internal->event & STATES_EVENT_COND_ACTION_VARIABLE) {
        engine_get_variable (engine, internal->comp, &cond_val) ;
        snprintf (buffer, len, "(%s [%d] %d%s)",
                cond, internal->action.param, (int)cond_val, term) ;

    } else {
        cond_val = (int32_t)internal->comp ;
        snprintf (buffer, len, "(%s %d%s)", cond, (int)cond_val, term) ;
    }

    buffer [len-1] = '\0' ;
    return buffer ;
}

/**
 * @brief       Log entry and exit actions.
 */
static void
log_function(PENGINE_T engine, uint32_t filter, char* pre, STATES_ACTION_T* action)
{
    if ((filter & _engine_log_filter) &&
        ((!engine || ((1 << engine->idx) & _engine_log_instance)))) {
        char buffer[24] ;
        const char  result = (action->action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET ? PARSE_PUSH_OP :
                (action->action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET ? PARSE_POP_OP :
                (action->action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET ? PARSE_SAVE_OP : ' ' ;

        engine_log (engine, filter, "%s      %s%c, %s",
                pre,
                parts_get_action_name(action->action & STATES_ACTION_ID_MASK),
                result,
                _log_param (engine, action, buffer, 24)) ;

    }
}

/**
 * @brief       Log actions.
 */
static void
log_action (PENGINE_T engine, uint32_t filter, const char* pre, const char* cond, STATES_INTERNAL_T* internal)
{
    if ((filter & _engine_log_filter) &&
        ((!engine || ((1 << engine->idx) & _engine_log_instance)))) {
        char buffer[24] ;
        char buffer2[24] ;
        STATES_ACTION_T action = internal->action ;

        const char  result = (action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET ? PARSE_PUSH_OP :
                (action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET ? PARSE_POP_OP :
                (action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET ? PARSE_SAVE_OP : ' ' ;
        engine_log (engine, filter, "%s      %s%c %s %s",
                pre,
                parts_get_action_name(action.action & STATES_ACTION_ID_MASK),
                result,
                _log_cond (engine, internal, cond, buffer, 24),
                _log_param (engine, &action, buffer2, 24)) ;

    }
}

/**
 * @brief       Log events.
 */
static void
log_event (PENGINE_T engine, uint16_t  event_id)
{
    if ((ENGINE_LOG_TYPE_EVENTS & _engine_log_filter) &&
        ((!engine || ((1 << engine->idx) & _engine_log_instance)))) {

        //uint16_t cond = (event_id & STATES_EVENT_COND_MASK) >> STATES_EVENT_COND_OFFSET ;
        int32_t acc = 0 ;
        int32_t reg = 0 ;
        engine_get_variable (engine, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
        engine_get_variable (engine, ENGINE_VARIABLE_EVENT, &reg) ;

        engine_log (engine, ENGINE_LOG_TYPE_EVENTS, "[evt] ---* %s ([a] %d, [e] %d)",
            parts_get_event_name (event_id), acc, reg) ;

    }

}

/**
 * @brief       Log transitions.
 */
static void
log_transition (PENGINE_T engine, uint16_t  cond, const STATEMACHINE_STATE_T* current,
        const STATEMACHINE_STATE_T*  next)
{
    if ((ENGINE_LOG_TYPE_TRANSITIONS & _engine_log_filter) &&
        ((!engine || ((1 << engine->idx) & _engine_log_instance)))) {

        const char * pcond  ;
        int32_t acc = 0 ;
        int32_t reg = 0 ;
        engine_get_variable (engine, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
        engine_get_variable (engine, ENGINE_VARIABLE_REGISTER, &reg) ;

        if (cond == STATES_EVENT_COND_IF) pcond = "if" ;
        else if (cond == STATES_EVENT_COND_NOT) pcond = "not" ;
        else if (cond == STATES_EVENT_COND_IF_R) pcond = "if_r" ;
        else if (cond == STATES_EVENT_COND_NOT_R) pcond = "not_r" ;
        else pcond = "" ;

        engine_log (engine, ENGINE_LOG_TYPE_TRANSITIONS,
                    "[trn] ---> '%s' to '%s'   (%s) ([a] %d, [r] %d)",
                    current ? (char*)current->name : "", (char*)next->name,
                    pcond, acc, reg) ;

    }

}

/**
 * @brief       List superstates of the 'state' parameter.
 * @param[in]   state_machine    state_machine to use
 * @param[in]   state           states to find superstaates of
 * @param[out]  superstate      caller allocated list to receive super states
 * @param[in]   count           count of states in superstate list
 * @return      status
 */
static uint32_t
list_super_states (const STATEMACHINE_T* statemachine,
        const STATEMACHINE_STATE_T *state,
        const STATEMACHINE_STATE_T *superstate[], uint32_t count)
{
    uint32_t i = 0 ;
    if (state) {
        superstate[0] = state ;
        for (i=1; i<count; i++) {
            if (state && (state->super_idx != STATEMACHINE_INVALID_STATE)) {
                state =  GET_STATEMACHINE_STATE_REF(statemachine, state->super_idx) ;
                superstate[i] = state ;

            } else {
                break ;

            }

        }

    }

    return i ;
}

/**
 * @brief       Find LCA (least common ancestor) between the list of state in
 *              'superstate' and the list of states in 'next_superstate'.
 * @param[in]       state_machine   state_machine to use
 * @param[out]      superstate      list of states
 * @param[in/out]   count           count of states in superstate list up to the LCA
 * @param[in]       next_superstate list of states
 * @param[in/out]   next_count      count of states in next_superstate list up to the LCA
 */
static void
find_lca_states (const STATEMACHINE_T* statemachine,
        const STATEMACHINE_STATE_T *superstate[], int32_t* count,
        const STATEMACHINE_STATE_T *next_superstate[], int32_t* next_count)
{
    int32_t i , j  ;
    bool found = false ;

    for (i=*count-1; i>=0; i--) {
        for (j=*next_count-1; j>=0; j--) {
            if (superstate[i] == next_superstate[j]) {
                *count = i  ;
                *next_count = j  ;
                found = true ;
                break ;

            }

        }

        if (!found) {
            break ;

        }

    }


    return ;
}

/**
 * @brief       Calls the functions (entry or exit) of the state.
 * @param[in]   engine
 * @param[in]   state
 * @param[in]   offset   offset of first function in list of functions
 * @param[in]   count    number of functions to call
 * @param[in]   entry    entry/or exit (for logging purposes only)
 * @return      status
 */
static int32_t
state_functions (PENGINE_T engine, const STATEMACHINE_STATE_T* state,
                    uint16_t offset, uint16_t count, uint16_t entry)
{
    int i ;

    _engine_active_instance = engine ;

    if (entry) {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_ENTRY_FUNCTIONS,
                "[ent] ---- entry actions (%s): ", state->name) ;

    } else {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_EXIT_FUNCTIONS,
                "[ext] ---- exit actions (%s): ", state->name) ;
    }
 
    for (i=0; i<count; i++) {
        STATES_ACTION_T* action = (STATES_ACTION_T*)&state->data[offset + i] ;
        uint32_t action_id = action->action & STATES_ACTION_ID_MASK ;
        PART_ACTION_FP fp = parts_get_action_fp (action_id) ;
        if (fp) {
            int32_t result ;
            uint32_t flags = PART_ACTION_FLAG_EXEC ;
            uint16_t action_type = action->action & STATES_ACTION_TYPE_MASK ;

            if (entry) {
                log_function(engine, ENGINE_LOG_TYPE_ENTRY_FUNCTIONS, "[ent]", action) ;
            } else {
                log_function(engine, ENGINE_LOG_TYPE_EXIT_FUNCTIONS, "[ext]", action) ;
            }

            engine->timer = engine_timestamp() ;
            engine->action = action_id ;

            if ((action->action & STATES_ACTION_RESULT_MASK) ==
                    STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET) {
                engine_pop (engine);
            }

            if (!action_type) {
                result = fp (engine, action->param, flags) ;
            }
            else if (action_type == STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
                flags |= PART_ACTION_FLAG_INDEXED ;
                result = fp (engine, action->param, flags) ;
            }
            else if (action_type == STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
                flags |= PART_ACTION_FLAG_STRING ;
                result = fp (engine, action->param, flags) ;
            }
            else /* if (action_type == STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET)*/ {
                int32_t val = 0 ;
                flags |= PART_ACTION_FLAG_VARIABLE ;
                engine_get_variable (engine, action->param, &val) ;
                result = fp (engine, val, flags) ;

            }

            if ((action->action & STATES_ACTION_RESULT_MASK) ==
                    STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET) {
                engine_push (engine, result);

            }
            else if ((action->action & STATES_ACTION_RESULT_MASK) ==
                    STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET) {
                engine_set_variable (engine, ENGINE_VARIABLE_REGISTER, result) ;

            }

            engine->timer = engine_timestamp() - engine->timer ;
            if (engine->timer > (500)) {
                ENGINE_LOG(0,
                        (engine->timer > (4000) ? ENGINE_LOG_TYPE_ERROR : ENGINE_LOG_TYPE_LOG),
                        "[err] %s action %s %s %s time elapsed %d",
                        entry ? "entry" : "exit",
                        engine->statemachine->name,
                        engine->current ? (const char*)engine->current->name : "",
                        parts_get_action_name(action->action),
                        engine->timer) ;

            }
            engine->timer = 0 ;


        } else {
            ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR, "[err]      invalid action id %x (%s)",
                    action_id, state->name) ;

        }

    }
    _engine_active_instance = 0 ;

    return ENGINE_OK ;
}


/**
 * @brief       Handles an event dispatched to the engine.
 * @note        Checks for deferred events.
 *              Handles internal / local transitions.
 *              Handles external transitions.
 * @param[in]   state
 * @param[in]   event
 * @param[out]  next_state  next state to transition to
 * @return      event_id resulted in the transition
 */
static uint16_t
state_event (PENGINE_T engine, uint16_t event, uint16_t * next_state)
{
    int i ;
    const STATEMACHINE_STATE_T* super_state[STATEMACHINE_SUPER_STATE_MAX] ;
    int superstates = 0 ;

    DBG_ENGINE_ASSERT (engine->current,
                "[err] ---> state_event") ;

    if (engine->current) {

        superstates = list_super_states (engine->statemachine, engine->current,
                    super_state, STATEMACHINE_SUPER_STATE_MAX) ;

        /* evaluate deferred events as well as internal/local transitions */
        for (i=0; i<superstates; i++) {

            if (state_deferred_event (engine, super_state[i], event)) {
                *next_state =  STATEMACHINE_INVALID_STATE ;
                return STATEMACHINE_INVALID_STATE ;

            }


            if (!state_action (engine, event, super_state[i] )) {
                /* terminate evaluation of event for all superstates in the list */
                break ;
            }

        }

        /* evaluate external transitions */
        for (i=0; i<superstates; i++) {

            const STATEMACHINE_STATE_T* pstate = super_state[i] ;

            int j ;

            for (j=0; j<pstate->events; j++) {
                STATES_EVENT_T* states_event = (STATES_EVENT_T*)&pstate->data[j] ;
                if ((states_event->event & STATES_EVENT_ID_MASK) == event) {

                    uint16_t cond = states_event->event & STATES_EVENT_COND_MASK ;

                    if (cond) {
                        /* check for guards */
                        cond = cond >> STATES_EVENT_COND_OFFSET ;
                        if ((cond == STATES_EVENT_COND_IF) && !engine->reg[ENGINE_VARIABLE_ACCUMULATOR])  continue ;
                        else if ((cond == STATES_EVENT_COND_NOT) && engine->reg[ENGINE_VARIABLE_ACCUMULATOR])  continue ;
                        else if ((cond == STATES_EVENT_COND_IF_R) && !engine->reg[ENGINE_VARIABLE_REGISTER])  continue ;
                        else if ((cond == STATES_EVENT_COND_NOT_R) && engine->reg[ENGINE_VARIABLE_REGISTER])  continue ;
                    }
                    *next_state = states_event->next_state_idx ;

                    return states_event->event ;

                }
            }

        }
    }

    /* no transition found */
    *next_state =  STATEMACHINE_INVALID_STATE ;
    return STATEMACHINE_INVALID_STATE ;
}

/**
 * @brief       Execute all the actions for the event.
 * @note        This is internal / local transition.
 * @param[in]   engine
 * @param[in]   event_id
 * @param[in]   state
 * @return      true if the terminate flag was set for the event (action execution terminated)
 */
static bool
state_action (const PENGINE_T engine, uint16_t event_id, const STATEMACHINE_STATE_T* state)
{
    int i, start ;
    int32_t result ;
    uint32_t terminate = 0 ;

    if (state && state->action) {

        _engine_active_instance = engine ;

        start = state->events + state->deferred + state->entry + state->exit ;
        for (i=0; i<state->action; i+=2) {

            STATES_INTERNAL_T* internal = (STATES_INTERNAL_T*)&state->data[i + start] ;

            if ((internal->event & STATES_EVENT_ID_MASK) == event_id) {
                uint16_t action_id = internal->action.action & STATES_ACTION_ID_MASK ;

                PART_ACTION_FP fp = parts_get_action_fp (action_id) ;

                /* if the STATES_INTERNAL_EVENT_TERMINATE flag is set and this
                   action executes, terminate further actions for this event */
                terminate = internal->event & STATES_INTERNAL_EVENT_TERMINATE  ;

                 if (fp) {
                    uint32_t flags = PART_ACTION_FLAG_EXEC ;
                    uint16_t event_cond = (internal->event & STATES_EVENT_COND_MASK) >> STATES_EVENT_COND_OFFSET ;
                    uint16_t action_type = internal->action.action & STATES_ACTION_TYPE_MASK ;

                    if (event_cond) {
                        /* check for guards */

                        int32_t comp ;
                        if (internal->event & STATES_EVENT_COND_ACTION_VARIABLE) {
                            engine_get_variable (engine, internal->comp, &comp) ;
                        } else {
                            comp = (int16_t)internal->comp ;
                        }

                        if (event_cond == STATES_INTERNAL_EVENT_COMP_E_EQ) {
                            if (engine->reg[ENGINE_VARIABLE_EVENT] != comp)  continue ;
                            else  log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "e_eq", internal) ;
                        }
                        else if (event_cond == STATES_INTERNAL_EVENT_COMP_LT) {
                            if (engine->reg[ENGINE_VARIABLE_ACCUMULATOR] >= comp)  continue ;
                            else  log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "lt", internal) ;
                        }
                        else if (event_cond == STATES_INTERNAL_EVENT_COMP_GT) {
                            if (engine->reg[ENGINE_VARIABLE_ACCUMULATOR] <= comp)  continue ;
                            else  log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "gt", internal) ;
                        }
                        else if (event_cond == STATES_INTERNAL_EVENT_COMP_EQ) {
                            if (comp != engine->reg[ENGINE_VARIABLE_ACCUMULATOR]) continue ;
                            else  log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "eq", internal) ;
                        }
                        else if (event_cond == STATES_INTERNAL_EVENT_COMP_NE) {
                            if (comp == engine->reg[ENGINE_VARIABLE_ACCUMULATOR]) continue ;
                            else  log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "ne", internal) ;
                        }
                        else if (event_cond == STATES_INTERNAL_EVENT_COMP_LOAD) {
                            log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", "ld", internal) ;
                        } else {
                            ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR, "[err]      invalid condition %d (%s)",
                                    event_cond, state->name) ;

                        }


                    } else {
                        log_action(engine, ENGINE_LOG_TYPE_ACTION, "[act]", 0, internal) ;
                    }

                    engine->timer = engine_timestamp() ;
                    engine->action = action_id ;

                    if ((internal->action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET) {
                        engine_pop (engine);
                    }

                    if (!action_type) {
                        result = fp (engine, internal->action.param, flags) ;
                    }
                    else if (action_type == STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET) {
                        flags |= PART_ACTION_FLAG_INDEXED ;
                        result = fp (engine, internal->action.param, flags) ;
                    }
                    else if (action_type == STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET) {
                        flags |= PART_ACTION_FLAG_STRING ;
                        result = fp (engine, internal->action.param, flags) ;
                    }
                    else /*if (action_type == STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET)*/ {
                        int32_t val = 0 ;
                        flags |= PART_ACTION_FLAG_VARIABLE ;
                        engine_get_variable (engine, internal->action.param, &val) ;
                        result = fp (engine, val, flags) ;
                    }


                    engine->timer = engine_timestamp() - engine->timer ;

                    if ((internal->action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET) {
                        engine_push (engine, result);
                    }
                    else if ((internal->action.action & STATES_ACTION_RESULT_MASK) == STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET) {
                        engine_set_variable (engine, ENGINE_VARIABLE_REGISTER, result) ;
                    }

                    if (event_cond == STATES_INTERNAL_EVENT_COMP_LOAD) {
                         engine_set_variable (engine, internal->comp, result) ;
                    }

                    if (engine->timer > (500)) {
                        ENGINE_LOG(0,
                                (engine->timer > (4000) ? ENGINE_LOG_TYPE_ERROR : ENGINE_LOG_TYPE_REPORT),
                                "[err] action %s %s %s time elapsed %d",
                                engine->statemachine->name,
                                engine->current ? (const char*)engine->current->name : "",
                                parts_get_action_name(internal->action.action),
                                engine->timer) ;
                    }

                    engine->timer = 0 ;

                   if (terminate) break ;


               } else {
                   ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR, "[err]      invalid action id %x (%s)",
                           action_id, state->name) ;
               }



           }
       }

        _engine_active_instance = 0 ;

    }


    return terminate ? false : true ;
}

/**
 * @brief       Allocate and queue a deferred event.
 * @param[in]   engine
 * @param[in]   event
 * @param[in]   reg
 * @return      status
 */
static int32_t
deferred_event_add (PENGINE_T engine, uint16_t event, int32_t reg)
{
    ENGINE_DEFERED_T * deferred = engine_port_malloc(heapMachine, sizeof(ENGINE_DEFERED_T)) ;
    ENGINE_DEFERED_T * start ;

    if (!deferred) {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR, "[err] state_is_deferred_event failed alloc") ;
        return ENGINE_NOMEM ;
    }

    ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG, "[dbg] deferred_event_add event %s",
        parts_get_event_name(event)) ;


    deferred->next = 0 ;
    deferred->event =  event ;
    deferred->event_register = reg;

    for (start = engine->deferred; start && start->next; start = start->next) ;
    if (start) start->next = deferred ;
    else engine->deferred = deferred;
    engine->deferred_cnt++ ;

    ENGINE_LOG (engine, ENGINE_LOG_TYPE_EVENTS, "[evt] --** %s (%d)",
        parts_get_event_name(event),
        engine->deferred_cnt) ;

    return ENGINE_OK ;
}

/**
 * @brief       state_deferred_event
 * @param[in]   engine
 * @param[in]   state
 * @param[in]   event
 * @return      true if the event was defered
 */
static bool
state_deferred_event (PENGINE_T engine, const STATEMACHINE_STATE_T* state,
        uint16_t event_id)
{
    uint32_t i ;
    ENGINE_DEFERED_T * start ;

    if (state && state->deferred) {

        while (engine->deferred_cnt >= STATEMACHINE_DEFERRED_MAX) {

            ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR,
                        "[err] deferred event %d overflow",
                        engine->deferred_cnt) ;
            /* free up all deferred events more than the defined max */
            start = engine->deferred ;
            if (start) {
                DBG_ENGINE_ASSERT (engine->deferred_cnt, "deferred_cnt zero!") ;
                engine->deferred = start->next ;
                engine_port_free (heapMachine, start) ;
                engine->deferred_cnt-- ;

            }

        }

        uint32_t last = state->events + state->deferred ;
        for (i=state->events; i<last; i++) {
            STATES_EVENT_T* event = (STATES_EVENT_T*)&state->data[i] ;
            if (/*((event->event_id & STATES_EVENT_ID_MASK) == STATEMACHINE_ALL_EVENTS) ||*/
                    ((event->event & STATES_EVENT_ID_MASK) == event_id)) {

                if (deferred_event_add (engine, event_id,
                        engine->reg[ENGINE_VARIABLE_EVENT]) == ENGINE_OK) {
                    return true ;

                }

            }

        }

    }

    return false ;
}

/**
 * @brief       queue all defered events
 * @param[in]   engine
 */
static void
queue_all_deferred (PENGINE_T engine)
{
    while (engine->deferred) {
        ENGINE_DEFERED_T * start  = engine->deferred ;
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG,
                "[dbg] remove deferred event %s (%d)",
                parts_get_event_name(start->event), engine->deferred_cnt) ;
        engine_queue_event (engine, start->event, start->event_register);
        engine->deferred = start->next ;
        engine_port_free (heapMachine, start);
        engine->deferred_cnt-- ;
    }

    DBG_ENGINE_ASSERT (!engine->deferred_cnt, "queue_all_deferred invalid!") ;
}

/**
 * @brief       Transition to the next state.
 * @param[in]   engine
 * @param[in]   next_idx
 * @param[in]   cond
 * @return      status
 */
int32_t
state_transition (PENGINE_T engine, uint16_t next_idx, uint16_t cond)
{
    const STATEMACHINE_STATE_T* next_state = 0 ;

    /* get the next state */
    if (next_idx == STATEMACHINE_PREVIOUS_STATE) {
        next_state = engine->prev[engine->prev_idx] ;
        engine->prev[engine->prev_idx] = 0 ;
        if (engine->prev_idx == 0) {
            engine->prev_idx = ENGINE_PREVIOUS_STACK ;

        }
        engine->prev_idx-- ;
        if (next_state == 0) next_state = engine->current ;
        engine->prev_pin = 0 ;

    }
    else if (next_idx == STATEMACHINE_CURRENT_STATE) {
        next_state = engine->current ;

    }
    else if (next_idx == STATEMACHINE_IGNORE_STATE) {
            next_state = 0 ;

    }
    else if (next_idx >= engine->statemachine->count) {
        ENGINE_LOG (engine, ENGINE_LOG_TYPE_ERROR,
                    "[err] ---> transition idx out of bounds",
                    next_idx) ;

    } else {
        next_state = GET_STATEMACHINE_STATE_REF(engine->statemachine, next_idx) ;

    }

    if (next_state) {
        int32_t i ;
        const STATEMACHINE_STATE_T* super_state[STATEMACHINE_SUPER_STATE_MAX] ;
        const STATEMACHINE_STATE_T* next_super_state[STATEMACHINE_SUPER_STATE_MAX] ;
        int32_t superstates = 0 ;
        int32_t next_superstates = 0 ;
        const STATEMACHINE_STATE_T* s ;

        TRANSITION_HANDLER_T * h = engine->transition_handler ;
        while (h) {
            h->handler (engine, next_idx, cond) ;
            h = h->next ;

        }

        log_transition (engine, cond, engine->current, next_state) ;

        next_superstates = list_super_states (engine->statemachine, next_state,
                next_super_state, STATEMACHINE_SUPER_STATE_MAX) ;
        if (engine->current) {
            superstates = list_super_states (engine->statemachine, engine->current,
                super_state, STATEMACHINE_SUPER_STATE_MAX) ;

            find_lca_states (engine->statemachine,
                    super_state, &superstates,
                    next_super_state, &next_superstates) ;

            /* exit actions for current state */
            s = engine->current;
            state_functions (engine, s, s->events + s->deferred + s->entry,
                    s->exit, 0) ;
            for (i=1; i<superstates; i++) {
                /* exit actions of each state up to the but not including
                   the lca superstate */
                s = super_state[i] ;
                state_functions (engine, s, s->events + s->deferred + s->entry,
                        s->exit, 0) ;

            }

        }

        for (i=next_superstates-1; i>0; i--) {
            /* entry actions from the superstate before the lca down to the
               current state */
            s = next_super_state[i] ;
            state_functions (engine, s, s->events + s->deferred,
                    s->entry, 1) ;

        }
        /* entry actions for next state */
        s = next_state ;
        state_functions (engine, s, s->events + s->deferred, s->entry, 1) ;

        /* push the previous state on the p[revious stack */
        if (!engine->prev_pin && (next_idx < engine->statemachine->count)) {
            if (engine->prev_idx == ENGINE_PREVIOUS_STACK - 1) {
                engine->prev_idx = 0 ;

            }
            else {
                engine->prev_idx++ ;

            }
            engine->prev[engine->prev_idx] = engine->current ;

        }

        engine->current = next_state ;

    } else {
        return next_idx == STATEMACHINE_IGNORE_STATE ? ENGINE_FAIL :
                ENGINE_OK  ; /* ENGINE_OK will dispatch the _state_start event again */

    }

    return ENGINE_OK ;
}


/**
 * @brief       Get a string from the stringtable
 * @param[in]   engine
 * @param[in]   idx
 * @param[out]  len
 * @return      string
 */
const char*
engine_get_string (PENGINE_T engine, uint16_t idx, uint16_t * len)
{
    const STATEMACHINE_STRING_T* pstr;

    if (len) {
        *len = 0;

    }

    if (!_engine_stringtable) {
        return 0;

    }

    if (idx >= _engine_stringtable->count) {
        return 0;

    }

    pstr = GET_STATEMACHINE_STRINGTABLE_REF(_engine_stringtable, idx) ;

    if ((pstr == 0) || (pstr->len == 0)) {
        return 0 ;

    }

    if (len) {
        *len = pstr->len;

    }

    ENGINE_LOG (engine, ENGINE_LOG_TYPE_DEBUG,
                "[dbg] ---> statemachine_get_string %d:%s",
                (unsigned int)idx, pstr->value) ;

    return (const char*) pstr->value;
}

void
engine_dump (bool active_only)
{
    int i ;
    int cnt = 0 ;

    for (i=0; i<ENGINE_MAX_INSTANCES; i++) {

        if (_engine_instance[i].statemachine) {
            if (!active_only || _engine_instance[i].timer) {
                if (_engine_instance[i].timer) cnt++ ;
                ENGINE_LOG(0, ENGINE_LOG_TYPE_REPORT,
                    "[rpt] %s -> %s   (last action %s, timer %d)",
                    _engine_instance[i].statemachine->name,
                    _engine_instance[i].current->name,
                    parts_get_action_name(_engine_instance[i].action & STATES_ACTION_ID_MASK),
                    _engine_instance[i].timer ? (engine_timestamp() - _engine_instance[i].timer) : 0 ) ;

            }

        }

    }

    ENGINE_LOG(0, ENGINE_LOG_TYPE_REPORT, "[rpt] %d tasks stuck.", cnt)
}

uint32_t
engine_check(const char ** name)
{
    int i ;
    uint32_t max = 0 ;
    for (i=0; i<ENGINE_MAX_INSTANCES; i++) {
        if (_engine_instance[i].statemachine) {
            if (_engine_instance[i].timer) {
                uint32_t time = engine_timestamp() - _engine_instance[i].timer ;
                if (time > max) {
                    max = time ;
                    if (*name) *name = parts_get_action_name(_engine_instance[i].action & STATES_ACTION_ID_MASK) ;

                }

            }

        } else {
            break ;

        }

    }

    return max ;
}

int32_t
engine_statemachine_idx (const char * name)
{
    int32_t idx ;

    for (idx=0; idx<engine_statemachine_count(); idx++) {
        if (strcmp(engine_statemachine_name(idx), name) == 0) {
            return idx ;

        }

    }

    return ENGINE_NOTFOUND ;
}

uint32_t
engine_statemachine_logmask (const char * name)
{
    int32_t idx = engine_statemachine_idx (name) ;

    if (idx > 0) {
        return 1 << (uint32_t)idx ;
    }

    return 0 ;
}


