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

/**
 * @file        platform.h
 * @brief       Example Platform Abstraction Layer for Qoraal
 * @details     This file provides example platform implementations for memory  
 *              management, logging, assertion handling, watchdog management,
 *              and flash memory operations. It abstracts hardware-level 
 *              functionality, providing a foundation for  higher-level services 
 *              within the Qoraal framework.  
 *
 * @author      Navaro
 * @version    
 * @date       
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include "qoraal/qoraal.h"
#include "qoraal/svc/svc_services.h"

/*===========================================================================*/
/* MACROS AND DEFINES                                                        */
/*===========================================================================*/


/** Flash memory sector size (4 KB) */
#define PLATFORM_FLASH_SECTOR_SIZE      (1024 * 4)

/*===========================================================================*/
/* FUNCTION PROTOTYPES                                                        */
/*===========================================================================*/

/**
 * @brief       Initializes platform services.
 * @details     Initializes Qoraal framework instances, flash system, 
 *              registry, and system logging.
 *
 * @return      int32_t Status code (EOK on success).
 */
int32_t platform_init(uint32_t flash_size);

/**
 * @brief       Starts platform services.
 * @details     Allocates flash memory, erases flash storage, and 
 *              starts registry and logging services.
 *
 * @return      int32_t Status code (EOK on success, E_NOMEM if out of memory).
 */
int32_t platform_start(void);

/**
 * @brief       Stops platform services.
 * @details     Stops registry and logging services, and releases 
 *              allocated memory.
 *
 * @return      int32_t Status code (EOK on success).
 */
int32_t platform_stop(void);

/**
 * @brief       Allocates memory from the specified heap.
 * @param[in]   heap    Heap identifier.
 * @param[in]   size    Number of bytes to allocate.
 * @return      void*   Pointer to allocated memory, or NULL on failure.
 */
void *platform_malloc(QORAAL_HEAP heap, size_t size);

/**
 * @brief       Frees allocated memory.
 * @param[in]   heap    Heap identifier.
 * @param[in]   mem     Pointer to the memory block to be freed.
 */
void platform_free(QORAAL_HEAP heap, void *mem);

/**
 * @brief       Prints a formatted message to the console.
 * @param[in]   format  Formatted string to be printed.
 */
void platform_print(const char *format);

/**
 * @brief       Triggers an assertion failure and aborts execution.
 * @param[in]   format  Formatted assertion failure message.
 */
void platform_assert(const char *format);

/**
 * @brief       Kicks the watchdog timer.
 * @details     Prevents the system from resetting due to watchdog timeout.
 *
 * @return      uint32_t Watchdog kick interval in milliseconds.
 */
uint32_t platform_wdt_kick(void);

/**
 * @brief       Retrieves the current system time in seconds.
 * @return      uint32_t Current system time in seconds.
 */
uint32_t platform_current_time(void);

/**
 * @brief       Retrieves a 32 bit random value.
 * @return      uint32_t random value.
 */
uint32_t platform_rand(void);

/**
 * @brief       Reads data from flash memory.
 * @param[in]   addr  Start address to read from.
 * @param[in]   len   Number of bytes to read.
 * @param[out]  data  Buffer to store the read data.
 * @return      int32_t Status code (EOK on success, E_PARM if invalid parameters).
 */
int32_t platform_flash_read(uint32_t addr, uint32_t len, uint8_t *data);

/**
 * @brief       Writes data to flash memory.
 * @param[in]   addr  Start address to write to.
 * @param[in]   len   Number of bytes to write.
 * @param[in]   data  Data to be written.
 * @return      int32_t Status code (EOK on success, E_PARM if invalid parameters).
 */
int32_t platform_flash_write(uint32_t addr, uint32_t len, const uint8_t *data);

/**
 * @brief       Erases a section of flash memory.
 * @note        Erase in blocks of 4 KB.
 * @param[in]   addr_start  Start address of the erase operation.
 * @param[in]   addr_end    End address of the erase operation.
 * @return      int32_t Status code (EOK on success, E_PARM if invalid parameters).
 */
int32_t platform_flash_erase(uint32_t addr_start, uint32_t addr_end);

#endif /* PLATFORM_H */
