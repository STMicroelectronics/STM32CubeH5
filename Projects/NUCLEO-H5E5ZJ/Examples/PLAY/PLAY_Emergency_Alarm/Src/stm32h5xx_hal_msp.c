/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h5xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief PLAY MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hplay: PLAY handle pointer
  * @retval None
  */
void HAL_PLAY_MspInit(HAL_PLAY_HandleTypeDef* hplay)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hplay->instance==PLAY1)
  {
    /* USER CODE BEGIN PLAY1_MspInit 0 */

    /* USER CODE END PLAY1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PLAY1;
    PeriphClkInitStruct.PLAY1ClockSelection = RCC_PLAY1CLKSOURCE_LSI;
    PeriphClkInitStruct.PLAY1ClockDivider = RCC_PLAY1_CLK_DIV_4096;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_PLAY1_CLK_ENABLE();
    __HAL_RCC_PLAY1APB_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**PLAY1 GPIO Configuration
    PF3     ------> PLAY1_OUT0
    PE9     ------> PLAY1_IN12
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_PLAY1_OUT;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_PLAY1_IN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* PLAY1 interrupt Init */
    HAL_NVIC_SetPriority(PLAY1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(PLAY1_IRQn);
    /* USER CODE BEGIN PLAY1_MspInit 1 */

    /* USER CODE END PLAY1_MspInit 1 */

  }

}

/**
  * @brief PLAY MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hplay: PLAY handle pointer
  * @retval None
  */
void HAL_PLAY_MspDeInit(HAL_PLAY_HandleTypeDef* hplay)
{
  if(hplay->instance==PLAY1)
  {
    /* USER CODE BEGIN PLAY1_MspDeInit 0 */

    /* USER CODE END PLAY1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_PLAY1_CLK_DISABLE();
    __HAL_RCC_PLAY1APB_CLK_DISABLE();

    /**PLAY1 GPIO Configuration
    PF3     ------> PLAY1_OUT0
    PE9     ------> PLAY1_IN12
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_9);

    /* PLAY1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(PLAY1_IRQn);
    /* USER CODE BEGIN PLAY1_MspDeInit 1 */

    /* USER CODE END PLAY1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
