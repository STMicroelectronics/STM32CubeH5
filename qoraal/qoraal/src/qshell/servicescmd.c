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
#include "qoraal/config.h"
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal/common/mlog.h"
#include "qoraal/common/logit.h"

SVC_SHELL_CMD_DECL("ctrl", qshell_cmd_ctrl, "[service name] [start/stop/restart] [arg]");
SVC_SHELL_CMD_DECL( "logmsg", qshell_cmd_logmsg,  "<msg> [severity]" );
SVC_SHELL_CMD_DECL( "sleep", qshell_cmd_sleep, "<msec>");
SVC_SHELL_CMD_DECL( "cls", qshell_cmd_cls,  "");
#if !defined CFG_COMMON_MEMLOG_DISABLE
SVC_SHELL_CMD_DECL( "dmesg", qshell_cmd_dmesg,  "[severity] [count]");
#endif

/**
 * @brief Sorting the Services in an alphabetic order
 * @notes   get to pointer, compare them and return -1 or 0 in
 * @param[in]*a
 * @param[in]*b
 * @return -1, 0
 */
static int 
compare_by_handle(const void *a, const void *b) {
    SCV_SERVICE_HANDLE ha = *(SCV_SERVICE_HANDLE*)a ;
    SCV_SERVICE_HANDLE hb = *(SCV_SERVICE_HANDLE*)b ;
    const char * namea  = svc_service_name(ha) ;
    const char * nameb  = svc_service_name(hb) ;

    // Check for NULL pointers just in case svc_service_name can return NULL
    if (!namea && !nameb) return 0;
    if (!namea) return -1;  // NULL should come before valid name
    if (!nameb) return 1;   // valid name should come before NULL

    return strcmp(namea, nameb);
}

static int32_t
qshell_cmd_ctrl (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    SCV_SERVICE_HANDLE h ;
    int i, j ;
    int services_cnt = 0 ;

    if (argc < 2) {
        for (h = svc_service_first(); h!=SVC_SERVICE_INVALID_HANDLE; ) {
            //count how many services we have
            services_cnt++;
            h = svc_service_next(h) ;

        }
        SCV_SERVICE_HANDLE services [services_cnt] ;
        for (i=0, h = svc_service_first(); h!=SVC_SERVICE_INVALID_HANDLE; i++) {
            //copy the services in to the array
            services[i] = h ;
            h = svc_service_next(h) ;

        }
        qsort(services, services_cnt, sizeof(SCV_SERVICE_HANDLE), compare_by_handle);
        //sort the created array
        for ( i = 0, j = 0; i < services_cnt ; i++) {
            //print the array
            if (svc_service_status(services[i]) != SVC_SERVICE_STATUS_RESIDENT) {
                char tmp [48];
                snprintf(tmp, sizeof(tmp), "%d%s   %s", j, j < 10 ? " " : "",
                        svc_service_name(services[i])) ;
                svc_shell_print_table(pif,  SVC_SHELL_OUT_STD, tmp, 20,
                        "%s\r\n", (char*) svc_service_status_name(services[i])) ;
                j++ ;

            }

        }
        return EOK ;

    }

    h = svc_service_get_by_name(argv[1]) ;
    if (h == SVC_SERVICE_INVALID_HANDLE) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "service %s not found\r\n", argv[1]) ;
        return EFAIL ;

    }

    if (argc == 2) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "%s %s\r\n", svc_service_name(h), svc_service_status_name(h)) ;
        return EOK ;

    }

    bool stop = strcasecmp("stop", argv[2]) == 0 ;
    bool start = strcasecmp("start", argv[2]) == 0 ;
    bool restart = strcasecmp("restart", argv[2]) == 0 ;

    if (!stop && !start && !restart) {
        return SVC_SHELL_CMD_E_PARMS ;

    }

    if (stop || restart) {
        int32_t res = svc_service_stop_timeout (h, 5000) ;
        if (res != EOK) {
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "ERROR: Stopping '%s' service failed with %d\r\n", argv[1], res) ;
            return res ;

        }

    }

    if (start || restart) {
        uint32_t arg = 0 ;
        if (argc > 3) {
            svc_shell_scan_int(argv[3], &arg) ;

        }
        int32_t res = svc_service_start_timeout (h, arg, 5000) ;
        if (res != EOK) {
            svc_shell_print (pif, SVC_SHELL_OUT_STD,
                "ERROR: Starting '%s' service failed with %d\r\n", argv[1], res) ;
            return res ;

        }

    }

    return EOK ;
}

static int32_t
qshell_cmd_logmsg (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    uint32_t level = SVC_LOGGER_SEVERITY_REPORT ;

    if (argc < 2) {
        return SVC_SHELL_CMD_E_PARMS ;
    }
    if (argc > 2) {
        sscanf(argv[2], "%u", (unsigned int*)&level) ;
    }

    return svc_logger_type_log (level, 0, argv[1]) ;
}

static int32_t
qshell_cmd_sleep(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    uint32_t timeout = 0 ;
    if (argc > 1) {
            sscanf(argv[1], "%u", (unsigned int*)&timeout) ;

    }
#if 0
    RTCLIB_TIME_T time = rtc_get_time () ;

    uint32_t elapsed = STATS_TIMER_GET() ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "start %.2d:%.2d:%.2d: %u (%u) " SVC_SHELL_NEWLINE ,
            time.hour, time.minute, time.second, os_sys_timestamp(), STATS_TIMER_GET()) ;
    os_thread_sleep(timeout) ;
    time = rtc_get_time () ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "stop  %.2d:%.2d:%.2d: %d (%u %u)" SVC_SHELL_NEWLINE ,
            time.hour, time.minute, time.second, os_sys_timestamp(), STATS_TIMER_GET(),
            STATS_TIMER_GET() - elapsed) ;
#else
    os_thread_sleep (timeout) ;
#endif
    return SVC_SHELL_CMD_E_OK ;
}

static int32_t 
qshell_cmd_cls(SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "\x1b[2J\x1B[H" ) ;
    return EOK ;
}

#if !defined CFG_COMMON_MEMLOG_DISABLE
static int32_t 
qshell_cmd_dmesg (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
#define LOG_MSG_SIZE    (sizeof(QORAAL_LOG_MSG_T) + 200)
    QORAAL_LOG_MSG_T *  msg = qoraal_malloc(QORAAL_HeapAuxiliary, LOG_MSG_SIZE) ;
    unsigned int cnt = 16 ;
    unsigned int severity = 6 ;
    QORAAL_LOG_IT_T * it = 0 ;

    if (argc > 1) {
        sscanf(argv[1], "%u", &severity) ;

    }
    if (argc > 2) {
        sscanf(argv[2], "%u", &cnt) ;

    }

    it = mlog_platform_it_create (MLOG_DBG) ;


    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "severity<=%d   (log [severity] [count])" SVC_SHELL_NEWLINE
            "---------------------------------------" SVC_SHELL_NEWLINE,
            severity) ;

    if (it) {
        while (cnt && (it->get (it, msg, LOG_MSG_SIZE) >= EOK)) {

            if (msg->severity <= severity) {

                svc_shell_print (pif, SVC_SHELL_OUT_STD,
                        "%.6d (%d) - "
                        "%.4d-%.2d-%.2d "
                        "%.2d:%.2d:%.2d:  "
                        "%s\r\n" ,
                        msg->id,
                        msg->severity,
                        msg->date.year, msg->date.month, msg->date.day,
                        msg->time.hour, msg->time.minute, msg->time.second,
                        msg->msg) ;

                cnt-- ;

            }

            if (it->prev(it) != EOK) break ;

        }
        mlog_platform_it_destroy (it) ;

    }

    qoraal_free (QORAAL_HeapAuxiliary, msg) ;

    return SVC_SHELL_CMD_E_OK ;
}
#endif

void
keep_servicescmds (void)
{
    (void)qshell_cmd_ctrl ;
    (void)qshell_cmd_logmsg ;
    (void)qshell_cmd_sleep ;
    (void)qshell_cmd_cls ;
#if !defined CFG_COMMON_MEMLOG_DISABLE
    (void)qshell_cmd_dmesg ;
#endif    
}


