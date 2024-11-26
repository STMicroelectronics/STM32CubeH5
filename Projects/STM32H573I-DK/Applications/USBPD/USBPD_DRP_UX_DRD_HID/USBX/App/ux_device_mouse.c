/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_mouse.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "ux_device_mouse.h"
#include "stm32h573i_discovery.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "tx_api.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CURSOR_STEP     4
uint32_t Old_TsState_Mouse ;
extern __IO uint32_t TsState_Mouse ;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void GetPointerData(UX_SLAVE_CLASS_HID_EVENT *hid_event);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
UINT app_usbx_device_thread_hid_callback(UX_SLAVE_CLASS_HID *hid,
                                         UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  return 0U;
}
/**
  * @brief  Function implementing usbx_hid_thread_entry.
  * @param arg: Not used
  * @retval None
  */
UX_SLAVE_DEVICE    *device_test;

UX_SLAVE_CLASS_HID_EVENT hid_event_test;
void usbx_hid_thread_entry(ULONG arg)
{
  UX_SLAVE_DEVICE    *device;
  UX_SLAVE_INTERFACE *interface;
  UX_SLAVE_CLASS_HID *hid;
  UX_SLAVE_CLASS_HID_EVENT hid_event;

  device = &_ux_system_slave->ux_system_slave_device;
  ux_utility_memory_set(&hid_event, 0, sizeof(UX_SLAVE_CLASS_HID_EVENT));

  while (1)
  {
    device_test=device;
    /* Check if the device state already configured */
    if (device->ux_slave_device_state == UX_DEVICE_CONFIGURED)
    {
      /* Get the interface */
      interface = device->ux_slave_device_first_interface;
      /* Get the instance */
      hid = interface->ux_slave_interface_class_instance;

      /* sleep for 10ms */
      tx_thread_sleep(0.01 * TX_TIMER_TICKS_PER_SECOND);

      if (TsState_Mouse != 0)
      {
        GetPointerData(&hid_event);
        ux_device_class_hid_event_set(hid, &hid_event);
        hid_event_test=hid_event;
      }

    }
    else
    {
      /* sleep for 10ms */
      tx_thread_sleep(0.01 * TX_TIMER_TICKS_PER_SECOND);
    }
  }
}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
static void GetPointerData(UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  if (TsState_Mouse == 0x01) /* Mouse_UP */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 0;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = 0;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = (UCHAR) -CURSOR_STEP;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = 0;
  }

  if (TsState_Mouse == 0x02) /* Mouse_LEFT */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 0;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = (UCHAR) -CURSOR_STEP;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = 0;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = 0;
  }

  if (TsState_Mouse == 0x03) /* Mouse_RIGHT */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 0;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = CURSOR_STEP;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = 0;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = 0;
  }

  if (TsState_Mouse == 0x04) /* Mouse_DOWN */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 0;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = 0;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = CURSOR_STEP;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = 0;
  }

  if (TsState_Mouse == 0x05) /* Mouse_SELECT */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 1;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = 0;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = 0;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = 0;
  }

  if ((TsState_Mouse == 0x06) && (Old_TsState_Mouse != TsState_Mouse)) /* Mouse_Released */
  {
    /* Mouse event. Length is fixed to . */
    hid_event->ux_device_class_hid_event_length = 4;
    /* Set X position. */
    hid_event->ux_device_class_hid_event_buffer[0] = 0;
    /* Set Y position. */
    hid_event->ux_device_class_hid_event_buffer[1] = 0;
    /* Set wheel position. */
    hid_event->ux_device_class_hid_event_buffer[2] = 0;
    /* Set Left Button value. */
    hid_event->ux_device_class_hid_event_buffer[3] = 0;
    Old_TsState_Mouse = TsState_Mouse;
    TsState_Mouse = 0;
  }
}

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
