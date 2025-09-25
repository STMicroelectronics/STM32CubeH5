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


#ifndef __NVOL3_H__
#define __NVOL3_H__

#include "qoraal/qoraal.h"
#include "qoraal/common/dictionary.h"
#include <stdint.h>

#define DBG_MESSAGE_NVOL3(severity, fmt_str, ...)       DBG_MESSAGE_T_REPORT(SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)
#define DBG_ASSERT_NVOL3                                DBG_ASSERT_T
#define DBG_CHECK_NVOL3                                 DBG_CHECK_T

/*===========================================================================*/
/* Module constants.                                                         */
/*===========================================================================*/


#define NVOL3_SECTOR_VERSION_0                  0x0000
#define NVOL3_SECTOR_VERSION_1                  0x0155
#define NVOL3_SECTOR_VERSION_2                  0x0255
#define NVOL3_SECTOR_VERSION_3                  0x0355
#define NVOL3_SECTOR_VERSION_4                  0x0455
#define NVOL3_SECTOR_VERSION_5                  0x0555
#define NVOL3_SECTOR_VERSION_6                  0x0655
#define NVOL3_SECTOR_VERSION                    NVOL3_SECTOR_VERSION_2


#define NVOL3_PAGE_SIZE                         0x20            /**< @brief one page used at the start of the sector */
#define NVOL3_HEADROOM                          0x04            /**< @brief min available slots before volume is full */
#define NVOL3_MALLOC(size)                      qoraal_malloc(QORAAL_HeapAuxiliary, size)
#define NVOL3_FREE(mem)                         qoraal_free(QORAAL_HeapAuxiliary, mem)

/*
 * ToDo: transactions
 */
#define NVOL3_TRANSACTION_STOP                  0
#define NVOL3_TRANSACTION_START                 1
#define NVOL3_TRANSACTION_ROLLBACK              2
#define NVOL3_TRANSACTION_COMMIT                3


/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   FLASH record header.
 */
#pragma pack(1)
typedef struct NVOL3_RECORD_HEAD_S {
    uint16_t            flags;                  /**< @brief     flags indicate variable status */
    uint16_t            length;                 /**< @brief     length of key and data, excluding this header length */
    uint16_t            checksum;               /**< @brief     2's complement checksum of key and data */
    uint16_t            reserved;
} NVOL3_RECORD_HEAD_T ;
#pragma pack()

#define NVOL3_RECORD_FLAGS_EMPTY                0xFFFF
#define NVOL3_RECORD_FLAGS_NEW                  0xAFFF
#define NVOL3_RECORD_FLAGS_PENDING              0xAAFF
#define NVOL3_RECORD_FLAGS_VALID                0xAAAF
#define NVOL3_RECORD_FLAGS_UPDATING             0xAAAA
#define NVOL3_RECORD_FLAGS_INVALID              0x0000

/**
 * @brief   FLASH record template.
 */
#pragma pack(1)
typedef struct NVOL3_RECORD_S
{
    NVOL3_RECORD_HEAD_T head ;                  /**< @brief     record header, opaque value for caller */
    uint8_t             key_and_data[];         /**< @brief     variable key used for lookup in dictionary followed by data */
} NVOL3_RECORD_T ;
#pragma pack()

/**
 * @brief   RAM entry for cached record and index into FLASH sector.
 *          This is the dictionary entry.
 */
#pragma pack(1)
typedef struct NVOL3_ENTRY_S
{
  uint16_t              idx;                    /**< @brief  offset (number of records) into current FLASH sector */
  uint16_t              length;                 /**< @brief  length of record cached in local */
  uint8_t               local[] ;               /**< @brief  local cache of record data (excluding the key)*/
} NVOL3_ENTRY_T;
#pragma pack()

struct NVOL3_INSTANCE_S ;
/*
 * Callback interface
 */
typedef int32_t (*NVLOL3_CALLBACK_T)(struct NVOL3_INSTANCE_S * /*inst*/, struct NVOL3_RECORD_S * /*record*/, uint32_t /*ctx*/) ;
typedef int32_t (*NVLOL3_TRANSACTION_CALLBACK_T)(struct NVOL3_INSTANCE_S * /*inst*/, int32_t /*cmd*/) ;

/*
 * Iterator callback
 */
typedef int32_t (*NVLOL3_IT_KEY_CMP_T)(const char * /* first*/, const char * /*second*/) ;

/*
 * ToDo: implement transactions
 */
#define NVOL3_TRANSACTION_CMD_GET               -1
#define NVOL3_TRANSACTION_CMD_SET_STOP          0
#define NVOL3_TRANSACTION_CMD_SET_START         1
#define NVOL3_TRANSACTION_CMD_SET_ROLLBACK      2
#define NVOL3_TRANSACTION_CMD_SET_COMMIT        3

/**
 * @brief   definition for a instance of a volume.
 */
typedef struct NVOL3_CONFIG_S {
    const char*         name ;
    uint32_t            sector1_addr ;          /**< @brief  start address of sector. address to be used by the driver */
    uint32_t            sector2_addr ;          /**< @brief  start address of sector. address to be used by the driver */
    uint32_t            sector_size ;           /**< @brief  size of sector 1 and to. this must be a multiple of the supported FLASH page sizes */
    uint16_t            record_size ;           /**< @brief  max record size including header, key and value */
    uint16_t            local_size ;            /**< @brief  size of value to cache in ram (only cached if length is <= than this size) */
    uint16_t            key_size ;              /**< @brief  key size used for indexing in dictionary (length allocated in FLASH) */
    uint16_t            hashsize ;              /**< @brief  hash size for lookup table in dictionary */
    uint32_t            keyspec ;               /**< @brief  key type as defined for dictionary */
    uint16_t            version ;               /**< @brief  sector version, saved per sector and checked when volume is loaded */
    uint16_t            reserved ;

    NVLOL3_TRANSACTION_CALLBACK_T transaction_cb ; /**< @brief  user keep track of the transaction state (should be persistent) */
    NVLOL3_CALLBACK_T   write_cb ;
    uint32_t            ctx ;

} NVOL3_CONFIG_T ;

/**
 * @brief   instance of a volume.
 */
typedef struct NVOL3_INSTANCE_S {

    const NVOL3_CONFIG_T*   config ;            /**< @brief  configuration for this nvol */
    uint16_t            version ;               /**< @brief  version loaded from FLASH */
    uint16_t            next_idx ;              /**< @brief  index for next empty slot */
    uint32_t            sector ;                /**< @brief  current sector in use */
    struct dictionary * dict ;                  /**< @brief  dictionary for record lookup by key index */
    uint32_t            inuse ;                 /**< @brief  current records in use */
    uint32_t            invalid ;               /**< @brief  current records invalid */
    uint32_t            error ;                 /**< @brief  current record errors */

} NVOL3_INSTANCE_T ;

/**
 * @brief   iterator for entries in the volume.
 */
typedef struct NVOL3_ITERATOR_S {
    struct dictionary_it    it ;
} NVOL3_ITERATOR_T ;

#define ALIGN_UP(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1))

/**
 * @brief   macros to declare instances of nvol. "name" to be used as NVOL3_INSTANCE_T instance parameter to the API
 */
#define NVOL3_INSTANCE_DECL(name, sector1, sector2, sector_size, key_size, keyspec, hashsize, data_size, local_size, tallie, version)  \
        static const NVOL3_CONFIG_T name ## _config = { #name, \
                        sector1, \
                        sector2, \
                        sector_size, \
                        ALIGN_UP(sizeof(NVOL3_RECORD_HEAD_T) + (key_size) + data_size, 32), \
                        local_size, \
                        key_size, \
                        hashsize, \
                        keyspec, \
                        version, \
                        0, \
                        0, \
                        nvol3_callback_tallie, \
                        tallie} ; \
        static NVOL3_INSTANCE_T name = { & name ## _config , 0, 0, 0, 0, 0, 0, 0 }


#define NVOL3_UINT_INSTANCE_DECL(name, sector1, sector2, sector_size, data_size, local_size, hashsize, tallie, version)  \
        NVOL3_INSTANCE_DECL(name, sector1, sector2, sector_size, sizeof(uint32_t), DICTIONARY_KEYSPEC_UINT, hashsize, data_size, \
        local_size, tallie, version)

#define NVOL3_USHORT_INSTANCE_DECL(name, sector1, sector2, sector_size, data_size, local_size, hashsize, tallie, version)  \
        NVOL3_INSTANCE_DECL(name, sector1, sector2, sector_size, sizeof(uint16_t), DICTIONARY_KEYSPEC_USHORT, hashsize, data_size, \
        local_size, tallie, version)

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Initialisation functions.
     */
    int32_t         nvol3_load (NVOL3_INSTANCE_T* instance) ;
    int32_t         nvol3_validate (NVOL3_INSTANCE_T* instance) ;
    int32_t         nvol3_reset (NVOL3_INSTANCE_T* instance) ;
    int32_t         nvol3_delete (NVOL3_INSTANCE_T* instance) ;
    int32_t         nvol3_repair (NVOL3_INSTANCE_T* instance) ;
    void            nvol3_unload (NVOL3_INSTANCE_T* instance) ;

    /*
     * API for writing to FLASH immediately.
     */
    int32_t         nvol3_record_set (NVOL3_INSTANCE_T* instance, NVOL3_RECORD_T *value, uint32_t key_and_data_length) ;
    int32_t         nvol3_record_get (NVOL3_INSTANCE_T* instance, NVOL3_RECORD_T *value) ;
    int32_t         nvol3_record_delete (NVOL3_INSTANCE_T* instance, NVOL3_RECORD_T *record) ;
    int32_t         nvol3_record_status (NVOL3_INSTANCE_T* instance, const char * key) ;
    int32_t         nvol3_record_key_and_data_length (NVOL3_INSTANCE_T* instance, const char * key) ;
    int32_t         nvol3_record_first (NVOL3_INSTANCE_T* instance, NVOL3_RECORD_T *value, NVOL3_ITERATOR_T * it, NVLOL3_IT_KEY_CMP_T cmp) ;
    int32_t         nvol3_record_next (NVOL3_INSTANCE_T* instance, NVOL3_RECORD_T *value, NVOL3_ITERATOR_T * it) ;
    const char *    nvol3_record_key (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;

    /*
     * API to access records from RAM and persist only on demand. locel_size should be same as data_size!
     */
    int32_t         nvol3_entry_first (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;
    int32_t         nvol3_entry_next (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;
    int32_t         nvol3_entry_at (NVOL3_INSTANCE_T* instance, const char * key, NVOL3_ITERATOR_T * it) ;
    const char *    nvol3_entry_key (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;
    int32_t         nvol3_entry_data (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it, char ** data) ;
    int32_t         nvol3_entry_save (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;
    int32_t         nvol3_entry_delete (NVOL3_INSTANCE_T* instance, NVOL3_ITERATOR_T * it) ;

    /*
     * print the status of the nvol to the debug output.
     */
    void            nvol3_entry_log_status (NVOL3_INSTANCE_T* instance, uint32_t verbose) ;

    /*
     * can be implemented to keep count of reads and writes to the nvol
     */
    int32_t         nvol3_callback_tallie (struct NVOL3_INSTANCE_S * inst, struct NVOL3_RECORD_S * record, uint32_t ctx) ;

    /*
     * ToDo: transactions
     */
    int32_t         nvol3_transaction_start (void) ;
    int32_t         nvol3_transaction_rollback (void) ;
    int32_t         nvol3_transaction_commit (void) ;


#ifdef __cplusplus
}
#endif


#endif /* __NVOL3_H__ */
