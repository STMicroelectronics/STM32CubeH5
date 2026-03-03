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
#include "stm32h5xx_nucleo.h"
#include "memory_toolbox.h"

#if defined(EXAMPLE_TRACE_ENABLE)
#include <stdio.h>
#endif /* EXAMPLE_TRACE_ENABLE */
#include <string.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* Size of buffers */
#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)

/* Trace system defines */
#if defined(EXAMPLE_TRACE_ENABLE)

/* Following defines allows to select how debug traces are output to user */
/* Please note that only one of the two below defines should be enabled */
#define EXAMPLE_TRACE_OUTPUT_UART
/* #define EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW */

/* Check that exactly one output type is defined */
#if defined(EXAMPLE_TRACE_OUTPUT_UART) && defined(EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW)
#error "Only one of EXAMPLE_TRACE_OUTPUT_UART or EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW must be defined!"
#elif !defined(EXAMPLE_TRACE_OUTPUT_UART) && !defined(EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW)
#error "You must define exactly one of EXAMPLE_TRACE_OUTPUT_UART or EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW!"
#endif /* EXAMPLE_TRACE_OUTPUT_UART && EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW */

#endif /* EXAMPLE_TRACE_ENABLE */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#if defined(EXAMPLE_TRACE_ENABLE)
#define EXECUTE_AND_CHECK(func_call)                                                                                   \
  do                                                                                                                   \
  {                                                                                                                    \
    char function_str[100];                                                                                            \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO,                                                                 \
                         (uint8_t *) "\n\r==========================================================================");\
    sprintf(function_str, "\n\r%s\n\r", #func_call);                                                                   \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) function_str);                                      \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO,                                                                 \
                         (uint8_t *) "==========================================================================\n\r");\
    exec_status = (func_call);                                                                                         \
    Memory_Toolbox_LogStatus(exec_status);                                                                             \
    if (exec_status != MEMORY_OK)                                                                                      \
    {                                                                                                                  \
      Error_Handler();                                                                                                 \
    }                                                                                                                  \
  } while (0);
#else
#define EXECUTE_AND_CHECK(func_call)    \
  do                                    \
  {                                     \
    exec_status = (func_call);          \
    if (exec_status != MEMORY_OK)       \
    {                                   \
      Error_Handler();                  \
    }                                   \
  } while (0);
#endif /* EXAMPLE_TRACE_ENABLE */

#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LDO_M2_Slot_Pin GPIO_PIN_5
#define LDO_M2_Slot_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
