# Collect all source files
set(QORAAL_SRCS
    qoraal.c
    debug.c
    os_mlock.c
    os_posix.c
    os.c
    svc/svc_events.c
    svc/svc_logger.c
    svc/svc_services.c
    svc/svc_shell.c
    svc/svc_tasks.c
    svc/svc_threads.c
    svc/svc_wdt.c
    common/cbuffer.c
    common/dictionary.c
    common/lists.c
    common/memdbg.c
    common/mlog.c
    common/rtclib.c
    common/strsub.c
    example/console.c
    example/platform.c
    example/posixcmd.c
    qshell/servicescmd.c
)

# Add the library target
add_library(qoraal STATIC ${QORAAL_SRCS})

# Link necessary libraries
target_link_libraries(qoraal pthread)

# Set compile-time definitions for the qoraal target
# target_compile_definitions(qoraal PRIVATE CFG_OS_POSIX)

