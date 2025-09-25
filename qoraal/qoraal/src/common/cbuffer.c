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
 * @file    cbuffer.c
 * @brief   cyclic memory buffer.
 *
 * @addtogroup various
 * @details
 * @pre
 * @{
 */


#include <stdint.h>
#include <string.h>
#include "qoraal/common/cbuffer.h"

static  void
flush_bufer(void* buffer, uint32_t size)
{
    // cacheBufferFlush 

}

/**
 * @brief   cbuffer_init
 * @details Initializes cyclic buffer
 *
 * @param[in] cq
 * @param[in] buffer
 * @param[in] dwsize
 *
 * @return      void
 *
 * @cbuffer
 */
static void cbuffer_init(CBUFFER_T * cb, uint32_t* buffer, size_t dwsize)
{
    cb->start = buffer;
    cb->end = buffer + dwsize;
    cb->write = cb->start;
    cb->read = cb->start;
    memset(cb->start, CBUFFER_FILL, dwsize * sizeof(uint32_t));

    flush_bufer (cb, sizeof(CBUFFER_T)) ;
    flush_bufer (cb->start, dwsize * sizeof(uint32_t)) ;

}


/**
 * @brief   cbuffer_alloc
 * @details Allocate memory from the cyclic buffer
 *
 * @param[in] cq
 * @param[in] dwsize
 *
 * @return      pointer to allocated dwords
 *
 * @cbuffer
 */
static uint32_t* cbuffer_alloc(CBUFFER_T *cb, uint32_t dwsize)
{
    uint32_t* p;

    // assert (dsize < (uint32_t)(cb->end - cb->start))
    if (cb->write >= cb->read) {

        if ((uint32_t)(cb->end - cb->write) >= dwsize) {
            p = cb->write;
            cb->write += dwsize;

        }
        else if ((uint32_t)(cb->read - cb->start) > dwsize) {
            p = cb->start;
            cb->write = cb->start + dwsize;

        }
        else {
            p = 0;

        }
    }
    else {
        if ((uint32_t)(cb->read - cb->write) > dwsize) {
            p = cb->write;
            cb->write += dwsize;

        }
        else {
            p = 0;

        }
    }

    if (p) {
        memset(p, 0, dwsize*sizeof(uint32_t));
        //flush_bufer (p, dwsize*sizeof(uint32_t)) ;
        flush_bufer (cb, sizeof(CBUFFER_T)) ;
    }

    return p;
}

/**
 * @brief   cbuffer_free
 * @details Free memory up to the read pointer
 *
 * @param[in] cq
 * @param[in] read
 *
 * @return      void
 *
 * @cbuffer
 */
static void cbuffer_free(CBUFFER_T *cb, uint32_t * read)
{
    if (read >= cb->read) {
        memset(cb->read, CBUFFER_FILL, (read - cb->read)*sizeof(uint32_t));
        flush_bufer (cb->read, (read - cb->read)*sizeof(uint32_t)) ;

    }
    else {
        memset(cb->write, CBUFFER_FILL, (cb->end - cb->write)*sizeof(uint32_t));
        flush_bufer (cb->write, (cb->end - cb->write)*sizeof(uint32_t)) ;
        memset(cb->start, CBUFFER_FILL, (read - cb->start)*sizeof(uint32_t));
        flush_bufer (cb->start, (read - cb->start)*sizeof(uint32_t)) ;
    }

    cb->read = read;
    if (cb->read == cb->write) {
        cb->write = cb->start;
        cb->read = cb->start;
    }
    flush_bufer (cb, sizeof(CBUFFER_T)) ;

    return;
}

/**
 * @brief   cqueue_init
 * @details Initialize cqueue using cbuffer for memory allocation.
 *
 * @param[in] cq
 * @param[in] buffer
 * @param[in] dwsize
 *
 * @return      void
 *
 * @cqueue
 */
void
cqueue_init(CBUFFER_QUEUE_T* cq, uint32_t* buffer, uint32_t dwsize)
{
    cbuffer_init(&cq->cb, buffer, dwsize);
    cq->next = (struct CBUFFER_ITEM_S *)cq;
    cq->prev = (struct CBUFFER_ITEM_S *)cq;
    cq->count = 0;
    cq->magic = CBUFFER_MAGIC ;
    flush_bufer (cq, sizeof(CBUFFER_QUEUE_T)) ;

}

CBUFFER_ITEM_T*
cqueue_enqueue(CBUFFER_QUEUE_T* cq, uint32_t dwsize)
{
    int32_t s ;
    CBUFFER_ITEM_T * item ;
    if (cq->magic != CBUFFER_MAGIC) {
        return 0 ;
    }
    s = (dwsize + sizeof(CBUFFER_ITEM_T) / sizeof(uint32_t));
    item = (CBUFFER_ITEM_T *)cbuffer_alloc(&cq->cb, s);

    if (item) {
        item->next = (CBUFFER_ITEM_T *)cq;
        item->prev = cq->prev;
        item->prev->next = item;
        cq->prev = item;

        item->dwsize = s;
        item->magic = CBUFFER_MAGIC;

        cq->count++;

        flush_bufer (cq, sizeof(CBUFFER_QUEUE_T)) ;
        flush_bufer (item, s * sizeof(uint32_t)) ;

    }

    return item;
}

uint32_t
cqueue_dequeue(CBUFFER_QUEUE_T* cq)
{

    uint32_t s;
    if (cq->magic != CBUFFER_MAGIC) {
        return 0 ;
    }

    if (cq->next != (CBUFFER_ITEM_T*)cq) {
        CBUFFER_ITEM_T* item = cq->next;
        cq->next = item->next;
        cq->next->prev = (CBUFFER_ITEM_T *)cq;

        s = item->dwsize;
        item->magic = 0 ;
        cbuffer_free(&cq->cb, (uint32_t*)item + s);

        cq->count--;

        flush_bufer (cq, sizeof(CBUFFER_QUEUE_T)) ;
        //flush_bufer (item, sizeof(CBUFFER_ITEM_T)) ;


    }
    else {
        s = 0;
    }

    return s;
}

int32_t
cqueue_count(CBUFFER_QUEUE_T* cq)
{
    return cq->count;
}

/**
 * @brief   cqueue_front
 * @details Returns the first (newest) item in the queue.
 *
 * @param[in] cq
 *
 * @return                          CBUFFER_ITEM_T
 * @retval 0                        If empty
 *
 * @cqueue
 */
CBUFFER_ITEM_T*
cqueue_front(CBUFFER_QUEUE_T* cq)
{
    if (cq->magic != CBUFFER_MAGIC) {
        return 0 ;
    }

    if (cq->next != (CBUFFER_ITEM_T*)cq) {
        return cq->prev;
    }

    return 0;
}

/**
 * @brief   cqueue_back
 * @details Returns the last (oldest) item in the queue.
 *
 * @param[in] cq
 *
 * @return                          CBUFFER_ITEM_T
 * @retval 0                        If empty
 *
 * @cqueue
 */
CBUFFER_ITEM_T*
cqueue_back(CBUFFER_QUEUE_T* cq)
{
    if (cq->magic != CBUFFER_MAGIC) {
        return 0 ;
    }

    if (cq->next != (CBUFFER_ITEM_T*)cq) {
        return cq->next;
    }

    return 0;

}

/**
 * @brief   cqueue_forward
 * @details Returns the next (newer) item in the queue.
 *
 * @param[in] cq
 *
 * @return                          CBUFFER_ITEM_T
 * @retval 0                        If empty or at front of queue.
 *
 * @cqueue
 */
CBUFFER_ITEM_T*
cqueue_forwards(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item)
{
    if (item->next == (struct CBUFFER_ITEM_S *)cq) {
        return 0;
    }

    return item->next;
}

/**
 * @brief   cqueue_backward
 * @details Returns the previous (older item in the queue.
 *
 * @param[in] cq
 *
 * @return                          CBUFFER_ITEM_T
 * @retval 0                        If empty or at back of queue.
 *
 * @cqueue
 */
CBUFFER_ITEM_T*
cqueue_backwards(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item)
{
    if (item->prev == (struct CBUFFER_ITEM_S *)cq) {
        return 0;
    }

    return item->prev;

}

/**
 * @brief   cqueue_validate_item
 * @details Validate the item to be in the queue.
 *
 * @param[in] cq
 * @param[in] item
 *
 * @return
 * @retval 0                        Invalid.
 * @retval 1                        Valid.
 *
 * @cqueue
 */
int32_t
cqueue_validate_item(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item)
{
    if (((uint32_t*)item >= cq->cb.end) ||
        ((uint32_t*)item < cq->cb.start)) {
        return 0;
    }


    if (item->magic != CBUFFER_MAGIC) {
        return 0;
    }
    if (item->dwsize > cq->cb.end - cq->cb.start) {
        return 0;
    }

    if (
        ((uint32_t*)item >= cq->cb.end) ||
        ((uint32_t*)item < cq->cb.start)
        ) {
        return 0;
    }

    if (item->next != (struct CBUFFER_ITEM_S *)cq) {

        if (((uint32_t*)item->next >= cq->cb.end) ||
            ((uint32_t*)item->next < cq->cb.start)) {

            return 0;
        }
    }

    if (item->prev != (struct CBUFFER_ITEM_S *)cq) {

        if (((uint32_t*)item->prev >= cq->cb.end) ||
            ((uint32_t*)item->prev < cq->cb.start)) {
            return 0;
        }
    }



    return 1;
}

/**
 * @brief   cqueue_validate
 * @details Validate the queue.
 *
 * @param[in] cq
 * @param[in] cbuffer
 * @param[in] dwsize
 *
 * @return
 * @retval 0                        Invalid.
 * @retval 1                        Valid.
 *
 * @cqueue
 */
int32_t
cqueue_validate(CBUFFER_QUEUE_T* cq, uint32_t* cbuffer, uint32_t dwsize)
{
    int res  = 1 ;
    int32_t count = 0 ;
    CBUFFER_ITEM_T *i;

    do {
        if (cq->magic != CBUFFER_MAGIC) {
            res = 0;
            break ;
        }
        if (cq->cb.start != cbuffer) {
            res = 0 ;
            break ;
        }
        if (cq->cb.end != cbuffer + dwsize) {
            res = 0 ;
            break ;
        }

        if ( ((uint32_t*)cq->cb.read >= cq->cb.end) ||
            ((uint32_t*)cq->cb.read < cq->cb.start) ) {
            res = 0;
            break ;
        }

        if ( ((uint32_t*)cq->cb.write >= cq->cb.end) ||
            ((uint32_t*)cq->cb.write < cq->cb.start) ) {
            res = 0;
            break ;
        }

        if (cq->next != (struct CBUFFER_ITEM_S *)cq) {

            if ( ((uint32_t*)cq->next >= cq->cb.end) ||
                ((uint32_t*)cq->next < cq->cb.start) ) {
                res = 0;
                break ;
            }

        }

        if (cq->prev != (struct CBUFFER_ITEM_S *)cq) {

            if ( ((uint32_t*)cq->prev >= cq->cb.end) ||
                ((uint32_t*)cq->prev < cq->cb.start) ) {
                res = 0;
                break ;
            }

        }

        for (i = cqueue_front(cq); i; ) {


            if (!cqueue_validate_item(cq, i)) {
                res = 0;
                break ;
            }
            count++ ;

            i = cqueue_backwards(cq, i);
        }
        if (!res) break ;

        cq->count = 0 ;

        for (i = cqueue_back(cq); i; ) {


            if (!cqueue_validate_item(cq, i)) {
                res = 0;
                break ;
            }
            cq->count++ ;

            i = cqueue_forwards(cq, i);

        }

        if (!res || (cq->count != count)) {
            cq->count = 0 ;
            res = 0;
            break ;

        }

        break ;
    } while (0) ;

    flush_bufer (cq, sizeof(CBUFFER_QUEUE_T)) ;

    return res ;
}

/**
 * @brief   cqueue_flushe_item
 * @details Flush the item .
 *
 * @param[in] cq
 * @param[in] item
 *
 * @return
 *
 * @cqueue
 */
void
cqueue_flush_item(CBUFFER_QUEUE_T* cq, CBUFFER_ITEM_T* item)
{
    flush_bufer (item->data, item->dwsize*sizeof(uint32_t)) ;
}

