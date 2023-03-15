/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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
#include "app_usbx_device.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBX_APP_STACK_SIZE     1024U
#define USBX_MEMORY_SIZE        (4U * 1024U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
UX_SYSTEM_SLAVE                 *ux_system;
TX_QUEUE                        ux_app_MsgQueue;
TX_QUEUE                        ux_usbpd_app_MsgQueue;
TX_THREAD                       ux_app_thread;
TX_THREAD                       usbx_dfu_download_thread;
UX_SLAVE_CLASS_DFU_PARAMETER    dfu_parameter;
extern PCD_HandleTypeDef        hpcd_USB_DRD_FS;

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN ux_dfu_downloadInfotypeDef  ux_dfu_download  __ALIGN_END;

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_MODE_STATE                  USB_Device_State_Msg  __ALIGN_END;

/* Private function prototypes -----------------------------------------------*/
void  usbx_app_thread_entry(ULONG arg);

/**
  * @brief  Application USBX Device Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

#if (USE_STATIC_ALLOCATION == 1U)

  UCHAR *pointer;
  ULONG device_framework_fs_length;
  ULONG string_framework_length;
  ULONG languge_id_framework_length;
  UCHAR *device_framework_full_speed;
  UCHAR *string_framework;
  UCHAR *language_id_framework;

  /* Allocate USBX_MEMORY_SIZE. */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       USBX_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Initialize USBX Memory */
  if (ux_system_initialize(pointer, USBX_MEMORY_SIZE, UX_NULL, 0U) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* Get_Device_Framework_Full_Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  /* Get_String_Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get_Language_Id_Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&languge_id_framework_length);

  /* The code below is required for installing the device portion of USBX.
     In this application */
  if (ux_device_stack_initialize(NULL,
                                 0U,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 languge_id_framework_length,
                                 DFU_Device_ConnectionCallback) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* Initialize the DFU parameters for the device. */
  dfu_parameter.ux_slave_class_dfu_parameter_instance_activate = DFU_Init;

  /* DeInitialize the DFU parameters for the device. */
  dfu_parameter.ux_slave_class_dfu_parameter_instance_deactivate = DFU_DeInit;

  /* Set the callback function "DFU_GetStatus" to get status parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_get_status = DFU_GetStatus;

  /* Set the callback function "DFU_Read" to read parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_read = DFU_Read;

  /* Set the callback function "DFU_Write" to write parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_write = DFU_Write;

  /* Set the callback function "DFU_Notify" to notify parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_notify = DFU_Notify;

  /* Manage the leave DFU requests */
  dfu_parameter.ux_device_class_dfu_parameter_custom_request = DFU_Leave;

  /* Set device framework parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_framework = device_framework_full_speed;

  /* Set device framework length parameter */
  dfu_parameter.ux_slave_class_dfu_parameter_framework_length = device_framework_fs_length;


  /* Initialize the device dfu class. The class is connected with interface 0 */
  if (ux_device_stack_class_register(_ux_system_slave_class_dfu_name,
                                     ux_device_class_dfu_entry, 1U, 0U,
                                     (VOID *)&dfu_parameter) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* Allocate the stack for main_usbx_app_thread_entry. */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       USBX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the usbx_app_thread_entry.  */
  if (tx_thread_create(&ux_app_thread, "main_usbx_app_thread_entry",
                       usbx_app_thread_entry, 0U, pointer, USBX_APP_STACK_SIZE,
                       10U, 10U, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Allocate the stack for usbx_dfu_download_thread_entry. */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       USBX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the usbx_dfu_download_thread_entry thread. */
  if (tx_thread_create(&usbx_dfu_download_thread, "usbx_dfu_download_thread_entry",
                       usbx_dfu_download_thread_entry, 0U, pointer,
                       USBX_APP_STACK_SIZE, 20U, 20U, TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the MsgQueue */
  if (tx_queue_create(&ux_app_MsgQueue, "Message Queue app", TX_1_ULONG,
                      pointer, APP_QUEUE_SIZE * sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }

  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, sizeof(APP_QUEUE_SIZE * sizeof(ULONG)),
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the MsgQueue */
  if (tx_queue_create(&ux_usbpd_app_MsgQueue, "Usbpd Message Queue app",
                      TX_1_ULONG, pointer, APP_QUEUE_SIZE * sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }

#endif

  return ret;
}

/**
  * @brief  Function implementing usbx_app_thread_entry.
  * @param arg: Not used
  * @retval None
  */
void usbx_app_thread_entry(ULONG arg)
{
  /* Initialization of USB device */
  MX_USB_Device_Init();

  HAL_PCD_Start(&hpcd_USB_DRD_FS);
}

/**
  * @brief MX_USB_Device_Init
  *        Initialization of USB device.
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_Device_Init(void)
{
  /* USB_Device_Init_PreTreatment */
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x00U, PCD_SNG_BUF, 0x0CU);
  HAL_PCDEx_PMAConfig(&hpcd_USB_DRD_FS, 0x80U, PCD_SNG_BUF, 0x4CU);

  /* Initialize the device controller driver*/
  _ux_dcd_stm32_initialize((ULONG)USB_DRD_FS, (ULONG)&hpcd_USB_DRD_FS);

}
