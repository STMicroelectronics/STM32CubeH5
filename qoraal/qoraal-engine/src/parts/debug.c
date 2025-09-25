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
#if !defined CFG_ENGINE_DEBUG_PART_DISABLE
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include "qoraal-engine/parts/parts.h"
#include "qoraal-engine/engine.h"



/*===========================================================================*/
/* part local functions.                                                */
/*===========================================================================*/
static int32_t      part_debug_cmd (PENGINE_T instance, uint32_t start) ;
static int32_t      action_debug_log_level (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_debug_log_output (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_debug_log_statemachine (PENGINE_T instance, uint32_t parm, uint32_t flags) ;
static int32_t      action_debug_dump (PENGINE_T instance, uint32_t parm, uint32_t flags) ;

/**
 * @brief   Initializes actions for part
 *
 */
ENGINE_ACTION_IMPL  (debug_log_level,           "Set the log level. To be used with the LOG_XXX constants.") ;
ENGINE_ACTION_IMPL  (debug_log_output,          "ENABLE/DISABLE output logging") ;
ENGINE_ACTION_IMPL  (debug_log_statemachine,    "A space separated list of statemachine to enable logging for.") ;
ENGINE_ACTION_IMPL  (debug_dump,                "Dump the current state of all statemachines.") ;

/**
 * @brief   Initializes events for part
 *
 */
//ENGINE_EVENT_IMPL     ( _debug_xxx,               "Debug event") ;

/**
 * @brief   Initializes constants for part
 *
 */
ENGINE_CONST_IMPL(0,                                LOG_NONE,           "NO logging");
ENGINE_CONST_IMPL(ENGINE_LOG_FILTER_ALL,            LOG_ALL,            "Log level ALL");
ENGINE_CONST_IMPL(ENGINE_LOG_FILTER_TRANSITIONS,    LOG_TRANSITIONS,    "Log TRANSITIONS only");
ENGINE_CONST_IMPL(ENGINE_LOG_FILTER_REPORT,         LOG_REPORT,         "Log level REPORT");

/**
 * @brief   part declaration.
 *
 */
ENGINE_CMD_FP_IMPL (part_debug_cmd) ;



/**
 * @brief   part_debug_cmd
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
part_debug_cmd (PENGINE_T instance, uint32_t start)
{
    (void)start ;
    return ENGINE_OK ;
}

/**
 * @brief   set the log level
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_debug_log_level (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX);
    }

    parm = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX);
    engine_logfilter (parm, ENGINE_LOG_FILTER_ALL | ENGINE_LOG_FILTER_TRANSITIONS | ENGINE_LOG_FILTER_REPORT) ;

    return ENGINE_OK ;
}

/**
 * @brief   enable/disable output logging.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_debug_log_output (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_int (instance, parm, flags, INT_MIN, INT_MAX);
    }
    parm = parts_get_int (instance, parm, flags, INT_MIN, INT_MAX);
    if (parm) {
        engine_logfilter (ENGINE_LOG_TYPE_PARTS, 0) ;
    } else {
        engine_logfilter (0, ENGINE_LOG_TYPE_PARTS) ;

    }

    return ENGINE_OK ;
}


/**
 * @brief   enable logging for statemachines. statemachines to enable in space
 *          seperated string in parm.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_debug_log_statemachine (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    const char * str ;
    int i, j ;
    char name[STATEMACHINE_NAME_SIZE]  ;
    int32_t res ;

    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return parts_valadate_string (instance, parm, flags) ;
    }

    str = parts_get_string(instance, parm, flags) ;

    engine_loginstance (0, ~0) ;

    if (str) {
        for (i=0, j=0; str[j] && (j<STATEMACHINE_NAME_SIZE); i++,j++) {
            if ((str[j] == ' ') || (str[j] == '\t')) {
                name[i] = 0 ;
                res = engine_statemachine_idx (name) ;
                if (res >= 0) {
                    engine_loginstance ((1<<res), 0) ;

                }
                i=0;

            } else {
                name[i] = str[j] ;

            }
        }

        name[i] = 0 ;
        res = engine_statemachine_idx (name) ;
        if (res >= 0) {
            engine_loginstance ((1<<res), 0) ;

        }

    }

    return ENGINE_OK ;
}

/**
 * @brief   dump state off all loaded statemachine to the debug output.
 * @param[in] instance      engine instance.
 * @param[in] parm          parameter.
 * @param[in] flags         validate and parameter type flag.
 */
int32_t
action_debug_dump (PENGINE_T instance, uint32_t parm, uint32_t flags)
{
    if (flags & (PART_ACTION_FLAG_VALIDATE)) {
        return ENGINE_OK;
    }

    engine_dump (false) ;

    return ENGINE_OK ;
}

#endif /* CFG_ENGINE_DEBUG_PART_DISABLE */
