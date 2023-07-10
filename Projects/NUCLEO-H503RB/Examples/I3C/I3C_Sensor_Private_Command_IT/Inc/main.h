/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Sensor_Private_Command_IT/Inc/main.h
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
#if defined(__GNUC__)
#include <stdio.h>      /*rtt*/
#include <stdlib.h>     /*rtt*/
#endif
#include "stdio.h"
#include "string.h"
#include "stm32h5xx_nucleo.h"
#include "stm32h5xx_util_i3c.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* Target descriptor */
typedef struct {
  char *        TARGET_NAME;          /*!< Marketing Target reference */
  uint32_t      TARGET_ID;            /*!< Target Identifier on the Bus */
  uint64_t      TARGET_BCR_DCR_PID;   /*!< Concatenation value of PID, BCR and DCR of the target */
  uint8_t       STATIC_ADDR;          /*!< Static Address of the target, value found in the datasheet of the device */
  uint8_t       DYNAMIC_ADDR;         /*!< Dynamic Address of the target preset by software/application */
} TargetDesc_TypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* Broadcast Command code */
#define Broadcast_DISEC             0x01
#define Broadcast_RSTDAA            0x06

/* Direct Command code */
#define Direct_DISEC                0x81
#define Direct_SETDASA              0x87
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* Define Target Identifier */
#define DEVICE_ID1                        0U

/* Define LSM6DS0 Static Address */
#define LSM6DSO_STA                       (0xD6>>1)

#define DISPLAY_REFRESH_DELAY             1000 /* Time between two display refresg in ms */

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
#define LSM6DSOCONFIGSIZE                 (COUNTOF(aLSM6DSO_Config))

/* Size of Reception buffer */
#define LSM6DSODATASIZE                   (COUNTOF(aLSM6DSO_Data))
#define RXBUFFERSIZE                      14
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
