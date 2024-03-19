/**
  ******************************************************************************
  * @file    low_level_obkeys_device.c
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_obkeys
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
#include "flash_layout.h"
#include "low_level_obkeys.h"

OBK_FlashRange erase_vect[] =
{
#if  defined(OEMUROT_ENABLE)
  { OBK_HDPL2_OFFSET + OBK_HDPL2_DATA_OFFSET, OBK_HDPL2_OFFSET + OBK_HDPL2_DATA_OFFSET + OBK_HDPL2_DATA_SIZE - 1},
#else
  { OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET, OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET + OBK_HDPL1_DATA_SIZE - 1},
#endif
};

OBK_FlashRange write_vect[] =
{
#if  defined(OEMUROT_ENABLE)
  { OBK_HDPL2_OFFSET + OBK_HDPL2_DATA_OFFSET, OBK_HDPL2_OFFSET + OBK_HDPL2_DATA_OFFSET + OBK_HDPL2_DATA_SIZE - 1},
#else
  { OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET, OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET + OBK_HDPL1_DATA_SIZE - 1},
#endif
};

OBK_LowLevelDevice OBK_FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(OBK_FlashRange), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(OBK_FlashRange), .range = write_vect},
  .read_error = 1
};
