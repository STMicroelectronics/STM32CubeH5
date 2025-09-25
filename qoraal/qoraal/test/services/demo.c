#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "services.h"

/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/

#define DBG_MESSAGE_DEMO(severity, fmt_str, ...)   DBG_MESSAGE_T_LOG (SVC_LOGGER_TYPE(severity,0), QORAAL_SERVICE_DEMO, fmt_str, ##__VA_ARGS__)

/*===========================================================================*/
/* Service Local Functions                                                   */
/*===========================================================================*/

static int32_t      qshell_demo_threads (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t      qshell_demo_tasks (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t      qshell_demo_waitable_tasks (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t      qshell_demo_events (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t      qshell_demo_timers (SVC_SHELL_IF_T * pif, char** argv, int argc) ;
static int32_t      qshell_demo_dbg (SVC_SHELL_IF_T * pif, char** argv, int argc) ;


SVC_SHELL_CMD_LIST_START(demo, QORAAL_SERVICE_DEMO)
SVC_SHELL_CMD_LIST( "demo_threads", qshell_demo_threads,  "")
SVC_SHELL_CMD_LIST( "demo_tasks", qshell_demo_tasks,  "")
SVC_SHELL_CMD_LIST( "demo_waitable_tasks", qshell_demo_waitable_tasks,  "")
SVC_SHELL_CMD_LIST( "demo_events", qshell_demo_events,  "")
SVC_SHELL_CMD_LIST( "demo_timers", qshell_demo_timers,  "")
SVC_SHELL_CMD_LIST( "demo_dbg", qshell_demo_dbg,  "")
SVC_SHELL_CMD_LIST_END()

/*===========================================================================*/
/* Service Functions                                                         */
/*===========================================================================*/

/**
 * @brief       demo_service_ctrl
 * @details
 * @note
 * @param[in] code
 * @param[in] arg
 * @return              status
 *
 * @power
 */
int32_t
demo_service_ctrl (uint32_t code, uintptr_t arg)
{
    int32_t res = EOK ;

    switch (code) {
    case SVC_SERVICE_CTRL_INIT:
        break ;

    case SVC_SERVICE_CTRL_START: {
        DBG_MESSAGE_DEMO (DBG_MESSAGE_SEVERITY_REPORT, "DEMO  : : Starting") ;
        SVC_SHELL_CMD_LIST_INSTALL(demo) ;
    }
    break ;

    case SVC_SERVICE_CTRL_STOP: {
        SVC_SHELL_CMD_LIST_UNINSTALL(demo) ;
    }
    break ;

    case SVC_SERVICE_CTRL_STATUS:
    default:
        res = E_NOIMPL ;
        break ;

    }

    return res ;
}


//==================================================================================================
//  Test events
//==================================================================================================
static SVC_EVENTS_HANDLER_T _event_handler ;

void events_callback (SVC_EVENTS_T id, void * ctx)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T*) ctx ;
    if (id == SVC_EVENTS_USER+1) {
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "events - user event callback.\r\n") ;
    }

}

int32_t
qshell_demo_events (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "events - user event register.\r\n") ;
    svc_events_register (SVC_EVENTS_USER+1,  &_event_handler, events_callback, (void*)pif) ;


    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "events - user event signal.\r\n") ;
    svc_events_signal (SVC_EVENTS_USER+1) ;
    
    os_thread_sleep (500) ;
    svc_events_unregister (SVC_EVENTS_USER+1,  &_event_handler) ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "events - test complete.\r\n") ;

    return SVC_SHELL_CMD_E_OK ;
}

//==================================================================================================
//  Test waitable tasks
//==================================================================================================

static void
test_waitable_task_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T*) parm ;
    if (reason == SERVICE_CALLBACK_REASON_RUN) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "waitable task - callback.\r\n");

    }

    svc_tasks_complete (task) ;

}

int32_t
qshell_demo_waitable_tasks (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    SVC_WAITABLE_TASKS_T task  ;

    svc_tasks_init_waitable_task (&task) ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "waitable task - schedule 2 seconds.\r\n") ;
    svc_tasks_schedule ((SVC_TASKS_T*)&task, test_waitable_task_cb, (uintptr_t)pif,
            SERVICE_PRIO_QUEUE5, SVC_TASK_MS2TICKS(2000)) ;

    int32_t res = svc_tasks_wait ((SVC_TASKS_T*)&task, SVC_TASK_MS2TICKS(4000)) ;
    DBG_ASSERT_T (res == EOK, "qshell_demo_waitable_tasks unexpectes!")

    svc_tasks_deinit_waitable_task (&task) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "waitable task - test complete.\r\n") ;

    return SVC_SHELL_CMD_E_OK;
}




//==================================================================================================
//  Test tasks
//==================================================================================================

static void
test_tqask_cb (SVC_TASKS_T *task, uintptr_t parm, uint32_t reason)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T*) parm ;
    if (reason == SERVICE_CALLBACK_REASON_RUN) {
        svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "task - callback.\r\n");

    }

    svc_tasks_complete (task) ;
    qoraal_free (QORAAL_HeapAuxiliary, task) ;
}

int32_t
qshell_demo_tasks (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    SVC_TASKS_T * task = qoraal_malloc (QORAAL_HeapAuxiliary, sizeof(SVC_TASKS_T)) ;

    svc_tasks_init_task (task) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "task - schedule 2 seconds.\r\n") ;

    svc_tasks_schedule (task, test_tqask_cb, (uintptr_t)pif,
            SERVICE_PRIO_QUEUE5, SVC_TASK_MS2TICKS(2000)) ;

    os_thread_sleep (4000) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "task - test complete.\r\n") ;

    return SVC_SHELL_CMD_E_OK ;
}


//==================================================================================================
//  Test threads
//==================================================================================================

static void 
test_thread_complete (SVC_THREADS_T * thd, void* arg)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T*) arg ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
            "thread - complete callback.\r\n") ;
    qoraal_free (QORAAL_HeapAuxiliary, thd) ;

}

static void 
test_thread (void *arg)
{
    SVC_SHELL_IF_T * pif = (SVC_SHELL_IF_T*) arg ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
        "thread - started.\r\n") ;
    os_thread_sleep (3000) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
        "thread - terminating.\r\n") ;
}


int32_t
qshell_demo_threads (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    SVC_THREADS_T * thd = qoraal_malloc (QORAAL_HeapAuxiliary, sizeof(SVC_THREADS_T)) ;

    svc_shell_print (pif, SVC_SHELL_OUT_STD, 
        "thread - create.\r\n") ;
    svc_threads_create (thd, test_thread_complete, 1024, OS_THREAD_PRIO_5, test_thread, (void*)pif, "test_thread") ;

    os_thread_sleep (1000) ;
    svc_shell_print (pif, SVC_SHELL_OUT_STD, "thread - exit while thread running.\r\n") ;

    return SVC_SHELL_CMD_E_OK ;
}



//==================================================================================================
//  Test timer
//==================================================================================================
void 
timer_callback (void *param) 
{
    DBG_MESSAGE_DEMO(DBG_MESSAGE_SEVERITY_REPORT,
            "DEMO  : : timer - expired '%s'", (char *)param);
}

int32_t
qshell_demo_timers (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    p_timer_t timer1, timer2, timer3, timer4;

    os_timer_create(&timer1, timer_callback, (void*)"Timer 1 (1 second)");
    os_timer_create(&timer2, timer_callback, (void*)"Timer 2 (2 seconds)");
    os_timer_create(&timer3, timer_callback, (void*)"Timer 3 (0.5 seconds)");
    os_timer_create(&timer4, timer_callback, (void*)"Timer 4 (3 seconds)");

    os_timer_set(&timer1, 1000);
    os_timer_set(&timer2, 2000);
    os_timer_set(&timer3, 500);
    os_timer_set(&timer4, 3000);

    os_thread_sleep(2000);

    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "timer - is Timer 1 set? %d\r\n", os_timer_is_set(&timer1));
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "timer - is Timer 2 set? %d\r\n", os_timer_is_set(&timer2));
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "timer - is Timer 3 set? %d\r\n", os_timer_is_set(&timer3));
    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "timer - is Timer 4 set? %d\r\n", os_timer_is_set(&timer4));

    os_timer_reset(&timer4);

    os_thread_sleep(2000);

    svc_shell_print (pif, SVC_SHELL_OUT_STD,
            "timer - is Timer 4 set? %d\r\n", os_timer_is_set(&timer4));

    os_timer_delete(&timer1);
    os_timer_delete(&timer2);
    os_timer_delete(&timer3);
    os_timer_delete(&timer4);

    return SVC_SHELL_CMD_E_OK ;
}


int32_t
qshell_demo_dbg (SVC_SHELL_IF_T * pif, char** argv, int argc)
{
    char mem[128] ;

    svc_logger_type_mem (SVC_LOGGER_SEVERITY_REPORT, 0, mem, sizeof(mem), "MEM DUMP:\r\n", "\r\n") ;

    return SVC_SHELL_CMD_E_OK ;
}