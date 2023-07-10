/* ########### HEADER GENERATED AUTOMATICALLY DONT TOUCH IT ########### */

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

#ifndef __APPLI_FLASH_LAYOUT_H__
#define __APPLI_FLASH_LAYOUT_H__


#define MCUBOOT_OVERWRITE_ONLY                 /* Defined: the FW installation uses overwrite method.
                                                  UnDefined: The FW installation uses swap mode. */

#define MCUBOOT_APP_IMAGE_NUMBER       0x2      /* 1: S and NS application binaries are assembled in one single image.
                                                   2: Two separated images for S and NS application binaries. */

#define MCUBOOT_S_DATA_IMAGE_NUMBER    0x0      /* 1: S data image for S application.
                                                   0: No S data image. */

#define MCUBOOT_NS_DATA_IMAGE_NUMBER   0x0      /* 1: NS data image for NS application.
                                                   0: No NS data image. */

#define FLASH_AREA_0_OFFSET            0x18000 /* Secure app image primary slot offset */

#define FLASH_AREA_0_SIZE              0x6000   /* Secure app image primary slot size */

#define FLASH_AREA_1_OFFSET            0x1E000  /* Non-secure app image primary slot offset */

#define FLASH_AREA_1_SIZE              0xA0000   /* Non-secure app image primary slot size */

#define FLASH_AREA_2_OFFSET            0xBE000  /* Secure app image secondary slot */

#define FLASH_AREA_2_SIZE              0x6000   /* Secure app image secondary slot size */

#define FLASH_AREA_4_OFFSET            0x0 /* Secure data image primary slot offset */

#define FLASH_AREA_4_SIZE              0x0   /* Secure data image primary slot size */

#define FLASH_AREA_5_OFFSET            0x0  /* Non-secure data image primary slot offset */

#define FLASH_AREA_5_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_AREA_6_OFFSET            0x0 /* Non-secure data image primary slot offset */

#define FLASH_AREA_6_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_AREA_7_OFFSET            0x0 /* Non-secure data image primary slot offset */

#define FLASH_AREA_7_SIZE              0x0   /* Non-secure data image primary slot size */

#define FLASH_PARTITION_SIZE           0xA6000  /* Secure and Non Secure partition size */

#define FLASH_NS_PARTITION_SIZE        0xA0000  /* Non Secure partition size */

#define FLASH_S_PARTITION_SIZE         0x6000   /* secure partition size */

#define FLASH_S_DATA_PARTITION_SIZE    0x0   /* secure data partition size */

#define FLASH_NS_DATA_PARTITION_SIZE   0x0   /* non secure data partition size */

#define FLASH_B_SIZE                   0x100000   /* flash bank size: 1 MBytes*/

#define FLASH_TOTAL_SIZE               (FLASH_B_SIZE+FLASH_B_SIZE) /* total flash size: 2 MBytes */

#define TRAILER_MAGIC_SIZE 16

#define S_IMAGE_PRIMARY_PARTITION_OFFSET     FLASH_AREA_0_OFFSET
#define S_IMAGE_SECONDARY_PARTITION_OFFSET   FLASH_AREA_2_OFFSET
#define NS_IMAGE_PRIMARY_PARTITION_OFFSET    FLASH_AREA_0_OFFSET + FLASH_S_PARTITION_SIZE
#define NS_IMAGE_SECONDARY_PARTITION_OFFSET  FLASH_AREA_2_OFFSET + FLASH_S_PARTITION_SIZE
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#define S_DATA_IMAGE_PRIMARY_PARTITION_OFFSET     FLASH_AREA_4_OFFSET
#define S_DATA_IMAGE_SECONDARY_PARTITION_OFFSET   FLASH_AREA_6_OFFSET
#endif /* MCUBOOT_S_DATA_IMAGE_NUMBER == 1 */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#define NS_DATA_IMAGE_PRIMARY_PARTITION_OFFSET    FLASH_AREA_5_OFFSET
#define NS_DATA_IMAGE_SECONDARY_PARTITION_OFFSET  FLASH_AREA_7_OFFSET
#endif /* MCUBOOT_NS_DATA_IMAGE_NUMBER == 1 */


#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2)
/* Flash Driver Used to Confirm Secure App Image */
#define  FLASH_PRIMARY_SECURE_DEV_NAME             TFM_Driver_FLASH0
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2) */
/* Flash Driver Used to Confirm NonSecure App Image or MCUBOOT_APP_IMAGE_NUMBER = 1 */
#define  FLASH_PRIMARY_NONSECURE_DEV_NAME          TFM_Driver_FLASH0
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
/* Flash Driver Used to Confirm Secure Data Image */
#define  FLASH_PRIMARY_DATA_SECURE_DEV_NAME        TFM_Driver_FLASH0
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
/* Flash Driver Used to Confirm NonSecure Data Image */
#define  FLASH_PRIMARY_DATA_NONSECURE_DEV_NAME     TFM_Driver_FLASH0
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#define FLASH_DEV_NAME                             TFM_Driver_FLASH0


#endif /* __FLASH_LAYOUT_H__ */
