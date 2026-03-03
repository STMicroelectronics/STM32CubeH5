/**
  ******************************************************************************
  * @file    usb_interface.c
  * @author  MCD Application Team
  * @brief   Contains USB protocol commands
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
#include "app_usbx_device.h"
#include "app_azure_rtos.h"

#include "main.h"

#include "openbl_core.h"
#include "usb_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t UsbDetected = 0U;

/* Exported variables --------------------------------------------------------*/
uint8_t UsbSofDetected = 0U;

PCD_HandleTypeDef hpcd;

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to configure USB pins and then initialize the used USB instance.
  * @retval None.
  */
void OPENBL_USB_Configuration(void)
{
  hpcd.Instance                     = USB_OTG_HS;
  hpcd.Init.dev_endpoints           = 9U;
  hpcd.Init.speed                   = USB_OTG_SPEED_HIGH;
  hpcd.Init.phy_itface              = USB_OTG_HS_EMBEDDED_PHY;
  hpcd.Init.use_dedicated_ep1       = DISABLE;
  hpcd.Init.dma_enable              = DISABLE;
  hpcd.Init.Sof_enable              = DISABLE;
  hpcd.Init.low_power_enable        = DISABLE;
  hpcd.Init.lpm_enable              = DISABLE;
  hpcd.Init.battery_charging_enable = DISABLE;
  hpcd.Init.vbus_sensing_enable     = DISABLE;

  if (HAL_PCD_Init(&hpcd) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is used to detect if there is any activity on USB protocol.
  * @retval Returns 1 if interface is detected else 0.
  */
uint8_t OPENBL_USB_ProtocolDetection(void)
{
  if (UsbSofDetected == 1U)
  {
    UsbDetected = 0U;

    /* Disable the other interfaces */
    OPENBL_InterfacesDeInit();

    /* The value of the variable "detect" will always be 0 and this is due to the fact that if this function returns 1,
       the USB interface will be disabled.
       For more details check the comment in the function "OpenBootloader_DetectInterfaceThread"
       in file "openbootloader_threadx.c" */
  }
  else
  {
    UsbDetected = 0U;
  }

  return UsbDetected;
}

/**
  * @brief  This function is used to De-initialize the I2C pins and instance.
  * @retval None.
  */
void OPENBL_USB_DeInit(void)
{
  /* Only de-initialize the USB if it is not the current detected interface */
  if (UsbDetected == 0U)
  {
   __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
   __HAL_RCC_USB_OTG_HS_FORCE_RESET();
   __HAL_RCC_USB_OTG_HS_RELEASE_RESET();

    HAL_PWREx_DisableUSBVoltageDetector();
    HAL_PWREx_DisableVddUSB();
    HAL_PWREx_DisableUSBOTGHSPhy();

    HAL_NVIC_DisableIRQ(OTG_HS_IRQn);

    HAL_PCD_DeInit(&hpcd);
  }
}

/**
  * @brief  Gets the page of a given address
  * @param  address address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t OPENBL_USB_GetPage(uint32_t address)
{
  /* Initialize the page number with a wrong value */
  uint32_t page = 0xFFFFU;

  if (address < (FLASH_BASE + FLASH_SIZE))
  {
    page = (address - FLASH_BASE) / FLASH_SECTOR_SIZE;
  }

  return page;
}
