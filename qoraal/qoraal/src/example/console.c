/*
 *  Copyright (C) 2015-2025, Navaro, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of CORAL Connect (https://navaro.nl)
 */

#if defined CFG_OS_POSIX
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"


/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/

SVC_SERVICES_T      _console_service_id = SVC_SERVICES_INVALID ;   

#define DBG_MESSAGE_SHELL(severity, fmt_str, ...)   DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), _console_service_id, fmt_str, ##__VA_ARGS__)

#define SHELL_VERSION_STR   "Navaro Qoraal Demo v '" __DATE__ "'"
#define SHELL_HELLO         "Enter 'help' or '?' to view available commands. "
#define SHELL_PROMPT        "[Qoraal] #> "

/*===========================================================================*/
/* Service Local Functions                                                   */
/*===========================================================================*/
static void     console_logger_cb (void* channel, LOGGER_TYPE_T type, uint8_t facility, const char* msg) ;
static int32_t  console_out (void* ctx, uint32_t out, const char* str);
static int32_t  console_get_line (char * buffer, uint32_t len) ;

SVC_SHELL_CMD_DECL("exit", qshell_cmd_exit, "");
SVC_SHELL_CMD_DECL("version", qshell_cmd_version, "");
SVC_SHELL_CMD_DECL("hello", qshell_cmd_hello, "");

extern void keep_posixcmds (void) ;

/*===========================================================================*/
/* Service Local Variables and Types                                         */
/*===========================================================================*/

static bool                 _shell_exit = false ;
static LOGGER_CHANNEL_T     _shell_log_channel = { .fp = console_logger_cb, .user = (void*)0, .filter = { { .mask = SVC_LOGGER_MASK, .type = SVC_LOGGER_SEVERITY_LOG | SVC_LOGGER_FLAGS_PROGRESS }, {0,0} } };

/*===========================================================================*/
/* Service Functions                                                         */
/*===========================================================================*/

/**
 * @brief       console_service_ctrl
 * @details
 * @note        For code SVC_SERVICE_CTRL_STATUS, if the return value is E_NOIMPL
 *              the status will be determined by the svc_services module.
 *
 * @param[in] code
 * @param[in] arg
 *
 * @return      status
 *
 * @services
 */
int32_t
console_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        keep_posixcmds () ;
        _console_service_id = svc_service_service ((SCV_SERVICE_HANDLE) arg ) ;
        break ;

    case SVC_SERVICE_CTRL_START:
        svc_logger_channel_add (&_shell_log_channel) ;
        break ;

    case SVC_SERVICE_CTRL_STOP: 
        DBG_MESSAGE_SHELL(DBG_MESSAGE_SEVERITY_LOG, "SHELL : : shell shutting down...");
        svc_logger_channel_remove (&_shell_log_channel) ;
        _shell_exit = true;
        break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        res = E_NOIMPL ;
        break ;

    }

    return res ;
}

/**
 * @brief       console_service_run
 * @details     Runs the shell service, processing input and executing commands
 *              until the "exit" command is issued.
 *
 * @param[in]   arg     Argument passed to the service.
 *
 * @return      status  The result of the shell execution.
 */
int32_t
console_service_run (uintptr_t arg)
{
    DBG_MESSAGE_SHELL (DBG_MESSAGE_SEVERITY_INFO, "SHELL : : shell STARTED");

    SVC_SHELL_IF_T  qshell_cmd_if ;
    svc_shell_if_init (&qshell_cmd_if, 0, console_out, 0) ;

    /*
     * Now process the input from the command line as shell commands until
     * the "exit" command is executed.
     */
    svc_shell_script_run (&qshell_cmd_if, "", "version", strlen("version")) ;
    svc_shell_script_run (&qshell_cmd_if, "", "hello", strlen("hello")) ;
    do {
        char line[1024];
        printf (SHELL_PROMPT) ;
        int len = console_get_line (line, sizeof(line)) ;
        if (!_shell_exit && len > 0) {
            svc_shell_script_run (&qshell_cmd_if, "", line, len) ;
            
        }

    } while (!_shell_exit) ;


    return EOK ;
}

/**
 * @brief       console_out
 * @details     Handles shell output operations.
 *
 * @param[in]   ctx     The context for the output operation.
 * @param[in]   out     The output channel.
 * @param[in]   str     The string to output.
 *
 * @return      status  The result of the operation.
 */
int32_t
console_out (void* ctx, uint32_t out, const char* str)
{
    if (str && (out && out < SVC_SHELL_IN_STD)) {
        printf ("%s", str) ;

    }

    return  SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       console_get_line
 * @details     Reads a line of input from the user.
 *
 * @param[out]  buffer  The buffer to store the input line.
 * @param[in]   len     The maximum length of the buffer.
 *
 * @return      length  The length of the input line.
 */
int32_t
console_get_line (char * buffer, uint32_t len)
{
    uint32_t i = 0 ;

    for (i=0; i<len; i++) {
        int c = getc(stdin);

        if (_shell_exit) break;

        if (c == EOF) {
            // If EOF is due to `/dev/null`, prevent infinite loop
            os_thread_sleep (1000);
            continue;
        }

        if (c == '\n') break;
        buffer[i] = (char)c;

    }

    return i ;
}

/**
 * @brief       console_logger_cb
 * @details     Callback function for logging messages from the shell.
 *
 * @param[in]   channel     The logger channel.
 * @param[in]   type        The type of log message.
 * @param[in]   facility    The logging facility.
 * @param[in]   msg         The log message to display.
 */
void
console_logger_cb (void* channel, LOGGER_TYPE_T type, uint8_t facility, const char* msg)
{
    printf("--- %s\n", msg) ;
}

typedef struct {
    SVC_SERVICES_T id ;
    p_sem_t sem ;
} CONSOLE_EXIT_T ;

static void 
status_callback (SVC_SERVICES_T  id, int32_t status, uintptr_t parm)
{
    CONSOLE_EXIT_T * pexit = (CONSOLE_EXIT_T *)parm ;
    p_sem_t    stop_sem = (p_sem_t) parm ;
    if ((status == SVC_SERVICE_STATUS_STOPPED || status == SVC_SERVICE_STATUS_STOPPING) && 
        (id == pexit->id || id == _console_service_id)) {
        os_sem_signal (&pexit->sem) ;
    }
}

void
console_wait_for_exit (SVC_SERVICES_T  id)
{
    CONSOLE_EXIT_T exit ;
    p_sem_t    stop_sem ;
    os_sem_create (&stop_sem, 0) ;

    exit.sem = stop_sem ;
    exit.id = id ;

    SVC_SERVICE_HANDLER_T  handler ;
    svc_service_register_handler (&handler, status_callback, (uintptr_t) &exit) ;
    os_sem_wait (&stop_sem) ;
    svc_service_unregister_handler (&handler) ;
    os_sem_delete (&stop_sem) ;
}


/**
 * @brief       qshell_cmd_version
 * @details     Outputs the version of the Qoraal shell.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_cmd_version (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n", SHELL_VERSION_STR) ;
    return SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       qshell_cmd_hello
 * @details     Outputs hello text of the Qoraal shell.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_cmd_hello (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n\r\n", SHELL_HELLO) ;
    return SVC_SHELL_CMD_E_OK ;
}

/**
 * @brief       qshell_cmd_exit
 * @details     Exits the shell service.
 *
 * @param[in]   pif     Shell interface pointer.
 * @param[in]   argv    Command-line arguments.
 * @param[in]   argc    Number of command-line arguments.
 *
 * @return      status  The result of the command execution.
 */
int32_t
qshell_cmd_exit (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    _shell_exit = true ;
    return SVC_SHELL_CMD_E_OK ;
}




#endif