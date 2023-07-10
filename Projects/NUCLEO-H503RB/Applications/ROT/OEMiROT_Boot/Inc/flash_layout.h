/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
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

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* This header file is included from linker scatter file as well, where only a
 * limited C constructs are allowed. Therefore it is not possible to include
 * here the platform_retarget.h to access flash related defines. To resolve this
 * some of the values are redefined here with different names, these are marked
 * with comment.
 */

/* Flash layout configuration : begin */

#define MCUBOOT_EXT_LOADER     /* Defined: Use system bootloader (in system flash).
                                               To enter it, press user button at reset.
                                      Undefined: Do not use system bootloader. */

#define MCUBOOT_DATA_IMAGE_NUMBER 0     /* 1: Data image for application.
                                           0: No data image. */

/* Flash layout configuration : end */

/* Only primary slot for each image : secondary slot mode not supported */
#define MCUBOOT_PRIMARY_ONLY

/* The FW installation uses overwrite method : secondary slot mode not supported */
#define MCUBOOT_OVERWRITE_ONLY


/* Total number of images */
#define MCUBOOT_APP_IMAGE_NUMBER 1
#define MCUBOOT_IMAGE_NUMBER (MCUBOOT_APP_IMAGE_NUMBER + MCUBOOT_DATA_IMAGE_NUMBER)
#define MCUBOOT_NS_DATA_IMAGE_NUMBER MCUBOOT_DATA_IMAGE_NUMBER

/* Use image hash reference to reduce boot time (signature check bypass) */
#define MCUBOOT_USE_HASH_REF

/* control configuration */
#if !defined(MCUBOOT_PRIMARY_ONLY) || !defined(MCUBOOT_OVERWRITE_ONLY)
#error "Config not supported:  secondary slot mode not supported."
#endif


#if defined(MCUBOOT_PRIMARY_ONLY) && !defined(MCUBOOT_OVERWRITE_ONLY)
#error "Config not supported: When MCUBOOT_PRIMARY_ONLY is enabled, MCUBOOT_OVERWRITE_ONLY is required."
#endif

/* The size of a partition. This should be large enough to contain a S or NS
 * sw binary. Each FLASH_AREA_IMAGE contains two partitions. See Flash layout
 * above.
 */
#define LOADER_FLASH_DEV_NAME             TFM_Driver_FLASH0

/* Flash layout info for BL2 bootloader */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x2000)     /* 8 KB */
#define FLASH_AREA_WRP_GROUP_SIZE       (0x2000)     /* 8 KB */
#define FLASH_B_SIZE                    (0x10000)    /* 64 KBytes */
#define FLASH_TOTAL_SIZE                (FLASH_B_SIZE+FLASH_B_SIZE) /* 128 KBytes */
#define FLASH_BASE_ADDRESS              (0x08000000)

/* Flash area IDs */
#define FLASH_AREA_0_ID                 (1)
#if (MCUBOOT_DATA_IMAGE_NUMBER == 1)
#define FLASH_AREA_5_ID                 (6)
#endif /* MCUBOOT_DATA_IMAGE_NUMBER == 1 */

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_0 and IMAGE_1, SCRATCH
 * is used as a temporary storage during image swapping.
 */
/* area for image HASH references */
#define FLASH_HASH_REF_AREA_OFFSET      (0x0000)
#if defined(MCUBOOT_USE_HASH_REF)
#define FLASH_HASH_REF_AREA_SIZE        (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_HASH_REF_AREA_SIZE        (0x0000)
#endif /* MCUBOOT_USE_HASH_REF */

/* area for HUK and anti roll back counter */
#define FLASH_BL2_NVCNT_AREA_OFFSET     (FLASH_HASH_REF_AREA_OFFSET + FLASH_HASH_REF_AREA_SIZE)
#define FLASH_BL2_NVCNT_AREA_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* scratch area */
#define FLASH_AREA_SCRATCH_SIZE         (0x0000) /* Not used in MCUBOOT_OVERWRITE_ONLY mode */

/* personal area */
#define FLASH_AREA_PERSO_OFFSET         (FLASH_BL2_NVCNT_AREA_OFFSET + FLASH_BL2_NVCNT_AREA_SIZE)
#define FLASH_AREA_PERSO_SIZE           (0x400)
/* control personal area */
#if (FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_PERSO_OFFSET not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* FLASH_AREA_PERSO_OFFSET % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/* area for BL2 code protected by hdp */
#define FLASH_AREA_BL2_OFFSET           (FLASH_AREA_PERSO_OFFSET+FLASH_AREA_PERSO_SIZE)
#define FLASH_AREA_BL2_SIZE             0xDC00
/* HDP area end at this address */
#define FLASH_BL2_HDP_END               (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE-1)
/* control area for BL2 code protected by hdp */
#if ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "HDP area must be aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
/* control area under WRP group protection */
#if (FLASH_AREA_PERSO_OFFSET % FLASH_AREA_WRP_GROUP_SIZE) != 0
#error "FLASH_AREA_PERSO_OFFSET not aligned on FLASH_AREA_WRP_GROUP_SIZE"
#endif /* (FLASH_AREA_PERSO_OFFSET % FLASH_AREA_WRP_GROUP_SIZE) != 0 */
#if ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_WRP_GROUP_SIZE) != 0
#error "(FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) not aligned on FLASH_AREA_WRP_GROUP_SIZE"
#endif /* ((FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE) % FLASH_AREA_WRP_GROUP_SIZE) != 0 */

/* BL2 partitions size */
#if (MCUBOOT_DATA_IMAGE_NUMBER == 1)
#define FLASH_PARTITION_SIZE            (0xC000) /* 48 KB */
#else
#define FLASH_PARTITION_SIZE            (0xE000) /* 56 KB */
#endif /* MCUBOOT_PRIMARY_ONLY */

#define FLASH_MAX_APP_PARTITION_SIZE    FLASH_PARTITION_SIZE
#if (MCUBOOT_DATA_IMAGE_NUMBER == 1)
#define FLASH_DATA_PARTITION_SIZE       (FLASH_AREA_IMAGE_SECTOR_SIZE)
#else
#define FLASH_DATA_PARTITION_SIZE       (0x0)
#endif /* (MCUBOOT_DATA_IMAGE_NUMBER == 1) */

#define FLASH_MAX_DATA_PARTITION_SIZE   (FLASH_DATA_PARTITION_SIZE)
#define FLASH_MAX_PARTITION_SIZE        ((FLASH_MAX_APP_PARTITION_SIZE >   \
                                         FLASH_MAX_DATA_PARTITION_SIZE) ? \
                                         FLASH_MAX_APP_PARTITION_SIZE : \
                                         FLASH_MAX_DATA_PARTITION_SIZE)

/* BL2 flash areas */
#define FLASH_AREA_BEGIN_OFFSET         (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE)
#define FLASH_AREAS_DEVICE_ID           (FLASH_DEVICE_ID - FLASH_DEVICE_ID)


/* Secure app image primary slot */
#if defined(FLASH_AREA_0_ID)
#define FLASH_AREA_0_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_0_OFFSET             (FLASH_AREA_BEGIN_OFFSET)
#define FLASH_AREA_0_SIZE               (FLASH_PARTITION_SIZE)
/* Control Secure app image primary slot */
#if (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_0_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_0_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */
#else /* FLASH_AREA_0_ID */
#define FLASH_AREA_0_SIZE               (0x0)
#endif /* FLASH_AREA_0_ID */

/* Non-secure data image primary slot */
#if defined(FLASH_AREA_5_ID)
#define FLASH_AREA_5_DEVICE_ID          (FLASH_AREAS_DEVICE_ID)
#define FLASH_AREA_5_OFFSET             (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_5_SIZE               (FLASH_DATA_PARTITION_SIZE)
/* Control Non-secure data image primary slot */
#if (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_5_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /* (FLASH_AREA_5_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0  */
#else /* FLASH_AREA_5_ID */
#define FLASH_AREA_5_SIZE               (0x0)
#endif /* FLASH_AREA_5_ID */

/* flash areas end offset */
#define FLASH_AREA_END_OFFSET           (FLASH_AREA_BEGIN_OFFSET + FLASH_AREA_0_SIZE + \
                                         FLASH_AREA_5_SIZE)
/* Control flash area end */
#if (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0
#error "FLASH_AREA_END_OFFSET  not aligned on FLASH_AREA_IMAGE_SECTOR_SIZE"
#endif /*  (FLASH_AREA_END_OFFSET  % FLASH_AREA_IMAGE_SECTOR_SIZE) != 0 */

/*
 * The maximum number of status entries supported by the bootloader.
 */
#define MCUBOOT_STATUS_MAX_ENTRIES         ((FLASH_MAX_PARTITION_SIZE) / \
                                            FLASH_AREA_SCRATCH_SIZE)
/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS           ((FLASH_MAX_PARTITION_SIZE) / \
                                           FLASH_AREA_IMAGE_SECTOR_SIZE)
/* Internal TMP buffer size used by the bootloader */
#define MCUBOOT_TMPBUF_SZ                 (0x80)

#define IMAGE_MAX_SIZE       FLASH_PARTITION_SIZE


/* Flash device name used by BL2 and NV Counter
 * Name is defined in flash driver file: low_level_flash.c
 */
/* Flash Driver Used to Confirm NonSecure App Image or MCUBOOT_APP_IMAGE_NUMBER = 1 */
#define  FLASH_PRIMARY_NONSECURE_DEV_NAME          TFM_Driver_FLASH0
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_DATA_IMAGE_NUMBER == 1)
/* Flash Driver Used to Confirm NonSecure Data Image */
#define  FLASH_PRIMARY_DATA_NONSECURE_DEV_NAME     TFM_Driver_FLASH0
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_DATA_IMAGE_NUMBER == 1) */
#define FLASH_DEV_NAME                             TFM_Driver_FLASH0


/* BL2 NV Counters definitions  */
#define BL2_NV_COUNTERS_AREA_ADDR        FLASH_BL2_NVCNT_AREA_OFFSET
#define BL2_NV_COUNTERS_AREA_SIZE        FLASH_BL2_NVCNT_AREA_SIZE


/* Systeme Flash description */
#define NSS_LIB_BASE                        (0x0BF84000U)
#define NSS_LIB_SIZE                        (0x3000U)
#define BOOTLOADER_BASE                     (0x0BF87000U)
#define BOOTLOADER_SIZE                     (0x9000U)
#define STM32_DESCRIPTOR_BASE_NS            (0x0BF8FE00U)
#define STM32_DESCRIPTOR_SIZE               (0x100U)

#endif /* __FLASH_LAYOUT_H__ */
