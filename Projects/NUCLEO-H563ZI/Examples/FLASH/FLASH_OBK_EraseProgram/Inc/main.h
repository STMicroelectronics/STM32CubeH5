/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_OBK_EraseProgram/Inc/main.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines of the application
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
#include "stm32h5xx_nucleo.h"
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

/* USER CODE BEGIN Private defines */

/* Base address of the Flash sectors */

#define HDPL1_START_ADRESS     (FLASH_OBK_HDPL1_BASE_NS)                            /* Base @ of HDPL 1 start address */
#define HDPL1_END_ADRESS       (FLASH_OBK_HDPL1_BASE_NS + FLASH_OBK_HDPL1_SIZE - 1) /* Base @ of HDPL 1 end address   */
#define HDPL2_START_ADRESS     (FLASH_OBK_HDPL2_BASE_NS)                            /* Base @ of HDPL 2 start address */
#define HDPL2_END_ADRESS       (FLASH_OBK_HDPL2_BASE_NS + FLASH_OBK_HDPL2_SIZE - 1) /* Base @ of HDPL 2 end address   */
#define HDPL3_START_ADRESS     (FLASH_OBK_HDPL3_BASE_NS)                            /* Base @ of HDPL 3 start address */
#define HDPL3_END_ADRESS       (FLASH_OBK_HDPL3_BASE_NS + FLASH_OBK_HDPL3_SIZE - 1) /* Base @ of HDPL 3 end address   */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
