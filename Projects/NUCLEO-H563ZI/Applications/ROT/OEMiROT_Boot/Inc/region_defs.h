/*
 * Copyright (c) 2017-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __REGION_DEFS_H__
#define __REGION_DEFS_H__
#include "flash_layout.h"

#define BL2_HEAP_SIZE           0x0000000
#define BL2_MSP_STACK_SIZE      0x0002000

/* GTZC specific Alignment */
#define GTZC_RAM_ALIGN 512
#define GTZC_FLASH_ALIGN 8192

#define _SRAM1_SIZE_MAX         (0x40000) /*!< SRAM1=256 KB */
#define _SRAM2_SIZE_MAX         (0x10000) /*!< SRAM2=64 KB */
#define _SRAM3_SIZE_MAX         (0x50000) /*!< SRAM3=320 KB */

/* Flash and internal SRAMs base addresses - Non secure aliased */
#define _FLASH_BASE_NS          (0x08000000) /*!< FLASH(2 MB) base address */
#define _SRAM1_BASE_NS          (0x20000000) /*!< SRAM1(256 KB) base address */
#define _SRAM2_BASE_NS          (0x20040000) /*!< SRAM2(64 KB) base address */
#define _SRAM3_BASE_NS          (0x20050000) /*!< SRAM3(320 KB) base address */
/* Flash and internal SRAMs base addresses - Secure aliased */
#define _FLASH_BASE_S           (0x0C000000) /*!< FLASH(2 MB) base address */
#define _SRAM1_BASE_S           (0x30000000) /*!< SRAM1(256 KB) base address */
#define _SRAM2_BASE_S           (0x30040000) /*!< SRAM2(64 KB) base address */
#define _SRAM3_BASE_S           (0x30050000) /*!< SRAM3(320 KB) base address */

#define TOTAL_ROM_SIZE          FLASH_TOTAL_SIZE
#define S_TOTAL_RAM_SIZE        (_SRAM2_SIZE_MAX) /*! SRAM size for Secure part */

/*  This area in SRAM 2 is updated BL2 and can be lock to avoid any changes */
#define BOOT_SHARED_DATA_SIZE        0
#define BOOT_SHARED_DATA_BASE        0

/*
 * Boot partition structure if MCUBoot is used:
 * 0x0_0000 Bootloader header
 * 0x0_0400 Image area
 * 0xz_zzzz Trailer
 */
/* IMAGE_CODE_SIZE is the space available for the software binary image.
 * It is less than the FLASH_PARTITION_SIZE because we reserve space
 * for the image header and trailer introduced by the bootloader.
 */

#define BL2_HEADER_SIZE                     (0x400) /*!< Appli image header size */
#define BL2_DATA_HEADER_SIZE                (0x20)  /*!< Data image header size */
#define BL2_TRAILER_SIZE                    (0x2000)

#ifdef BL2
#define S_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_0_OFFSET)
#define S_IMAGE_SECONDARY_PARTITION_OFFSET  (FLASH_AREA_2_OFFSET)
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE)
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET    (FLASH_AREA_4_OFFSET)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET   (FLASH_AREA_5_OFFSET)
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */
#else
#error "Config without BL2 not supported"
#endif /* BL2 */


#define S_ROM_ALIAS_BASE                    (_FLASH_BASE_S)
#define NS_ROM_ALIAS_BASE                   (_FLASH_BASE_NS)

#define S_RAM_ALIAS_BASE                    (_SRAM1_BASE_S)
#define NS_RAM_ALIAS_BASE                   (_SRAM1_BASE_NS)

/* Alias definitions for secure and non-secure areas*/
#define S_ROM_ALIAS(x)                      (S_ROM_ALIAS_BASE + (x))
#define NS_ROM_ALIAS(x)                     (NS_ROM_ALIAS_BASE + (x))

#define S_RAM_ALIAS(x)                      (S_RAM_ALIAS_BASE + (x))
#define NS_RAM_ALIAS(x)                     (NS_RAM_ALIAS_BASE + (x))


/* Secure regions */
#define S_IMAGE_PRIMARY_AREA_OFFSET         (S_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define S_CODE_START                        (S_ROM_ALIAS(S_IMAGE_PRIMARY_AREA_OFFSET))

#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_AREA_OFFSET    (S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_DATA_HEADER_SIZE)
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */

/* Non-secure regions */
#define NS_IMAGE_PRIMARY_AREA_OFFSET        (NS_IMAGE_PRIMARY_PARTITION_OFFSET + BL2_HEADER_SIZE)
#define NS_CODE_START                       (NS_ROM_ALIAS(NS_IMAGE_PRIMARY_AREA_OFFSET))

#ifdef BL2
/* Bootloader region protected by hdp */
#define BL2_CODE_START                      (S_ROM_ALIAS(FLASH_AREA_BL2_OFFSET))
#define BL2_CODE_SIZE                       (FLASH_AREA_BL2_SIZE)
#define BL2_CODE_LIMIT                      (BL2_CODE_START + BL2_CODE_SIZE - 1)

/* Bootloader boot address */
#define BL2_BOOT_VTOR_ADDR                  (BL2_CODE_START)

/*  keep 256 bytes unused to place while(1) for non secure to enable */
/*  regression from local tool with non secure attachment
 *  This avoid blocking board in case of hardening error */
#define BL2_DATA_START                      (_SRAM2_BASE_S)
#define BL2_DATA_SIZE                       (_SRAM2_SIZE_MAX)
#define BL2_DATA_LIMIT                      (BL2_DATA_START + BL2_DATA_SIZE - 1)

/* Define BL2 MPU SRAM protection to remove execution capability */
/* Area is covering the complete SRAM memory space non secure alias and secure alias */
#define BL2_SRAM_AREA_BASE                  (_SRAM1_BASE_NS)
#define BL2_SRAM_AREA_END                   (_SRAM3_BASE_S + _SRAM3_SIZE_MAX - 1)
#endif /* BL2 */


/* Additional Check to detect flash download slot overlap or overflow */
#define FLASH_AREA_END_OFFSET_MAX (FLASH_TOTAL_SIZE)
#if FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX
#error "Flash memory overflow"
#endif /* FLASH_AREA_END_OFFSET > FLASH_AREA_END_OFFSET_MAX */

#endif /* __REGION_DEFS_H__ */
