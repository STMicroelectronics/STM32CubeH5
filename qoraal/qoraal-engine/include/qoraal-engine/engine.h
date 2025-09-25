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


#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "qoraal/qoraal.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Module macros.                                                            */
/*===========================================================================*/

#define DBG_ENGINE_LOG(type, fmt_str, ...)              engine_log(0, type, fmt_str, ##__VA_ARGS__ )
#define DBG_ENGINE_CHECK(cond, ret, fmt_str, ...)       {if (!(cond)) {  engine_log(0, ENGINE_LOG_TYPE_VERBOSE, fmt_str, ##__VA_ARGS__ ) ; return ret ; }}
#define DBG_ENGINE_ASSERT(cond, msg)                    {if (!(cond)) {  qoraal_debug_assert(msg) ; }}

/*===========================================================================*/
/* Error Codes                                                                 */
/*===========================================================================*/

#define ENGINE_OK                           0
#define ENGINE_FAIL                         -1
#define ENGINE_NOTFOUND                     -2
#define ENGINE_PARM                         -3
#define ENGINE_NOT_IMPL                     -6
#define ENGINE_NOMEM                        -8

/*===========================================================================*/
/* Engine Configuration                                                      */
/*===========================================================================*/

/**
 * Maximum number of Engine instances (statemachines).
 *
 * Default: 20
 */
#ifndef ENGINE_MAX_INSTANCES
#define ENGINE_MAX_INSTANCES                20
#endif

/**
 * Engine name size used for logging and debugging purposes.
 *
 * Default: 48
 */
#ifndef ENGINE_NAME_SIZE
#define ENGINE_NAME_SIZE                    48
#endif

/**
 * Statemachine name size used for logging and debugging purposes.
 *
 * Default: 32
 */
#ifndef STATEMACHINE_NAME_SIZE
#define STATEMACHINE_NAME_SIZE              24
#endif

/**
 * State name size used for logging and debugging purposes.
 *
 * Default: 24
 */
#ifndef STATEMACHINE_STATE_NAME_SIZE
#define STATEMACHINE_STATE_NAME_SIZE        24
#endif

/**
 * Maximum number of deferred events to be saved for a state..
 *
 * Default: 8
 */
#ifndef STATEMACHINE_DEFERRED_MAX
#define STATEMACHINE_DEFERRED_MAX           8
#endif

/**
 * Maximum number of super states allowed.
 *
 * Default: 8
 */
#ifndef STATEMACHINE_SUPER_STATE_MAX
#define STATEMACHINE_SUPER_STATE_MAX        12
#endif

/**
 * Maximum number of engine instance local variables.
 *
 * Default: 5
 */
#ifndef ENGINE_REGISTER_COUNT
#define ENGINE_REGISTER_COUNT               5
#endif

/**
 * Depth of previous-state stack.
 *
 * Default: 8
 */
#ifndef ENGINE_PREVIOUS_STACK
#define ENGINE_PREVIOUS_STACK               12
#endif

/**
 * Depth of accumulator-stack.
 *
 * Default: 4
 */
#ifndef ENGINE_ACCUMULATOR_STACK
#define ENGINE_ACCUMULATOR_STACK            8
#endif


/*===========================================================================*/
/* Constants                                                                 */
/*===========================================================================*/

#define ENGINE_VARIABLE_ACCUMULATOR         0
#define ENGINE_VARIABLE_REGISTER            1
#define ENGINE_VARIABLE_PARAMETER           2
#define ENGINE_VARIABLE_EVENT               3


#define ENGINE_LOG_TYPE_VERBOSE             (0xFFFF)
#define ENGINE_LOG_TYPE_DEBUG               (1 << 0)
#define ENGINE_LOG_TYPE_ERROR               (1 << 1)
#define ENGINE_LOG_TYPE_REPORT              (1 << 2)
#define ENGINE_LOG_TYPE_LOG                 (1 << 3)
#define ENGINE_LOG_TYPE_ENTRY_FUNCTIONS     (1 << 4)
#define ENGINE_LOG_TYPE_EXIT_FUNCTIONS      (1 << 5)
#define ENGINE_LOG_TYPE_TRANSITIONS         (1 << 6)
#define ENGINE_LOG_TYPE_EVENTS              (1 << 7)
#define ENGINE_LOG_TYPE_INIT                (1 << 8)
#define ENGINE_LOG_TYPE_VALIDATE            (1 << 9)
#define ENGINE_LOG_TYPE_ACTION              (1 << 10)
#define ENGINE_LOG_TYPE_PARTS               (1 << 11)
#define ENGINE_LOG_TYPE_PORT                (1 << 15)


#define ENGINE_LOG_FILTER_ALL               ( \
                                            ENGINE_LOG_TYPE_ERROR | \
                                            ENGINE_LOG_TYPE_ENTRY_FUNCTIONS | \
                                            ENGINE_LOG_TYPE_EXIT_FUNCTIONS | \
                                            ENGINE_LOG_TYPE_TRANSITIONS | \
                                            ENGINE_LOG_TYPE_EVENTS |  \
                                            ENGINE_LOG_TYPE_LOG | \
                                            ENGINE_LOG_TYPE_INIT| \
                                            ENGINE_LOG_TYPE_VALIDATE| \
                                            ENGINE_LOG_TYPE_ACTION | \
                                            ENGINE_LOG_FILTER_REPORT \
                                            /*  | ENGINE_LOG_TYPE_PORT */ \
                                            )
#define ENGINE_LOG_FILTER_REPORT            ( \
                                            ENGINE_LOG_TYPE_ERROR | \
                                            ENGINE_LOG_TYPE_REPORT \
                                            )
#define ENGINE_LOG_FILTER_TRANSITIONS       ( \
                                            ENGINE_LOG_TYPE_ERROR | \
                                            ENGINE_LOG_TYPE_REPORT | \
                                            ENGINE_LOG_TYPE_TRANSITIONS \
                                            )

#ifdef NDEBUG
#define ENGINE_LOG_FILTER_DEFAULT           0
#define ENGINE_LOG_FILTER_ALWAYS            0
#else
#define ENGINE_LOG_FILTER_DEFAULT           ENGINE_LOG_FILTER_REPORT
#define ENGINE_LOG_FILTER_ALWAYS            ENGINE_LOG_TYPE_ERROR
#endif


#define ENGINE_INSTANCE_LOG_MASK(name)      (1<<engine_statemachine_idx(name))


#define STATEMACHINE_MAGIC                  0x1304

#define STATEMACHINE_INVALID_STATE          ((uint16_t)-1)
#define STATEMACHINE_PREVIOUS_STATE         ((uint16_t)-2)
#define STATEMACHINE_CURRENT_STATE          ((uint16_t)-3)
#define STATEMACHINE_IGNORE_STATE           ((uint16_t)-4)

#define STATEMACHINE_STATE_START            ENGINE_EVENT_ID_GET(_state_start)

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef struct ENGINE_S * PENGINE_T ;

 /**
  * Parts can register this callback to receive callback for every transition that occurs.
  */
typedef void (*TRANSITION_HANDLER) (PENGINE_T engine, uint16_t next_idx, int16_t cond) ;

typedef struct TRANSITION_HANDLER_S {
    struct TRANSITION_HANDLER_S *   next ;
    TRANSITION_HANDLER          handler ;

} TRANSITION_HANDLER_T ;

/**
 * A union presenting both /ref STATES_EVENT_T and /ref STATES_EVENT_T in the data array of /ref STATEMACHINE_STATE_T
 */
#pragma pack(1)
typedef struct STATE_DATA_S {
    union {
        uint32_t                value ;
        struct {
            uint16_t            id ;            /**< action or event */
            uint16_t            param ;         /**< action param or next_state_idx or comparator */
        } ;
     } ;

} STATE_DATA_T ;
#pragma pack()

/**
 * A structure representing a state in the state machine
 */
#pragma pack(1)
typedef struct STATEMACHINE_STATE_S {
    uint16_t                    size;
    uint16_t                    magic;
    /*@{*/
    uint8_t                     name[STATEMACHINE_STATE_NAME_SIZE] ;      /**< a name for the state showed in the trace */
    /*@}*/
   /**
    * @name  Indexes
    */
    /*@{*/
    uint16_t                    idx ;           /**< index of this state as referred to by the state machine. This is also the index in the lookup table. */
    /*@}*/
   /**
    * @name  Offsets into the data array of this state for entry and exit actions and so on
    */
    /*@{*/
    uint16_t                    def_idx ;       /**< default state index for this state */
    uint16_t                    super_idx ;     /**< super state index for this state */
    uint16_t                    reserved1 ;

    uint8_t                     events ;        /**< events count /ref STATES_EVENT_T starts */
    uint8_t                     deferred ;      /**< deferred events count /ref STATES_EVENT_T starts */
    uint8_t                     entry ;         /**< entry actions count/ref STATES_ACTION_T starts */
    uint8_t                     exit ;          /**< exit actions count/ref STATES_ACTION_T starts */
    uint8_t                     action ;        /**< actions count/ref STATES_INTERNAL_T starts */
    uint8_t                     reserved2 ;
   /*@}*/
   /**
    * @name  data of state event, deferred events, entry, exit, and actions.
    */
    /*@{*/
    STATE_DATA_T                data[] ;        /**< total number of entries in data determined when created */
    /*@}*/
} STATEMACHINE_STATE_T ;
#pragma pack()


/**
 * A structure to represent a transition, the event that will trigger the transition to the next state.
 */
#pragma pack(1)
typedef struct STATES_EVENT_S {
    /*@{*/
    uint16_t                    event ;          /**< event that will trigger the transition or a n action*/
    uint16_t                    next_state_idx  ;   /**< the state to transition to, offset into the states defined inn /ref STATEMACHINE_T */
    /*@}*/
}   STATES_EVENT_T ;
#pragma pack()

#define STATES_EVENT_ID_MASK                0x07FF
#define STATES_EVENT_DECL_START             (STATES_EVENT_ID_MASK - 0xFF)
/**
 * Flag mask for events_id
 */
#define STATES_EVENT_COND_MASK              0x7000
#define STATES_EVENT_COND_OFFSET            12
/**
 * Flags for event transitions
 */
#define STATES_EVENT_COND_IF                1
#define STATES_EVENT_COND_NOT               2
#define STATES_EVENT_COND_IF_R              3
#define STATES_EVENT_COND_NOT_R             4
/**
 * Flag to pin the previous state
 */
#define STATES_EVENT_PREVIOUS_PIN           (1 << 15)

/**
 * A structure to represent entry or exit action in a state. THis structure is
 * also used for the action of an internal transition in a state.
 */
#pragma pack(1)
typedef struct STATES_ACTION_S {
    /*@{*/
    uint16_t                    action ;     /**< this is the offset into the action call table for the state machine */
    uint16_t                    param ;      /**< the parameter passed to the action for this particular action in the state machine */
    /*@}*/
}   STATES_ACTION_T ;
#pragma pack()

/**
 * Flag to indicate condition is a variable (as opposed to a constant)
 * (only applicable to actions).
 */
#define STATES_ACTION_ID_MASK               0x0FFF      /**< index into function table */
/**
 * task to perform before or after the action.
 */
#define STATES_ACTION_RESULT_MASK           0x3000      /**< result operator mask */
#define STATES_ACTION_RESULT_OFFSET         12          /**< result operator offset */
#define STATES_ACTION_RESULT_PUSH           1           /**< operator: push accumulator then save result in the accumulator */
#define STATES_ACTION_RESULT_POP            2           /**< operator: pop accumulator before action is executed */
#define STATES_ACTION_RESULT_SAVE           3           /**< operator: save result in register */
/**
 * parameter type for the action.
 */
#define STATES_ACTION_TYPE_MASK             0xC000      /**< param type mask */
#define STATES_ACTION_TYPE_OFFSET           14          /**< param type offset */
#define STATES_ACTION_TYPE_INDEXED          1           /**< index for lookup in registry  */
#define STATES_ACTION_TYPE_STRING           2           /**< index for lookup in string table  */
#define STATES_ACTION_TYPE_VARIABLE         3           /**< param is variable */

/**
 * A structure to represent a internal transition / action in a state
 */
#pragma pack(1)
typedef struct STATES_INTERNAL_S {
    /*@{*/
    uint16_t                    event ;     /**< event that will trigger the transition */
    uint16_t                    comp  ;     /**< Value used in comparison with accumulator */
    STATES_ACTION_T             action ;
    /*@}*/
}   STATES_INTERNAL_T ;
#pragma pack()

/**
 * Flags for event actions
 */
#define STATES_INTERNAL_EVENT_ID_MASK       0x07FF
/**
 * Flag to indicate condition is a variable (as opposed to a constant)
 * (only applicable to actions).
 */
#define STATES_INTERNAL_EVENT_TERMINATE     (1 << 11)

/**
 * Condition to be true for the comparator for action to be executed
 */
/**
 * Flag mask for events_id
 */
#define STATES_INTERNAL_EVENT_COMP_MASK     0x7000
#define STATES_INTERNAL_EVENT_COMP_OFFSET   12

#define STATES_INTERNAL_EVENT_COMP_E_EQ     1   /**< compare the comparator to the event register */
#define STATES_INTERNAL_EVENT_COMP_LT       2
#define STATES_INTERNAL_EVENT_COMP_GT       3
#define STATES_INTERNAL_EVENT_COMP_EQ       4
#define STATES_INTERNAL_EVENT_COMP_NE       5
#define STATES_INTERNAL_EVENT_COMP_LOAD     6

/**
 * Flag indicating the condition is a variable (not a constant)
 */
#define STATES_EVENT_COND_ACTION_VARIABLE   (1 << 15)



/**
 * A structure to represent a string table entry
 */
#pragma pack(1)
 typedef struct STATEMACHINE_STRING_S {

     uint16_t                   len ;
     uint16_t                   id ;
     uint8_t                    value[] ;

 } STATEMACHINE_STRING_T;
 #pragma pack()


/**
 * A structure to store the stringtable
 */
 #pragma pack(1)
 typedef struct STRINGTABLE_S {

    uint16_t                    size;                   /**< total size including header */
    uint16_t                    magic;
    uint16_t                    flags ;                 /**< creator flags for example the parser */
    uint16_t                    count ;
    STATEMACHINE_STRING_T *     strings_offset[] ;      /**< pointer to array of pointers to states in this state machine.
                                                            All indexes to states are offsets into this array */
 } STRINGTABLE_T;
 #pragma pack()


#define GET_STATEMACHINE_STRINGTABLE_REF(stringtable, string_idx)  \
    ((STATEMACHINE_STRING_T*) ((uintptr_t)stringtable + (uintptr_t)stringtable->strings_offset[string_idx]))

#define SET_STATEMACHINE_STRINGTABLE(stringtable, string_idx, string)  \
    do { stringtable->strings_offset[string_idx] = (STATEMACHINE_STRING_T *) ((uintptr_t)string - (uintptr_t)stringtable) ; } while(0)


/**
 * A structure to represent a State machine
 */
#pragma pack(1)
typedef struct STATEMACHINE_S {
    /*@{*/
    uint16_t                    size;               /**< total size including header */
    uint16_t                    magic;
    uint32_t                    flags ;             /**< creator flags for example the parser */
    uint32_t                    version;            /**< the version number for the state machine defined by this definition */
    uint8_t                     name[STATEMACHINE_NAME_SIZE] ;      /**< name for the statemachine  */
    uint16_t                    start_idx ;         /**< Start index for the state machine */
    uint16_t                    count ;             /**< number of states /ref STATEMACHINE_STATE_T defined in this state machine */
    STATEMACHINE_STATE_T *      states_offset[] ;   /**< pointer to array of pointers to states in this state machine. All indexes to states are offsets into this array */
     /*@}*/
} STATEMACHINE_T ;
#pragma pack()

#define GET_STATEMACHINE_STATE_REF(statemachine, state_idx)  \
    ((STATEMACHINE_STATE_T*) ((uintptr_t)statemachine + (uintptr_t)statemachine->states_offset[state_idx]))

#define SET_STATEMACHINE_STATE(statemachine, state_idx, state)  \
    do { statemachine->states_offset[state_idx] =  (STATEMACHINE_STATE_T *) ((uintptr_t)state   -  (uintptr_t)statemachine) ; } while(0)

//#include "port/port.h"

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Functions used to create and manage state machines
     */
    int32_t                 engine_init (void * arg) ;
    int32_t                 engine_add_statemachine (const STATEMACHINE_T *statemachine) ;
    const STATEMACHINE_T*   engine_remove_statemachine (int idx) ;
    const STATEMACHINE_T*   engine_get_statemachine (int idx) ;
    int32_t                 engine_set_stringtable (const STRINGTABLE_T * stringtable) ;
    const STRINGTABLE_T*    engine_remove_stringtable (void) ;
    const STRINGTABLE_T*    engine_get_stringtable (void) ;
    int32_t                 engine_set_version (int32_t version) ;
    int32_t                 engine_set_name (const char * name) ;
    int32_t                 engine_init_variables (uint32_t count) ;
    int32_t                 engine_start (void) ;
    int32_t                 engine_stop (void) ;
    uint32_t                engine_is_started (void) ;
    int32_t                 engine_get_version (void);
    const char*             engine_get_name (void);
    uint32_t                engine_statemachine_count (void) ;
    const char*             engine_statemachine_name (uint32_t idx) ;

    /*
     * Logging
     */
    uint32_t                engine_logfilter (uint16_t set, uint16_t clear) ;
    uint32_t                engine_loginstance (uint32_t set, uint32_t clear) ;
    bool                    engine_would_log (PENGINE_T engine, uint32_t type) ;
    void                    engine_log (PENGINE_T engine, uint32_t type, const char* fmt_str, ...) ;

    /*
     * Functions used from actions/functions
     */
    const char*             engine_get_string (PENGINE_T engine, uint16_t idx, uint16_t * len);
    int32_t                 engine_instance_idx (PENGINE_T engine);
    void                    engine_add_transition_handler (PENGINE_T engine, TRANSITION_HANDLER_T * handler);
    void                    engine_remove_transition_handler (PENGINE_T engine, TRANSITION_HANDLER_T * handler) ;
    int32_t                 engine_get_variable (PENGINE_T engine, uint32_t var, int32_t * val) ;
    int32_t                 engine_set_variable (PENGINE_T engine, uint32_t var, int32_t val) ;
    int32_t                 engine_pop (PENGINE_T engine) ;
    int32_t                 engine_push (PENGINE_T engine, int32_t value) ;
    int32_t                 engine_swap (PENGINE_T engine) ;

    /*
     * Event generation
     */
    uint32_t                engine_get_mask (PENGINE_T engine);
    void                    engine_event (PENGINE_T engine, uint16_t event, int32_t event_register) ;
    void                    engine_mask_event (uint32_t mask, uint16_t event, int32_t event_register) ;
    int32_t                 engine_queue_event (PENGINE_T engine, uint16_t event, int32_t event_register);
    int32_t                 engine_queue_masked_event (uint32_t mask, uint16_t event, int32_t event_register) ;

   /*
    * Debugging functions
    */
    void                    engine_dump (bool active_only) ;
    uint32_t                engine_check (const char ** name) ;
    int32_t                 engine_statemachine_idx (const char * name) ;
    uint32_t                engine_statemachine_logmask (const char * name) ;

#ifdef __cplusplus
}
#endif

#endif /* __ENGINE_H__ */
