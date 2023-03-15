/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"

#define _HEX(n) 0x##n
#define HEX(n)  _HEX(n)

/* Enumeration that is used by the assemble.py and imgtool\main.py scripts
 * for correct binary generation when nested macros are used
 */

enum image_attributes
{
/* area for preparing images */
  RE_IMAGE_FLASH_SIZE = (IMAGE_MAX_SIZE),
/* area for flashing images */
  RE_IMAGE_FLASH_ADDRESS = (FLASH_BASE_ADDRESS+FLASH_AREA_0_OFFSET),

  RE_AREA_0_OFFSET = (FLASH_AREA_0_OFFSET),
  RE_AREA_0_SIZE = (FLASH_AREA_0_SIZE),

  RE_DATA_IMAGE_NUMBER = (HEX(MCUBOOT_DATA_IMAGE_NUMBER)),
#if (MCUBOOT_DATA_IMAGE_NUMBER == 1)
  RE_IMAGE_FLASH_ADDRESS_DATA = (FLASH_BASE_ADDRESS+FLASH_AREA_5_OFFSET),
  RE_IMAGE_FLASH_DATA_UPDATE = (FLASH_BASE_ADDRESS+FLASH_AREA_5_OFFSET),
  RE_IMAGE_FLASH_DATA_IMAGE_SIZE = (FLASH_DATA_PARTITION_SIZE),
#else
  RE_IMAGE_FLASH_ADDRESS_DATA = (0x0),
  RE_IMAGE_FLASH_DATA_UPDATE = (0x0),
  RE_IMAGE_FLASH_DATA_IMAGE_SIZE = (0x0),
#endif /* (MCUBOOT_DATA_IMAGE_NUMBER == 1) */
  RE_BL2_PERSO_ADDRESS = (FLASH_BASE_ADDRESS+FLASH_AREA_PERSO_OFFSET),
  RE_BL2_BOOT_ADDRESS = (BL2_BOOT_VTOR_ADDR),
  /* area for programming hardening on page */
  RE_BL2_WRP_START = (FLASH_AREA_PERSO_OFFSET),
  RE_BL2_WRP_END = (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE-0x1),
#if defined(OEMUROT_ENABLE)
  RE_BL2_HDP_START = 0x1,
  RE_BL2_HDP_END = 0x0,
#else
  RE_BL2_HDP_START = 0x0,
  RE_BL2_HDP_END = (FLASH_AREA_BL2_OFFSET+FLASH_AREA_BL2_SIZE-0x1),
#endif
  RE_OVER_WRITE = (0x1),
  RE_FLASH_PAGE_NBR = (0x7)
};
