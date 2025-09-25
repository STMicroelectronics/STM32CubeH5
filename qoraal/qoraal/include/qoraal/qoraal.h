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

#ifndef __QORAAL_H__
#define __QORAAL_H__
#include <stdint.h>
#include <stddef.h>
#include "qoraal/config.h"
#include "qoraal/os.h"
#include "qoraal/errordef.h"
#include "qoraal/debug.h"
#include "qoraal/svc/svc_services.h"

/*===========================================================================*/
/* Data structures and types.                                                */
/*===========================================================================*/

typedef enum {
    QORAAL_HeapOperatingSystem,
    QORAAL_HeapAuxiliary,
    QORAAL_HeapDirectMemoryAccess,
    QORAAL_HeapLast
} QORAAL_HEAP;

// Structure to define component dependencies
typedef struct {
    /**
     * @brief Allocate memory.
     * @param size The number of bytes to allocate.
     * @return Pointer to the allocated memory or NULL on failure.
     */
    void *(*malloc) (QORAAL_HEAP heap, size_t size);

    /**
     * @brief Free allocated memory.
     * @param ptr Pointer to the memory to free.
     */
    void (*free) (QORAAL_HEAP heap, void *mem);

    /**
     * @brief Print a debug message.
     * @param message The debug string to print.
     */
    void (*debug_print) (const char *message);

    /**
     * @brief Assert with a debug message.
     * @param message The debug string to print.
     */
    void (*debug_assert) (const char *message);

    /**
     * @brief get system time
     * @return Seconds Since Epoch
     */
    uint32_t (*current_time) (void);

    /**
     * @brief random number
     * @return random number
     */
    uint32_t (*rand) (void);

    /**
     * @brief Kick the WDT
     * @return Seconds before wdt will reset
     */
    uint32_t (*wdt_kick) (void);

} QORAAL_CFG_T;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

extern const QORAAL_CFG_T * 	_qoraal_instance ;


#ifdef __cplusplus
extern "C" {
#endif

int32_t     qoraal_init_default (const QORAAL_CFG_T * instance, SVC_SERVICE_T * list) ;
int32_t     qoraal_start_default (void) ;
int32_t     qoraal_stop_default (void) ;


int32_t     qoraal_instance_init (const QORAAL_CFG_T * instance);
int32_t     qoraal_svc_init (SVC_SERVICE_T * list) ;
void *      qoraal_malloc (QORAAL_HEAP heap, size_t size) ;
void        qoraal_free (QORAAL_HEAP heap, void *mem) ;

static inline void qoraal_debug_print (const char *message) {
    if (_qoraal_instance && _qoraal_instance->debug_print) _qoraal_instance->debug_print (message);
}

static inline void qoraal_debug_assert (const char *message) {
    if (_qoraal_instance && _qoraal_instance->debug_assert) _qoraal_instance->debug_assert (message);
}

static inline uint32_t qoraal_current_time (void) {
    if (_qoraal_instance && _qoraal_instance->current_time) return _qoraal_instance->current_time ();
    return 0 ;
}

static inline uint32_t qoraal_rand (void) {
    if (_qoraal_instance && _qoraal_instance->rand) return _qoraal_instance->rand ();
    return 0 ;
}

static inline uint32_t qoraal_wdt_kick (void) {
    if (_qoraal_instance && _qoraal_instance->wdt_kick) return _qoraal_instance->wdt_kick ();
    return 0 ;
}

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
#endif /* __QORAAL_H__ */
