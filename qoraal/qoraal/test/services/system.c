



#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_shell.h"
#include "services.h"

/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/

#define DBG_MESSAGE_SYSTEM(severity, fmt_str, ...)   DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), QORAAL_SERVICE_SYSTEM, fmt_str, ##__VA_ARGS__)

/*===========================================================================*/
/* Service local variables and types.                                        */
/*===========================================================================*/

static p_sem_t              _system_stop_sem ;
static SVC_TASKS_DECL       (_system_startup_task) ;
static SVC_TASKS_DECL       (_system_periodic_task) ;

/*===========================================================================*/
/* Service local functions.                                                  */
/*===========================================================================*/

static void     system_startup_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason) ;
static void     system_periodic_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason) ;
static int32_t  qshell_system_status (SVC_SHELL_IF_T * pif, char** argv, int argc) ;

SVC_SHELL_CMD_LIST_START(system, QORAAL_SERVICE_SYSTEM)
SVC_SHELL_CMD_LIST( "status", qshell_system_status,  "")
SVC_SHELL_CMD_LIST_END()

/*===========================================================================*/
/* Service Functions                                                         */
/*===========================================================================*/

/**
 * @brief       system_service_ctrl
 * @details
 * @note        For code SVC_SERVICE_CTRL_STATUS, if the return value is E_NOIMPL 
 *              the status will be determined by the svc_services module.
 *
 * @param[in] code
 * @param[in] arg
 *
 * @return              status
 *
 * @services
 */
int32_t
system_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT: {
        
     }
    break ;

    case SVC_SERVICE_CTRL_START:{
        DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_REPORT, 
                        "SYS   : : system starting...\r\n");
        os_sem_create (&_system_stop_sem, 0) ;
        svc_tasks_schedule (&_system_startup_task, system_startup_cb, 0,
                SERVICE_PRIO_QUEUE2, SVC_TASK_MS2TICKS(5000)) ;        
        svc_tasks_schedule (&_system_periodic_task, system_periodic_cb, 0,
                SERVICE_PRIO_QUEUE4, SVC_TASK_S2TICKS(40)) ;    
        SVC_SHELL_CMD_LIST_INSTALL(system) ;
        } 
        break ;

    case SVC_SERVICE_CTRL_STOP: {
        DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_REPORT, 
                        "SYS   : : system stopping...\r\n");
        SVC_SHELL_CMD_LIST_UNINSTALL(system) ;
        svc_tasks_cancel (&_system_startup_task) ;
        svc_tasks_cancel (&_system_periodic_task) ;
        os_sem_signal (&_system_stop_sem) ;
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
system_service_run (uintptr_t arg)
{
    DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_INFO, "SYS   : : system started.");

    while (1) {
        if (os_sem_wait_timeout (&_system_stop_sem, OS_S2TICKS(120)) == EOK) {
            break ;
        }

        DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_REPORT, 
                        "SYS   : : system running...");

    }

    os_sem_delete (&_system_stop_sem) ;
    
    DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_REPORT, 
                    "SYS   : : system complete.");

    return EOK ;
}



static void
system_startup_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    if (reason == SERVICE_CALLBACK_REASON_RUN) {

        DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_REPORT, 
                        "SYS   : : system 'STARTUP TASK'...");

    }

    svc_tasks_complete (task) ;
}

static void
system_periodic_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    if (reason == SERVICE_CALLBACK_REASON_RUN) {

        DBG_MESSAGE_SYSTEM (DBG_MESSAGE_SEVERITY_LOG, 
                        "SYS   : : system 'PERIODIC TASK'...");

        svc_tasks_schedule (&_system_periodic_task, system_periodic_cb, 0,
                SERVICE_PRIO_QUEUE4, SVC_TASK_S2TICKS(90)) ;        

    }

    svc_tasks_complete (task) ;
}

int32_t
qshell_system_status (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    int32_t res = SVC_SHELL_CMD_E_OK ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD, "'PERIODIC TASK' scheduled in %d seconds\r\n",
            SVC_TASK_TICKS2MS(svc_task_expire(&_system_periodic_task))/1000) ;
    
    return res ;
}

