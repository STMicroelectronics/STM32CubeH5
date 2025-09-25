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
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_events.h"
#include "qoraal/svc/svc_tasks.h"
#include "qoraal/svc/svc_logger.h"
#include "qoraal/svc/svc_threads.h"
#include "qoraal/svc/svc_wdt.h"
#include "qoraal/svc/svc_services.h"
#include "qoraal/svc/svc_shell.h"
#include "qoraal/common/mlog.h"

#ifndef CFG_OS_STATIC_DECLARATIONS
#error "CFG_OS_CHIBIOS, CFG_OS_FREERTOS, CFG_OS_THREADX or CFG_OS_POSIX must be defined when compiling qoraal"
#endif

extern void keep_servicescmds (void) ;


const QORAAL_CFG_T * _qoraal_instance = 0 ;

int32_t
qoraal_instance_init (const QORAAL_CFG_T * instance)
{
    if (!_qoraal_instance) {
        _qoraal_instance = instance;

        if (!_qoraal_instance->malloc) qoraal_debug_assert ("qoraal malloc required") ;
        if (!_qoraal_instance->free) qoraal_debug_assert ("qoraal free required") ;

    }
    return EOK;
}


int32_t
qoraal_init_default (const QORAAL_CFG_T * instance, SVC_SERVICE_T * list)
{
    int32_t res = qoraal_instance_init (instance) ;
    if (res != EOK) {
       return E_UNEXP ;

    }
    
    svc_events_init () ;
    svc_tasks_init (0) ;
    svc_wdt_init () ;
    svc_logger_init (SERVICE_PRIO_QUEUE3) ;
    svc_threads_init () ;
    svc_shell_init () ;
    if (list) {
        svc_service_services_init (list) ;
    }

    keep_servicescmds () ;
    
    return EOK ;
}

int32_t
qoraal_start_default (void)
{
    if (!_qoraal_instance) {
       return E_UNEXP ;

    }

    svc_wdt_start () ;
    svc_events_start () ;
    svc_tasks_start () ;
    svc_logger_start () ;
    svc_threads_start () ;
    svc_shell_start () ;
    svc_service_services_start (SVC_SERVICE_FLAGS_AUTOSTART) ;

    return EOK ;
}


int32_t
qoraal_stop_default (void)
{
    if (!_qoraal_instance) {
       return E_UNEXP ;

    }

    svc_service_system_halt () ;
    svc_shell_stop () ;
    svc_tasks_stop (100) ;
    svc_events_stop () ;
    svc_wdt_stop () ;

    return EOK ;
}

#if defined CFG_OS_MEM_DEBUG_ENABLE
#define DEBUG_SIZE_HEAD      8
#define DEBUG_SIZE_TAIL      8
#define DEBUG_HEAD_FILL      0x77
#define DEBUG_TAIL_FILL      0x77
#define DEBUG_CLEAR_FILL     0xED
#define DEBUG_HEADER         16
#define DEBUG_BYTES          (DEBUG_SIZE_HEAD + DEBUG_SIZE_TAIL + DEBUG_HEADER + sizeof(uint32_t))
#else
#define DEBUG_BYTES          0
#endif

// Debug allocation layout in memory:
//
//   [ size (4 bytes) | debugHeader (16 bytes) | HEAD (8 bytes) | userData (bytes) | TAIL (8 bytes) ]
//
// The 'size' tells us how many bytes of user data follow. We place a short debug string
// (line info, etc.) in the 'debugHeader' field. HEAD and TAIL are both filled with known
// values to catch buffer underruns (if someone writes before userData) and overruns
// (writes after userData).
//
// If CFG_OS_MEM_DEBUG_ENABLE is 0, we skip all this overhead and just do plain malloc/free.

#if defined CFG_OS_MEM_DEBUG_ENABLE
void*
heap_do_check(void* mem, int clear, const char* name, uint32_t line)
{
    if (mem) {
        // Step backwards to reach our stored size and the HEAD region
        char* pmem = (char*)mem - (DEBUG_SIZE_HEAD + DEBUG_HEADER + sizeof(uint32_t));

        // The first 4 bytes in this overhead store how big the user block is
        uint32_t bytes = *((uint32_t*)pmem);
        pmem += sizeof(uint32_t);

#if (DEBUG_SIZE_HEAD + DEBUG_HEADER + DEBUG_SIZE_TAIL)
        // If clear is requested, we zap the user data with a "dead" fill pattern
        // to make post-free usage more obvious. Optionally rewrite the debug header
        // so we remember which line/name freed it.
        if (clear) {
            memset(mem, DEBUG_CLEAR_FILL, bytes);
            if (DEBUG_HEADER && name) {
                snprintf(pmem, DEBUG_HEADER, "%u:%s", (unsigned int)line, name);
            }
        }

        // Move past the debug header to verify the HEAD canary
        pmem += DEBUG_HEADER;
        for (int i = 0; i < DEBUG_SIZE_HEAD; i++) {
            DBG_ASSERT_T(
                pmem[i] == DEBUG_HEAD_FILL,
                "heap_do_check : buffer underrun"
            );
        }

        // Advance beyond HEAD + userData to verify the TAIL canary
        pmem += DEBUG_SIZE_HEAD + bytes;
        for (int i = 0; i < DEBUG_SIZE_TAIL; i++) {
            DBG_ASSERT_T(
                pmem[i] == DEBUG_TAIL_FILL,
                "heap_do_check : buffer overrun"
            );
        }
#endif

        // Return pointer to the start of the entire allocation,
        // which the lower-level free() is expecting.
        return (char*)mem - (DEBUG_SIZE_HEAD + DEBUG_HEADER + sizeof(uint32_t));
    }

    // If passed a null pointer, nothing to check
    return 0;
}
#endif

#if defined CFG_OS_MEM_DEBUG_ENABLE
void*
heap_add_check(void* mem, uint32_t bytes, const char* name, uint32_t line)
{
    if (!mem) return mem;

    // We'll place the overhead info at the start, then skip past it
    char* pmem = (char*)mem;

    // Store the requested size for later reference
    *((uint32_t*)pmem) = bytes;
    pmem += sizeof(uint32_t);

#if (DEBUG_SIZE_HEAD + DEBUG_HEADER + DEBUG_SIZE_TAIL)
    // Optionally store the line/name in a short debug header
    if (DEBUG_HEADER && name) {
        snprintf(pmem, DEBUG_HEADER, "%u:%s", (unsigned int)line, name);
    }
    pmem += DEBUG_HEADER;

    // Fill the HEAD region with a known pattern
    memset(pmem, DEBUG_HEAD_FILL, DEBUG_SIZE_HEAD);
    pmem += DEBUG_SIZE_HEAD;

    // The user data starts here, so the TAIL region goes right after the user area
    char* ptail = pmem + bytes;
    memset(ptail, DEBUG_TAIL_FILL, DEBUG_SIZE_TAIL);
#endif

    // Return the pointer where the user’s data actually starts
    return (void*)pmem;
}
#endif

void* 
qoraal_malloc (QORAAL_HEAP heap, size_t size)
{
    if (!_qoraal_instance) return 0;

    // Allocate enough space for user data plus the debug overhead
    void* mem = _qoraal_instance->malloc(heap, size + DEBUG_BYTES);
    if (!mem) return 0;

#if defined CFG_OS_MEM_DEBUG_ENABLE
    // Insert our canaries and store metadata
    mem = heap_add_check(mem, size, 0, 0);
#endif
    return mem;
}

void 
qoraal_free (QORAAL_HEAP heap, void *mem)
{
    if (!_qoraal_instance) return;
    if (!mem) return;

#if defined CFG_OS_MEM_DEBUG_ENABLE
    // Validate our canaries; optionally clear user data
    mem = heap_do_check(mem, 1, 0, 0);
#endif
    _qoraal_instance->free(heap, mem);
}
