/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/common/strsub.h"

#if 1 // !defined CFG_NBOOT
SVC_SHELL_CMD_DECL( "help", qshell_help, "[filter]");
SVC_SHELL_CMD_DECL( "?", qshell_help, 0);
#endif
SVC_SHELL_CMD_DECL( "rem", qshell_rem, 0);
SVC_SHELL_CMD_DECL( "nop", qshell_nop, "[res]");
SVC_SHELL_CMD_DECL( "wdt_kick", qshell_wdt_kick, "");
SVC_SHELL_CMD_DECL( "wdt_deactivate", qshell_wdt_deactivate, "");
SVC_SHELL_CMD_DECL( "wdt_activate", qshell_wdt_activate, "");


char _qshell_buffer[SVC_SHELL_PRINT_BUFFER_SIZE]  ;

typedef struct SVC_SHELL_CMD_LIST_IT_S {
    SVC_SHELL_CMD_LIST_T*        lst ;
    uint32_t idx ;
} SVC_SHELL_CMD_LIST_IT_T ;


static SVC_SHELL_CMD_LIST_T _qshell_static_list = {
        0,
        0,
        0,
        0
};


__attribute__((weak, section(".rodata.qshell.cmds."))) char __qshell_cmds_base__;
__attribute__((weak, section(".rodata.qshell.cmds."))) char __qshell_cmds_end__;

int32_t 
svc_shell_init(void)
{
    uintptr_t base = (uintptr_t)&__qshell_cmds_base__;
    uintptr_t end = (uintptr_t)&__qshell_cmds_end__;
    uint32_t cnt = (end - base) / sizeof(SVC_SHELL_CMD_T);
    if (cnt > 0) {
        // Cast the base address to the appropriate pointer type
        _qshell_static_list.cmds = (SVC_SHELL_CMD_T *)base;
        _qshell_static_list.cnt = cnt;
    }


    return SVC_SHELL_CMD_E_OK ;
}

int32_t      
svc_shell_start (void)
{
    return SVC_SHELL_CMD_E_OK ;
}

int32_t      
svc_shell_stop (void)
{
    return SVC_SHELL_CMD_E_OK ;
}

const SVC_SHELL_CMD_T*
_cmd_first(SVC_SHELL_CMD_LIST_IT_T * it)
{
    it->idx = 0 ;

    if (!_qshell_static_list.cmds || !_qshell_static_list.cmds[0].cmd) {
        it->lst = _qshell_static_list.next ;
    } else {
        it->lst = &_qshell_static_list ;
    }

    if (!it->lst) return 0 ;
    return &it->lst->cmds[it->idx] ;
}

const SVC_SHELL_CMD_T*
_cmd_next(SVC_SHELL_CMD_LIST_IT_T * it)
{
    it->idx++ ;
    if (
            !it->lst ||
            (it->lst->cnt && (it->idx >= it->lst->cnt)) ||
            (it->lst->cmds[it->idx].cmd == 0)
        ) {
        if (it->lst->next == 0) {
            return 0 ;
        }
        it->lst = it->lst->next ;
        it->idx = 0 ;
    }

    return &it->lst->cmds[it->idx] ;
}

const SVC_SHELL_CMD_T*
_cmd_get(SVC_SHELL_CMD_LIST_IT_T * it)
{
    return &it->lst->cmds[it->idx] ;
}

int
_cmd_cmp(SVC_SHELL_CMD_LIST_IT_T * it1, SVC_SHELL_CMD_LIST_IT_T * it2)
{
    const SVC_SHELL_CMD_T * cmd1 = &it1->lst->cmds[it1->idx] ;
    const SVC_SHELL_CMD_T * cmd2 = &it2->lst->cmds[it2->idx] ;

    return strcmp(cmd1->cmd, cmd2->cmd) ;
}

void
_cmd_help(SVC_SHELL_IF_T * pif,
        SVC_SHELL_CMD_LIST_IT_T * it, const char * filter)
{
    const SVC_SHELL_CMD_T*cmd = _cmd_get(it) ;

    if (cmd->usage) {
        if (!filter || (filter && strstr (cmd->cmd, filter))) {
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                    "%s %s" SVC_SHELL_NEWLINE, cmd->cmd, cmd->usage) ;
        }
    }
}


uint32_t
svc_shell_install (SVC_SHELL_CMD_LIST_T * list)
{
    SVC_SHELL_CMD_LIST_T * l = &_qshell_static_list ;

    while (l->next != 0) {
        l = l->next ;
        if (l == list) {
            return E_PARM ;
        }
    }

    list->next = 0 ;
    l->next = list ;

    return SVC_SHELL_CMD_E_OK ;
}

uint32_t
svc_shell_uninstall (SVC_SHELL_CMD_LIST_T * list)
{
    SVC_SHELL_CMD_LIST_T * l = &_qshell_static_list ;
    SVC_SHELL_CMD_LIST_T * prev = 0 ;

    for (  ; (l!=0) && (l!=list) ; ) {

        prev = l ;
        l = l->next;

    }

    if ((l == list) && prev) {
            prev->next = l->next ;

    }

    return SVC_SHELL_CMD_E_OK ;
}


int32_t svc_shell_print_table(SVC_SHELL_IF_T * pif, uint32_t out,
        const char * left, int32_t tabright, const char * fmtstr, ...)
{
    va_list         args;
    va_start (args, fmtstr) ;

    int count = snprintf ((char*)_qshell_buffer, 
                    SVC_SHELL_PRINT_BUFFER_SIZE - 3, "%s", (char*)left) ;
    do {
        _qshell_buffer[count++] = ' ' ;
    } while ((count < tabright) && (count < SVC_SHELL_PRINT_BUFFER_SIZE - 3)) ;
    count += vsnprintf ((char*)&_qshell_buffer[count], 
                    SVC_SHELL_PRINT_BUFFER_SIZE - count, (char*)fmtstr, args) ;
    va_end (args) ;
    pif->out (pif->ctx, out, _qshell_buffer) ;
    return count ;
}

int32_t svc_shell_print(SVC_SHELL_IF_T * pif, uint32_t out, const char * fmtstr, ...)
{
    va_list         args;
    va_start (args, fmtstr) ;

    int32_t count = vsnprintf ((char*)_qshell_buffer, 
                    SVC_SHELL_PRINT_BUFFER_SIZE, (char*)fmtstr, args) ;
    va_end (args) ;
    pif->out (pif->ctx, out, _qshell_buffer) ;

    return count ;
}

int32_t svc_shell_write(SVC_SHELL_IF_T * pif, uint32_t out,
        const char * str, uint32_t len)
{
    uint32_t offset = 0 ;
    do {
        uint32_t write = len > 480 ? 480 : len ;
        strncpy (_qshell_buffer, &str[offset], write) ;
        len -= write ;
        offset += write ;
        _qshell_buffer[write] = '\0' ;
        pif->out (pif->ctx, out, _qshell_buffer) ;
    } while (len) ;
 
    return offset ;
}


int32_t svc_shell_scan_int (const char * str, uint32_t * val)
{
    uint32_t i = 0 ;
    int32_t type = 0;

    if ((str[0] == '+') || (str[0] == '-')) {
        i = 1 ;

    }
    if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'b'))
    ) {
        type = 1 ;
        i = 2 ;

    }

    while (str[i]) {
        if ((type == 0) && !isdigit((int)str[i])) {
            type = 1 ;

        }
        if ((type == 1) && !isxdigit((int)str[i])) {
            type = 2 ;
            break ;

        }
        i++ ;

    }

    if (type == 0) {
        sscanf(str, "%i", (int*)val) ;

    }
    else if (type == 1) {
        if ((str[0] == '0') && (str[1] == 'x')) {
            sscanf(str, "0x%x", (unsigned int*)val) ;

        } 
#if 0
        else if ((str[0] == '0') && (str[1] == 'b')) {
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wformat"
            sscanf(str, "0b%b", (unsigned int*)val) ;
#pragma GCC diagnostic pop
        } 
#endif        
        else {
            sscanf(str, "%x", (unsigned int*)val) ;

        }

    } else {
        return SVC_SHELL_CMD_E_FAIL ;

    }

    return SVC_SHELL_CMD_E_OK ;
}

int32_t      
svc_shell_if_init (SVC_SHELL_IF_T * pif, void* ctx, SVC_SHELL_OUT_FP out, SVC_SHELL_IN_FP in)
{
    pif->ctx = ctx ;
    pif->out = out ;
    pif->in = in ;
    pif->status = 0 ;

    
    return EOK ;
}

int32_t
svc_shell_cmd_run (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res = SVC_SHELL_CMD_E_NOT_FOUND ;
    SVC_SHELL_CMD_LIST_T * list = 0 ;
    SVC_SHELL_CMD_LIST_IT_T it ;
    const SVC_SHELL_CMD_T * cmd ;
    int started  ;

    if (*argv[0] == '#') {
        return SVC_SHELL_CMD_E_OK ;
    }

    svc_wdt_register (&pif->wdt, TIMEOUT_60_SEC) ;
    svc_wdt_activate (&pif->wdt) ;

    cmd = _cmd_first(&it);
    while (cmd) {

        if (list != it.lst) {
            list = it.lst ;
            started = -1 ;

        }

        if (strcmp (cmd->cmd, argv[0]) == 0) {
            int32_t usage = 0 ;
            if ((argc <= 1) || (*argv[1] != '?')) {

                if ((started < 0) && list->service) {
                    started = svc_service_status(svc_service_get(list->service))
                            >= SVC_SERVICE_STATUS_STARTED ;
                }

                if (started != 0) {
                    res = cmd->fp (pif, argv, argc) ;

                } else {
                    res = SVC_SHELL_CMD_E_NOT_READY ;

                }

            } else {
                usage = 1 ;

            }

            if (cmd->usage && (usage || (res == SVC_SHELL_CMD_E_PARMS))) {
                svc_shell_print (pif, SVC_SHELL_OUT_STD,
                        "usage: %s %s" SVC_SHELL_NEWLINE,
                        cmd->cmd, cmd->usage) ;
                res = SVC_SHELL_CMD_E_OK ;

            }
#if CFG_PLATFORM_SVC_SERVICES
            else if (!started) {
                svc_shell_print (pif, SVC_SHELL_OUT_STD,
                        "'%s' require service '%s'" SVC_SHELL_NEWLINE,
                        cmd->cmd, svc_service_name(svc_service_get(list->service))) ;

            }
#endif
            break ;

        }

        cmd = _cmd_next(&it) ;

    }

    if (res == SVC_SHELL_CMD_E_NOT_FOUND) {
         svc_shell_print (pif, SVC_SHELL_OUT_ERR,
                "ERROR: '%s' not found!" SVC_SHELL_NEWLINE, argv[0]) ;

    }

    svc_wdt_unregister (&pif->wdt, TIMEOUT_60_SEC) ;

    return res ;
}

int32_t svc_shell_write_hex(SVC_SHELL_IF_T * pif, uint32_t out,
        const uint8_t * buffer, uint32_t len)
{
    int32_t res = len ;
    uint32_t offset = 0;
    uint32_t i;
    uint32_t hex_len = sizeof(_qshell_buffer) - 1;  // Reserve space for null terminator
    char *buf_ptr = _qshell_buffer;

    while (len > 0) {
        // Reset the buffer pointer and offset within each chunk
        buf_ptr = _qshell_buffer;
        offset = 0;

        // Convert data from buffer to hex, chunk by chunk
        for (i = 0; i < len && offset < hex_len; ++i) {
            offset += snprintf(buf_ptr + offset, hex_len - offset, "%02x", buffer[i]);
            if (offset >= hex_len) {
                break;
            }
        }

        // Write the hex data to the output function
        pif->out(pif->ctx, out, _qshell_buffer);

        // Update buffer pointer and remaining length
        buffer += i;
        len -= i;
    }

    return res ;
}

uint32_t
svc_shell_cmd_help (char *buffer, size_t len)
{
    unsigned offset = 0 ;
    SVC_SHELL_CMD_LIST_IT_T it ;
    const SVC_SHELL_CMD_T * cmd ;

    for (cmd = _cmd_first(&it); cmd; cmd = _cmd_next(&it)) {

        if (cmd->usage && (offset + 3 <= len)) {
            unsigned int l = strlen(cmd->usage) + strlen(cmd->cmd)  ;


            if (offset + l + 4 < len) {
                offset += snprintf (&buffer[offset], len - offset,
                        "%s %s\r\n", cmd->cmd, cmd->usage ) ;
            }
            else {
                break ;
            }
        }
    }

    return offset + 1 ;
}

int32_t
qshell_rem(SVC_SHELL_IF_T * pif, char** argv, int argc)
{

    return SVC_SHELL_CMD_E_OK ;
}

int32_t
qshell_nop(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res = SVC_SHELL_CMD_E_OK ;

    if (argc > 1) {
        svc_shell_scan_int(argv[1], (uint32_t*)&res) ;

    }

    return res  ;
}

int32_t
qshell_help(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    SVC_SHELL_CMD_LIST_IT_T it ;
    int found = 1 ;
    SVC_SHELL_CMD_LIST_IT_T firstit ;
    SVC_SHELL_CMD_LIST_IT_T lastit ;
    SVC_SHELL_CMD_LIST_IT_T nextit ;
    const SVC_SHELL_CMD_T * cmd ;

    //SVC_SHELL_CMD_LIST_T*  this = 0 ;
    _cmd_first(&firstit) ;
    _cmd_first(&nextit) ;
    _cmd_first(&lastit) ;

    for (cmd = _cmd_first(&it); cmd; cmd = _cmd_next(&it)) {
        if (_cmd_cmp(&it,&firstit) < 0) {
            firstit = nextit = it ;
        }
        if (_cmd_cmp(&it,&lastit) > 0) {
            lastit = it ;
        }
    }

    do  {

        _cmd_help(pif,
                &nextit, (argc > 1) ? argv[1] : 0) ;

        found = 0 ;
        nextit = lastit ;
        for (cmd = _cmd_first(&it); cmd; cmd = _cmd_next(&it)) {
            if (_cmd_cmp(&it,&firstit) <= 0) {
                continue ;
            }
            if (_cmd_cmp(&it,&nextit) < 0) {
                nextit = it ;
                found = 1 ;
            }

        }
        firstit = nextit ;


    } while (found) ;

    _cmd_help(pif,
            &nextit, (argc > 1) ? argv[1] : 0) ;


    return SVC_SHELL_CMD_E_OK ;
}

int32_t
qshell_wdt_kick (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_wdt_handler_kick (&pif->wdt) ;
    return SVC_SHELL_CMD_E_OK ;
}

int32_t
qshell_wdt_activate (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_wdt_activate (&pif->wdt) ;
    return SVC_SHELL_CMD_E_OK ;
}

int32_t
qshell_wdt_deactivate (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_wdt_deactivate (&pif->wdt) ;
    return SVC_SHELL_CMD_E_OK ;
}

void         
svc_shell_wdt_kick (SVC_SHELL_IF_T * pif)
{
    svc_wdt_handler_kick (&pif->wdt) ;
}

void         
svc_shell_wdt_activate (SVC_SHELL_IF_T * pif)
{
    svc_wdt_activate (&pif->wdt) ;
}

void         
svc_shell_wdt_deactivate (SVC_SHELL_IF_T * pif)
{
    svc_wdt_deactivate (&pif->wdt) ;
}

size_t
svc_shell_cmd_split(char *buffer, size_t len, char *argv[], size_t argv_size)
{
    char *p, *start_of_word = buffer ;
    int c;
    int strchar ;
    enum states { DULL, IN_WORD, IN_STRING, IN_WORD_STRING } state = DULL;
    size_t argc = 0;
    argv[0] = "" ;

    for (p = buffer; argc < argv_size && *p != '\0' &&
                    ((unsigned int)(p - buffer) < len) ; p++) {

        c = (unsigned char) *p;
        switch (state) {
        case DULL:
            if (isspace(c)) {
                continue;
            }

            if ((c == '"') || (c == '\'')) {
                state = IN_STRING;
                strchar = c ;
                start_of_word = p + 1;
                continue;
            }
            state = IN_WORD;
            start_of_word = p;
            continue;

        case IN_STRING:
            if (c == strchar) {
                *p = 0;
                argv[argc++] = start_of_word;
                state = DULL;
            }
            continue;

        case IN_WORD:
            if (isspace(c)) {
                *p = 0;
                argv[argc++] = start_of_word;
                state = DULL;
            } else if (c == '"') {
                state = IN_WORD_STRING;
            }
            continue;

        case IN_WORD_STRING:
            if (c == '"') {
               state = IN_WORD;
            }
            continue ;
        }

    }

    if (state != DULL && argc < argv_size)
        argv[argc++] = start_of_word;

    return argc;
}


int32_t
svc_shell_script_clear_last_error (SVC_SHELL_IF_T * pif)
{
    pif->status = 0 ;
    return SVC_SHELL_CMD_E_OK ;
}


int32_t
svc_shell_script_run (SVC_SHELL_IF_T * pif, const char* name,
                    char* start, int length)
{
    int32_t status = SVC_SHELL_CMD_E_OK ;
    int32_t lasterror = SVC_SHELL_CMD_E_OK ;
    int32_t cancel ;
    int lineno = 0 ;
    char* line ;
    int len ;
    int i = 0;
    //int i_line_next ;
    //int i_line_error = length ;
    char *argv[SVC_SHELL_ARGC_MAX];
    int argc ;
    char * current_line  ;
#if !defined CFG_COMMON_STRSUB_DISABLE
    char * strsub_line  ;
#endif
    if (!name) name = "" ;

    enum  {
        stateNormal,
        stateInHandler,
        stateInError,


    } error_state = stateNormal ;

    pif->recurse = 0 ;
#if 0
    if (recurse++ == 0) {
//      svc_system_speed(SYSTEM_SVC_SPEED_FAST, SYSTEM_SVC_SPEED_REQUESTOR_SCRIPT) ;

    }
#endif

    cancel = pif->out (pif->ctx, SVC_SHELL_OUT_NULL, 0) ;
    if (cancel < SVC_SHELL_CMD_E_OK) {
        return cancel ;

    }

    current_line = SVC_SHELL_MALLOC(SVC_SHELL_LINE_SIZE_MAX) ;
    if (!current_line) {
        return SVC_SHELL_CMD_E_MEMORY ;
    }
#if !defined CFG_COMMON_STRSUB_DISABLE
    strsub_line = SVC_SHELL_MALLOC(SVC_SHELL_LINE_STRSUB_SIZE_MAX) ;
    if (!strsub_line) {
        SVC_SHELL_FREE(current_line) ;
        return SVC_SHELL_CMD_E_MEMORY ;
    }
#endif

    line = start ;
    while (line) {
        len = 0 ;

        while ((start[i] != '\r') &&
                (start[i] != '\n') &&
                (start[i] != '\0') &&
                (start[i] != SVC_SHELL_COMMAND_SEPARATOR) &&
                (i < length) &&
                (len < SVC_SHELL_LINE_SIZE_MAX-1)) {
            current_line[len] = start[i] ;
            i++ ; len++ ;
        }
        current_line[len] = '\0' ;
        if (start[i] == '\n') lineno++ ;
        //i_line_next = i ;

#if !defined CFG_COMMON_STRSUB_DISABLE
        len = strsub_parse_string_to (0, current_line, len, strsub_line, 
                        SVC_SHELL_LINE_STRSUB_SIZE_MAX) ;
        argc = svc_shell_cmd_split(strsub_line, len, argv, SVC_SHELL_ARGC_MAX-1);
#else
        argc = svc_shell_cmd_split(current_line, len, argv, SVC_SHELL_ARGC_MAX-1);
#endif


        if (argc > 0) {
#if 1
            if (!strcmp (argv[0], ":exit")) {
                svc_shell_print (pif, SVC_SHELL_OUT_STD,
                        "shell '%s' exit on line %d!" SVC_SHELL_NEWLINE,
                        name, lineno) ;
                break ;

            } else
#endif
            if (strcmp (argv[0], ":onerror") == 0) {

                if (status >= SVC_SHELL_CMD_E_OK) {
                    //break ;
                    error_state = stateInError ;
                }

                else {


                    error_state = stateInError ;

                    if (argc > 1) {

                        int shell_errno ;
                        if (    (sscanf(argv[1], "%d", &shell_errno) > 0) &&
                                (status == shell_errno) ) {
                            error_state = stateInHandler ;


                        } else if ((*argv[1] == 'x') || (*argv[1] == '#')) {
                            error_state = stateInHandler ;


                        }


                    } else {
                        error_state = stateInHandler ;

                    }

                    if (error_state == stateInHandler) {
                        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                                "shell '%s' onerror %d handler on line %d!" SVC_SHELL_NEWLINE,
                                name, status, lineno) ;

                    }

                }


            } else if (strcmp (argv[0], ":clearerror") == 0) {


                if (error_state > stateNormal) {
                    svc_shell_print (pif, SVC_SHELL_OUT_STD,
                            "shell '%s' clearerror %d on line %d!" SVC_SHELL_NEWLINE,
                            name, status, lineno) ;

                    lasterror = SVC_SHELL_CMD_E_OK ;
                    status = SVC_SHELL_CMD_E_OK ;
                    pif->status = SVC_SHELL_CMD_E_OK ;


                }
                error_state = stateNormal ;


            } else if (error_state > stateInHandler) {

                // continue until clear error or end of file

            }
            else {

                status = svc_shell_cmd_run (pif, &argv[0],  argc-0) ;
                if (status < SVC_SHELL_CMD_E_OK) {

                    error_state = stateInError ;

                    //i_line_error = i_line_next ;
                    if (lineno > 1) {
                        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                                "shell '%s %s %s' error %d for '%s' on line %d!" 
                                SVC_SHELL_NEWLINE,
                                name, argc>1 ? argv[1] : "", argc>2 ? argv[2] : "",
                                status, argv[0], lineno) ;

                    }

                }



                if (pif->status >= SVC_SHELL_CMD_E_OK) {
                    if (status < SVC_SHELL_CMD_E_OK) lasterror = status ;
                    cancel = pif->out (pif->ctx, SVC_SHELL_OUT_NULL, 0) ;
                    if (cancel < SVC_SHELL_CMD_E_OK) {
                        svc_shell_print (pif, SVC_SHELL_OUT_STD,
                                "shell '%s %s %s' cancelled with %d on line %d!" 
                                SVC_SHELL_NEWLINE,
                                name, argc>1 ? argv[1] : "", argc>2 ? argv[2] : "",
                                status, lineno) ;
                        lasterror = status = cancel ;

                    }

                }

            }


        }

        while (((start[i] == '\r') ||
                (start[i] == '\n') ||
                (start[i] == '\0') ||
                (start[i] == SVC_SHELL_COMMAND_SEPARATOR)
                ) && (i < length)) {
            if (start[i] == '\n') lineno++ ;
            i++;
        }
        if (i < length) {
            line = &start[i] ;
        }
        else {
            line = 0 ;
        }

    }

    SVC_SHELL_FREE(current_line) ;
#if !defined CFG_COMMON_STRSUB_DISABLE
    SVC_SHELL_FREE(strsub_line) ;
#endif


    if (pif->recurse) {
        pif->recurse-- ;
    }
    if (!pif->recurse) {
#if 0
        svc_system_speed(SYSTEM_SVC_SPEED_IDLE, SYSTEM_SVC_SPEED_REQUESTOR_SCRIPT) ;
#endif
    }


    if (lasterror < 0) {
        pif->status = lasterror ;
    }
    return pif->status ;

}
