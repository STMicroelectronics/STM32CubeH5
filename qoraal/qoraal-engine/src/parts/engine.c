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
#if !defined CFG_ENGINE_ENGINE_PART_DISABLE

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"
#include "../port/port.h"

/*===========================================================================*/
/* part local functions.                                                     */
/*===========================================================================*/
static int32_t      part_state_cmd (PENGINE_T instance, uint32_t start) ;
static int32_t      action_state_timeout (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timeout_sec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event_local (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event_if (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event_local_if (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event_not (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_event_local_not (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer1 (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer1_sec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer1_active (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer2 (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer2_sec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_timer2_active (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_keepalive1 (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_keepalive1_sec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_keepalive2 (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_state_keepalive2_sec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_get (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_strlen (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_rand (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_load (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_mov (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_get (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_and (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_or (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_mult (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_div (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_add (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_sub (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_not (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_mod (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_push (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_swap (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_pop (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_inc (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_dec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_eq (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_lt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_a_gt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_e_eq (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_e_lt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_e_gt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_load (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_inc (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_dec (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_set (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_clear (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_eq (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_lt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_r_gt (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_p_load (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_p_add (PENGINE_T instance, uint32_t parm, uint32_t flags) ;

/**
 * @brief   Declare actions for part
 *
 */
ENGINE_ACTION_IMPL  (   state_timeout,              "Set state timeout timer (milliseconds) (cancelled on the first transition)") ;
ENGINE_ACTION_IMPL  (   state_timeout_sec,          "Set state timeout timer (seconds) (cancelled on the first transition)") ;
ENGINE_ACTION_IMPL  (   state_timer1,               "Set state timer 1 (milliseconds)") ;
ENGINE_ACTION_IMPL  (   state_timer1_sec,           "Set state timer 1 (seconds)") ;
ENGINE_ACTION_IMPL  (   state_timer1_active,        "Return TRUE if timer active") ;
ENGINE_ACTION_IMPL  (   state_timer2,               "Set state timer 2 (milliseconds)") ;
ENGINE_ACTION_IMPL  (   state_timer2_sec,           "Set state timer 2 (seconds)") ;
ENGINE_ACTION_IMPL  (   state_timer2_active,        "Return TRUE if timer active") ;
ENGINE_ACTION_IMPL  (   state_keepalive1,           "Set state keep-alive timer (autorepeat milliseconds)") ;
ENGINE_ACTION_IMPL  (   state_keepalive1_sec,       "Set state keep-alive timer (autorepeat seconds)") ;
ENGINE_ACTION_IMPL  (   state_keepalive2,           "Set state keep-alive timer (autorepeat milliseconds)") ;
ENGINE_ACTION_IMPL  (   state_keepalive2_sec,       "Set state keep-alive timer (autorepeat seconds)") ;

ENGINE_ACTION_IMPL  (   state_event,                "Fire the event to all state machines") ;
ENGINE_ACTION_IMPL  (   state_event_local,          "Fire the event to this state machine only") ;
ENGINE_ACTION_IMPL  (   state_event_if,             "Fire the event to all state machines if accumulator set") ;
ENGINE_ACTION_IMPL  (   state_event_local_if,       "Fire the event to this state machine only if accumulator set") ;
ENGINE_ACTION_IMPL  (   state_event_not,            "Fire the event to all state machines if accumulator clear") ;
ENGINE_ACTION_IMPL  (   state_event_local_not,      "Fire the event to this state machine only if accumulator clear") ;

ENGINE_ACTION_IMPL  (   get,                        "Load and return the value.") ;
ENGINE_ACTION_IMPL  (   strlen,                     "Return the string length.") ;
ENGINE_ACTION_IMPL  (   rand,                       "Return rand value % parm.") ;

ENGINE_ACTION_IMPL  (   a_load,                     "[a] = parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_mov,                      "[r] = [a] ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_get,                      "return [a]") ;
ENGINE_ACTION_IMPL  (   a_and,                      "[a] = [a] && parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_or,                       "[a] = [a] || parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_add,                      "[a] += parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_sub,                      "[a] -= parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_mult,                     "[a] *= parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_div,                      "[a] /= parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_not,                      "[a] = ![a] ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_mod,                      "[a] %= parm ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_inc,                      "[a]++ (up to parm) ;  return [a]") ;
ENGINE_ACTION_IMPL  (   a_dec,                      "[a]-- (down to parm) ; return [a]") ;
ENGINE_ACTION_IMPL  (   a_eq,                       "return [a] == parm") ;
ENGINE_ACTION_IMPL  (   a_gt,                       "return [a] > parm") ;
ENGINE_ACTION_IMPL  (   a_lt,                       "return [a] < parm") ;
ENGINE_ACTION_IMPL  (   e_eq,                       "return [e] == parm") ;
ENGINE_ACTION_IMPL  (   e_gt,                       "return [e] > parm") ;
ENGINE_ACTION_IMPL  (   e_lt,                       "return [e] < parm") ;
ENGINE_ACTION_IMPL  (   r_load,                     "[r] = parm ; return [r]") ;
ENGINE_ACTION_IMPL  (   r_inc,                      "if ++[r] == parm { [a] = 1; } else { [a] = 0; } return [a]") ;
ENGINE_ACTION_IMPL  (   r_dec,                      "if --[r] == parm { [a] = 1; } else { [a] = 0; } return [a]") ;
ENGINE_ACTION_IMPL  (   r_set,                      "[r] = 1 ; retutn [r]") ;
ENGINE_ACTION_IMPL  (   r_clear,                    "[r] = 0 ; retutn [r]") ;
ENGINE_ACTION_IMPL  (   r_eq,                       "[a] = [r] == parm ; retutn [a]") ;
ENGINE_ACTION_IMPL  (   r_lt,                       "[a] = [r] < parm ; retutn [a]") ;
ENGINE_ACTION_IMPL  (   r_gt,                       "[a] = [r] > parm ; retutn [a]") ;
ENGINE_ACTION_IMPL  (   p_load,                     "[p] = parm ; return [p]") ;
ENGINE_ACTION_IMPL  (   p_add,                      "[p] += parm ; return [p]") ;

ENGINE_ACTION_IMPL  (   a_push,                     "Push accumulator") ;
ENGINE_ACTION_IMPL  (   a_pop,                      "Pop accumulator") ;
ENGINE_ACTION_IMPL  (   a_swap,                     "Swap accumulator") ;

/**
 * @brief   Declare events for part
 *
 */
ENGINE_EVENT_IMPL   (   _state_timeout,             "Event for state_timeout()") ;
ENGINE_EVENT_IMPL   (   _state_keepalive1,          "Event for state_keepalive1()") ;
ENGINE_EVENT_IMPL   (   _state_keepalive2,          "Event for state_keepalive2()") ;
ENGINE_EVENT_IMPL   (   _state_timer1,              "Event for state_timer1()") ;
ENGINE_EVENT_IMPL   (   _state_timer2,              "Event for state_timer2()") ;

/**
 * @brief   Declare constants for part
 *
 */
ENGINE_CONST_IMPL   (   STATEMACHINE_INVALID_STATE,     INVALID,            "Invalid State (use instead of a state name to for an event transition)") ;
ENGINE_CONST_IMPL   (   STATEMACHINE_PREVIOUS_STATE,    PREVIOUS,           "Previous State (use instead of a state name for an event transition)") ;
ENGINE_CONST_IMPL   (   STATEMACHINE_CURRENT_STATE,     CURRENT,            "Current State (use instead of a state name for an event transition)") ;
ENGINE_CONST_IMPL   (   STATEMACHINE_IGNORE_STATE,      IGNORE,             "Ignore State (use instead of a state name for an event transition)") ;
ENGINE_CONST_IMPL   (   SHRT_MAX,                       VAL_MAX,            "Maximum for constant value") ;
ENGINE_CONST_IMPL   (   SHRT_MIN,                       VAL_MIN,            "Minimum for constant value") ;
ENGINE_CONST_IMPL   (   1,                              TRUE,               "True") ;
ENGINE_CONST_IMPL   (   0,                              FALSE,              "False") ;
ENGINE_CONST_IMPL   (   1,                              ENABLE,             "Enable") ;
ENGINE_CONST_IMPL   (   0,                              DISABLE,            "Disable") ;


/**
 * @brief   Part declaration.
 *
 */
ENGINE_CMD_FP_IMPL (part_state_cmd) ;


#define STATE_TASK_TIMEOUT          0
#define STATE_TASK_TIMER1           1
#define STATE_TASK_TIMER2           2
#define STATE_TASK_KEEPALIVE1       3
#define STATE_TASK_KEEPALIVE2       4


static PENGINE_EVENT_T          _part_tasks[ENGINE_MAX_INSTANCES][STATE_TASK_KEEPALIVE2+1] = {0};

int32_t
inst_set_task (PENGINE_T engine, uint32_t idx, PENGINE_EVENT_T task)
{
    int32_t inst_idx = engine_instance_idx (engine) ;
    DBG_ENGINE_ASSERT (((inst_idx >= 0) && (inst_idx < ENGINE_MAX_INSTANCES)),
            "[err] ---> inst_set_task") ;

    PENGINE_EVENT_T prev = _part_tasks[inst_idx][idx] ;
    _part_tasks[inst_idx][idx] = task ;
    if (prev) {
        return engine_port_event_cancel (prev) ;
    }

    return 0 ;
}

PENGINE_EVENT_T
inst_get_task (PENGINE_T engine, uint32_t idx)
{
    int32_t inst_idx = engine_instance_idx (engine) ;
    DBG_ENGINE_ASSERT (((inst_idx >= 0) && (inst_idx < ENGINE_MAX_INSTANCES)),
            "[err] ---> inst_set_task") ;

    return _part_tasks[inst_idx][idx] ;
}



static void
on_transition (PENGINE_T engine, uint16_t next_idx, int16_t cond)
{
    inst_set_task (engine, STATE_TASK_TIMEOUT, 0)  ;
}

/**
 * @brief   Receive start/stop commands from engine
 * @param[in] instance      engine instance.
 * @param[in] start         start/stop.
 */
int32_t
part_state_cmd (PENGINE_T instance, uint32_t start)
{
    static TRANSITION_HANDLER_T handler = {0, on_transition} ;
    if (instance) {
        if (!start) {
            int i ;
            for (i=0; i<STATE_TASK_KEEPALIVE2; i++) {
                inst_set_task (instance, i, 0) ;

            }
            engine_remove_transition_handler (instance, &handler) ;

        } else {
            engine_add_transition_handler (instance, &handler) ;

        }

    }

    return ENGINE_OK ;
}


static void
action_state_task_cb (PENGINE_EVENT_T task, uint16_t event_id, int32_t event_register, uintptr_t parm)
{
    int32_t inst_idx = engine_instance_idx ((PENGINE_T)parm) ;
    _part_tasks[inst_idx][event_register] = 0 ;
    engine_event ((PENGINE_T)parm, event_id, 0) ;
}

static int32_t
do_state_timeout (PENGINE_T instance, uint32_t parm, uint32_t flags, uint32_t mult)
{
    int32_t value ;
    int32_t ret ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    ret = inst_set_task (instance, STATE_TASK_TIMEOUT, 0) ;

    if (value) {
        value *= mult ;

        PENGINE_EVENT_T task = engine_port_event_create (action_state_task_cb) ;
        if (!task ||
                (engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_timeout),
                STATE_TASK_TIMEOUT, (uintptr_t) instance, value) != ENGINE_OK)) {
            return ENGINE_FAIL ;

        }

        inst_set_task (instance, STATE_TASK_TIMEOUT, task) ;

    }

    return ret / mult ;
}

/**
 * @brief   timeout event after parm in msec. cancelled on next transition if not expired.
 * @note    dispatches _state_timeout on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timeout (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timeout (instance, parm, flags, 1) ;
}

/**
 * @brief   timeout event after parm in seconds. cancelled on next transition if not expired.
 * @note    dispatches _state_timeout on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timeout_sec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timeout (instance, parm, flags, 1000) ;
}



static int32_t
do_state_timer1 (PENGINE_T instance, uint32_t parm, uint32_t flags, uint32_t mult)
{
    int32_t value ;
    int32_t ret ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    ret = inst_set_task (instance, STATE_TASK_TIMER1, 0) ;

    if (value) {
        value *= mult ;

        PENGINE_EVENT_T task = engine_port_event_create (action_state_task_cb) ;
        if (!task ||
            (engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_timer1),
                STATE_TASK_TIMER1, (uintptr_t) instance, value) != ENGINE_OK)){
            return ENGINE_FAIL ;

        }
        inst_set_task (instance, STATE_TASK_TIMER1, task) ;

    }

    return ret / mult ;
}

/**
 * @brief   general purpose 1 shot timer.
 * @note    dispatches _state_timer1 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timer1 (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timer1 (instance, parm, flags, 1) ;
}

/**
 * @brief   general purpose 1 shot timer
 * @note    dispatches _state_timer1 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timer1_sec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timer1 (instance, parm, flags, 1000) ;
}

/**
 * @brief   check is timer is still active (not expired)
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 * @return                  true if timer active
 */
int32_t
action_state_timer1_active (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK ;
    }
    return inst_get_task (instance, STATE_TASK_TIMER1) ? 1 : 0 ;
}

int32_t
do_state_timer2 (PENGINE_T instance, uint32_t parm, uint32_t flags, uint32_t mult)
{
    int32_t value ;
    int32_t ret ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    ret = inst_set_task (instance, STATE_TASK_TIMER2, 0) ;

    if (value) {
        value *= mult ;

        PENGINE_EVENT_T task = engine_port_event_create (action_state_task_cb) ;
        if (!task ||
            (engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_timer2),
                STATE_TASK_TIMER2, (uintptr_t) instance, value) != ENGINE_OK)) {
            return ENGINE_FAIL ;

        }

        inst_set_task (instance, STATE_TASK_TIMER2, task) ;

    } 

    return ret / mult ;
}

/**
 * @brief   general purpose 1 shot timer
 * @note    dispatches _state_timer2 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timer2 (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timer2 (instance, parm, flags, 1) ;
}

/**
 * @brief   general purpose 1 shot timer
 * @note    dispatches _state_timer2 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_timer2_sec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_timer2 (instance, parm, flags, 1000) ;
}

/**
 * @brief   check is timer is still active (not expired)
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 * @return                  true if timer active
 */
int32_t
action_state_timer2_active (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK ;
    }
    return inst_get_task (instance, STATE_TASK_TIMER2) ? 1 : 0 ;
}

static void
state_keepalive1_timer_cb (PENGINE_EVENT_T task, uint16_t event_id, int32_t event_register, uintptr_t parm)
{
    int32_t inst_idx = engine_instance_idx ((PENGINE_T)parm) ;
    _part_tasks[inst_idx][STATE_TASK_KEEPALIVE1] = 0 ;
    engine_event ((PENGINE_T)parm, event_id, 0) ;

    task = engine_port_event_create (state_keepalive1_timer_cb) ;
    engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_keepalive1),
            event_register, parm, event_register) ;
    inst_set_task ((PENGINE_T)parm, STATE_TASK_KEEPALIVE1, task) ;
}


static void
state_keepalive2_timer_cb (PENGINE_EVENT_T task, uint16_t event_id, int32_t event_register, uintptr_t parm)
{
    int32_t inst_idx = engine_instance_idx ((PENGINE_T)parm) ;
    _part_tasks[inst_idx][STATE_TASK_KEEPALIVE2] = 0 ;
    engine_event ((PENGINE_T)parm, event_id, 0) ;

    task = engine_port_event_create (state_keepalive2_timer_cb) ;
    engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_keepalive2),
            event_register, parm, event_register) ;
    inst_set_task ((PENGINE_T)parm, STATE_TASK_KEEPALIVE2, task) ;
}

int32_t
do_state_keepalive1 (PENGINE_T instance, uint32_t parm, uint32_t flags, uint32_t mult)
{
    int32_t value ;
    int32_t ret ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    ret = inst_set_task (instance, STATE_TASK_KEEPALIVE1, 0) ;

    if (value) {
        value *= mult ;

        PENGINE_EVENT_T task = engine_port_event_create (state_keepalive1_timer_cb) ;
        if (!task ||
            (engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_keepalive1),
                value, (uintptr_t) instance, value) != ENGINE_OK)) {
            return ENGINE_FAIL ;

        }

        inst_set_task (instance, STATE_TASK_KEEPALIVE1, task) ;

    }

    return ret / mult ;
}

/**
 * @brief   general purpose timer. Repeats at interval parm in msec.
 * @note    dispatches _state_keepalive1 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_keepalive1 (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_keepalive1 (instance, parm, flags, 1) ;
}

/**
 * @brief   general purpose timer. Repeats at interval parm in seconds.
 * @note    dispatches _state_keepalive1 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_keepalive1_sec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_keepalive1 (instance, parm, flags, 1000) ;
}


static int32_t
do_state_keepalive2 (PENGINE_T instance, uint32_t parm, uint32_t flags, uint32_t mult)
{
    int32_t value ;
    int32_t ret ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    ret = inst_set_task (instance, STATE_TASK_KEEPALIVE2, 0) ;

    if (value) {
        value *= mult ;

        PENGINE_EVENT_T task = engine_port_event_create (state_keepalive2_timer_cb) ;
        if (!task ||
            (engine_port_event_queue (task, ENGINE_EVENT_ID_GET(_state_keepalive2),
                value, (uintptr_t) instance, value) != ENGINE_OK)) {
            return ENGINE_FAIL ;

        }

        inst_set_task (instance, STATE_TASK_KEEPALIVE2, task) ;

    }

    return ret / mult ;
}

/**
 * @brief   general purpose timer. Repeats at interval parm in msec.
 * @note    dispatches _state_keepalive2 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_keepalive2 (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_keepalive2 (instance, parm, flags, 1) ;
}

/**
 * @brief   general purpose timer. Repeats at interval parm in msec.
 * @note    dispatches _state_keepalive2 on expiration
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_keepalive2_sec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_keepalive2 (instance, parm, flags, 1000) ;
}

static int32_t
do_state_event_if (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t val = 0 ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        if ((parm & STATES_EVENT_ID_MASK) >= STATES_EVENT_DECL_START) {
            return ENGINE_OK ;
        }
        if (parts_get_event ((uint16_t)parm)) {
            return ENGINE_OK ;
        }
        return ENGINE_FAIL ;
    }

    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &val) ;


    if (val) engine_queue_event (instance, parm, 0) ;

    return ENGINE_OK ;
}

static int32_t
do_state_event_not (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t val = 0 ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        if ((parm & STATES_EVENT_ID_MASK) >= STATES_EVENT_DECL_START) {
            return ENGINE_OK ;
        }
        if (parts_get_event ((uint16_t)parm)) {
            return ENGINE_OK ;
        }
        return ENGINE_FAIL ;
    }

    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &val) ;

    if (!val) engine_queue_event (instance, parm, 0) ;

    return ENGINE_OK ;
}


static int32_t
do_state_event (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        if ((parm & STATES_EVENT_ID_MASK) >= STATES_EVENT_DECL_START) {
            return ENGINE_OK ;
        }
        if (parts_get_event ((uint16_t)parm)) {
            return ENGINE_OK ;
        }
        return ENGINE_FAIL ;
    }


    engine_queue_event (instance, parm, 0) ;

    return ENGINE_OK ;
}

/**
 * @brief   dispatches parm as event to all engines
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event (0, parm, flags) ;
}

/**
 * @brief   dispatches parm as event only to this  instance
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event_local (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event (instance, parm, flags) ;
}

/**
 * @brief   dispatches parm as event to all engines if accumulator is set.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event_if (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event_if (0, parm, flags) ;
}

/**
 * @brief   dispatches parm as event only to this instance if accumulator is set.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event_local_if (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event_if (instance, parm, flags) ;
}

/**
 * @brief   dispatches parm as event to all engines if accumulator is clear.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event_not (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event_not (0, parm, flags) ;
}

/**
 * @brief   dispatches parm as event only to this instance if accumulator is clear.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_state_event_local_not (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return do_state_event_not (instance, parm, flags) ;
}

/**
 * @brief   get the parameter value and return it.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_get (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }

    return parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;
}

/**
 * @brief   return strlen if param is a string.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_strlen (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    const char* str = 0 ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_string (instance, parm, flags) ;
    }
    str = parts_get_string(instance, parm, flags) ;

    if (!str) {
        return 0 ;
    }

    return strlen(str)  ;
}


/**
 * @brief   rand() % parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_rand (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, 0, INT_MAX) ;

    parm = rand () ;
    if (value) {
        parm %= value ; ;
    }

    return parm ;
}

/**
 * @brief   [a] = parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         Compile, validate and parameter type flag.
 */
int32_t
action_a_load (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, value) ;

    return value ;
}


/**
 * @brief   [r] = [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_mov (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK ;
    }

    int32_t acc  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    engine_set_variable (instance, ENGINE_VARIABLE_REGISTER, acc) ;

    return acc ;
}

/**
 * @brief   return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         Compile, validate and parameter type flag.
 */
int32_t
action_a_get (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK ;
    }

    int32_t acc  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;

    return acc ;
}

/**
 * @brief   [a] *= parm ;  return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_mult (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0  ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc *= value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] = [a] && parm ;  return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_and (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0  ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc = acc && value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] = [a] || parm ;  return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_or (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc = acc || value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] /= parm; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_div (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    if (!value) return ENGINE_PARM ;
    int32_t acc = 0  ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc /= value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}


/**
 * @brief   [a] += parm; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */

int32_t
action_a_add (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc += value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] -= parm; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */

int32_t
action_a_sub (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0  ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc -= value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] %= parm ; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */

int32_t
action_a_mod (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc = 0  ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc %= value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a] = ![a] ; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_not (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    //int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK ;
    }

    int32_t acc = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc = !acc ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   return [a] == parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_eq (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc   ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;

    return acc == value ;
}

/**
 * @brief   return [a] < parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_lt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc   ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;

    return acc < value ;
}

/**
 * @brief   return [a] > parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_gt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc   ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;

    return acc > value ;
}

/**
 * @brief   return [e] == parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_e_eq (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc   ;
    engine_get_variable (instance, ENGINE_VARIABLE_EVENT, &acc) ;

    return acc == value ;
}

/**
 * @brief   return [e] < parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_e_lt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc   ;
    engine_get_variable (instance, ENGINE_VARIABLE_EVENT, &acc) ;

    return acc < value ;
}

/**
 * @brief   return [e] > parm
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_e_gt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc  ;
    engine_get_variable (instance, ENGINE_VARIABLE_EVENT, &acc) ;

    return acc > value ;
}


/**
 * @brief   push accumulator
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_push (PENGINE_T instance, uint32_t parm, uint32_t flags)
{

    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_push (instance, value);

    return ENGINE_OK ;
}


/**
 * @brief   swap accumulator
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_swap (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t res = 1 ;

    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return res ? ENGINE_OK : ENGINE_FAIL;
    }

    engine_swap (instance);

    return ENGINE_OK ;
}


/**
 * @brief   pop accumulator
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_pop (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t res = 1 ;

    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return res ? ENGINE_OK : ENGINE_FAIL;
    }

    engine_pop (instance);

    return ENGINE_OK ;
}

/**
 * @brief   [a]++ with parm as max ; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_inc (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    acc++ ;
    if (acc > value) {
        acc = value ;
    }
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [a]-- with parm as min ; return [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_a_dec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t acc  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, &acc) ;
    if (acc > value) {
        acc-- ;
    }
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, acc) ;

    return acc ;
}

/**
 * @brief   [r] = parm ; return [r]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_load (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_set_variable (instance, ENGINE_VARIABLE_REGISTER, value) ;

    return value ;
}

/**
 * @brief   [r]++ ; return [r]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_inc (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t reg  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_REGISTER, &reg) ;
    reg++ ;
    if (reg >= value) {
        engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, 1) ;
        value = 1 ;
    } else {
        engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, 0) ;
        value = 0 ;
    }
    engine_set_variable (instance, ENGINE_VARIABLE_REGISTER, reg) ;

    return value ;
}

/**
 * @brief   [r]++ ; return [r]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_dec (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t reg  = 0 ;
    engine_get_variable (instance, ENGINE_VARIABLE_REGISTER, &reg) ;
    reg-- ;
    if (reg <= value) {
        engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, 1) ;
        value = 1 ;
    } else {
        engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, 0) ;
        value = 0 ;
    }
    engine_set_variable (instance, ENGINE_VARIABLE_REGISTER, reg) ;

    return value ;
}


/**
 * @brief   [r] = 1
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_set (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, ENGINE_REGISTER_COUNT-1) ;
    }
    value = parts_get_int (instance, parm, flags, 0, ENGINE_REGISTER_COUNT-1) ;

    engine_set_variable (instance, value, 1) ;

    return ENGINE_OK ;
}

/**
 * @brief   [r] = 0
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_clear (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, 4) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_set_variable (instance, value, 0) ;

    return ENGINE_OK ;
}

/**
 * @brief   return [r] == parm in [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_eq (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t reg   ;
    engine_get_variable (instance, ENGINE_VARIABLE_REGISTER, &reg) ;

    int32_t res = reg == value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, res) ;
    return res ;
}

/**
 * @brief   return [r] == parm in [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_lt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t reg   ;
    engine_get_variable (instance, ENGINE_VARIABLE_REGISTER, &reg) ;

    int32_t res = reg < value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, res) ;
    return res ;
}

/**
 * @brief   return [r] == parm in [a]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_r_gt (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    int32_t reg   ;
    engine_get_variable (instance, ENGINE_VARIABLE_REGISTER, &reg) ;

    int32_t res = reg > value ;
    engine_set_variable (instance, ENGINE_VARIABLE_ACCUMULATOR, res) ;
    return res ;
}


/**
 * @brief   [p] = parm ; return [p]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_p_load (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_set_variable (instance, ENGINE_VARIABLE_PARAMETER, value) ;

    return value ;
}

/**
 * @brief   [p] += parm ; return [p]
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_p_add (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t value ;
    int32_t p = 0 ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX) ;
    }
    value = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX) ;

    engine_get_variable (instance, ENGINE_VARIABLE_PARAMETER, &p) ;
    value += p ;
    engine_set_variable (instance, ENGINE_VARIABLE_PARAMETER, value) ;

    return value ;
}

#endif /* CFG_ENGINE_ENGINE_PART_DISABLE */

    /**@}*/
