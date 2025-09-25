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
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"


/*===========================================================================*/
/* part local functions.                                                */
/*===========================================================================*/
static int32_t      part_toaster_cmd (PENGINE_T instance, uint32_t start) ;
static int32_t      action_toaster_heater (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_toaster_lamp (PENGINE_T instance, uint32_t parm, uint32_t flags) ;

/**
 * @brief   Initializes actions for part
 *
 */
ENGINE_ACTION_IMPL  (toaster_heater,        "Turn the heater ON/OFF.") ;
ENGINE_ACTION_IMPL  (toaster_lamp,          "Turn the lamp ON/OFF.") ;

/**
 * @brief   Initialises events for part
 *
 */
ENGINE_EVENT_IMPL   ( _toaster_smoke_alert,     "Toaster smoke alert event.") ;

/**
 * @brief   Initialises constants for part
 *
 */
ENGINE_CONST_IMPL(1, ON,                    "Turn heater/lamp ON");
ENGINE_CONST_IMPL(0, OFF,                   "Turn heater/lamp OFF");


/**
 * @brief   part declaration.
 *
 */
ENGINE_CMD_FP_IMPL (part_toaster_cmd) ;


/**
 * @brief   part_toaster_cmd
 * @param[in] instance      Statemachine instance.
 * @param[in] start         Start/stop.
 */
int32_t
part_toaster_cmd (PENGINE_T instance, uint32_t start)
{
    (void)start ;
     return ENGINE_OK ;
}

/**
 * @brief   turn the toaster heater on or off
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_toaster_heater (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, 1) ;

    }
    parm = parts_get_int (instance, parm, flags, 0, 1) ;
    engine_log (instance, ENGINE_LOG_TYPE_PARTS,
            "#################### HEATER %s\r\n", parm ? "ON" : "OFF") ;

    return ENGINE_OK ;
}

/**
 * @brief   turn the toaster lamp on or off
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_toaster_lamp (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, 0, 1) ;

    }
    parm = parts_get_int (instance, parm, flags, 0, 1) ;
    engine_log (instance, ENGINE_LOG_TYPE_PARTS,
            "#################### LAMP    %s\r\n", parm ? "ON" : "OFF") ;

    return ENGINE_OK ;
}

