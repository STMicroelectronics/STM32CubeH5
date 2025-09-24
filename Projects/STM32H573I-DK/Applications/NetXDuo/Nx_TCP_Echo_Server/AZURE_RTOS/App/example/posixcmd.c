
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

#include "qoraal/config.h"
#if CFG_OS_POSIX
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"

/*
 * Declare commands for use with the shell. Use the SVC_SHELL_CMD_DECL for the
 * commands to be accessible from the command shell interface.
 */
SVC_SHELL_CMD_DECL("ls", qshell_cmd_ls, "");
SVC_SHELL_CMD_DECL("cd", qshell_cmd_cd, "<path>");
SVC_SHELL_CMD_DECL("source", qshell_cmd_source, "<file>");
SVC_SHELL_CMD_DECL(".", qshell_cmd_source, "<file>");
SVC_SHELL_CMD_DECL("cat", qshell_cmd_cat, "<file>");
SVC_SHELL_CMD_DECL("pwd", qshell_cmd_pwd, "");
SVC_SHELL_CMD_DECL("echo", qshell_cmd_echo, "[string]");

static int32_t
qshell_cmd_ls (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    /* Here we will list the directory. */
    const char *dir = "." ;
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh) {
        if (errno == ENOENT) {
            svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                    "Directory doesn't exist");
        }
        else {
            svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                    "Unable to read directory");
        }
        return SVC_SHELL_CMD_E_FAIL ;
    }
    /* While the next entry is not readable we will print directory files */
    while ((d = readdir(dh)) != NULL) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "%s\r\n", d->d_name);
    }

    closedir(dh);

    return SVC_SHELL_CMD_E_OK ;
}

static int32_t
qshell_cmd_cd (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    /* Change the current directory. */
    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS ;
    }

    if (chdir(argv[1]) != 0) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "failed");
        return SVC_SHELL_CMD_E_FAIL ;

    }

    return SVC_SHELL_CMD_E_OK ;
}

static int32_t
qshell_cmd_pwd (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    /* Print the current directory. */
    char buffer[256] ;
    char * pbuffer = getcwd(buffer, 256);
    if (!pbuffer) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "unable to get current directory.\r\n");
        return SVC_SHELL_CMD_E_FAIL ;
    }

    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "%s\r\n", pbuffer);

    return SVC_SHELL_CMD_E_OK ;
}


static int32_t
read_file (SVC_SHELL_IF_T * pif, const char * filename, char ** pbuffer)
{
    *pbuffer = NULL ;

    /*
     * Read the script specified on the command line.
     */
    FILE * fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "unable to open file \"%s\" for read.\r\n", filename);
         return SVC_SHELL_CMD_E_NOT_FOUND ;

    }
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (!sz) {
        fclose(fp);
        return SVC_SHELL_CMD_E_EOF ;
    }

    char * buffer = malloc (sz + 1 ) ;
    if (!buffer) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "out of memory.\r\n");
        fclose(fp);
         return SVC_SHELL_CMD_E_MEMORY ;

    }
    long num = fread( buffer, 1, sz, fp );
    if (!num) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
                "unable to read file \"%s\".\r\n", filename);
        fclose(fp);
        free (buffer) ;
         return SVC_SHELL_CMD_E_FAIL ;

    }
    fclose(fp);

    buffer[num] = '\0' ;
    *pbuffer = buffer ;

    return sz ;
}

static int32_t
qshell_cmd_source (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res  ;

    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS ;
    }

    char * buffer ;

    res = read_file(pif, argv[1], &buffer) ;
    if (res > 0) {

        /*
         * Run the script read from the file.
         */
        res = svc_shell_script_run (pif, "", buffer, res) ;

        free (buffer) ;

    }

    return res ;
}

static int32_t
qshell_cmd_cat (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res  ;

    if (argc < 2) {
        return SVC_SHELL_CMD_E_OK ;
    }

    char * buffer ;

    res = read_file(pif, argv[1], &buffer) ;
    if (res > 0) {

        svc_shell_print (pif, SVC_SHELL_OUT_STD, buffer) ;
        svc_shell_print (pif, SVC_SHELL_OUT_STD, "\r\n") ;
        free (buffer) ;
        res = SVC_SHELL_CMD_E_OK ;

    }

    return res ;
}

static int32_t
qshell_cmd_echo (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    /*
     * Echo the first argument. Can be used to demostrate the string
     * substitution for registry strings, eg. "echo [test]"
     */
    if (argc < 2) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, "\r\n") ;

    } else {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, "%s\r\n", argv[1]) ;

    }

    return SVC_SHELL_CMD_E_OK ;
}

void
keep_posixcmds (void) 
{
    (void)qshell_cmd_ls;
    (void)qshell_cmd_cd ;
    (void)qshell_cmd_source ;
    (void)qshell_cmd_source ;
    (void)qshell_cmd_cat ;
    (void)qshell_cmd_pwd ;
    (void)qshell_cmd_echo ;
}
#endif /* CFG_OS_POSIX */
