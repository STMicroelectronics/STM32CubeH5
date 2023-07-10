/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for main application file.
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
#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "com.h"
#include "appli_flash_layout.h"
#define CODE_OFFSET      0x12000 /* This define is updated automatically from ROT_BOOT project */
#define CODE_SIZE        0xC000 /* This define is updated automatically from ROT_BOOT project */
#define IMAGE_HEADER_SIZE  (0x400)  /* mcuboot headre size */
#define S_CODE_START       (FLASH_BASE_NS + CODE_OFFSET + IMAGE_HEADER_SIZE)
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#endif /* MAIN_H */
