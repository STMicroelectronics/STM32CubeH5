#include <stdio.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal-flash/syslog.h"

SVC_SHELL_CMD_DECL( "log", qshell_cmd_log,  "[cnt] [severity]");
SVC_SHELL_CMD_DECL( "assertlog", qshell_cmd_assertlog,  "[cnt] [severity]");

static int32_t _log (SVC_SHELL_IF_T * pif, uint32_t log, uint32_t severity, uint32_t cnt)
{
#define LOG_MSG_SIZE    (sizeof(QORAAL_LOG_MSG_T) + 200)
    QORAAL_LOG_MSG_T *  msg = qoraal_malloc(QORAAL_HeapAuxiliary, LOG_MSG_SIZE) ;
    QORAAL_LOG_IT_T * it = 0 ;

    it = syslog_platform_it_create (log) ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "%s log - severity<=%d:" SVC_SHELL_NEWLINE
            "---------------------------------------" SVC_SHELL_NEWLINE,
            log ? "Assert" : "System", severity) ;

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
        syslog_platform_it_destroy (it) ;

    }

    qoraal_free (QORAAL_HeapAuxiliary, msg) ;

    return SVC_SHELL_CMD_E_OK ;
}


int32_t qshell_cmd_log (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    unsigned int cnt = 16 ;
    unsigned int severity = 5 ;

    if (argc > 1) {
        sscanf(argv[1], "%u", &cnt) ;

    }
    if (argc > 2) {
        sscanf(argv[2], "%u", &severity) ;

    }

    return _log (pif, 0,  severity,  cnt) ;
}

int32_t qshell_cmd_assertlog (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    unsigned int cnt = 16 ;
    unsigned int severity = 6 ;

    if (argc > 1) {
        sscanf(argv[1], "%u", &cnt) ;

    }
    if (argc > 2) {
        sscanf(argv[2], "%u", &severity) ;

    }

    return _log (pif, 1,  severity,  cnt) ;
}


void
keep_syslogcmds(void)
{
    (void)qshell_cmd_log ;
}