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
  { OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET, OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET + OBK_HDPL1_DATA_SIZE - 1},
};

OBK_FlashRange write_vect[] =
{
  { OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET, OBK_HDPL1_OFFSET + OBK_HDPL1_DATA_OFFSET + OBK_HDPL1_DATA_SIZE - 1},
};

OBK_LowLevelDevice OBK_FLASH0_DEV =
{
  .erase = { .nb = sizeof(erase_vect) / sizeof(OBK_FlashRange), .range = erase_vect},
  .write = { .nb = sizeof(write_vect) / sizeof(OBK_FlashRange), .range = write_vect},
  .read_error = 1
};
