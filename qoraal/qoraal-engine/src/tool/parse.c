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


#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include "parse.h"
#include "collection.h"
#include "lex.h"
#include "machine.h"
#if defined CFG_ENGINE_REGISTRY_ENABLE
#include "qoraal-flash/registry.h"
#endif
#include "qoraal-engine/engine.h"


static void     __LexError(struct LexState * Lexer, enum LexError Error, char* Message )  ;
static int      __LexParseToken (struct LexState * Lexer, enum LexToken Token, struct Value* Value) ;
static int      __LexGetString(struct LexState * Lexer, const char* name, int len, struct Value* Value) ;
static int      __LexGetIndex(struct LexState * Lexer, enum LexToken idxtok, const char* name, int len, struct Value* Value) ;
static int      __LexGetIdentifier (struct LexState * Lexer, const char* name, int len, struct Value* Value) ;
static int      __LexGetReservedWord(struct LexState * Lexer, const char* name, int len, enum LexToken * Token) ;

#define PARSER_LOG(logif, message, ...)         if (logif && logif->Log) logif->Log (message, ##__VA_ARGS__)
#define PARSER_REPORT(logif, message, ...)      if (logif && logif->Report) logif->Report (message, ##__VA_ARGS__)
#define PARSER_ERROR(logif, message, ...)       if (logif && logif->Error) logif->Error (message, ##__VA_ARGS__)

enum parserState {
    parseNone,
    parseStartupDeclare,
    parseEventsDeclare,
    parseVariablesDeclare,
    parseVersionDeclare,
    parseNameDeclare,
    parseStateDeclare,
    parseStatemachineDeclare,
};



struct ReservedWord
{
    const char *Word;
    enum LexToken Token;
};

static const struct ReservedWord ReservedWords[] =
{
    { "decl_events",    TokenEvents },
    { "decl_variables", TokenVariables },
    { "decl_version",   TokenVersion },
    { "decl_name",      TokenName },
    { "decl_startup",   TokenStartup },
    { "statemachine",   TokenStatemachine },
    { "state",          TokenState },
    { "super",          TokenSuperState },
    { "default",        TokenDefaultState },
    { "enter",          TokenEnter },
    { "event",          TokenEvent },
    { "event_if",       TokenEventIf },
    { "event_nt",       TokenEventNot },
    { "event_if_r",     TokenEventIfR },
    { "event_nt_r",     TokenEventNotR },
    { "exit",           TokenExit },
    { "action",         TokenAction },
    { "action_eq_e",    TokenActionEventEq },
    { "action_lt",      TokenActionLt },
    { "action_gt",      TokenActionGt },
    { "action_eq",      TokenActionEq },
    { "action_ne",      TokenActionNe },
    { "action_ld",      TokenActionLoad },
    { "deferred",       TokenDeferred },
    { "startstate",     TokenStartState },
};


static LEX_CB_IF _lexer_cb = {
    __LexGetReservedWord,
    __LexGetIdentifier,
    __LexGetString,
    __LexGetIndex,
    __LexParseToken,
    __LexError
} ;


#define PARSER_ID_TYPE(id)              ((uint16_t)(id >> 16) & 0xFF)
#define PARSER_ID_VALUE(id)             (uint16_t)(id & 0xFFFF)
#define PARSER_ID(type, value)          ((uint32_t) ((type << 16) | (value & 0xFFFF)))
#define PARSER_ID_GET_OP(id)            ((uint16_t)(id >> 24) & 0xFF)
#define PARSER_ID_SET_OP(id, op)        (id) |= ((op) << 24)

const char* parser_get_type(uint32_t id)
{
    switch (PARSER_ID_TYPE(id)) {
    case parseEvent:            return "event" ;
    case parseVariable:         return "variable" ;
    case parseState:            return "state" ;
    case parseStateMachine:     return "state machine" ;
    case parseAction:           return "action" ;
    case parseConst:            return "const" ;
    case parseRegId:            return "regid" ;
    case parseStringId:         return "stringid" ;
    default:                    return "undefined" ;
    }
}

typedef int (*PARSER_PF)  (struct LexState * /*Lexer*/, enum LexToken /*Token*/, struct Value* /*Value*/) ;
typedef struct PARSER_SECTION_S {

    struct PARSER_SECTION_S *   next ;
    enum parserState            state ;
    PARSER_PF                   pf ;

} PARSER_SECTION_T;

typedef struct PARSER_SUPER_SECTION_S {

    struct PARSER_SUPER_SECTION_S * next ;
    const char *                super ;
    uint16_t                    idx ;

} PARSER_SUPER_SECTION_T;

typedef struct PARSER_STATEMACHINE_S {

    const char*                 name ;
    const char*                 start ;
    const char*                 end ;
    int                         start_line ;

    int                         states ;
    int                         entries ;
    int                         brace_cnt ;

    const char*                 current ;
    PARSER_SUPER_SECTION_T *    super_stack ;
    STATEMACHINE_STATE_T*       pstate ;
    STATEMACHINE_T*             pstatemachine ;

    PARSE_CB_IF *               pif ;
    PARSE_LOG_IF *              logif ;

} PARSER_STATEMACHINE_T ;

static PARSER_SECTION_T *       _parser_stack   = 0 ;

static struct collection *      _parser_reserved = 0 ;
static struct collection *      _parser_strings = 0 ;
static struct collection *      _parser_declared = 0 ;
static struct collection *      _parser_declared_local = 0 ;
static struct collection *      _parser_undeclared = 0 ;
static enum parserState         _parser_state = parseNone ;


static unsigned short           _parser_events = STATES_EVENT_DECL_START ;
static unsigned short           _parser_variables = 0 ;
static unsigned short           _parser_statemachines = 0 ;

#define PARSER_INSTALL_STRING_SIZE          2
#define PARSER_INSTALL_IDENTIFIER_SIZE      1

static void
parse_push (PARSER_PF pf, enum parserState state)
{
    PARSER_SECTION_T * ps = (PARSER_SECTION_T *)engine_port_malloc (heapParser, sizeof(PARSER_SECTION_T)) ;
    ps->pf = pf ;
    ps->state = _parser_state = state ;
    ps->next = _parser_stack ;
   _parser_stack = ps ;

}

static bool
parse_pop (void)
{
    PARSER_SECTION_T * ps = _parser_stack ;
    if (ps) {
        _parser_stack = ps->next ;
        engine_port_free (heapParser, ps) ;
    } 
    if (_parser_stack) {
        _parser_state = _parser_stack->state ;
    }
    else {
        _parser_state = parseNone ;
    }

    return ps ? true : false ;
}

static int
parse_push_super (PARSER_STATEMACHINE_T * statemachine, const char * super, uint16_t idx)
{
    PARSER_SUPER_SECTION_T * ps = (PARSER_SUPER_SECTION_T *)engine_port_malloc (heapParser, sizeof(PARSER_SUPER_SECTION_T)) ;

    if (!ps) return ErrorMemory ;

    ps->super  = super ;
    ps->next = statemachine->super_stack ;
    ps->idx = idx ;
    statemachine->super_stack = ps ;

    return 1 ;
}

static bool
parse_pop_super (PARSER_STATEMACHINE_T * statemachine)
{
    PARSER_SUPER_SECTION_T * ps = statemachine->super_stack ;
    if (ps) {
        statemachine->super_stack = ps->next ;
        engine_port_free (heapParser, ps) ;
    }

    return statemachine->super_stack != 0 ;
}

static uint16_t
parse_get_super (PARSER_STATEMACHINE_T * statemachine) {
    PARSER_SUPER_SECTION_T * ps = statemachine->super_stack ;
    if (ps) {
        return ps->idx ;

    }

    return STATEMACHINE_INVALID_STATE ;
}

static int
parse_install_identifier (struct collection * dict, const char* name, int len, unsigned short type, unsigned short value, struct Value* Value)
{
    struct clist * np ;
    int res = 0;

    np = collection_get(dict, name, len) ;
    if (!np) {
        np = collection_install_size(dict, name, len, sizeof(unsigned int)*PARSER_INSTALL_IDENTIFIER_SIZE) ;
        if (!np) {
            res = ErrorMemory ;
        } else {
            *(unsigned int*)collection_get_value (dict, np) = PARSER_ID(type, value) ;
            res =  1 ;
        }

    }
    if (np) {
        Value->Id = *(unsigned int*)collection_get_value (dict, np) ;  // DLIST_UINT_GET(np) ;
        Value->Val.Identifier = (char*)collection_get_key (dict, np) ;
        Value->Typ = TypeIdentifier ;

    }

    return res ;
}

static int
parse_get_identifier (const char* name, int len, struct Value* Value)
{
    struct clist * np ;
    if ((np = collection_get(_parser_declared, name, len))) {
       Value->Id = *(unsigned int*)collection_get_value (_parser_declared, np) ;
       Value->Val.Identifier = (char*)collection_get_key (_parser_declared, np) ;
       Value->Typ = TypeIdentifier ;

       return 1 ;
    }

    return 0 ;
}

static int
parse_get_string (const char* name, int len, struct Value* Value)
{
    struct clist * np ;
    if ((np = collection_get(_parser_strings, name, len))) {
       Value->Id = *(unsigned int*)collection_get_value (_parser_strings, np) ;
       Value->Val.Identifier = (char*)collection_get_key (_parser_strings, np) ;
       Value->Typ = TypeIdentifier ;

       return 1 ;
    }

    return 0 ;
}



static int
parse_install_string (struct collection * dict, const char* name, int len, unsigned int id, struct Value* Value)
{
    struct clist * np ;
    int res = 0 ;

    if (len == 0 || !name) {
        name = "" ;

    }

    const char * newname = engine_port_sanitize_string (name, (uint32_t *)&len)  ;

    if (newname) name = newname ;

    np = collection_get(dict, name, len) ;

    if (!np) {
        np = collection_install_size(dict, name, len, sizeof(unsigned int)*PARSER_INSTALL_STRING_SIZE) ;
        if (np) {
            *(unsigned int*)collection_get_value (dict, np) = id ;
            if (Value->Typ == TypeInt) {
                ((unsigned int*)collection_get_value (dict, np))[1] = Value->Val.Integer ;
            } else {
                ((unsigned int*)collection_get_value (dict, np))[1] = 0 ;

            }

            res =  1 ;
        }
        }
    if (np) {
        Value->Id = *(unsigned int*)collection_get_value (dict, np) ;
        Value->Typ = TypeCharPointer;
        Value->Val.Pointer = (char*)collection_get_key (dict, np);

    }

    if (newname) engine_port_release_string (newname) ;


    return res ;
}

static int
parse_remove_string (struct collection * dict, const char* name)
{
    return collection_remove(dict, name, strlen(name)) ;
}

static int
parse_is_reserved (struct LexState * Lexer, enum LexToken Token)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    if ((Token >= TokenEvents) &&
            (Token <= TokenStartState)) {
        unsigned int i ;
        for (i=0; i<sizeof(ReservedWords)/sizeof(ReservedWords[0]); i++) {
            if (ReservedWords[i].Token == Token) {
                PARSER_REPORT(statemachine->logif, "warning: reserved word (%s)!\r\n", ReservedWords[i].Word) ;
            }
        }

        return 0 ;

    }

    return 1 ;
}


static void
value_init (struct Value* Parm)
{
    memset (Parm, 0, sizeof(struct Value)) ;
    Parm->Typ = TypeInt ;

}

static enum LexToken
read_value (struct LexState * Lexer, struct Value* Parm)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    char val[8] ;
    int sign = 1 ;
    struct Value Parm2 ;
    enum LexToken t_next ;
    enum LexToken t = LexScanGetToken (Lexer, Parm) ;
    if (t == TokenMinus) {
        sign = -1 ;
        t = LexScanGetToken (Lexer, Parm) ;
    }
    if (t == TokenPlus) {
        t = LexScanGetToken (Lexer, Parm) ;
    }
    t_next = LexScanGetToken (Lexer, &Parm2) ;
    if (t == TokenIndexConstant) {

        if (Parm->Typ == TypeInt) {
               if ((Parm->Val.Integer < ENGINE_REGISTER_COUNT) && (Parm->Val.Integer >= 0)) {
                   Parm->Id = PARSER_ID(parseVariable, PARSER_ID_VALUE(Parm->Id)) ;
                   return t_next ;
               }
               PARSER_REPORT(statemachine->logif, "warning: register index %d out of range!\r\n", Parm->Val.Integer) ;

        }
        else if (   (Parm->Typ == TypeIdentifier) ||
                    (Parm->Typ == TypeCharPointer) ) {

               if (strlen(Parm->Val.Identifier) == 0) {
                   // accumulator value empty [] or [0]
                   Parm->Val.LongInteger = 0 ;
                   Parm->Typ = TypeInt ;
                   Parm->Id = PARSER_ID(parseVariable, PARSER_ID_VALUE(Parm->Id)) ;
                   return t_next ;

               } else if (parse_get_identifier (Parm->Val.Identifier, 0, Parm)) {
                   if (PARSER_ID_TYPE(Parm->Id) == parseVariable) {
                           // return as is - indexed variable [x]
                           return t_next ;

                    }
                    PARSER_REPORT(statemachine->logif, "warning: expected identifier (%s) (expected variable)!\r\n",
                            LexGetValue(Parm, val, 8)) ;
               }
               else {
                   // string for indexed lookup like registry
                   if (parse_get_string(Parm->Val.Identifier, 0, Parm)) {
                       Parm->Id = PARSER_ID(parseRegId, PARSER_ID_VALUE(Parm->Id)) ;
                       return t_next ;
                   }
                   PARSER_REPORT(statemachine->logif, "warning: expected identifier (%s) (expected string)!\r\n",
                            LexGetValue(Parm, val, 8)) ;

               }

           }

        return TokenError ;

    }

    else {

        // constant integer value
        if ((t == TokenStringConstant)) {
            Parm->Id = PARSER_ID(parseStringId, PARSER_ID_VALUE(Parm->Id)) ;
            return t_next ;
        }
        if ((t >= TokenIdentifier) &&
                (t <= TokenCharacterConstant)) {
            Parm->Val.LongInteger *= sign ;
            return t_next ;
        }

        if (!parse_is_reserved(Lexer, t)) {
            PARSER_REPORT(statemachine->logif, "warning: expected identifier (%s)!\r\n", LexGetValue(Parm, val, 8)) ;

        }

    }

    return TokenError ;
}


static enum LexToken
read_identifier (struct LexState * Lexer, struct Value* Parm)
{
    struct Value Value ;
    enum LexToken t  ;

    LexScanGetToken (Lexer, Parm) ;
    t = LexScanGetToken (Lexer, &Value) ;
    if (t == PARSE_PUSH_TOKEN) {
        PARSER_ID_SET_OP(Parm->Id, PARSE_PUSH_OP) ;
        t = LexScanGetToken (Lexer, &Value) ;
    }
    else if (t == PARSE_POP_TOKEN) {
        PARSER_ID_SET_OP(Parm->Id, PARSE_POP_OP) ;
        t = LexScanGetToken (Lexer, &Value) ;
    }
    else if (t == PARSE_SAVE_TOKEN) {
        PARSER_ID_SET_OP(Parm->Id, PARSE_SAVE_OP) ;
        t = LexScanGetToken (Lexer, &Value) ;
    }
    else if (t == PARSE_PIN_TOKEN) {
        PARSER_ID_SET_OP(Parm->Id, PARSE_PIN_OP) ;
        t = LexScanGetToken (Lexer, &Value) ;
    }
    else if (t == PARSE_TERMINATE_TOKEN) {
        PARSER_ID_SET_OP(Parm->Id, PARSE_TERMINATE_OP) ;
        t = LexScanGetToken (Lexer, &Value) ;
    }

    return t ;
}

static bool
get_param_value32 (struct LexState * Lexer, int32_t* data, struct Value* parm)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    switch (parm->Typ) {
        case TypeShort:
        case TypeChar:
        case TypeUnsignedShort:
        case TypeUnsignedChar:
        case TypeInt:
        case TypeLong:
        case TypeUnsignedInt:
        case TypeUnsignedLong:
            *data = (uint32_t)parm->Val.Integer ;
            break ;
        case TypeIdentifier:
        case TypeCharPointer:
            *data = PARSER_ID_VALUE(parm->Id);
            break ;

        default:
            PARSER_REPORT(statemachine->logif,  "warning: undefined type %d!\r\n",
                    parm->Typ) ;
            *data = 0 ;
            return false ;
            break ;

    }

    return true ;

}

static bool
get_param_value (struct LexState * Lexer, int16_t* data, struct Value* parm)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    int32_t data32 ;

    if (!get_param_value32 (Lexer, &data32, parm)) {
        return false ;
    }

    if ((data32 < SHRT_MIN) || (data32 > SHRT_MAX)) {
        char val[8] ;
        PARSER_REPORT(statemachine->logif,  "warning: out of range %s!\r\n",
                LexGetValue(parm, val, 8)) ;

        if (data32 < SHRT_MIN) {
            data32 = SHRT_MIN ;
        }
        else if (data32 > SHRT_MAX) {
            data32 = SHRT_MAX ;
        }
    }
    *data = (int16_t) data32;

    return true ;
}


int __LexGetReservedWord(struct LexState * Lexer, const char* name, int len, enum LexToken * Token)
{
    struct clist * np ;
    np  = collection_get(_parser_reserved, name, len) ;
    if (np) {
        *Token = *(unsigned int*)collection_get_value (_parser_reserved, np) ;
        return 1 ;
    }

    return 0 ;
}

int __LexGetIdentifier (struct LexState * Lexer, const char* name, int len, struct Value* Value)
{
    unsigned short states = 0 ;
    struct clist * np ;
    int res ;

    switch (_parser_state) {
    case parseVersionDeclare:
        return 1 ;

    case parseStartupDeclare:
        return 1 ;

    case parseNameDeclare:
        return 1 ;

    case parseEventsDeclare:
        if ((res = parse_install_identifier(_parser_declared, name, len,
                parseEvent, _parser_events, Value)) > 0) {
            _parser_events++;
            if (_parser_events > STATES_EVENT_ID_MASK) {
                PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
                PARSER_REPORT(statemachine->logif,
                        "warning: events exceed %d!\r\n", STATES_EVENT_ID_MASK) ;
                return 0 ;

            }
        }
        if (res == 0) {
            return ErrorRedeclared ;
        }
        return res ;

    case parseVariablesDeclare:
        if ((res = parse_install_identifier(_parser_declared, name, len,
                parseVariable, _parser_variables+ENGINE_REGISTER_COUNT, Value)) > 0) {
            _parser_variables++;

        }
        if (res == 0) {
            return ErrorRedeclared ;
        }
        return res ;

    case parseStatemachineDeclare:
        if ((res = parse_install_identifier(_parser_declared, name, len,
                parseStateMachine, _parser_statemachines, Value)) > 0) {
            _parser_statemachines++ ;
        }
        if (res == 0) {
            return ErrorRedeclared ;
        }
        return res ;

    case parseStateDeclare:
        states = collection_count (_parser_declared_local) ;
        if ((res = parse_install_identifier(_parser_declared_local, name, len,
                parseState, states, Value)) > 0) {
            collection_remove(_parser_undeclared, name, len) ;
        }

        return res < 0 ? res : 1 ;
        

    default:
        break;

    }

    if ((np = collection_get(_parser_declared, name, len))) {
        Value->Id = *(unsigned int*)collection_get_value (_parser_declared, np) ;
        Value->Val.Identifier = (char*)collection_get_key (_parser_declared, np) ;
        Value->Typ = TypeIdentifier ;

    } else if ((np = collection_get(_parser_declared_local, name, len))) {
        Value->Id = *(unsigned int*)collection_get_value (_parser_declared_local, np) ;
        Value->Val.Identifier = (char*)collection_get_key (_parser_declared_local, np) ;
        Value->Typ = TypeIdentifier ;

    } else {
        parse_install_identifier(_parser_undeclared, name, len, 0, (unsigned short)-1, Value) ;

    }

    return 1 ;

}

int 
__LexGetString(struct LexState * Lexer, const char* name, int len, struct Value* Value)
{
    int idx = 0 ;


    idx = collection_count (_parser_strings) ;
    if ( parse_install_string(_parser_strings, name, len, idx, Value) ) {
 //       idx++ ;
    }
    return 1 ;
}



int
__LexGetIndex(struct LexState * Lexer, enum LexToken idxtok, const char* name, int len, struct Value* Value)
{
    int idx = 0 ;

    if (/*(idxtok == TokenIndexConstant) && */(isdigit((int)name[0]))) {
        Value->Typ = TypeInt;
        Value->Id = 0 ; // PARSER_ID(parseConst, PARSER_ID_VALUE(Value->Id)) ;
        if (sscanf (name, "%d", &Value->Val.Integer) != 1) {
            return 0 ;

        }

    }
    else {

        if (parse_get_identifier(name, len, Value)) {
            /*
             * If it is a variable it is already in the _parser_declared collection.
             */
            if (PARSER_ID_TYPE(Value->Id) != parseVariable) {
                // not a variable
                PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
                PARSER_REPORT(statemachine->logif, "warning: %s unexpected!\r\n",
                        Value->Val.Identifier) ;
                return 0 ;

            }

        } else {
            /*
             * Put it in the string table to use for indexed lookup in an action or function.
             */

            idx = collection_count (_parser_strings) ;
            if ( parse_install_string(_parser_strings, name, len, idx, Value) ) {

            }

        }
    }

    return 1 ;
}


int ParseReadDeclaration(struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    int res ;
    struct Value tmp ;
    enum parserState state = _parser_state  ;
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    switch(Token) {
    case TokenStatemachine:
        _parser_state = parseStatemachineDeclare ;
        break ;
    case TokenState:
        _parser_state = parseStateDeclare ;
        break ;
    default:
        PARSER_REPORT(statemachine->logif, "warning: expected statemachine or state declaration!\r\n") ;
        return 0 ;

    }
    Token = LexScanGetToken (Lexer, Value)  ;
    res = parse_is_reserved(Lexer, Token) ;
    if (res == 1) {

        _parser_state = state ;
        res &= LexScanGetToken (Lexer, &tmp) == TokenLeftBrace ;
        if (res == 0) {
            char val [8] ;
            PARSER_REPORT(statemachine->logif, "warning: expected left brace (%s)\r\n", LexGetValue(Value, val, 8)) ;
        }

    }

    return res ;

}

int ParserVariablesDeclare (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    if (Value->Typ == TypeIdentifier) {

        int32_t val ;
        struct Value Parm ;
        int32_t intval = 0 ;
        int idx = PARSER_ID_VALUE(Value->Id) ;
        PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

        if (idx < ENGINE_REGISTER_COUNT) {
            PARSER_REPORT(statemachine->logif, "warning: invalid variable index!\r\n") ;
            return 0 ;
        }

        if (engine_port_variable_read (idx - ENGINE_REGISTER_COUNT, &val) != ENGINE_OK) {
            PARSER_REPORT(statemachine->logif, "warning: variable %d not supported by port!\r\n",
                                        idx - ENGINE_REGISTER_COUNT) ;
            return 0 ;

        }

        enum LexToken t = LexScanGetToken (Lexer, &Parm) ;
        if (t == TokenAssign) {

            _parser_state = parseNone ;
            enum LexToken res = read_value (Lexer, &Parm) ;
            _parser_state = parseVariablesDeclare ;
            if (res == TokenError) {
                PARSER_REPORT(statemachine->logif, "warning: variable assignment failed!\r\n") ;
                return 0 ;

            }

            if (((PARSER_ID_TYPE(Parm.Id) == parseConst) || !PARSER_ID_TYPE(Parm.Id)) &&
                    get_param_value32 (Lexer, &intval, &Parm)) {
                engine_port_variable_write (idx - ENGINE_REGISTER_COUNT, intval) ;

            } 
#if defined CFG_ENGINE_REGISTRY_ENABLE   
            else if (PARSER_ID_TYPE(Parm.Id) == parseRegId) {
                if (registry_int32_get (Parm.Val.Identifier, &intval) != ENGINE_OK) {
                    PARSER_REPORT(statemachine->logif,
                            "warning: registry entry '%s' not a valid integer value !\r\n",
                            Parm.Val.Identifier) ;
                    return 0 ;

                }
            }
#endif
            else {
                PARSER_REPORT(statemachine->logif, "warning: expected value!\r\n") ;
                return 0 ;

            }

        }

    }

    if(Token == TokenRightBrace) {
        parse_pop () ;
    }
    return 1 ;
}

int ParserVersionDeclare (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    parse_pop () ;

    if (Value->Typ == TypeInt) {
       if (statemachine->pif->SetVersion) {
            statemachine->pif->SetVersion (Value->Val.Integer) ;
        }

        return 1 ;

    }

    PARSER_REPORT(statemachine->logif, "warning: expected integer version!\r\n") ;

    return 0 ;
}

int ParserNameDeclare (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    parse_pop () ;
    if (Value->Typ == TypeCharPointer) {
       if (statemachine->pif->SetName) {
            statemachine->pif->SetName (Value->Val.Identifier) ;
            parse_remove_string(_parser_strings, Value->Val.Identifier);
        }

        return 1 ;

    }

    PARSER_REPORT(statemachine->logif, "warning: expected string name!\r\n") ;

    return 0 ;
}


int ParserStartupDeclare (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    int res = 1 ;
    if ((Value->Typ == TypeIdentifier) ||
         (Value->Typ == TypeCharPointer)) {

        if (engine_port_shellcmd (Value->Val.Identifier) != ENGINE_OK) {
            //// res = 0  ;
        }

        parse_remove_string(_parser_strings, Value->Val.Identifier);
    }

    if(Token == TokenRightBrace) {
        parse_pop () ;
    }

    return res ;
}


int ParserEventsDeclare  (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    if(Token == TokenRightBrace) {
        parse_pop () ;
    }

    return 1 ;
}



int read_4_params (struct LexState * Lexer, struct Value* Parm1, struct Value* Parm2, struct Value* Parm3, struct Value* Parm4)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    struct Value Value ;
    char* val1[8] ;
    char* val2[8] ;
    enum LexToken t ;
    value_init (Parm1) ;
    value_init (Parm2) ;
    value_init (Parm3) ;
    value_init (Parm4) ;

    if (LexScanGetToken (Lexer, &Value) != TokenOpenBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 4 params, expected open bracket (%s)!\r\n",
                LexGetValue(&Value, (char*)val1, 8)) ;
        return 0 ;

    }

    t = read_identifier (Lexer, Parm1) ;
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 4 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm1, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_value(Lexer, Parm2) ;
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 4 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm2, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_identifier (Lexer, Parm3) ;
    if (t == TokenCloseBracket) {
        Parm4->Typ = TypeInt ;
        return 1 ;

    }
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 4 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm3, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_value(Lexer, Parm4) ;
    if (t != TokenCloseBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 4 params, expected close bracket (%s %s)!\r\n",
                LexGetValue(Parm4, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    return 1 ;
}

int read_3_params (struct LexState * Lexer, struct Value* Parm1, struct Value* Parm2, struct Value* Parm3)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    struct Value Value ;
    char* val1[8] ;
    char* val2[8] ;
    enum LexToken t ;
    value_init (Parm1) ;
    value_init (Parm2) ;
    value_init (Parm3) ;

    if (LexScanGetToken (Lexer, &Value) != TokenOpenBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 3 params, expected open bracket (%s)!\r\n",
                LexGetValue(&Value, (char*)val1, 8)) ;
        return 0 ;

    }

    t = read_identifier (Lexer, Parm1) ;
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 3 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm1, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_identifier (Lexer, Parm2) ;
    if (t == TokenCloseBracket) {
        Parm3->Val.LongInteger = 0 ;
        Parm3->Typ = TypeInt ;
        return 1 ;

    }
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 3 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm2, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_value(Lexer, Parm3) ;
    if (t != TokenCloseBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 3 params, expected close bracket (%s %s)!\r\n",
                LexGetValue(Parm3, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    return 1 ;
}

int read_2_params (struct LexState * Lexer, struct Value* Parm1, struct Value* Parm2)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    struct Value Value ;
    char* val1[8] ;
    char* val2[8] ;
    enum LexToken t ;
    value_init (Parm1) ;
    value_init (Parm2) ;

    if (LexScanGetToken (Lexer, &Value) != TokenOpenBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 2 params, expected open bracket (%s)!\r\n",
                LexGetValue(&Value, (char*)val1, 8)) ;
        return 0 ;

    }

    t = read_identifier (Lexer, Parm1) ;
    if (t == TokenCloseBracket) {
        Parm2->Val.LongInteger = 0 ;
        Parm2->Typ = TypeInt ;
        return 1 ;

    }
    if (t != TokenComma) {
        PARSER_REPORT(statemachine->logif, "warning: read 2 params, expected comma (%s %s)!\r\n",
                LexGetValue(Parm1, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    t = read_value(Lexer, Parm2) ;
    if (t != TokenCloseBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 2 params, expected close bracket (%s %s)!\r\n",
                LexGetValue(Parm2, (char*)val1, 8), LexGetValue(&Value, (char*)val2, 8)) ;
        return 0 ;

    }

    return 1 ;
}

int read_1_params (struct LexState * Lexer, struct Value* Parm1)
{
    struct Value Value ;
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    char* val1[8] ;
    char* val2[8] ;
    enum LexToken t ;
    value_init (Parm1) ;

    if (LexScanGetToken (Lexer, &Value) != TokenOpenBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 1 params, expected open bracket (%s)!\r\n",
                LexGetValue(&Value, (char*)val1, 8)) ;
        return 0 ;

    }
    t = read_identifier (Lexer, Parm1) ;
    if (t != TokenCloseBracket) {
        PARSER_REPORT(statemachine->logif, "warning: read 1 params, expected close bracket (%s %s)!\r\n",
                LexGetValue(Parm1, (char*)val2, 8), LexGetValue(&Value, (char*)val1, 8)) ;
        return 0 ;

    }

    return 1 ;
}


int ParserStateCreate (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    struct Value Parm[4] ;
    STATE_DATA_T data ;
    STATE_DATA_T data2 ;
    char val1[8] ;
    char val2[8] ;
    char val3[8] ;
    char val4[8] ;
    int res = 1 ;

    switch (Token) {
    case TokenDefaultState:
        if ((res = read_1_params (Lexer, &Parm[0]))) {
            PARSER_LOG(statemachine->logif, " . . default    %s\r\n",
                 LexGetValue(&Parm[0], val1, 8)) ;

            if (PARSER_ID_TYPE(Parm[0].Id) != parseState) {
                PARSER_REPORT(statemachine->logif,  "warning: state expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            data.id = PARSER_ID_VALUE(Parm[0].Id) ;
            data.param = 0 ;

            machine_state_default_idx (statemachine->pstate, data.id) ;

        }
        break ;

    case TokenDeferred:
        if ((res = read_1_params (Lexer, &Parm[0]))) {
            PARSER_LOG(statemachine->logif, " . . deferred   %s (%.4x)\r\n",
                LexGetValue(&Parm[0], val1, 8), PARSER_ID_VALUE(Parm[0].Id)) ;

            if (PARSER_ID_TYPE(Parm[0].Id) != parseEvent) {
                PARSER_REPORT(statemachine->logif,  "warning: event expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }
            data.id = PARSER_ID_VALUE(Parm[0].Id) ;
            data.param = 0 ;

            res = machine_state_add_deferred (statemachine->pstate, data) ;

        }
        break ;
    case TokenEnter:
    case TokenExit: 
        if ((res = read_2_params (Lexer, &Parm[0], &Parm[1]))) {
            PARSER_LOG(statemachine->logif,  " . . %s %s ( %s (%d) )\r\n",
                Token == TokenEnter? "enter     " : "exit      ", LexGetValue(&Parm[0], val1, 8),
                        LexGetValue(&Parm[1], val2, 8), PARSER_ID_VALUE(Parm[1].Id)) ;


            if (PARSER_ID_TYPE(Parm[0].Id) != parseAction) {
                PARSER_REPORT(statemachine->logif,  "warning: action expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            data.id = PARSER_ID_VALUE(Parm[0].Id);
            if (PARSER_ID_GET_OP(Parm[0].Id) == PARSE_PUSH_OP) {
                data.id |= STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[0].Id) == PARSE_POP_OP) {
                data.id |= STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[0].Id) == PARSE_SAVE_OP) {
                data.id |= STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[0].Id)) {
                PARSER_REPORT(statemachine->logif, "error: invalid operator '%c'!\r\n",
                        PARSER_ID_GET_OP(Parm[0].Id)) ;
                res = 0 ;
                break ;

            }

            if  (!get_param_value (Lexer, (int16_t*)&data.param, &Parm[1])) {
                PARSER_REPORT(statemachine->logif,  "warning: invalid value for %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }
            if (PARSER_ID_TYPE(Parm[1].Id) == parseRegId) {
                data.id |= STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET ;
            }
            else if ((PARSER_ID_TYPE(Parm[1].Id) == parseStringId)) {
                data.id |= STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET ;
            }
            else if (PARSER_ID_TYPE(Parm[1].Id) == parseVariable) {
                data.id |= STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET ;
            }


            if (Token == TokenEnter) {
                res = machine_state_add_entry (statemachine->pstate, data) ;

            }
            else if (Token == TokenExit) {
                res = machine_state_add_exit (statemachine->pstate, data) ;

            }

        }
        break ;

    case TokenEvent: 
    case TokenEventIf:
    case TokenEventNot:
    case TokenEventIfR:
    case TokenEventNotR:
        if ((res = read_2_params (Lexer, &Parm[0], &Parm[1]))) {
            PARSER_LOG(statemachine->logif,  " . . %s   %s (%.4x) ( %s )\r\n",
                    "event     ", LexGetValue(&Parm[0], val1, 8), PARSER_ID_VALUE(Parm[0].Id),
                    LexGetValue(&Parm[0], val2, 8)) ;

            if (PARSER_ID_TYPE(Parm[0].Id) != parseEvent) {
                PARSER_REPORT(statemachine->logif,  "warning: event expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }
            if (PARSER_ID_TYPE(Parm[1].Id) == parseConst) {
                if( (PARSER_ID_VALUE(Parm[1].Id) != STATEMACHINE_PREVIOUS_STATE) &&
                    (PARSER_ID_VALUE(Parm[1].Id) != STATEMACHINE_CURRENT_STATE) &&
                    (PARSER_ID_VALUE(Parm[1].Id) != STATEMACHINE_IGNORE_STATE) ) {
                    PARSER_REPORT(statemachine->logif,  "warning: invalid constant state %s %s!\r\n",
                            LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                    res = 0 ;
                    break ;
                }

            }
            else if (PARSER_ID_TYPE(Parm[1].Id) != parseState) {
                PARSER_REPORT(statemachine->logif,  "warning: state expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            data.id = PARSER_ID_VALUE(Parm[0].Id) ;
            data.param = PARSER_ID_VALUE(Parm[1].Id );

            if (PARSER_ID_GET_OP(Parm[0].Id) == PARSE_PIN_OP) {
                data.id |= STATES_EVENT_PREVIOUS_PIN ;

            } else if (PARSER_ID_GET_OP(Parm[0].Id)) {
                PARSER_REPORT(statemachine->logif, "warning: invalid operator '%c'!\r\n",
                        PARSER_ID_GET_OP(Parm[0].Id)) ;
                res = 0 ;
                break ;

            }

            if (Token == TokenEventIf) {
                data.id |= (STATES_EVENT_COND_IF<<STATES_EVENT_COND_OFFSET) ;
            }
            else if (Token == TokenEventNot) {
                data.id |= (STATES_EVENT_COND_NOT<<STATES_EVENT_COND_OFFSET) ;
            }
            else if (Token == TokenEventIfR) {
                data.id |= (STATES_EVENT_COND_IF_R<<STATES_EVENT_COND_OFFSET) ;
            }
            else if (Token == TokenEventNotR) {
                data.id |= (STATES_EVENT_COND_NOT_R<<STATES_EVENT_COND_OFFSET) ;
            }

            res = machine_state_add_event (statemachine->pstate, data) ;

        }
        break ;

    case TokenAction:
    case TokenActionEventEq:
    case TokenActionLt:
    case TokenActionGt:
    case TokenActionEq:
    case TokenActionNe:
    case TokenActionLoad:
        if (Token == TokenAction) {
            value_init (&Parm[1]) ;
            res = read_3_params (Lexer, &Parm[0], &Parm[2], &Parm[3]) ;
        } else {
            res = read_4_params (Lexer, &Parm[0], &Parm[1], &Parm[2], &Parm[3]) ;
        }

        if (res) {
            int f = Token - TokenAction ;
            PARSER_LOG(statemachine->logif,  " . . %s   %s (%.4x) [%s] . %s ( %s (%d) )\r\n",
                ReservedWords[TokenAction-TokenEvents+f].Word, LexGetValue(&Parm[0], val1, 8),
                PARSER_ID_VALUE(Parm[0].Id),
                LexGetValue(&Parm[1], val2, 8), LexGetValue(&Parm[2], val3, 8),
                LexGetValue(&Parm[3], val4, 8), PARSER_ID_VALUE(Parm[3].Id)) ;

            if (PARSER_ID_TYPE(Parm[0].Id) != parseEvent) {
                PARSER_REPORT(statemachine->logif,  "warning: event expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            if (Token == TokenActionLoad) {
                if (PARSER_ID_TYPE(Parm[1].Id) != parseVariable) {
                    PARSER_REPORT(statemachine->logif,  "warning: action expected variable for %s!\r\n",
                            LexGetValue(&Parm[2], val1, 8)) ;
                    res = 0 ;
                    break ;

                }
            }


            data.id = PARSER_ID_VALUE(Parm[0].Id) | (f<<STATES_EVENT_COND_OFFSET) ;
            if (PARSER_ID_TYPE(Parm[1].Id) == parseVariable) {
                data.id |= STATES_EVENT_COND_ACTION_VARIABLE ;

            }

            if (PARSER_ID_GET_OP(Parm[0].Id) == PARSE_TERMINATE_OP) {
                data.id |= STATES_INTERNAL_EVENT_TERMINATE ;

            } else if (PARSER_ID_GET_OP(Parm[0].Id)) {
                PARSER_REPORT(statemachine->logif, "warning: invalid operator '%c'!\r\n",
                        PARSER_ID_GET_OP(Parm[0].Id)) ;
                res = 0 ;
                break ;

            }


            if (PARSER_ID_TYPE(Parm[2].Id) != parseAction) {
                PARSER_REPORT(statemachine->logif,  "warning: action expected %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            if  (!get_param_value (Lexer, (int16_t*)&data.param, &Parm[1])) {
                PARSER_REPORT(statemachine->logif,  "warning: invalid value for %s %s!\r\n",
                        LexGetValue(&Parm[0], val1, 8), LexGetValue(&Parm[1], val2, 8)) ;
                res = 0 ;
                break ;

            }

            data2.id = PARSER_ID_VALUE(Parm[2].Id) ;
            if  (!get_param_value (Lexer, (int16_t*)&data2.param, &Parm[3])) {
                PARSER_REPORT(statemachine->logif,  "warning: invalid value for %s %s!\r\n",
                        LexGetValue(&Parm[2], val1, 8), LexGetValue(&Parm[3], val2, 8)) ;
                res = 0 ;
                break ;

            }

            if (PARSER_ID_GET_OP(Parm[2].Id) == PARSE_PUSH_OP) {
                data2.id |= STATES_ACTION_RESULT_PUSH << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[2].Id) == PARSE_POP_OP) {
                data2.id |= STATES_ACTION_RESULT_POP << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[2].Id) == PARSE_SAVE_OP) {
                data2.id |= STATES_ACTION_RESULT_SAVE << STATES_ACTION_RESULT_OFFSET ;

            }
            else if (PARSER_ID_GET_OP(Parm[2].Id)) {
                PARSER_REPORT(statemachine->logif, "warning: invalid operator '%c'!\r\n",
                        PARSER_ID_GET_OP(Parm[2].Id)) ;
                res = 0 ;
                break ;

            }

            if (PARSER_ID_TYPE(Parm[3].Id) == parseRegId) {
                data2.id |= STATES_ACTION_TYPE_INDEXED << STATES_ACTION_TYPE_OFFSET ;
            }
            else if ((PARSER_ID_TYPE(Parm[3].Id) == parseStringId)) {
                data2.id |= STATES_ACTION_TYPE_STRING << STATES_ACTION_TYPE_OFFSET ;
            }
            else if (PARSER_ID_TYPE(Parm[3].Id) == parseVariable) {
                data2.id |= STATES_ACTION_TYPE_VARIABLE << STATES_ACTION_TYPE_OFFSET ;
            }

            res = machine_state_add_action (statemachine->pstate, data, data2) ;

        }
        break ;

    case TokenRightBrace:
       PARSER_LOG(statemachine->logif,  " . %s\r\n", statemachine->current) ;
       parse_pop () ;
       break ;

    default:
        break ;

    }
    return res ;
}

int ParserStatemachineCreate  (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    int res ;
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    switch (Token) {
    case TokenState:
        if (!ParseReadDeclaration (Lexer, Token, Value)) {
            return 0 ;
        }
        statemachine->current = Value->Val.Identifier ;
        PARSER_LOG(statemachine->logif,  " . %s\r\n", statemachine->current) ;
        statemachine->pstate = machine_next_state (statemachine->pstatemachine,
                statemachine->pstate, Value->Id, parse_get_super (statemachine)) ;
        if (!statemachine->pstate) {
            PARSER_REPORT(statemachine->logif,
                    "warning: invalid state '%s'!\r\n", Value->Val.Identifier) ;
            return 0 ;

        }
        if (!machine_state_name (statemachine->pstatemachine, statemachine->pstate, Value->Val.Identifier)) {
            PARSER_REPORT(statemachine->logif,
                    "warning: duplicate state '%s'!\r\n", Value->Val.Identifier) ;
            return 0 ;

        }
        parse_push (ParserStateCreate, parseNone) ;
        break ;

    case TokenStartState:
        if (LexScanGetToken (Lexer, Value) != TokenIdentifier) {
            return 0 ;
        }

        machine_start_state (statemachine->pstatemachine, Value->Id) ;
        break ;

    case TokenSuperState:
        if (LexScanGetToken (Lexer, Value) != TokenIdentifier) {
            return 0 ;

        }
        if (PARSER_ID_TYPE(Value->Id) != parseState) {
            char val1[8] ;
            PARSER_REPORT(statemachine->logif,  "warning: state expected %s!\r\n",
                    LexGetValue(Value, val1, 8)) ;
            return  ErrorUnexpected ;

        }

        res = parse_push_super (statemachine, Value->Val.Identifier, PARSER_ID_VALUE(Value->Id)) ;
        if (res < 0) {
            return res ;

        }

        PARSER_LOG(statemachine->logif,  " { %s\r\n", Value->Val.Identifier) ;

        if (LexScanGetToken (Lexer, Value) != TokenLeftBrace) {
            PARSER_REPORT(statemachine->logif,  "warning: expected left brace!\r\n") ;
            return 0 ;

        }
        break ;

    case TokenRightBrace:
        if (statemachine->super_stack) {
            PARSER_LOG(statemachine->logif,
                    " } %s\r\n", statemachine->super_stack->super) ;
            parse_pop_super (statemachine) ;

        }
        break ;

    default:
        break ;

    }
    return 1 ;

}
 
int ParserStateDeclare  (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    switch (Token) {
    case TokenAction:
    case TokenActionEventEq:
    case TokenActionLt:
    case TokenActionGt:
    case TokenActionEq:
    case TokenActionNe:
    case TokenActionLoad:
        statemachine->entries += 2;
        break ;

    case TokenDefaultState:
    case TokenEnter:
    case TokenEvent: 
    case TokenEventIf:
    case TokenEventNot:
    case TokenEventIfR:
    case TokenEventNotR:
    case TokenExit: 
    case TokenDeferred:
        statemachine->entries++ ;
        break ;

    case TokenRightBrace:
        parse_pop () ;
        break ;

    default:
        break ;

    }

    return 1 ;
}


int ParserStatemachine  (struct LexState * Lexer, enum LexToken Token, struct Value* Value)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    struct collection_it it ;
    struct clist * p ;
    struct LexState StatemachineLexer ;

    if (Token == TokenState) {
        statemachine->states++ ;
        if(!ParseReadDeclaration (Lexer, Token, Value)) {
            PARSER_REPORT (statemachine->logif, "warning: state expected!\r\n");
            return 0 ;
        }
        parse_push (ParserStateDeclare, parseNone) ;

    } else if (Token == TokenLeftBrace) {
        statemachine->brace_cnt++ ;

    } else if(Token == TokenRightBrace) {
        if (statemachine->brace_cnt) {
            statemachine->brace_cnt--;

        }
        else {
            statemachine->end = Lexer->Pos ;
            parse_pop () ;

            PARSER_LOG(statemachine->logif, "\r\nCompiling state machine '%s':\r\n", statemachine->name) ;
            PARSER_LOG(statemachine->logif, "<b> . states %d</b>\r\n", statemachine->states) ;
            PARSER_LOG(statemachine->logif, "<b> . entries %d</b>\r\n", statemachine->entries) ;
            PARSER_LOG(statemachine->logif, "Declared:\r\n") ;
            for (p = collection_it_first (_parser_declared_local, &it) ; p;  ) {
                uint32_t tmp = *(unsigned int*)collection_get_value (_parser_declared_local, p) ;
                PARSER_LOG(statemachine->logif, " . %s (%s)\r\n",
                        (char*)collection_get_key (_parser_declared_local, p), parser_get_type(tmp)) ;
                p = collection_it_next (_parser_declared_local, &it) ;
            }
            if (collection_count(_parser_undeclared)) {
                PARSER_REPORT(statemachine->logif, "warning: undeclared:\r\n") ;
                for (p = collection_it_first (_parser_undeclared, &it) ; p;  ) {
                    PARSER_REPORT(statemachine->logif, " . %s\r\n", (char*)collection_get_key (_parser_undeclared, p)) ;
                    p = collection_it_next (_parser_undeclared, &it) ;
                }
                return ErrorUndeclared ;

            }

            PARSER_LOG(statemachine->logif, "%s\r\n", statemachine->name) ;

            statemachine->pstatemachine = machine_create (statemachine->name, statemachine->states, statemachine->entries) ;
            if (!statemachine->pstatemachine) {
                PARSER_REPORT(statemachine->logif, "warning: error creating statemachine %s:\r\n", statemachine->name) ;
                return ErrorMemory ;

            }
            statemachine->pstate = 0 ;

            parse_push (ParserStatemachineCreate, parseNone) ;
            LexInit(&StatemachineLexer,  &_lexer_cb, (void*)statemachine ) ;

            if (LexAnalyse (&StatemachineLexer, statemachine->start, statemachine->end - statemachine->start, statemachine->start_line) == TokenError) {
                return 0 ;
            }

            if (!statemachine->pif->AddStatemachine ||
                    (statemachine->pif->AddStatemachine (statemachine->pstatemachine) != ENGINE_OK)) {
                machine_destroy (statemachine->pstatemachine) ;
            }
            collection_remove_all (_parser_declared_local, 0, 0) ;
            statemachine->pstatemachine = 0 ;
            statemachine->pstate = 0 ;
            statemachine->states = 0 ;
            statemachine->entries = 0;
            statemachine->brace_cnt = 0;
            statemachine->current = 0 ;
            while (parse_pop_super(statemachine)) ;

            PARSER_LOG(statemachine->logif, "%s\r\n", statemachine->name) ;

            parse_pop () ;

        }

    }
    return 1 ;
}
 
int __LexParseToken (struct LexState * Lexer, enum LexToken Token, struct Value* Value) 
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;
    int status ;
    if (_parser_stack) {
        if ((status =_parser_stack->pf (Lexer, Token, Value)) <= 0) {
            return status ;

        }
        return 1 ;
    }

    if (Token == TokenEvents) {
           parse_push (ParserEventsDeclare, parseEventsDeclare) ;

    } else if (Token == TokenVariables) {
           parse_push (ParserVariablesDeclare, parseVariablesDeclare) ;

    } else if (Token == TokenStartup) {
           parse_push (ParserStartupDeclare, parseStartupDeclare) ;

    } else if (Token == TokenVersion) {
           parse_push (ParserVersionDeclare, parseVersionDeclare) ;

    } else if (Token == TokenName) {
           parse_push (ParserNameDeclare, parseNameDeclare) ;

    } else if (Token == TokenStatemachine) {
           if (!ParseReadDeclaration (Lexer, Token, Value)) {
                return 0 ;

            }
            statemachine->name = Value->Val.Identifier ;
            statemachine->start = Lexer->Pos ;
            statemachine->start_line = Lexer->Line ;
            parse_push (ParserStatemachine, parseNone) ;

    }  else if (Token == TokenRightBrace) {
        PARSER_REPORT(statemachine->logif,  "warning: unexpected right brace!\r\n") ;
        return ErrorUnexpected ;

    }

    return 1 ;
}


void __LexError(struct LexState * Lexer, enum LexError Error, char* Message)
{
    PARSER_STATEMACHINE_T * statemachine = (PARSER_STATEMACHINE_T *)Lexer->ctx ;

    PARSER_ERROR(statemachine->logif, "error: %d ('%s') at line %d\r\n",
                Error, Message, Lexer->Line) ;

}

int ParseInit (void)
{
    uint32_t i;
    struct clist * np ;
    _parser_strings  = collection_init (heapParser, 43);
    _parser_declared  = collection_init (heapParser, 83);
    _parser_declared_local = collection_init (heapParser, 31) ;
    _parser_undeclared  = collection_init (heapParser, 5);
    _parser_reserved = collection_init (heapParser, 43);

    for (i = 0; i < sizeof(ReservedWords) / sizeof(struct ReservedWord); i++) {
        np = collection_install_size( _parser_reserved, ReservedWords[i].Word, 0, sizeof(unsigned int));
        if (!np) return ENGINE_NOMEM ;
        *(unsigned int*)collection_get_value (_parser_reserved, np) = ReservedWords[i].Token ;
    }

    struct Value Value ;
    parse_install_identifier(_parser_declared, "a", 0, parseVariable, ENGINE_VARIABLE_ACCUMULATOR, &Value) ;
    parse_install_identifier(_parser_declared, "r", 0, parseVariable, ENGINE_VARIABLE_REGISTER, &Value) ;
    parse_install_identifier(_parser_declared, "p", 0, parseVariable, ENGINE_VARIABLE_PARAMETER, &Value) ;
    parse_install_identifier(_parser_declared, "e", 0, parseVariable, ENGINE_VARIABLE_EVENT, &Value) ;

    _parser_events = STATES_EVENT_DECL_START ;
    _parser_variables = 0 ;
    _parser_statemachines = 0 ;

    return 0 ;
}

int ParseDestroy (void)
{
   collection_destroy (_parser_strings) ;
   collection_destroy (_parser_declared) ;
   collection_destroy (_parser_declared_local) ;
   collection_destroy (_parser_undeclared) ;
   collection_destroy (_parser_reserved) ;

   _parser_strings = 0 ;
   _parser_declared = 0 ;
   _parser_declared_local = 0 ;
   _parser_undeclared = 0 ;
   _parser_reserved = 0 ;

   while (parse_pop() == true) ;

   return 0 ;
}


int ParserAddAction (const char* Name, uint32_t Id)
{
   struct Value Value ;
   return parse_install_identifier(_parser_declared, Name, 0, parseAction, Id, &Value) ;
}

int ParserAddConst (const char* Name, uint32_t Id)
{
   struct Value Value ;
   return parse_install_identifier(_parser_declared, Name, 0, parseConst, Id, &Value) ;
}


int ParserAddEvent (const char* Name, uint32_t Id)
{
    struct Value Value ;
    return parse_install_identifier(_parser_declared, Name, 0, parseEvent, Id, &Value) ;
}

enum parseType
ParseGetIdentifierId (const char * name, uint32_t len, uint32_t * Id)
{
    struct Value Value ;
    enum parseType type ;
    *Id = 0 ;
    if (len == 0) len = strlen (name) ;
    if (parse_get_identifier(name, len, &Value)) {
        *Id = PARSER_ID_VALUE(Value.Id) ;
        type = PARSER_ID_TYPE(Value.Id) ;
        return type ;
    }

    return parseInvalid ;
}

int ParseAnalyse(const char *Source, int SourceLen, PARSE_CB_IF * pif, PARSE_LOG_IF * logif)
{
    struct LexState Lexer ;
    PARSER_STATEMACHINE_T parser_statemachine ;

    int res = 1 ;

    memset (&parser_statemachine, 0, sizeof(PARSER_STATEMACHINE_T)) ;
    parser_statemachine.pif = pif ;
    parser_statemachine.logif = logif ;
    LexInit(&Lexer, &_lexer_cb, (void*)&parser_statemachine ) ;

    if (LexAnalyse (&Lexer, Source, SourceLen, 1) != TokenError) {
        PARSER_LOG(parser_statemachine.logif, "%d lines compiled.\r\n", Lexer.Line) ;

    } else {
        PARSER_ERROR(parser_statemachine.logif,
                "error: compiling statemachine failed!\r\n") ;
        res = 0 ;

    }

    if (_parser_state != parseNone) {
        while (parse_pop()) ;
        PARSER_ERROR(parser_statemachine.logif,
                "error: incomplete statemachine!\r\n") ;
        res = 0 ;

    }

    if(parser_statemachine.pstatemachine) {
        machine_destroy (parser_statemachine.pstatemachine) ;
    }
 

    return res ;
}

int ParseComplete(PARSE_CB_IF * pif, PARSE_LOG_IF  *logif)
{
    int res = 1 ;
    int i ;
    STRINGTABLE_T * pstringtable ;


    pstringtable = machine_stringtable_create (_parser_strings) ;
    if (!pif->SetStringtable || (pif->SetStringtable (pstringtable) != ENGINE_OK)) {
        machine_stringtable_destroy (pstringtable) ;
        res = 0 ;

    }

    PARSER_LOG(logif, "\r\nStringtable %d entries\r\n", pstringtable->count) ;
    for (i=0; i<pstringtable->count; i++) {
        STATEMACHINE_STRING_T * string = GET_STATEMACHINE_STRINGTABLE_REF(pstringtable, i)  ;
        PARSER_LOG(logif, "    string %d (%d): %s\r\n", i, string->id, string->value) ;

    }
    PARSER_LOG(logif, "\r\n\r\n") ;

    return res ;
}
