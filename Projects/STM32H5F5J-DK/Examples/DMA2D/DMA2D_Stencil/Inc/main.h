/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "BGND_Wood_ARGB4444.h"
#include "BGND_Blue_ARGB444.h"
#include "ST_LOGO_ARGB4444.h"
#include "ST_Txt_Stencil.h"
#include "stm32h5f5j_discovery.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ST1633I_WIDTH 480
#define ST1633I_HEIGHT 272
#define ST1633I_HSYNC 4
#define ST1633I_HBP 43
#define ST1633I_HFP 0
#define ST1633I_VSYNC 4
#define ST1633I_VBP 7
#define ST1633I_VFP 8
#define LCD_FRAME_BUFFER 0x20010000
#define FRAME_BUFFER_SIZE (ST_LOGO_IMAGE_WIDTH*ST_LOGO_IMAGE_HEIGHT*ST_LOGO_IMAGE_BPP)/32
#define LCD_BL_CTRL_Pin GPIO_PIN_8
#define LCD_BL_CTRL_GPIO_Port GPIOF
#define LCD_ON_Pin GPIO_PIN_3
#define LCD_ON_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
