/**
  ******************************************************************************
  * @file    app_usbx_device.h
  * @author  MCD Application Team
  * @brief   USBX Device applicative header file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_USBX_DEVICE_H__
#define __APP_USBX_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_device_stack.h"
#include "ux_dcd_stm32.h"
#include "ux_device_descriptors.h"
#include "ux_device_dfu_media.h"
#include "app_azure_rtos_config.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
UINT MX_USBX_Device_Init(VOID *memory_ptr);

void MX_USB_Device_Init(void);

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  STOP_USB_DEVICE = 1,
  START_USB_DEVICE,
} USB_MODE_STATE;

/* Private defines -----------------------------------------------------------*/
#define APP_QUEUE_SIZE          1U

#ifdef __cplusplus
}
#endif
#endif /* __APP_USBX_DEVICE_H__ */
