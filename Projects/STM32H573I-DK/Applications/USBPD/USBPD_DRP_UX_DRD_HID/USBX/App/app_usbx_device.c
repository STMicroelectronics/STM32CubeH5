/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file    app_usbx_device.c
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
#include "app_usbx_device.h"
#include "app_usbx_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_device_stack.h"
#include "ux_dcd_stm32.h"
#include "ux_device_descriptors.h"
#include "ux_device_mouse.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Define constants.  */

/* USB stack size */
#define USBX_APP_STACK_SIZE       2048

/* USB memory size */
#define USBX_MEMORY_SIZE          (32 * 1024)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* ux_device_app_thread struct */
TX_THREAD ux_device_app_thread;

/* ux app msg queue */
TX_QUEUE ux_app_Device_MsgQueue_UCPD;
extern TX_QUEUE ux_app_Host_MsgQueue_UCPD;
/* ux_hid_thread struct */
TX_THREAD ux_hid_thread;

/* EventFlag group struct */
TX_EVENT_FLAGS_GROUP EventFlag;

/* HID Class Calling Parameter structure */
UX_SLAVE_CLASS_HID_PARAMETER hid_parameter;

extern PCD_HandleTypeDef hpcd_USB_DRD_FS;
extern USB_DRD_ModeMsg_TypeDef USB_DRD_State_Msg;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void  usbx_device_app_thread_entry(ULONG arg);
/* USER CODE END PFP */
/**
  * @brief  Application USBX Device Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_USBX_Device_MEM_POOL */
  CHAR    *pointer;
  /* Device framework FS length*/
  ULONG device_framework_fs_length;
  ULONG device_framework_hs_length;
  /* Device String framework length*/
  ULONG string_framework_length;
  /* Device language id framework length*/
  ULONG languge_id_framework_length;
  /* Device_Framework_Full_Speed */
  UCHAR *device_framework_full_speed;
  UCHAR *device_framework_high_speed;
  /* String Framework*/
  UCHAR *string_framework;
  /* Language_Id_Framework*/
  UCHAR *language_id_framework;
  /* Status Tx */
  UINT tx_status = UX_SUCCESS;

  /* Get_Device_Framework_Full_Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED,
                                                                &device_framework_hs_length);
  /* Get_String_Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get_Language_Id_Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&languge_id_framework_length);

  /* The code below is required for installing the device portion of USBX.
  In this application */
  ret =  ux_device_stack_initialize(device_framework_high_speed,
                                    device_framework_hs_length,
                                    device_framework_full_speed,
                                    device_framework_fs_length,
                                    string_framework,
                                    string_framework_length,
                                    language_id_framework,
                                    languge_id_framework_length, UX_NULL);

  /* Check the Stack initialize status */
  if (ret != UX_SUCCESS)
  {
    Error_Handler();
    return ret;
  }

  /* Initialize the hid class parameters for the device. */
  hid_parameter.ux_device_class_hid_parameter_report_address = USBD_Get_Device_HID_MOUSE_ReportDesc();

  hid_parameter.ux_device_class_hid_parameter_report_length = USBD_HID_MOUSE_REPORT_DESC_SIZE;

  hid_parameter.ux_device_class_hid_parameter_report_id = UX_FALSE;

  /* Initialize the device hid class. The class is connected with interface 1
  due to first instance in UserClassInstance is HID*/

  ret = _ux_device_stack_class_register(_ux_system_slave_class_hid_name,
                                        ux_device_class_hid_entry, 1, 0,
                                        (VOID *)&hid_parameter);


  /* Check the device stack class status */
  if (ret != UX_SUCCESS)
  {
    Error_Handler();
  }


  /* Allocate the stack for main_usbx_app_thread_entry. */
  tx_status = tx_byte_allocate(byte_pool, (VOID **) &pointer,
                               USBX_APP_STACK_SIZE*1.5, TX_NO_WAIT);

  /* Check memory allocation */
  if (UX_SUCCESS != tx_status)
  {
    Error_Handler();
    return tx_status;
  }

  /* Create the usbx_device_app_thread_entry main thread. */
  tx_status = tx_thread_create(&ux_device_app_thread, "main_usbx_app_thread_entry",
                               usbx_device_app_thread_entry, 0, pointer, USBX_APP_STACK_SIZE*1.5,
                               25, 25, TX_NO_TIME_SLICE, TX_AUTO_START);

  /* Check usbx_device_app_thread_entry creation */
  if (UX_SUCCESS != tx_status)
  {
    Error_Handler();
    return tx_status;
  }


  /* Create the event flags group. */
  if (tx_event_flags_create(&EventFlag, "Event Flag") != TX_SUCCESS)
  {
    ret = TX_GROUP_ERROR;
  }

  /* Allocate the stack for hid_usbx_app_thread_entry. */
  tx_status = tx_byte_allocate(byte_pool, (VOID **) &pointer,
                               USBX_APP_STACK_SIZE, TX_NO_WAIT);

  /* Check memory allocation */
  if (UX_SUCCESS != tx_status)
  {
    Error_Handler();
    return tx_status;
  }

  /* Create the usbx_hid_thread_entry thread. */
  tx_status = tx_thread_create(&ux_hid_thread, "hid_usbx_device_app_thread_entry",
                               usbx_hid_thread_entry, 1,
                               pointer, USBX_APP_STACK_SIZE, 25, 25,
                               TX_NO_TIME_SLICE, TX_AUTO_START);

  /* Check usbx_hid_thread_entry creation */
  if (UX_SUCCESS != tx_status)
  {
    Error_Handler();
    return tx_status;
  }

  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       sizeof(APP_QUEUE_SIZE*sizeof(ULONG)),
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    ret = TX_POOL_ERROR;
  }

  /* Create the MsgQueue */
  if (tx_queue_create(&ux_app_Device_MsgQueue_UCPD, "Message Queue Device",
                      TX_1_ULONG, pointer,
                      APP_QUEUE_SIZE*sizeof(ULONG)) != TX_SUCCESS)
  {
    ret = TX_QUEUE_ERROR;
  }

  /* USER CODE END MX_USBX_Device_Init */

  return ret;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing usbx_device_app_thread_entry.
  * @param arg: Not used
  * @retval None
  */
void usbx_device_app_thread_entry(ULONG arg)
{
  /* Wait for message queue to start/stop the device */
  while(1)
  {
    /* Wait for a device to be connected */
    if (tx_queue_receive(&ux_app_Device_MsgQueue_UCPD, &USB_DRD_State_Msg,
                         TX_WAIT_FOREVER)!= TX_SUCCESS)
    {
      /*Error*/
      Error_Handler();
    }
    /* Check if received message equal to USB_PCD_START */
    if (USB_DRD_State_Msg.DeviceState == START_USB_DEVICE)
    {
      /* Initialization of USB device */
      MX_USB_Device_Init();

      /* Start device USB */
      HAL_PCD_Start(&hpcd_USB_DRD_FS);

      USBH_UsrLog("USB Device library started.\n");
    }
    /* Check if received message equal to USB_PCD_STOP */
    else if (USB_DRD_State_Msg.DeviceState == STOP_USB_DEVICE)
    {

      /* Stop device USB */
      HAL_PCD_Stop(&hpcd_USB_DRD_FS);

      HAL_PCD_DeInit(&hpcd_USB_DRD_FS);
      /* uninitialize the device controller driver*/
      _ux_dcd_stm32_uninitialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);

      USBH_UsrLog("USB Device library stopped.\n");

      if (USB_DRD_State_Msg.HostState == START_USB_HOST)
      {
        /* Send message to start device */
        if (tx_queue_send(&ux_app_Host_MsgQueue_UCPD, &USB_DRD_State_Msg, TX_WAIT_FOREVER) != TX_SUCCESS)
        {
          Error_Handler();
        }
      }

    }
    /* Else Error */
    else
    {
      /*Error*/
      Error_Handler();
    }
  }
}

/**
  * @brief MX_USB_Device_Init
  *        Initialization of USB device.
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_Device_Init(void)
{
  /* USER CODE BEGIN USB_Device_Init_PreTreatment_0 */
  /* USER CODE END USB_Device_Init_PreTreatment_0 */

  /* USB_OTG_HS init function */
  MX_USB_DRD_FS_PCD_Init();

  /* USER CODE BEGIN USB_Device_Init_PreTreatment_1 */

  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00, PCD_SNG_BUF, 0x0C);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80, PCD_SNG_BUF, 0x4C);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x81, PCD_SNG_BUF, 0x8C);

  /* USER CODE END USB_Device_Init_PreTreatment_1 */

  /* initialize the device controller driver*/
  _ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);

  /* USER CODE BEGIN USB_Device_Init_PostTreatment */
  /* USER CODE END USB_Device_Init_PostTreatment */
}

/* USER CODE END 1 */
