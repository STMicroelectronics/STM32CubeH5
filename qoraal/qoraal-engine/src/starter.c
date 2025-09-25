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
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "qoraal-engine/starter.h"
#include "qoraal-engine/engine.h"
#include "qoraal-engine/parts/parts.h"
#include "tool/machine.h"
#include "tool/parse.h"
#include "port/port.h"


static STARTER_OUT_FP                       _starter_log = 0 ;
static void *                               _starter_log_ctx = 0 ;
static uint32_t                             _starter_loaded_size = 0 ;


/*===========================================================================*/
/* Parser callback interface functions.                                      */
/*===========================================================================*/

static int
statemachine (STATEMACHINE_T* statemachine)
{
    if (statemachine) {
        _starter_loaded_size += statemachine->size ;
        return engine_add_statemachine (statemachine) ;

    }

    return ENGINE_FAIL ;
}

static int
stringtable(STRINGTABLE_T* stringtable)
{
    if (stringtable) {
        _starter_loaded_size += stringtable->size ;
        return engine_set_stringtable (stringtable) ;

    }

    return ENGINE_FAIL ;
}

static void
version (int version)
{
    engine_set_version (version) ;
}

static void
name (char* name)
{
    engine_set_name (name) ;
}

/*===========================================================================*/
/* Parser logging interface functions.                                       */
/*===========================================================================*/

static void
_vlog (char* fmt_str , va_list args) {
    if (_starter_log) {
        char buffer[196] ;
        vsnprintf(buffer, 196, fmt_str, args) ;
        _starter_log (_starter_log_ctx, STARTER_OUT_OUT_STD, buffer) ;
    }
}

static void
parser_log (char* message, ...)
{
    va_list args;
    va_start(args, message);
    _vlog (message, args) ;
    va_end (args) ;
}

static void
parser_report (char* message, ...)
{
    va_list args;
    va_start(args, message);
    _vlog (message, args) ;
    va_end (args) ;
}

static void
parser_error (char* message, ...)
{
    va_list args;
    va_start(args, message);
    _vlog(message, args) ;
    va_end (args) ;
}


/**
 * @brief       Initialises parser by adding all the actions, events and constatns
 * @return      status
 */
static int32_t
starter_parser_init (void)
{
    uint32_t j ;

     PART_ACTION_T* paction = (PART_ACTION_T*)&__engine_action_base__ ;
     for (j = 0; paction < (PART_ACTION_T*)&__engine_action_end__ ; j++) {

        if (!ParserAddAction (paction->name, j)) {
            DBG_ENGINE_ASSERT (0 , "ENG  :E: fail adding action!") ;

        }
        paction++ ;

     }

     PART_EVENT_T* pevent = (PART_EVENT_T*)&__engine_event_base__ ;
     for (j = 0; pevent < (PART_EVENT_T*)&__engine_event_end__ ; j++) {

        if (!ParserAddEvent (pevent->name, j)) {
            DBG_ENGINE_ASSERT (0 , "ENG  :E: fail adding event!") ;

        }
        pevent++ ;

     }


     PART_CONST_T* pconst = (PART_CONST_T*)&__engine_const_base__ ;
     for (j = 0; pconst < (PART_CONST_T*)&__engine_const_end__ ; j++) {

        if (!ParserAddConst (pconst->name, pconst->id)) {
            DBG_ENGINE_ASSERT (0 , "ENG  :E: fail adding const!") ;

        }
        pconst++ ;

     }

     return ENGINE_OK ;
}


/**
 * @brief       Initialises starter
 * @param[in] arg       port defined argument passed to engine and eventually engine port
 * @return      status
 */
int32_t
starter_init (void * arg)
{
     engine_init (arg) ;

     return ENGINE_OK ;
}

/**
 * @brief       Debug function to list all actions, events and constants exported
 *              to the parser theoug a callback.
 * @param[in] ctx       context for callback function
 * @param[in] list      callback function
 * @return      status
 */
int32_t
starter_parts_list (void* ctx, STARTER_LIST_FP list)
{
    uint32_t j ;
    char buffer[64] ;

    PART_ACTION_T* paction = (PART_ACTION_T*)&__engine_action_base__ ;
     for (j = 0; paction < (PART_ACTION_T*)&__engine_action_end__ ; j++) {

         list (ctx, typeAction, paction->name, paction->desc) ;
        paction++ ;
     }

    PART_EVENT_T* pevent = (PART_EVENT_T*)&__engine_event_base__ ;
     for (j = 0; pevent < (PART_EVENT_T*)&__engine_event_end__ ; j++) {

         list (ctx, typeEvent, pevent->name, pevent->desc) ;
        pevent++ ;

     }

     PART_CONST_T* pconst = (PART_CONST_T*)&__engine_const_base__ ;
     for (j = 0; pconst < (PART_CONST_T*)&__engine_const_end__ ; j++) {

        snprintf(buffer, 64, "%s (%d)", pconst->name, (int)pconst->id) ;
        list (ctx, typeConstant, buffer, pconst->desc) ;
        pconst++ ;

     }

     return ENGINE_OK ;
}

/**
 * @brief       Compile and load all statemachines emitted by the parser into the engine..
 * @param[in] buffer        Engine machine language format for all the statemachines to compile.
 * @param[in] length        length of buffer.
 * @return      status
 */
static int32_t
_starter_compile (const char* buffer, uint32_t length, void* ctx,
        STARTER_OUT_FP log, bool verbose)
{
#if STARTER_DEBUG_MEM
    uint32_t mem_used, mem_avail ;
#endif

    int32_t result  = ENGINE_FAIL ;
    uint32_t timer = engine_timestamp() ;

    PARSE_CB_IF parser_cb = {
        statemachine,
        stringtable,
        version,
        name,

    } ;

    PARSE_LOG_IF log_cb = {
        parser_log,
        parser_report,
        parser_error

    } ;

    _starter_log_ctx = ctx ;
    _starter_log = log ;

    if (!verbose) {
        log_cb.Log = 0 ;

    }

    ParseInit () ;
    starter_parser_init () ;

    _starter_loaded_size = 0 ;

    if (ParseAnalyse (buffer, length, &parser_cb, &log_cb)) {

        ParseComplete (&parser_cb, &log_cb) ;

        log_cb.Log = 0 ;

        const STRINGTABLE_T* stringtable = engine_get_stringtable () ;
        if (machine_stringtable_validate (stringtable, &log_cb) != ENGINE_OK) {
            ParseDestroy ();
            starter_stop () ;
            return ENGINE_FAIL ;

        }


         int idx = 0 ;
         const STATEMACHINE_T* statemachine ;
         for (statemachine = engine_get_statemachine (idx++) ; statemachine; ) {

                if (machine_validate (statemachine, stringtable, &log_cb) != ENGINE_OK) {
                     ParseDestroy ();
                     starter_stop () ;
                     return ENGINE_FAIL ;

                }

                statemachine = engine_get_statemachine (idx++) ;

         }

         timer = engine_timestamp() - timer ;
         parser_report ("'%s' v%d compiled %d bytes in %d.%03d seconds!\r\n",
                    engine_get_name(), engine_get_version(),
                    _starter_loaded_size, timer/1000, timer%1000 ) ;

         result = ENGINE_OK ;

    } else {
        parser_error ("STATEMACHINE validation FAIL!!!\r\n") ;

    }

    ParseDestroy ();

    if (result != ENGINE_OK) {
        starter_stop () ;

    }

#if STARTER_DEBUG_MEM
     heap_stats (HEAP_SPACE_EXTERNAL, &mem_used, &mem_avail) ;
     engine_port_log (0,
            "STATEMACHINE PARSING mem usegae stop: %.6d / %.6d\r\n",
            mem_used, mem_avail) ;
#endif
     return result ;

}


/**
 * @brief       Compile the input buffer and start the Engine for the loaded statemachines.
 * @note        Additional callback for debug purposes showing compiler progress and status.
 * @param[in] buffer        Engine machine language format for all the statemachines to compile.
 * @param[in] length        length of buffer.
 * @param[in] ctx           context for callback function
 * @param[in] log           callback function
 * @param[in] verbose
 * @return      status
 */
int32_t
starter_start (const char* buffer, uint32_t length,
        void* ctx, STARTER_OUT_FP log, bool verbose)
{
    int32_t result = _starter_compile(buffer, length, ctx, log, verbose) ;

    if (result == ENGINE_OK) {
        result = engine_start () ;

    }

    return result ;

}

/**
 * @brief       Only compile the input buffer.
 * @note        For testing and debug purposes.
 * @param[in] buffer        Engine machine language format for all the statemachines to compile.
 * @param[in] length        length of buffer.
 * @param[in] ctx           context for callback function
 * @param[in] log           callback function
 * @param[in] verbose
 * @return      status
 */
int32_t
starter_compile (const char* buffer, uint32_t length, void* ctx,
        STARTER_OUT_FP log, bool verbose)
{
    int32_t result  = _starter_compile (buffer, length, ctx, log, verbose) ;
    starter_stop () ;

    return result ;

}

/**
 * @brief       Stop the engine and free all allocated resources.
 * @return      status
 */
int32_t
starter_stop (void)
{
    int i  ;
    const STATEMACHINE_T* statemachine ;
    STRINGTABLE_T* stringtable ;

    engine_stop () ;

    for (i=0; i < ENGINE_MAX_INSTANCES; i++) {
        statemachine = engine_remove_statemachine (i) ;
        if (statemachine) {
            machine_destroy (statemachine) ;

        }
    }

    stringtable = (STRINGTABLE_T*)engine_remove_stringtable () ;
    if (stringtable) {
        machine_stringtable_destroy (stringtable) ;

    }

    engine_log_mem_usage () ;

    return ENGINE_OK ;
}



