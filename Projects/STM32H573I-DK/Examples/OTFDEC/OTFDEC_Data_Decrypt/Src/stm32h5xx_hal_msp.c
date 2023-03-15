/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h5xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
* @brief CRYP MSP Initialization
* This function configures the hardware resources used in this example
* @param hcryp: CRYP handle pointer
* @retval None
*/
void HAL_CRYP_MspInit(CRYP_HandleTypeDef* hcryp)
{
  if(hcryp->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspInit 0 */

  /* USER CODE END AES_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_AES_CLK_ENABLE();
  /* USER CODE BEGIN AES_MspInit 1 */

  /* USER CODE END AES_MspInit 1 */
  }

}

/**
* @brief CRYP MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcryp: CRYP handle pointer
* @retval None
*/
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef* hcryp)
{
  if(hcryp->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspDeInit 0 */

  /* USER CODE END AES_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_AES_CLK_DISABLE();
  /* USER CODE BEGIN AES_MspDeInit 1 */

  /* USER CODE END AES_MspDeInit 1 */
  }

}

/**
* @brief DCACHE MSP Initialization
* This function configures the hardware resources used in this example
* @param hdcache: DCACHE handle pointer
* @retval None
*/
void HAL_DCACHE_MspInit(DCACHE_HandleTypeDef* hdcache)
{
  if(hdcache->Instance==DCACHE1)
  {
  /* USER CODE BEGIN DCACHE1_MspInit 0 */

  /* USER CODE END DCACHE1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_DCACHE1_CLK_ENABLE();
  /* USER CODE BEGIN DCACHE1_MspInit 1 */

  /* USER CODE END DCACHE1_MspInit 1 */
  }

}

/**
* @brief DCACHE MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hdcache: DCACHE handle pointer
* @retval None
*/
void HAL_DCACHE_MspDeInit(DCACHE_HandleTypeDef* hdcache)
{
  if(hdcache->Instance==DCACHE1)
  {
  /* USER CODE BEGIN DCACHE1_MspDeInit 0 */

  /* USER CODE END DCACHE1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCACHE1_CLK_DISABLE();
  /* USER CODE BEGIN DCACHE1_MspDeInit 1 */

  /* USER CODE END DCACHE1_MspDeInit 1 */
  }

}

/**
* @brief XSPI MSP Initialization
* This function configures the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspInit(XSPI_HandleTypeDef* hxspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hxspi->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

  /* USER CODE END OCTOSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL1Q;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PG9     ------> OCTOSPI1_IO6
    PF10     ------> OCTOSPI1_CLK
    PG6     ------> OCTOSPI1_NCS
    PC2     ------> OCTOSPI1_IO2
    PD12     ------> OCTOSPI1_IO1
    PC0     ------> OCTOSPI1_IO7
    PD13     ------> OCTOSPI1_IO3
    PB1     ------> OCTOSPI1_IO0
    PH3     ------> OCTOSPI1_IO5
    PB2     ------> OCTOSPI1_DQS
    PH2     ------> OCTOSPI1_IO4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPI1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPI1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPI1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPI1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_OCTOSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPI1;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* OCTOSPI1 interrupt Init */
    HAL_NVIC_SetPriority(OCTOSPI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OCTOSPI1_IRQn);
  /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

  /* USER CODE END OCTOSPI1_MspInit 1 */
  }

}

/**
* @brief XSPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hxspi: XSPI handle pointer
* @retval None
*/
void HAL_XSPI_MspDeInit(XSPI_HandleTypeDef* hxspi)
{
  if(hxspi->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

  /* USER CODE END OCTOSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PG9     ------> OCTOSPI1_IO6
    PF10     ------> OCTOSPI1_CLK
    PG6     ------> OCTOSPI1_NCS
    PC2     ------> OCTOSPI1_IO2
    PD12     ------> OCTOSPI1_IO1
    PC0     ------> OCTOSPI1_IO7
    PD13     ------> OCTOSPI1_IO3
    PB1     ------> OCTOSPI1_IO0
    PH3     ------> OCTOSPI1_IO5
    PB2     ------> OCTOSPI1_DQS
    PH2     ------> OCTOSPI1_IO4
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2|GPIO_PIN_0);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12|GPIO_PIN_13);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1|GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_3|GPIO_PIN_2);

    /* OCTOSPI1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(OCTOSPI1_IRQn);
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

  /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }

}

/**
* @brief OTFDEC MSP Initialization
* This function configures the hardware resources used in this example
* @param hotfdec: OTFDEC handle pointer
* @retval None
*/
void HAL_OTFDEC_MspInit(OTFDEC_HandleTypeDef* hotfdec)
{
  if(hotfdec->Instance==OTFDEC1)
  {
  /* USER CODE BEGIN OTFDEC1_MspInit 0 */

  /* USER CODE END OTFDEC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_OTFDEC1_CLK_ENABLE();
    /* OTFDEC1 interrupt Init */
    HAL_NVIC_SetPriority(OTFDEC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTFDEC1_IRQn);
  /* USER CODE BEGIN OTFDEC1_MspInit 1 */

  /* USER CODE END OTFDEC1_MspInit 1 */
  }

}

/**
* @brief OTFDEC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hotfdec: OTFDEC handle pointer
* @retval None
*/
void HAL_OTFDEC_MspDeInit(OTFDEC_HandleTypeDef* hotfdec)
{
  if(hotfdec->Instance==OTFDEC1)
  {
  /* USER CODE BEGIN OTFDEC1_MspDeInit 0 */

  /* USER CODE END OTFDEC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OTFDEC1_CLK_DISABLE();

    /* OTFDEC1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(OTFDEC1_IRQn);
  /* USER CODE BEGIN OTFDEC1_MspDeInit 1 */

  /* USER CODE END OTFDEC1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
