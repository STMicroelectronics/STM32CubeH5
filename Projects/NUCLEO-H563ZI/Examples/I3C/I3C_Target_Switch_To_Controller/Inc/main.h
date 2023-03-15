/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : I3C/I3C_Target_Switch_To_Controller/Inc/main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#include "stm32h5xx_util_i3c.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define DEVICE_ID 0x01

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
