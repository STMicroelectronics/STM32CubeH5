/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
/* Constants used by Serial Command Line Mode */
#define TX_TIMEOUT          ((uint32_t)1000U)
#define RX_TIMEOUT          ((uint32_t)2000U)
/*#define DATA_IMAGE_EN*/
#define S_CODE_OFFSET      0x00000000 /* This define is updated automatically from ROT_BOOT project */
#define S_CODE_SIZE        0x00020000 /* This define is updated automatically from ROT_BOOT project */
#define IMAGE_HEADER_SIZE  (0x400)  /* mcuboot headre size */
#define S_CODE_START       (FLASH_BASE_S + S_CODE_OFFSET + IMAGE_HEADER_SIZE)
/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
