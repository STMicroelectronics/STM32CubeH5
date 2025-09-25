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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if defined CFG_ENGINE_REGISTRY_ENABLE
#include "qoraal-flash/registry.h"
#endif
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"
#include "../port/port.h"

#define PART_STING_BUFFER_SIZE          256

static char         _parts_string_buffer[PART_STING_BUFFER_SIZE]  ;

ENGINE_EVENT_IMPL  (_state_start,   "State start (always the first event after a transition).") ;

static int32_t      action_nop (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
ENGINE_ACTION_IMPL  (   nop,                        "No Operation") ;

/**
 * @brief   action_nop
 * @param[in] instance      Statemachine instance.
 * @param[in] parm          Parameter.
 * @param[in] flags         Compile, validate and parameter type flag.
 */
int32_t
action_nop (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return ENGINE_OK ;
}




/**
 * @brief   Call all parts.
 * @param[in] cmd    cmd to call for all parts.
 * @return              Error.
 */
int32_t
parts_cmd (PENGINE_T instance, uint32_t cmd)
{
    uint32_t i ;
    uint32_t res  ;

    PART_CMD_T* pcmd = (PART_CMD_T*)&__engine_cmd_base__ ;
    for (i = 0; pcmd < (PART_CMD_T*)&__engine_cmd_end__ ; i++) {

        if (pcmd->fp) {
            engine_log (0, ENGINE_LOG_TYPE_DEBUG, "[dbg] cmd %u part %s",
                    cmd, pcmd->name) ;

            res = pcmd->fp(instance, cmd) ;
            if (res != ENGINE_OK) {
                engine_log (0, ENGINE_LOG_TYPE_ERROR, "[err] cmd %u part %s",
                        cmd, pcmd->name) ;

            }

        }

        pcmd++ ;

    }

     return ENGINE_OK ;
}

/**
 * @brief   get the action for the id.
 * @param[in] action_id    id.
 * @return              PART_ACTION_T.
 */
const PART_ACTION_T*
parts_get_action (uint16_t action_id)
{
    PART_ACTION_T* paction = (PART_ACTION_T*)(void*)&__engine_action_base__ ;

    return &paction[action_id & STATES_ACTION_ID_MASK] ;
}

/**
 * @brief   get the function for the action id.
 * @param[in] action_id    id.
 * @return              PART_ACTION_FP.
 */
PART_ACTION_FP
parts_get_action_fp (uint16_t action_id)
{
    const PART_ACTION_T* action = parts_get_action (action_id) ;
    if (!action) {
        return 0 ;

    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    return action->fp ;
#pragma GCC diagnostic pop
}

/**
 * @brief   get the name of the action id.
 * @note    used for debugging purposes.
 * @param[in] action_id    id.
 * @return              name.
 */
const char*
parts_get_action_name (uint16_t action_id)
{
    const PART_ACTION_T* action = parts_get_action (action_id) ;
    if (action == 0) {
        return 0 ;

    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    return action->name ;
#pragma GCC diagnostic pop
}

/**
 * @brief   get the event for the id.
 * @param[in] event_id    id.
 * @return              PART_EVENT_T.
 */
const PART_EVENT_T*
parts_get_event (uint16_t event_id)
{
    uint32_t id = event_id & STATES_EVENT_ID_MASK ;

    if (id < STATES_EVENT_DECL_START) {
         PART_EVENT_T* pevent = (PART_EVENT_T*)&__engine_event_base__ ;
         pevent = &pevent[id] ;
         return pevent ;

    }

    return 0 ;
}

/**
 * @brief   get the name for the id.
 * @note    used for debugging purposes.
 * @param[in] event_id    id.
 * @return              name.
 */
const char*
parts_get_event_name (uint16_t event_id)
{
    const PART_EVENT_T* event ;

    event = parts_get_event(event_id) ;

    if (event == 0) {
        static char name[16] ;
        snprintf(name, 16, "0x%x", event_id) ;
        return name ;

    } else {
        return event->name ;

    }
}

/**
 * @brief   get the event id for the event name.
 * @note    used for debugging purposes.
 * @param[in] name    event name.
 * @return              id.
 */
int32_t
parts_find_event_id (const char* name)
{
    uint32_t j ;

    PART_EVENT_T* pevent = (PART_EVENT_T*)(void*)&__engine_event_base__;
    PART_EVENT_T* event_end = (PART_EVENT_T*)(void*)&__engine_event_end__;
    size_t num_events = ((uintptr_t)event_end - (uintptr_t)pevent) / sizeof(PART_EVENT_T);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

    for (j = 0; j < num_events ; j++) {

        if (strcmp(pevent[j].name, name) == 0) {
            return j ;
        }

    }

#pragma GCC diagnostic pop

    return ENGINE_FAIL ;
}

/**
 * @brief   For use by part action to validate if the param is a valid string.
 * @param[in] instance      engine instance (from action)
 * @param[in] parm          parameter (from action)
 * @param[in] flags         validate and parameter type flag (from action)
 * @return              ENGINE_OK or ENGINE_FAIL
 */
int32_t
parts_valadate_string (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    #if defined CFG_ENGINE_REGISTRY_ENABLE
    if (flags & PART_ACTION_FLAG_INDEXED) {
            const char * str = engine_get_string (instance, parm, 0) ;
            if (str) {
                if (registry_string_get (str, 0, 0) > 0) {
                    return ENGINE_OK ;

                }
                DBG_ENGINE_LOG (ENGINE_LOG_TYPE_ERROR,
                        "ENG   :W: validation failed for registry string '%s'",
                        str) ;

            }
            return ENGINE_FAIL ;

    }
#endif

    if (flags & PART_ACTION_FLAG_STRING) {
        const char* str = engine_get_string (instance, parm, 0) ;
        if (str) {
            return ENGINE_OK ;

        }

    }

    return ENGINE_FAIL ;
}

/**
 * @brief   For use by part action to validate if the param is a valid int.
 * @param[in] instance      engine instance (from action)
 * @param[in] parm          parameter (from action)
 * @param[in] flags         validate and parameter type flag (from action)
 * @return              ENGINE_OK or ENGINE_FAIL
 */
int32_t
parts_valadate_int (PENGINE_T instance, uint32_t parm, uint32_t flags, int32_t min, int32_t max)
{
    int32_t value ;
#if defined CFG_ENGINE_REGISTRY_ENABLE
    if (flags & PART_ACTION_FLAG_INDEXED) {
        const char * str = engine_get_string (instance, parm, 0) ;
        if (str) {
                if (registry_int32_get(str, &value) < 0) {
                    DBG_ENGINE_LOG (ENGINE_LOG_TYPE_ERROR,
                            "ENG   :W: validation failed for registry value '%s'",
                            str) ;
                    return ENGINE_FAIL ;

                }
                return  ENGINE_OK  ;

        } else {
            return ENGINE_FAIL ;

        }

    }
    else
#endif

    if (flags & PART_ACTION_FLAG_VARIABLE) {
        return ENGINE_OK ;

    }
    else if (flags & PART_ACTION_FLAG_STRING) {
        return ENGINE_FAIL ;


    } else {
        value = (int16_t) parm ;

    }

    return ((int32_t)value >= min) && ((int32_t)value <= max) ? ENGINE_OK : ENGINE_FAIL ;
}

const char*
parts_copy_string(PENGINE_T instance, uint32_t parm, uint32_t flags, char* buffer, int len)
{
    buffer[0] = '\0' ;
    #if defined CFG_ENGINE_REGISTRY_ENABLE
    if (flags & PART_ACTION_FLAG_INDEXED) {
        const char * str = engine_get_string (instance, parm, 0) ;
        if (str) {
            int32_t res = registry_string_get (str, (char*)buffer, len - 2) ;
            if (res > 0) {
                buffer[len-1] = '\0' ;

            }

        }

    }
    else
#endif
    if (flags & PART_ACTION_FLAG_STRING) {
        const char* str = engine_get_string (instance, parm, 0) ;
        if (str) {
            strncpy(buffer, str, len-2) ;
            buffer[len-1] = '\0' ;

        }

    }

    return buffer ;

}

const char*
parts_get_string(PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    return parts_copy_string (instance, parm, flags, _parts_string_buffer, sizeof(_parts_string_buffer)) ;

}


int32_t
parts_get_int(PENGINE_T instance, uint32_t parm, uint32_t flags, int32_t min, int32_t max)
{
    int32_t value = 0 ;
    #if defined CFG_ENGINE_REGISTRY_ENABLE
    if (flags & PART_ACTION_FLAG_INDEXED) {
        const char * str = engine_get_string (instance, parm, 0) ;
        if (str) {
            registry_int32_get (str, &value) ;

        }

    }
    else
#endif
    if (flags & PART_ACTION_FLAG_VARIABLE) {
        value =  parm ;

    }
    else {
        value =  (int16_t) parm ;

    }


    if ((int32_t)value > max) value = max ;
    else if ((int32_t)value < min) value = min ;

    return (int32_t)value ;
}




