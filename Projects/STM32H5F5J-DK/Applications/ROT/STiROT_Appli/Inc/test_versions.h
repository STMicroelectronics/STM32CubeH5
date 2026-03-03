/**
  ******************************************************************************
  * @file    test_versions.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Test Version functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_VERSIONS_H
#define TEST_VERSIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#define APPLI_CODE_HEADER  (S_CODE_START - IMAGE_HEADER_SIZE)
/* Includes ------------------------------------------------------------------*/

/** @addtogroup TEST_VERSIONS
  * @{
  */
/** @defgroup TEST_VERSIONS_Exported_Constant Exported Constant
  * @{
  */

/**
  * @}
  */

/** @defgroup TEST_VERSIONS_Exported_Types Exported Types
  * @{
  */

struct image_version
{
  uint8_t iv_major;
  uint8_t iv_minor;
  uint16_t iv_revision;
  uint32_t iv_build_num;
};

/** Image header.  All fields are in little endian byte order. */
struct image_header
{
  uint32_t ih_magic;
  uint32_t ih_load_addr;
  uint16_t ih_hdr_size;           /* Size of image header (bytes). */
  uint16_t ih_protect_tlv_size;   /* Size of protected TLV area (bytes). */
  uint32_t ih_img_size;           /* Does not include header. */
  uint32_t ih_flags;              /* IMAGE_F_[...]. */
  struct image_version ih_ver;
  uint32_t _pad1;
};

/**
  * @}
  */

/** @addtogroup  TEST_VERSIONS_Exported_Functions
  * @{
  */
void TEST_VERSIONS_PrintVersions(void);

/** @addtogroup  S_DATA_Display function
  * @{
  */
void S_DATA_Display(void);
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* TEST_VERSIONS_H */
