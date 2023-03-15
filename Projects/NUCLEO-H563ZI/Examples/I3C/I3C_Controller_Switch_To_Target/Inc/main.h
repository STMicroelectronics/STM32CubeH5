/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Controller_Switch_To_Target/Inc/main.h
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
#include <stdio.h>	/*rtt*/
#include <stdlib.h>	/*rtt*/
#endif
#include "stdio.h"
#include "string.h"
#include "stm32h5xx_nucleo.h"
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
/* Direct Command code */
#define Direct_ENEC                 0x80
#define Direct_DISEC                0x81
#define Direct_ENTAS0               0x82
#define Direct_ENTAS1               0x83
#define Direct_ENTAS2               0x84
#define Direct_ENTAS3               0x85
#define Direct_SETDASA              0x87
#define Direct_SETNEWDA             0x88
#define Direct_SETMWL               0x89
#define Direct_SETMRL               0x8A
#define Direct_GETMWL               0x8B
#define Direct_GETMRL               0x8C
#define Direct_GETPID               0x8D
#define Direct_GETBCR               0x8E
#define Direct_GETDCR               0x8F
#define Direct_GETSTATUS            0x90
#define Direct_GETACCCR             0x91
#define Direct_GETMXDS              0x94
#define Direct_GETCAPS              0x95
#define Direct_SETXTIME             0x98
#define Direct_GETXTIME             0x99
#define Direct_RSTACT               0x9A
#define Direct_SETGRPA              0x9B
#define Direct_RSTGRPA              0x9C
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
#define DEVICE_ID1      0U
#define DEVICE_ID2      1U
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
