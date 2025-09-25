/*
 *  Copyright (C) 2015-2025, Navaro, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of CORAL Connect (https://navaro.nl)
 */

/**
 * @file    cbuffer.h
 * @brief   cyclic memory buffer. Intended for uninitialised memory to be persistent across resets.
 *
 * @addtogroup various
 * @details
 * @pre
 * @{
 */

/*
* cbuffer.h
*
*  Created on: Jun 7, 2016
*      Author: natie
*/

#ifndef __CBUFFER_H__
#define __CBUFFER_H__

#include <stdint.h>
#include <stddef.h>

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/


#define CBUFFER_MAGIC       0x2608
#define CBUFFER_FILL        0xEE

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
* @brief   Date structure.
* @details This structure represents a circular buffer.
*/
typedef struct CBUFFER_S {
    uint32_t *              write;          // pointer to head
    uint32_t *              read;           // pointer to tail
    uint32_t *              start;          // data buffer
    uint32_t *              end;            // data buffer
} CBUFFER_T;

/**
* @brief   Date structure.
* @details This structure represents a circular buffer item.
*/
typedef struct CBUFFER_ITEM_S {
    struct CBUFFER_ITEM_S * next;       // pointer to next
    struct CBUFFER_ITEM_S * prev;       // pointer to previous
    uint16_t                dwsize;     // size of data buffer
    uint16_t                magic;      // magic
    uint32_t                data[];
} CBUFFER_ITEM_T;

#define CBUFFER_ITEM_DATA_SIZE(item)    (item->dwsize - sizeof(CBUFFER_ITEM_T)/sizeof(uint32_t))

/**
* @brief   Date structure.
* @details This structure represents a circular buffer with 0 or more items.
*/
typedef struct CBUFFER_QUEUE_S {
    struct CBUFFER_ITEM_S * next;       // pointer to head
    struct CBUFFER_ITEM_S * prev;       // pointer to head
    uint16_t                count;
    uint16_t                magic;      // magic
    CBUFFER_T               cb; // circular  buffer
} CBUFFER_QUEUE_T;

#ifdef __cplusplus
extern "C" {
#endif

    extern void                 cqueue_init(CBUFFER_QUEUE_T* cq, uint32_t* buffer, uint32_t dwsize);
    extern CBUFFER_ITEM_T*      cqueue_enqueue(CBUFFER_QUEUE_T* cq, uint32_t dwsize);
    extern uint32_t             cqueue_dequeue(CBUFFER_QUEUE_T* cq);
    extern void                 cqueue_flush_item(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item) ;

    extern int32_t              cqueue_count(CBUFFER_QUEUE_T* cq);
    extern CBUFFER_ITEM_T*      cqueue_front(CBUFFER_QUEUE_T* cq);
    extern CBUFFER_ITEM_T*      cqueue_back(CBUFFER_QUEUE_T* cq);
    extern CBUFFER_ITEM_T*      cqueue_forwards(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item);
    extern CBUFFER_ITEM_T*      cqueue_backwards(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item);

    extern int32_t              cqueue_validate_item(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item);
    extern int32_t              cqueue_validate(CBUFFER_QUEUE_T* cq, uint32_t* cbuffer, uint32_t dwsize);

#ifdef __cplusplus
}
#endif

#endif /* __CBUFFER_H__ */
