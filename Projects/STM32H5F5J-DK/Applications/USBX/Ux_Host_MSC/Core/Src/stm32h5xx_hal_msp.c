/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h5xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
  * @brief UART MSP Initialization
  * This function configures the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(huart->Instance==USART2)
  {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = USART2_TX_Pin|USART2_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspInit 1 */

    /* USER CODE END USART2_MspInit 1 */

  }

}

/**
  * @brief UART MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, USART2_TX_Pin|USART2_RX_Pin);

    /* USART2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspDeInit 1 */

    /* USER CODE END USART2_MspDeInit 1 */
  }

}

/**
  * @brief HCD MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hhcd: HCD handle pointer
  * @retval None
  */
void HAL_HCD_MspInit(HCD_HandleTypeDef* hhcd)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hhcd->Instance==USB_OTG_HS)
  {
    /* USER CODE BEGIN USB_OTG_HS_HCD_MspInit 0 */

    /* USER CODE END USB_OTG_HS_HCD_MspInit 0 */

    /** Initializes the peripherals clock
      */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OTGHS;
    PeriphClkInitStruct.OtghsClockSelection = RCC_OTGHSCLKSOURCE_HSE_DIV2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /** Enable USB Voltage detector
      */
    HAL_PWREx_EnableUSBVoltageDetector();

    /* Enable VDDUSB */
    HAL_PWREx_EnableVddUSB();

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    __HAL_RCC_OTGPHY_CLK_ENABLE();

    /* Configure desired clock settings for OTG_HS PHY */
    __HAL_RCC_OTGPHY_CONFIG(RCC_OTGPHYREFCKCLKSOURCE_24M);

    /* Adjust the disconnect threshold and the squelch threshold */
    HAL_SBS_SetOTGPHYDisconnectThreshold(SBS_OTG_HS_PHY_DISCONNECT_5_9PERCENT);
    HAL_SBS_SetOTGPHYSquelchThreshold(SBS_OTG_HS_PHY_SQUELCH_15PERCENT);

    /* Enable the OTG_HS PHY */
    HAL_PWREx_EnableUSBOTGHSPhy();

    /* USB_OTG_HS interrupt Init */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
    /* USER CODE BEGIN USB_OTG_HS_HCD_MspInit 1 */

    /* USER CODE END USB_OTG_HS_HCD_MspInit 1 */

  }

}

/**
  * @brief HCD MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hhcd: HCD handle pointer
  * @retval None
  */
void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hhcd)
{
  if(hhcd->Instance==USB_OTG_HS)
  {
    /* USER CODE BEGIN USB_OTG_HS_HCD_MspDeInit 0 */

    /* USER CODE END USB_OTG_HS_HCD_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
    __HAL_RCC_OTGPHY_CLK_DISABLE();

    /* Disable the OTG_HS PHY */
    HAL_PWREx_DisableUSBOTGHSPhy();

    /* Disable VDDUSB */
    HAL_PWREx_DisableVddUSB();

    /** Disable USB Voltage detector
      */
    HAL_PWREx_DisableUSBVoltageDetector();

    /* USB_OTG_HS interrupt DeInit */
    HAL_NVIC_DisableIRQ(OTG_HS_IRQn);

    /* USER CODE BEGIN USB_OTG_HS_HCD_MspDeInit 1 */

    /* USER CODE END USB_OTG_HS_HCD_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
