#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"
#include "services/www/wserver_inst.h"
#include "qoraal/example/platform.h"
#include "qoraal/example/console.h"
#include "services/services.h"

/*===========================================================================*/
/* Macros and Defines                                                        */
/*===========================================================================*/



/*===========================================================================*/
/* Service Local Variables and Types                                         */
/*===========================================================================*/


SVC_SERVICE_LIST_START(_qoraal_services_list)
SVC_SERVICE_RUN_DECL("shell",  console_service_run, console_service_ctrl, 0, 6000, OS_THREAD_PRIO_8, QORAAL_SERVICE_SHELL, SVC_SERVICE_FLAGS_AUTOSTART)
SVC_SERVICE_DECL("engine", engine_service_ctrl, 0, QORAAL_SERVICE_ENGINE, SVC_SERVICE_FLAGS_AUTOSTART)
SVC_SERVICE_RUN_DECL("www",  wserver_service_run, wserver_service_ctrl, 0, 6000, OS_THREAD_PRIO_4, QORAAL_SERVICE_WWW, SVC_SERVICE_FLAGS_AUTOSTART)
SVC_SERVICE_LIST_END()

static const QORAAL_CFG_T           _qoraal_cfg = {
    .malloc       = platform_malloc,
    .free         = platform_free,
    .debug_print  = platform_print,
    .debug_assert = platform_assert,
    .current_time = platform_current_time,
    .rand         = platform_rand,
    .wdt_kick     = platform_wdt_kick
};

/*===========================================================================*/
/* Local Functions                                                           */
/*===========================================================================*/

/**
 * @brief Initialization after scheduler was started.
 * @note  If the services completely define the application, you can  
 *        safely exit here, and the service threads will clean up this 
 *        thread's resources.
 * 
 */
static void
main_thread(void* arg)
{
    platform_start () ;
    /*
     * Lets get the thing started!
     */
    qoraal_start_default () ;
}

/**
 * @brief Pre-scheduler initialization.
 *
 */
void
main_init (void)
{
    /* 
     * If you can malloc before the scheduler is started, no need for a
     * static declaration here as you can free it in the svc_threads_create
     * complete callback.  
     */
    static SVC_THREADS_T thd ;

    platform_init (0) ;
    qoraal_init_default (&_qoraal_cfg, _qoraal_services_list) ;
    qoraal_http_init_default () ;

    svc_threads_create (&thd, 0,
                4000, OS_THREAD_PRIO_1, main_thread, 0, 0) ;

}

#if defined CFG_OS_THREADX 
void tx_application_define(void *first_unused_memory)
{
    main_init () ;
}
#endif

/**
 * @brief Main entry point.
 *
 */
int main( void )
{
#if !defined CFG_OS_THREADX
    main_init() ;
#endif

    /* Start the scheduler. */
    os_sys_start ();

    /* 
     * Dependinmg on the RTOS, if you get here it will most likely be in a  
     * threading context. So alternatively, main_thread can be called from here.
     */

    /*
     * For the demo, we wait for the wait for the web server to shutdown or the 
    *  shell to be exited with the "exit" command.
     */
    console_wait_for_exit (QORAAL_SERVICE_WWW) ;
    svc_service_stop_timeout (svc_service_get(QORAAL_SERVICE_WWW), 1200) ;
    qoraal_stop_default () ;
    platform_stop () ;
    // for( ;; ) os_thread_sleep (32768);
}



