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
#if !defined CFG_ENGINE_CONSOLE_PART_DISABLE
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"
#include <qoraal/qoraal.h>
#if !defined CFG_ENGINE_STRSUB_DISABLE
#include "qoraal/common/strsub.h"
#endif


/*===========================================================================*/
/* part local functions.                                                */
/*===========================================================================*/
static int32_t      part_console_cmd (PENGINE_T instance, uint32_t start) ;
static int32_t      action_console_events_register (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_console_write (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_console_writeln (PENGINE_T instance, uint32_t parm, uint32_t flags) ;

/**
 * @brief   Initializes actions for part
 *
 */
ENGINE_ACTION_IMPL  (console_events_register,   "Register \"statemachine\" to receive console events") ;
ENGINE_ACTION_IMPL  (console_write,             "Write a 'character' the console.") ;
ENGINE_ACTION_IMPL  (console_writeln,           "Write a \"line\" to the console.") ;

/**
 * @brief   Initializes events for part
 *
 */
ENGINE_EVENT_IMPL   (_console_char,             "A character was received in [e].") ;

/**
 * @brief   Initializes constants for part
 *
 */
//ENGINE_CONST_IMPL(1, CONSOLE_1, "Test");

/**
 * @brief   part declaration.
 *
 */
ENGINE_CMD_FP_IMPL (part_console_cmd) ;


int32_t     _console_event_mask = 0 ;


/**
 * @brief   part_console_cmd
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
part_console_cmd (PENGINE_T instance, uint32_t start)
{
    (void)start ;
    _console_event_mask = 0 ;
    return ENGINE_OK ;
}


static void
console_out(const char * str)
{
    qoraal_debug_print (str) ;
}

/**
 * @brief   register the engine instance for events dispatched with
 *          engine_console_event()
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_console_events_register (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK;
    }

    if (parm) {
        _console_event_mask |= engine_get_mask (instance) ;
    } else {
        _console_event_mask &= ~engine_get_mask (instance) ;

    }

    return ENGINE_OK ;
}

/**
 * @brief   write parm string to the console
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_console_write (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    const char* str = 0 ;

    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_string (instance, parm, flags) ;
    }
    str = parts_get_string(instance, parm, flags) ;

    if (str) {

#if !defined CFG_ENGINE_STRSUB_DISABLE
        char buffer[96] ;
        strsub_parse_string_to (0, str, strlen(str), buffer, 96) ;
        console_out (buffer) ;
#else
        console_out (str) ;
#endif

    }

    return ENGINE_OK ;
}

/**
 * @brief   write parm string as well as newline to the console
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_console_writeln (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    int32_t res = action_console_write (instance, parm, flags) ;
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return res ;
    }

    console_out ("\r\n") ;

    return res ;
}


/**
 * @brief   dispatch a event to all  engine instances registered for console
 *          with action_console_events_register().
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
engine_console_event (uint16_t event, uint32_t ch)
{
    int32_t status = engine_queue_masked_event (_console_event_mask, event, ch) ;

    return status ;
}



#endif /* CFG_ENGINE_CONSOLE_PART_DISABLE */