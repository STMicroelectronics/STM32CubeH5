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
  * Copyright (c) 2026 STMicroelectronics.
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
  * @brief PCD MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hpcd PCD handle pointer
  * @retval None
  */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  GPIO_InitTypeDef gpio_init_struct               = {0U};
  RCC_PeriphCLKInitTypeDef periph_clk_init_struct = {0U};

  if (hpcd->Instance == USB_OTG_HS)
  {
    /** Initializes the peripherals clock */
    periph_clk_init_struct.PeriphClockSelection = LL_AHB2_GRP1_PERIPH_OTGHS;
    periph_clk_init_struct.OtghsClockSelection  = RCC_OTGHSCLKSOURCE_HSE_DIV2;

    if (HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /** USB GPIO Configuration
        PA11     ------> USB_DM
        PA12     ------> USB_DP
      */
    gpio_init_struct.Pin       = GPIO_PIN_11 | GPIO_PIN_12;
    gpio_init_struct.Mode      = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull      = GPIO_NOPULL;
    gpio_init_struct.Speed     = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Alternate = GPIO_AF10_OTG_HS;

    HAL_GPIO_Init(GPIOA, &gpio_init_struct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();

    /* Enable VDD33USB supply voltage monitor */
    HAL_PWREx_EnableUSBVoltageDetector();

    /* Check if VDDUSB is ready */
    while (!(__HAL_PWR_GET_FLAG(PWR_FLAG_USB33RDY)))
    {
      HAL_Delay(100);
    }

    /* Enable VDDUSB supply */
    HAL_PWREx_EnableVddUSB();

    /* Enable high-speed PHY */
    HAL_PWREx_EnableUSBOTGHSPhy();

    /* USB_HS interrupt Init */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 3U, 0U);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
  }
}

/**
  * @brief PCD MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hpcd PCD handle pointer
  * @retval None
  */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  if (hpcd->Instance == USB_OTG_HS)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_HS_CLK_DISABLE();

    /** USB GPIO Configuration
        PA11     ------> USB_DM
        PA12     ------> USB_DP
      */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

    /* USB_HS interrupt disable */
    HAL_NVIC_DisableIRQ(OTG_HS_IRQn);
  }
}
