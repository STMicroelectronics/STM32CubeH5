/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Controller_Direct_Command_IT/Inc/main.h
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
#include "stm32h5xx.h"
#include "stm32h5xx_ll_i3c.h"
#include "stm32h5xx_ll_icache.h"
#include "stm32h5xx_ll_pwr.h"
#include "stm32h5xx_ll_crs.h"
#include "stm32h5xx_ll_rcc.h"
#include "stm32h5xx_ll_bus.h"
#include "stm32h5xx_ll_system.h"
#include "stm32h5xx_ll_exti.h"
#include "stm32h5xx_ll_cortex.h"
#include "stm32h5xx_ll_utils.h"
#include "stm32h5xx_ll_dma.h"
#include "stm32h5xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__GNUC__)
#include <stdio.h>      /*rtt*/
#include <stdlib.h>     /*rtt*/
#endif
#include "stdio.h"
#include "string.h"
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

/* Data Buffer descriptor */
typedef struct
{
  uint8_t   *pBuffer;  /*!< Pointer to the buffer containing all data values to transfer */
  uint32_t  Size;      /*!< The size of pBuffer in bytes                                 */

} I3C_DataTypeDef;

/* Common Command Code descriptor */
typedef struct
{
  uint8_t            TargetAddr; /*!< Dynamic or Static target Address                                      */
  uint8_t            CCC;        /*!< CCC value code                                                        */
  I3C_DataTypeDef    CCCBuf;     /*!< Contain size of associated data and size of defining byte if any.
                                      Contain pointer to CCC associated data                                */
  uint32_t           Direction;  /*!< CCC read and/or write direction message                               */

} I3C_CCCTypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/**
  * @brief Broadcast Command code
  */

#define Broadcast_ENEC          0x00
#define Broadcast_DISEC         0x01
#define Broadcast_ENTAS0        0x02
#define Broadcast_ENTAS1        0x03
#define Broadcast_ENTAS2        0x04
#define Broadcast_ENTAS3        0x05
#define Broadcast_RSTDAA        0x06
#define Broadcast_ENTDAA        0x07
#define Broadcast_DEFSLVS       0x08
#define Broadcast_SETMWL        0x09
#define Broadcast_SETMRL        0x0A
#define Broadcast_ENTTM         0x0B
#define Broadcast_SETXTIME      0x28
#define Broadcast_SETAASA       0x29
#define Broadcast_RSTACT        0x2A
#define Broadcast_DEFGRPA       0x2B
#define Broadcast_RSTGRPA       0x2C

/**
  * @brief Direct Command code
  */

#define Direct_ENEC             0x80
#define Direct_DISEC            0x81
#define Direct_ENTAS0           0x82
#define Direct_ENTAS1           0x83
#define Direct_ENTAS2           0x84
#define Direct_ENTAS3           0x85
#define Direct_SETDASA          0x87
#define Direct_SETNEWDA         0x88
#define Direct_SETMWL           0x89
#define Direct_SETMRL           0x8A
#define Direct_GETMWL           0x8B
#define Direct_GETMRL           0x8C
#define Direct_GETPID           0x8D
#define Direct_GETBCR           0x8E
#define Direct_GETDCR           0x8F
#define Direct_GETSTATUS        0x90
#define Direct_GETACCMST        0x91
#define Direct_GETMXDS          0x94
#define Direct_GETCAPS          0x95
#define Direct_SETXTIME         0x98
#define Direct_GETXTIME         0x99
#define Direct_RSTACT           0x9A
#define Direct_SETGRPA          0x9B
#define Direct_RSTGRPA          0x9C

/**
  * @brief Toggle periods for various blinking modes
  */

#define LED_BLINK_FAST          200
#define LED_BLINK_SLOW          500
#define LED_BLINK_ERROR         1000
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void UserButton_Callback(void);
void Target_Request_DynamicAddrCallback(uint64_t targetPayload);
void Controller_Complete_Callback(void);
void Controller_Reception_Callback(void);
void Controller_Transmit_Callback(void);
void Controller_Frame_Update_Callback(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define USER_BUTTON_EXTI_IRQn EXTI13_IRQn
#define LED2_Pin LL_GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif

/* USER CODE BEGIN Private defines */
/* Define Target Identifier */
#define DEVICE_ID1              0U
#define DEVICE_ID2              1U
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
