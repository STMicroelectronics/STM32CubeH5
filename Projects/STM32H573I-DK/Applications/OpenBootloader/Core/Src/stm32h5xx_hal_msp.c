/**
  ******************************************************************************
  * @file    stm32h5xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file provides code for the MSP Initialization
  *          and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "interfaces_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

/**
  * @brief This function configures FDCAN used hardware resources
  * @param hfdcan: FDCAN handle pointer
  * @retval None
  */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable all resources clocks ---------------------------------------------*/
  /* Enable used GPIOx clocks */
  FDCANx_GPIO_CLK_TX_ENABLE();
  FDCANx_GPIO_CLK_RX_ENABLE();

  /* Enable FDCANx clock */
  FDCANx_CLK_ENABLE();

  /* Configure peripheral GPIO */
  /* Configure FDCAN TX as alternate function */
  GPIO_InitStruct.Pin        = FDCANx_TX_PIN;
  GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate  = FDCANx_TX_AF;
  LL_GPIO_Init(FDCANx_TX_GPIO_PORT, &GPIO_InitStruct);

  /* Configure FDCAN RX as alternate function */
  GPIO_InitStruct.Pin = FDCANx_RX_PIN;
  LL_GPIO_Init(FDCANx_RX_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  DeInitializes the FDCAN MSP.
  * @param  hfdcan: pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @retval None
  */
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
  /* 1- Reset peripherals */
  FDCANx_FORCE_RESET();
  FDCANx_RELEASE_RESET();

  /* 2- Disable peripherals and GPIO Clocks */
  /* Configure FDCANx Tx as alternate function */
  HAL_GPIO_DeInit(FDCANx_TX_GPIO_PORT, FDCANx_TX_PIN);

  /* Configure FDCANx Rx as alternate function */
  HAL_GPIO_DeInit(FDCANx_RX_GPIO_PORT, FDCANx_RX_PIN);
}

/**
  * @brief PCD MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hpcd: PCD handle pointer
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  GPIO_InitTypeDef GPIO_InitStruct             = {0U};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0U};

  if (hpcd->Instance == USB_DRD_FS)
  {
    /** Initializes the peripherals clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_HSI48;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /** USB GPIO Configuration
        PA11     ------> USB_DM
        PA12     ------> USB_DP
      */
    GPIO_InitStruct.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_USB;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* USB_DRD_FS interrupt Init */
    HAL_NVIC_SetPriority(USB_DRD_FS_IRQn, 3U, 0U);
    HAL_NVIC_EnableIRQ(USB_DRD_FS_IRQn);

    HAL_PWREx_EnableVddUSB();
  }
}

/**
  * @brief PCD MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hpcd: PCD handle pointer
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  if (hpcd->Instance == USB_DRD_FS)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USB_CLK_DISABLE();

    /** USB GPIO Configuration
        PA11     ------> USB_DM
        PA12     ------> USB_DP
      */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

    /* USB_DRD_FS interrupt DeInit */
    HAL_NVIC_DisableIRQ(USB_DRD_FS_IRQn);
  }
}
