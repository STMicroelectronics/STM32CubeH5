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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-engine/engine.h"
#include "qoraal-engine/starter.h"
#include "services.h"


/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/


#define DBG_MESSAGE_ENGIE(severity, fmt_str, ...)   DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), QORAAL_SERVICE_ENGINE, fmt_str, ##__VA_ARGS__)



/*===========================================================================*/
/* Service Local Functions                                                   */
/*===========================================================================*/

static int32_t  qshell_cmd_run (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t  qshell_cmd_compile (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t  qshell_cmd_list (SVC_SHELL_IF_T * pif, char** argv, int argc) ;


static int32_t qshell_cmd_event (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t qshell_cmd_trans (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t qshell_cmd_engine_dbg (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t qshell_cmd_engine (SVC_SHELL_IF_T * pif, char** argv, int argc) ;


SVC_SHELL_CMD_LIST_START(engine, QORAAL_SERVICE_ENGINE)
SVC_SHELL_CMD_LIST("run", qshell_cmd_run, "<filename>")
SVC_SHELL_CMD_LIST("compile", qshell_cmd_compile, "<filename> [verbose]")



SVC_SHELL_CMD_LIST_END()

static void     engine_startup (void) ;

/*===========================================================================*/
/* Service Local Variables and Types                                         */
/*===========================================================================*/

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
engine_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        starter_init (0) ;
        break ;

    case SVC_SERVICE_CTRL_START: {
        SVC_SHELL_CMD_LIST_INSTALL(engine) ;
        engine_startup () ;
        }
        break ;

    case SVC_SERVICE_CTRL_HALT:
    case SVC_SERVICE_CTRL_STOP: {
        SVC_SHELL_CMD_LIST_UNINSTALL(engine) ;
        starter_stop () ;
        }
        break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        res = E_NOIMPL ;
        break ;

    }

    return res ;
}



int32_t
engine_machine_start (const char *filename, void* ctx, STARTER_OUT_FP log_cb, bool start, bool verbose)
{
    int32_t res ;
    /*
    * Read the Machine Definition File specified on the command line.
    */
    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return E_NOTFOUND;
    }

    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char *buffer = qoraal_malloc(QORAAL_HeapAuxiliary, sz);
    if (!buffer) {
        fclose(fp);
        return E_NOMEM;
    }

    long num = fread(buffer, 1, sz, fp);
    if (!num) {
        fclose(fp);
        qoraal_free(QORAAL_HeapAuxiliary, buffer);
        return E_FILE;
    }

    fclose(fp);

    /*
     * Lets get the engine started...
     */
    starter_stop();
    if (start) {
        res = starter_start (buffer, sz, ctx, log_cb, verbose);
        if (res) {
            starter_stop ();

        }

    } else {
        res = starter_compile (buffer, sz, ctx, log_cb, verbose) ;

    }

    qoraal_free (QORAAL_HeapAuxiliary, buffer);

    return res;
}

void
engine_machine_stop (void)
{
    starter_stop ();
}


static int32_t
starter_out(void* ctx, uint32_t out, const char* str)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T *) ctx ;
    if (pif) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s", str) ;
        size_t len = strlen(str) ;
        if (str[len-1] != '\n') svc_shell_print (pif, SVC_SHELL_OUT_STD,  ("\r\n")) ;
    } else {
        DBG_MESSAGE_ENGIE (DBG_MESSAGE_SEVERITY_REPORT, "ENG   : : %s", str) ;
    }

    return 0 ;
}

static void
engine_startup(void)
{
    DBG_MESSAGE_ENGIE (DBG_MESSAGE_SEVERITY_REPORT, 
            "ENG   : : starting default machine '%s'", ENGINE_STARTUP_MACHINE) ;
    engine_machine_start (ENGINE_STARTUP_MACHINE, 0, starter_out, true, false) ;
}

int32_t
qshell_cmd_run(SVC_SHELL_IF_T *pif, char **argv, int argc)
{
    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS;
    }

    int32_t res = engine_machine_start (argv[1], pif, starter_out, true, false);

    switch (res) {
    case E_NOTFOUND:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: unable to open file \"%s\" for read.\r\n", argv[1]);
        break;
    case E_NOMEM:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: out of memory.\r\n");
        break;
    case E_FILE:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: unable to read file \"%s\".\r\n", argv[1]);
        break;
    default:
        if (res != EOK) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD,
                            "starting \"%s\" failed with %d\r\n\r\n",
                            argv[1], (int)res);
        }
        break;
    }

    return res == EOK ? SVC_SHELL_CMD_E_OK : res;
}

int32_t
qshell_cmd_compile (SVC_SHELL_IF_T *pif, char **argv, int argc)
{
    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS;
    }

    int32_t res = engine_machine_start (argv[1], pif, starter_out, false, argc > 2);

    switch (res) {
    case E_NOTFOUND:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: unable to open file \"%s\" for read.\r\n", argv[1]);
        break;
    case E_NOMEM:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: out of memory.\r\n");
        break;
    case E_FILE:
        svc_shell_print(pif, SVC_SHELL_OUT_STD,
                        "terminal failure: unable to read file \"%s\".\r\n", argv[1]);
        break;
    default:
        if (res != EOK) {
            svc_shell_print(pif, SVC_SHELL_OUT_STD,
                            "compiling \"%s\" failed with %d\r\n\r\n",
                            argv[1], (int)res);
        }
        break;
    }

    return res == EOK ? SVC_SHELL_CMD_E_OK : res;

}
