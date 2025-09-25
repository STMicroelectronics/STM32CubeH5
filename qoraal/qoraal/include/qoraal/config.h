
/* CFG_DEBUG_SVC_LOGGER_DISABLE
    If defined, the logger service will not use svc_logger to route debug logging.
*/
// #define CFG_DEBUG_SVC_LOGGER_DISABLE    1

/* CFG_SVC_THREADS_DISABLE_IDLE
    If defined, system should call svc_threads_complete_check() from an idle thread to cleanup 
    terminated threds.
*/
#if defined CFG_OS_FREERTOS
#define CFG_SVC_THREADS_DISABLE_IDLE    1
#endif


/* CFG_SVC_WDT_DISABLE_PLATFORM
    If defined, platform does not support a watchdog, functions will be implemented as stubs only.
*/
// #define CFG_SVC_WDT_DISABLE_PLATFORM    1


/* CFG_COMMON_STRSUB_DISABLE
    If defined the shell will not support string substitution in the command parser
*/
// #define CFG_COMMON_STRSUB_DISABLE        1


/* CFG_COMMON_MEMLOG_DISABLE 
    If defined, the platform does not support memory logging.
*/
// #define CFG_COMMON_MEMLOG_DISABLE      1


/* CFG_OS_OS_TIMER_DISABLE
    If defined, the platform does not support os timers
*/
// #define CFG_OS_OS_TIMER_DISABLE     1

/* CFG_OS_EVENT_DISABLE
    If defined, the platform does not support os events 
*/
// #define CFG_OS_EVENT_DISABLE        1

/* CFG_OS_MLOCK_DISABLE
    If defined, the platform does not support mlock
*/
// #define CFG_OS_MLOCK_DISABLE        1


/* CFG_OS_MEM_DEBUG_ENABLE
    If defined, the platform does aditioanl checking an memory allocations
*/
// #define CFG_OS_MEM_DEBUG_ENABLE        1

/* CFG_PLATFORM_ENABLE_WS2
    If defined, the platform does initialisatin for ws2 (windows) sockets
*/
// #define CFG_PLATFORM_ENABLE_WS2        1
