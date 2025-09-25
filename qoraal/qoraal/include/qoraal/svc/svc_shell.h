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


#ifndef __SVC_SHELL_H__
#define __SVC_SHELL_H__

#include <stdint.h>
#include <stddef.h>
#include "svc_services.h"
#include "svc_wdt.h"

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

#define SVC_SHELL_NEWLINE               "\r\n"
#define SVC_SHELL_NEWLINE_CLEARLINE     "\r\n\x1B[K"
#define SVC_SHELL_CLEARLINE             "\x1B[K"
#define SVC_SHELL_CLEARSCREEN           "\x1b[2J\x1B[H"
#define SVC_SHELL_CLEARPAGE             "\x1B[J"
#define SVC_SHELL_HOME                  "\x1B[H"

#define SVC_SHELL_ANSI_CLEAR            "\e[0m"
#define SVC_SHELL_ANSI_GREEN            "\e[32m"
#define SVC_SHELL_ANSI_YELLOW           "\e[33m"
#define SVC_SHELL_ANSI_RED              "\e[31m"

/*===========================================================================*/
/* Constants                                                                 */
/*===========================================================================*/

#define SVC_SHELL_OUT_NULL               (0)
#define SVC_SHELL_OUT_STD                (1)
#define SVC_SHELL_OUT_ERR                (2)
#define SVC_SHELL_OUT_SYS                (3)
#define SVC_SHELL_IN_STD                 (4)

#define SVC_SHELL_CMD_E_OK               (0)
#define SVC_SHELL_CMD_E_FAIL             (-1)
#define SVC_SHELL_CMD_E_NOT_FOUND        (-2)
#define SVC_SHELL_CMD_E_PARMS            (-3)
#define SVC_SHELL_CMD_E_BUSY             (-4)
#define SVC_SHELL_CMD_E_WOULD_BLOCK      (-5)
#define SVC_SHELL_CMD_E_NOT_IMPL         (-6)
#define SVC_SHELL_CMD_E_NOT_READY        (-7)
#define SVC_SHELL_CMD_E_MEMORY           (-8)
#define SVC_SHELL_CMD_E_BREAK            (-9)
#define SVC_SHELL_CMD_E_CANCEL           (-10)
#define SVC_SHELL_CMD_E_EXIST            (-11)
#define SVC_SHELL_CMD_E_EOF              (-12)

#define SVC_SHELL_ARGC_MAX               14
#define SVC_SHELL_PRINT_BUFFER_SIZE      488
#define SVC_SHELL_LINE_SIZE_MAX          256
#define SVC_SHELL_LINE_STRSUB_SIZE_MAX   320

#ifndef ALIGN
#ifdef CFG_PORT_POSIX
#define ALIGN           __attribute__ ((aligned (4)))
#else
#define ALIGN
#endif
#endif

#if !defined SVC_SHELL_COMMAND_SEPARATOR 
#define SVC_SHELL_COMMAND_SEPARATOR      '&'
#endif

/*===========================================================================*/
/* Macros.                                                                   */
/*===========================================================================*/

#define SVC_SHELL_MALLOC(size)          qoraal_malloc(QORAAL_HeapAuxiliary, size)
#define SVC_SHELL_FREE(mem)             qoraal_free(QORAAL_HeapAuxiliary, mem)


/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/

typedef int32_t (*SVC_SHELL_OUT_FP)(void* /*ctx*/, uint32_t /*out*/, const char* /*str*/);
typedef int32_t (*SVC_SHELL_IN_FP)(void* /*ctx*/);

typedef struct  SVC_SHELL_IF_S {
    void*                   ctx ;
    SVC_SHELL_OUT_FP        out ;
    SVC_SHELL_IN_FP         in ;
    int32_t                 status ;
    uint32_t                recurse ;
    SVC_WDT_HANDLE_T        wdt ;
} SVC_SHELL_IF_T ;


typedef int32_t (*SVC_SHELL_CMD_FP)(SVC_SHELL_IF_T * pif, char** /*argv*/, int /*argc*/);

typedef struct __attribute__((packed)) SVC_SHELL_CMD_S {
    const char*         cmd ;
    SVC_SHELL_CMD_FP     fp ;
    const char*         usage ;
} SVC_SHELL_CMD_T ;


typedef struct SVC_SHELL_CMD_LIST_S {
    struct SVC_SHELL_CMD_LIST_S*         next ;
    SVC_SERVICES_T         	service ;
    const SVC_SHELL_CMD_T *  cmds;
    uint32_t                cnt;
} SVC_SHELL_CMD_LIST_T ;


#define SVC_SHELL_CMD_LIST_START(name, service) \
        const SVC_SHELL_CMD_T    _qshell_##name[] ; \
        SVC_SHELL_CMD_LIST_T     _qshell_##name##_list = { 0, service, _qshell_##name } ; \
        const SVC_SHELL_CMD_T    _qshell_##name[] = {
#define SVC_SHELL_CMD_LIST(name, function, usage)    { name, function, usage },
#define SVC_SHELL_CMD_LIST_END() {0, 0, 0}   } ;

#define SVC_SHELL_CMD_LIST_INSTALL(name) \
        extern SVC_SHELL_CMD_LIST_T  _qshell_##name##_list ; \
        svc_shell_install (&_qshell_##name##_list) ;

#define SVC_SHELL_CMD_LIST_UNINSTALL(name) \
        extern SVC_SHELL_CMD_LIST_T  _qshell_##name##_list ; \
        svc_shell_uninstall (&_qshell_##name##_list) ;

/*
        __qshell_cmds_base__ = .;
        KEEP(*(SORT(.qshell.cmds.*)))
        . = ALIGN(4);
         __qshell_cmds_end__ = .;
*/

#define CONCAT_LINE_HELPER(x, y, z) x##y##z
#define CONCAT_LINE(x, y, z) CONCAT_LINE_HELPER(x, y, z)

#define SVC_SHELL_CMD_DECL(name, function, usage)                            \
    static int32_t function (SVC_SHELL_IF_T * pif, char** argv, int argc) ;        \
    static const SVC_SHELL_CMD_T CONCAT_LINE(__qoraalcmd_, __LINE__, function) \
        __attribute__((used, section(".qshell.cmds." #function), aligned(1))) = \
    {                                                                        \
        name,                                                                \
        function,                                                            \
        usage                                                                \
    }



/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    /*===========================================================================*/
    /* qshell  interface.                                                    */
    /*===========================================================================*/

    extern int32_t      svc_shell_init (void) ;
    extern int32_t      svc_shell_start (void) ;
    extern int32_t      svc_shell_stop (void) ;

    extern int32_t      svc_shell_if_init (SVC_SHELL_IF_T * pif, void* ctx, SVC_SHELL_OUT_FP out, SVC_SHELL_IN_FP in) ;
    extern size_t       svc_shell_cmd_split(char *buffer, size_t len, char *argv[], size_t argv_size) ;
    extern int32_t      svc_shell_cmd_run (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
    extern int32_t      svc_shell_script_run (SVC_SHELL_IF_T * pif, const char* name, char* start, int length) ;
    extern int32_t      svc_shell_script_clear_last_error (SVC_SHELL_IF_T * pif) ;
    extern void         svc_shell_wdt_kick (SVC_SHELL_IF_T * pif) ;
    extern void         svc_shell_wdt_activate (SVC_SHELL_IF_T * pif) ;
    extern void         svc_shell_wdt_deactivate (SVC_SHELL_IF_T * pif) ;

    extern uint32_t     svc_shell_cmd_help (char *buffer, size_t len) ;
    extern uint32_t     svc_shell_install (SVC_SHELL_CMD_LIST_T * list) ;
    extern uint32_t     svc_shell_uninstall (SVC_SHELL_CMD_LIST_T * list) ;

    extern int32_t      svc_shell_scan_int (const char * str, uint32_t * val) ;
    extern int32_t      svc_shell_print (SVC_SHELL_IF_T * pif, uint32_t out, const char * fmtstr, ...) ;
    extern int32_t      svc_shell_print_table (SVC_SHELL_IF_T * pif, uint32_t out, const char * left, int32_t tabright, const char * fmtstr, ...) ;
    extern int32_t      svc_shell_write (SVC_SHELL_IF_T * pif, uint32_t out, const char * str, uint32_t len) ;
    extern int32_t      svc_shell_write_hex(SVC_SHELL_IF_T * pif, uint32_t out, const uint8_t * buffer, uint32_t len) ;

#ifdef __cplusplus
}
#endif

#endif /* __SVC_SHELL_H__ */
