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
 * @file    nvol2.c
 * @brief   Template for c-files.
 * @details Implements the syslog interface.
 *
 * @addtogroup services
 * @details Implements the syslog interface.
 * @{
 */

#include <string.h>
#include "qoraal-flash/qoraal.h"
#include "qoraal-flash/qoraal.h"
#include "qoraal-flash/nvram/nlog2.h"


#define FLASH_READ(address, len, data)          qoraal_flash_read (address, len, data)
#define FLASH_WRITE(address, len, data)         qoraal_flash_write (address, len, data)
#define FLASH_ERASE(start, end)                 qoraal_flash_erase (start, end)

#define NLOG2_LOG_RECORD_ALIGN                  8

// sector flags
#define NLOG2_RECORD_FLAGS_EMPTY        0xFFFFFFFF
#define NLOG2_RECORD_FLAGS_ACTIVE       0xAAFFFFFF
#define NLOG2_RECORD_FLAGS_CLOSED       0xAAAAFFFF

#define NLOG2_RECORD_TYPE_RECORD        0xEFFFFFFF
#define NLOG2_RECORD_TYPE_SECTOR        0xDFFFFFFF
#define NLOG2_RECORD_TYPE_PATCH         0xCFFFFFFF

#define RECORD_TYPE_SECTOR              0
#define RECORD_TYPE_PATCH               1
#define RECORD_TYPE_RECORD              2
#define RECORD_TYPE_CLOSED              3
#define RECORD_TYPE_EMPTY               4
#define RECORD_TYPE_INVALID             5

#define NLOG2_IS_VALID_ADDR(addr)       ((addr >= plog->startaddr) && \
                        (addr <= (plog->startaddr + \
                        (plog->sectorsize * plog->sectorcount) - \
                        sizeof(NLOG2_LOG_HEADER_T))) )


static uint32_t
record_type (NLOG2_T * plog, NLOG2_LOG_HEADER_T * header)
{

    if (header->flags == NLOG2_RECORD_FLAGS_EMPTY) {
        return RECORD_TYPE_EMPTY ;

    }
    else if (header->flags == NLOG2_RECORD_FLAGS_CLOSED) {
        return RECORD_TYPE_CLOSED ;

    }
    else if (!  (NLOG2_IS_VALID_ADDR(header->this) &&
            NLOG2_IS_VALID_ADDR(header->next) &&
            NLOG2_IS_VALID_ADDR(header->previous)) ) {
        return RECORD_TYPE_INVALID ;

    }
    else if (header->record_type == NLOG2_RECORD_TYPE_RECORD) {
        return RECORD_TYPE_RECORD ;
    }
    else if (header->record_type == NLOG2_RECORD_TYPE_SECTOR) {
        return RECORD_TYPE_SECTOR ;
    }
    else if (header->record_type == NLOG2_RECORD_TYPE_PATCH) {
        return RECORD_TYPE_PATCH ;
    }

    return RECORD_TYPE_INVALID ;
}


static int32_t
sector_erase (NLOG2_T * plog, uint32_t sector_nr)
{
    uint32_t start  ;
    if (sector_nr >= plog->sectorcount) {
        return E_PARM ;
    }

    start = plog->startaddr + plog->sectorsize * sector_nr ;
    return FLASH_ERASE (start, start + plog->sectorsize - 1) ;
}

static uint32_t
sector_address (NLOG2_ITERATOR_T *it)
{
    NLOG2_T * plog = it->plog ;
    return plog->startaddr + plog->sectorsize * it->sector ;
}

static uint32_t
sector_of_address (NLOG2_T * plog, uint32_t addr)
{
    if (!NLOG2_IS_VALID_ADDR(addr)) return 0 ;
    return (addr - plog->startaddr) / plog->sectorsize  ;
}


static uint32_t
sector_next (NLOG2_ITERATOR_T *it)
{
    NLOG2_T * plog = it->plog ;
    return (it->sector < plog->sectorcount-1) ? it->sector + 1 : 0 ;
}

static int32_t
sector_available (NLOG2_ITERATOR_T *it)
{
    NLOG2_T * plog = it->plog ;
    return plog->startaddr + plog->sectorsize * (it->sector + 1) -
            it->header.next  - sizeof (NLOG2_LOG_HEADER_T) * 2 ; // allocate space for an
                                // extra header to be inserted for a link to the next sector.
}

static uint32_t
sector_flags_get (NLOG2_T * plog, uint32_t sector)
{
    uint32_t flags = NLOG2_RECORD_FLAGS_EMPTY ;
    FLASH_READ(plog->startaddr + plog->sectorsize * sector + offsetof(NLOG2_LOG_HEADER_T, flags),
            sizeof(uint32_t), (uint8_t*)&flags) ;
    return flags ;
}

static uint32_t
sector_type_get (NLOG2_T * plog, uint32_t sector)
{
    NLOG2_LOG_HEADER_T  header  ;
    memset (&header, 0xFF, sizeof(NLOG2_LOG_HEADER_T)) ;
    FLASH_READ(plog->startaddr + plog->sectorsize * sector,
            sizeof(NLOG2_LOG_HEADER_T), (uint8_t*)&header) ;
    return record_type (plog, &header) ;
}

static int32_t
sector_flags_update (NLOG2_T * plog, uint32_t sector, uint32_t flags)
{
    return FLASH_WRITE(plog->startaddr + plog->sectorsize * sector + offsetof(NLOG2_LOG_HEADER_T, flags),
            sizeof(uint32_t), (uint8_t*)&flags) ;
}

static int32_t
sector_find_active (NLOG2_ITERATOR_T *it)
{
    uint16_t i ;
    NLOG2_T * plog = it->plog ;
    int32_t res = EFAIL ;

    for (i=0; i<plog->sectorcount; i++) {
        uint32_t addr = sector_address(it) ;
        it->sector = i ;
        if (FLASH_READ (addr,  sizeof(NLOG2_LOG_HEADER_T) ,
                (uint8_t*)&it->header) == EOK) {
            if (
                    (record_type(plog, &it->header) == RECORD_TYPE_SECTOR)
                ) {

                if (sector_type_get(plog, sector_next(it)) != RECORD_TYPE_SECTOR) {
                    res = EOK ;
                    break ;

                }

            }

        }

    }

    return res ;
}

static int32_t
sector_init_first (NLOG2_ITERATOR_T *it, uint32_t previous)
{
    NLOG2_T * plog = it->plog ;
    int32_t res ;
 // initialize first sector.
    // insert empty record.
    sector_erase (plog,  it->sector) ;

    it->header.this = sector_address (it) ;
    it->header.previous = previous ;
    it->header.next = it->header.this + sizeof (NLOG2_LOG_HEADER_T) ;
    it->header.record_type = NLOG2_RECORD_TYPE_SECTOR ;
    it->header.flags = NLOG2_RECORD_FLAGS_ACTIVE ;
    it->header.user_type = NLOG2_RECORD_FLAGS_EMPTY ;
    it->header.id = 0 ;
    it->header.len = 0 ;

//  sector_erase (plog,  sector_next (it)) ;
    res = FLASH_WRITE(it->header.this, sizeof(NLOG2_LOG_HEADER_T), (uint8_t*)&it->header) ;

    return res ;
}

static int32_t
sector_init_next (NLOG2_ITERATOR_T *it)
{
    NLOG2_T * plog = it->plog ;
    NLOG2_LOG_HEADER_T next ;

    //sector_flags_update (plog, it->sector, NLOG2_RECORD_FLAGS_CLOSED) ;

    uint32_t next_sector = sector_next (it) ;
    uint32_t next_flags = sector_flags_get (plog, next_sector) ;

    if (    (next_flags != RECORD_TYPE_EMPTY) &&
            (next_flags !=NLOG2_RECORD_FLAGS_CLOSED)) {
        sector_flags_update (plog, next_sector, NLOG2_RECORD_FLAGS_CLOSED) ;

    }

    it->sector = sector_next (it) ;
    // insert an empty link pointing to the next sector
    next.this = it->header.next ;
    next.next = sector_address (it) ;
    next.previous = it->header.this ;
    next.record_type = NLOG2_RECORD_TYPE_PATCH ;
    next.flags = NLOG2_RECORD_FLAGS_ACTIVE ;
    next.user_type = NLOG2_RECORD_FLAGS_EMPTY ;
    next.id = 0 ;
    next.len = 0 ;

    FLASH_WRITE (next.this, sizeof(NLOG2_LOG_HEADER_T), (uint8_t*)&next) ;

    sector_init_first (it, next.this) ;

    return EOK ;
}


/**
 * @brief   Starts nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t
nlog2_init (NLOG2_T * plog)
{
    NLOG2_ITERATOR_T *it = &plog->it ;

    memset (it, 0, sizeof(NLOG2_ITERATOR_T)) ;
    it->plog = plog ;

    if (sector_find_active(it) != EOK) {
        // reset all sectors and initialize first sector
       nlog2_reset (plog) ;

    } else {

       NLOG2_LOG_HEADER_T next ;
       uint32_t type ;

       do {


           if (FLASH_READ (it->header.next,
                sizeof(NLOG2_LOG_HEADER_T), (uint8_t*)&next) != EOK) {
                nlog2_reset (plog) ;
                break ;

           }

           type = record_type (plog, &next) ;
           if ((type == RECORD_TYPE_RECORD) ||
                   (type == RECORD_TYPE_PATCH)) {
               if (next.previous != it->header.this) {
                   nlog2_reset (plog) ;
                   break ;
               }
               memcpy ((uint8_t*)&it->header, &next, sizeof(NLOG2_LOG_HEADER_T)) ;
               plog->id = next.id + 1 ;
               continue ;

           }
           else if (type == RECORD_TYPE_EMPTY) {
               it->sector = sector_of_address (plog, it->header.this) ;
               break ;

           }
           else  {
               nlog2_reset (plog) ;
               break ;

           }

       } while (1) ;

    }

    return EOK ;

}

/**
 * @brief   Starts nvol service.
 *
 * @param[in] parm    input parameter
 *
 * @return              Error.
 *
 * @init
 */
int32_t
nlog2_reset (NLOG2_T * plog)
{
    uint32_t i ;
    NLOG2_ITERATOR_T *it = &plog->it ;

      DBG_MESSAGE_NLOG(DBG_MESSAGE_SEVERITY_WARNING,
                       "NLOG  :W: nlog_reset!!!") ;

    for (i=1; i<plog->sectorcount; i++) {
        sector_erase (plog,  i) ;
    }
    it->plog = plog ;
    it->sector = 0 ;
    it->type = 0 ;

    // initialize first sector.
    //sector_erase (plog,  it->sector) ;
    sector_init_first (&plog->it, sector_address (it)) ;

    return EOK ;
}


uint32_t
nlog2_get_id (NLOG2_T * plog)
{
    return plog->id ;
}



/**
 * @brief   nlog_append
 *
 * @param[in] msg    msg
 *
 * @return              Error.
 *
 * @init
 */
int32_t
nlog2_append (NLOG2_T * plog, uint32_t type, const char* data, uint32_t len)
{
    NLOG2_ITERATOR_T * it  = &plog->it;
    NLOG2_LOG_HEADER_T next  ;
    int32_t available ;
    uint32_t alloc = 0 ;

    if (len) {
        alloc = (len + NLOG2_LOG_RECORD_ALIGN) & ~(NLOG2_LOG_RECORD_ALIGN - 1)  ;
    }

    available = sector_available(it) ;
    if ((available < 0) || ((uint32_t)available < alloc)) {
        // we dont cross a sector boundary.
        // a sector always start with a record.
        sector_init_next (it) ;
        available = sector_available(it) ;
        if ((available < 0) || ((uint32_t)available < alloc)) {
            return E_PARM ;

        }

    }

    next.this = it->header.next ;
    next.next = next.this + sizeof(NLOG2_LOG_HEADER_T) + alloc ;
    next.previous = it->header.this ;
    next.record_type = NLOG2_RECORD_TYPE_RECORD ;
    next.flags = NLOG2_RECORD_FLAGS_ACTIVE ;
    next.user_type = type ;
    next.id = plog->id++ ;
    next.len = len ;

    FLASH_WRITE (next.this, sizeof(NLOG2_LOG_HEADER_T), (uint8_t*)&next) ;
    FLASH_WRITE (next.this + sizeof(NLOG2_LOG_HEADER_T), len + 1, (uint8_t*)data) ;

    memcpy (&it->header, &next, sizeof(NLOG2_LOG_HEADER_T)) ;

    return EOK ;
}


static int32_t
_iterator_init (NLOG2_T * plog, NLOG2_ITERATOR_T *it)
{
    // return a copy of the write iterator, pointing to the end of the log.
    memcpy (it, &plog->it, sizeof (NLOG2_ITERATOR_T)) ;

    return EOK ;
}

int32_t
nlog2_iterator_init (NLOG2_T * plog, uint32_t type, NLOG2_ITERATOR_T *it)
{
    int32_t res = _iterator_init (plog, it) ;
    it->type = type ;

    while ((res == EOK) && (record_type(plog, &it->header) != RECORD_TYPE_RECORD)) {
        res = nlog2_iterator_prev (it) ;

    }

    return res ;
}

static int32_t
_iterator_next (NLOG2_ITERATOR_T *it)
{
    NLOG2_T * plog = it->plog;
    uint32_t type ;
    NLOG2_LOG_HEADER_T next ;
    int32_t res = EFAIL ;

    type = record_type (plog, &it->header) ;

    if (    (type == RECORD_TYPE_CLOSED) ||
            (type == RECORD_TYPE_EMPTY) ||
            (type == RECORD_TYPE_INVALID)
            ) {
        res = EFAIL ;

    } else {
        // read next record
        FLASH_READ (it->header.next,  sizeof(NLOG2_LOG_HEADER_T) ,
                (uint8_t*)&next) ;
        type = record_type (plog, &next) ;

         if (   (type == RECORD_TYPE_CLOSED) ||
                 (type == RECORD_TYPE_INVALID)) {
            res = EFAIL ;

        } else if (type == RECORD_TYPE_EMPTY) {
            res = E_EOF ;

        } else {
            memcpy (&it->header, &next, sizeof (NLOG2_LOG_HEADER_T)) ;
            res = EOK ;

        }

    }


    return res ;
}

int32_t
nlog2_iterator_next (NLOG2_ITERATOR_T *it)
{
    int32_t res = _iterator_next (it) ;
    NLOG2_T * plog = it->plog ;

    while ((res == EOK) && (record_type(plog, &it->header) != RECORD_TYPE_RECORD)) {
        res = _iterator_next (it) ;
    }

    return res ;
}

int32_t
nlog2_iterator_read (NLOG2_ITERATOR_T *it, char* data, int32_t len)
{
    NLOG2_T * plog = it->plog ;
    uint32_t type ;

    type = record_type (plog, &it->header) ;

    if (type == RECORD_TYPE_RECORD) {

        if ((uint32_t)len > it->header.len) len = it->header.len ;
        if (len && data) {
            FLASH_READ (it->header.this + sizeof(NLOG2_LOG_HEADER_T),  len, (uint8_t*)data) ;

        }
        else if (len == 0) len = it->header.len ;
        else len = E_PARM ;

    } else {
        return EFAIL ;

    }

    return len ;
}

static int32_t
_iterator_prev (NLOG2_ITERATOR_T *it)
{
    int32_t res = EFAIL ;
    NLOG2_T * plog ;
    uint32_t type ;
    NLOG2_LOG_HEADER_T prev ;

    plog = it->plog ;
    type = record_type (plog, &it->header) ;


    if (    (type == RECORD_TYPE_EMPTY) ||
            (type == RECORD_TYPE_INVALID)) {
        res = EFAIL ;

    } else if ( (it->header.previous == it->header.this) ||
                (type == RECORD_TYPE_CLOSED)) {
        res = E_BOF ;

    } else  {

        // read the previous record
        memset (&prev, 0xFF, sizeof(NLOG2_LOG_HEADER_T)) ;
        FLASH_READ (it->header.previous,  sizeof(NLOG2_LOG_HEADER_T) ,
                        (uint8_t*)&prev) ;
        type = record_type(plog, &prev) ;

        if (    (type == RECORD_TYPE_CLOSED) ||
                (type == RECORD_TYPE_EMPTY) ||
                (type == RECORD_TYPE_INVALID)
                ) {
            res = E_BOF ;

        } else  {
            memcpy (&it->header, &prev, sizeof(NLOG2_LOG_HEADER_T)) ;
            res = EOK ;

        }
    }

    return res ;
}

int32_t
nlog2_iterator_prev (NLOG2_ITERATOR_T *it)
{
    int32_t res = _iterator_prev (it) ;
    NLOG2_T * plog = it->plog ;

    while ((res == EOK) && (record_type(plog, &it->header) != RECORD_TYPE_RECORD)) {
        res = _iterator_prev (it) ;
    }
    if (res == E_BOF) {
        _iterator_next (it) ;
    }

    return res ;
}


uint32_t
nlog2_iterator_id (NLOG2_ITERATOR_T *it)
{
    return it->header.id ;
}



