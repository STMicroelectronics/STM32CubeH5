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

#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>
#include "qoraal-engine/engine.h"


#define PARSE_PUSH_OP           '+'
#define PARSE_POP_OP            '-'
#define PARSE_SAVE_OP           '='
#define PARSE_PIN_OP            '^'
#define PARSE_TERMINATE_OP      '!'


#define PARSE_PUSH_TOKEN        TokenPlus
#define PARSE_POP_TOKEN         TokenMinus
#define PARSE_SAVE_TOKEN        TokenAssign
#define PARSE_PIN_TOKEN         TokenArithmeticExor
#define PARSE_TERMINATE_TOKEN   TokenUnaryNot

enum parseType {
    parseInvalid = 0x0,
    parseEvent = 0x1,
    parseVariable,
    parseState ,
    parseStateMachine ,
    parseAction,
    parseConst ,
    parseRegId ,
    parseStringId ,
};

typedef struct PARSE_CB_IF_S {

    int (*AddStatemachine) (STATEMACHINE_T* /*statemachine*/) ;
    int (*SetStringtable)(STRINGTABLE_T* /*stringtable*/) ;
    void (*SetVersion) (int /*version*/) ;
    void (*SetName) (char* /*name*/) ;

} PARSE_CB_IF ;

typedef struct PARSE_LOG_IF_S {

    void (*Log) (char* /*message*/, ...) ;
    void (*Report) (char* /*message*/, ...) ;
    void (*Error) (char* /*message*/, ...) ;

} PARSE_LOG_IF ;

#ifdef __cplusplus
extern "C" {
#endif

    extern int      ParseInit (void) ;
    extern int      ParseDestroy (void) ;
    extern int      ParserAddAction (const char* Name, uint32_t Id) ;
    extern int      ParserAddConst (const char* Name, uint32_t Id) ;
    extern int      ParserAddEvent (const char* Name, uint32_t Id) ;
    extern enum parseType      ParseGetIdentifierId (const char * name, uint32_t len, uint32_t * Id) ;
    extern int      ParseAnalyse (const char *Source, int SourceLen, PARSE_CB_IF * pif, PARSE_LOG_IF* lif) ;
    extern int      ParseComplete (PARSE_CB_IF * pif, PARSE_LOG_IF* logif) ;

#ifdef __cplusplus
}
#endif


#endif /* __PARSE_H__ */
