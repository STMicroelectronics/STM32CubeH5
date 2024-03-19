/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_EDATA_EraseProgram/Inc/main.h
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

#define ADDR_EDATA1_STRT_0     (0x0900A800U) /* Base @ of last sector of Bank1 reserved to EDATA (EDATA1_STRT = 0), 6 Kbytes    */
#define ADDR_EDATA1_STRT_1     (0x09009000U) /* Base @ of last 2 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 1), 6 Kbytes */
#define ADDR_EDATA1_STRT_2     (0x09007800U) /* Base @ of last 3 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 2), 6 Kbytes */
#define ADDR_EDATA1_STRT_3     (0x09006000U) /* Base @ of last 4 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 3), 6 Kbytes */
#define ADDR_EDATA1_STRT_4     (0x09004800U) /* Base @ of last 5 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 4), 6 Kbytes */
#define ADDR_EDATA1_STRT_5     (0x09003000U) /* Base @ of last 6 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 5), 6 Kbytes */
#define ADDR_EDATA1_STRT_6     (0x09001800U) /* Base @ of last 7 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 6), 6 Kbytes */
#define ADDR_EDATA1_STRT_7     (0x09000000U) /* Base @ of last 8 sectors of Bank1 reserved to EDATA (EDATA1_STRT = 7), 6 Kbytes */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
