/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_usb_if.c
  * @author  MCD Application Team
  * @brief   This file contains the usb if functions.
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
#include "main.h"
#include "usbpd_usb_if.h"
#include "usbpd_trace.h"
#include "usbpd_dpm_user.h"
/* USER CODE BEGIN Includes */
#include "app_usbx_device.h"
#include "app_usbx_host.h"
/* USER CODE END Includes */

/* USB include files ----------------------------------------------------------*/

/* USER CODE BEGIN USB Include files */

/* USER CODE END USB Include file */

/* Private enums -------------------------------------------------------------*/
/* USER CODE BEGIN USB Private enums */

/* USER CODE END USB Private enums */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN USB Private typedef */

/* USER CODE END USB Private typedef */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN USB Private define */
extern UCHAR String_Descriptor[50] ;
/* USER CODE END USB Private define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN USB Private macro */

/* USER CODE END USB Private macro */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN USB Private function prototypes */

/* USER CODE END USB Private function prototypes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN USB Private variables */
extern TX_QUEUE ux_app_Host_MsgQueue_UCPD;
extern TX_QUEUE ux_app_Device_MsgQueue_UCPD;

#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_DRD_ModeMsg_TypeDef USB_DRD_Event __ALIGN_END;

/* USER CODE END USB Private variables */

/* Exported functions ---------------------------------------------------------*/
/**
* @brief  Initialize billboard driver
* @param  None
* @retval status
*/
int32_t USBPD_USBIF_Init(void)
{
  /* USER CODE BEGIN USBPD_USBIF_Init */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, 0, 0, (uint8_t *) "USB_IF init", 11);
  return 0;
  /* USER CODE END USBPD_USBIF_Init */
}

/** @addtogroup USBPD_CORE_USBIF_Exported_Functions
* @{
*/
void USBPD_USBIF_DeviceStart(uint32_t PortNum)
{
  /* USER CODE BEGIN USBPD_USBIF_DeviceStart */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF Device start", 18);

  /* Set Device state */
  USB_DRD_Event.DeviceState = START_USB_DEVICE;

  /* Send message to start device */
  if (tx_queue_send(&ux_app_Device_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END USBPD_USBIF_DeviceStart */
}

void USBPD_USBIF_DeviceStop(uint32_t PortNum)
{
  /*Init DPM GetSRC capa */
  for(int8_t index=0; index < 4; index++)
  {
    DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] =0 ;
  }

  /*Init DPM GetSNK capa */
  for(int8_t index=0; index < 4; index++)
  {
    DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] =0 ;
  }

  DPM_Ports[PortNum].DPM_RequestedCurrent =0  ;
  DPM_Ports[PortNum].DPM_RequestedVoltage =0  ;

  /* USER CODE BEGIN USBPD_USBIF_DeviceStop */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF Device stop", 17);

  /* Set Device state */
  USB_DRD_Event.DeviceState = STOP_USB_DEVICE;

  /* Send message to stop device */
  if (tx_queue_send(&ux_app_Device_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* USER CODE END USBPD_USBIF_DeviceStop */
}

void USBPD_USBIF_HostStart(uint32_t PortNum)
{
  /* USER CODE BEGIN USBPD_USBIF_HostStart */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF host start", 16);

  /* Set Host state */
  USB_DRD_Event.HostState = START_USB_HOST;

  /* Send message to start device */
  if (tx_queue_send(&ux_app_Host_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END USBPD_USBIF_HostStart */
}

void USBPD_USBIF_HostStop(uint32_t PortNum)
{
  for (int i=0 ; i< 49 ; i++)
  {
    String_Descriptor[i] = 0 ;
  }

  /*Init DPM GetSRC capa */
  for(int8_t index=0; index < 4; index++)
  {

    DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index] =0 ;
  }

  /*Init DPM GetSNK capa */
  for(int8_t index=0; index < 4; index++)
  {
    DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index] =0 ;
  }

  DPM_Ports[PortNum].DPM_RequestedCurrent =0  ;
  DPM_Ports[PortNum].DPM_RequestedVoltage =0  ;

  /* USER CODE BEGIN USBPD_USBIF_HostStop */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF host stop", 15);

  /* Set Host state */
  USB_DRD_Event.HostState = STOP_USB_HOST;

  /* Send message to start device */
  if (tx_queue_send(&ux_app_Host_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END USBPD_USBIF_HostStop */
}

void USBPD_USBIF_DeviceBillboard(uint32_t PortNum)
{
  /* USER CODE BEGIN USBPD_USBIF_DeviceBillboard */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF device billboard", 22);
  /* USER CODE END USBPD_USBIF_DeviceBillboard */
}

void USBPD_USBIF_Swap2Host(uint32_t PortNum)
{
  /* USER CODE BEGIN USBPD_USBIF_Swap2Host */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF swap to host", 18);

  /* Set Device state */
  USB_DRD_Event.DeviceState = STOP_USB_DEVICE;
  USB_DRD_Event.HostState = START_USB_HOST;

  /* Send message to stop device */
  if (tx_queue_send(&ux_app_Device_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* USER CODE END USBPD_USBIF_Swap2Host */
}

void USBPD_USBIF_Swap2Device(uint32_t PortNum)
{
  /* USER CODE BEGIN USBPD_USBIF_Swap2Device */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF swap to device", 20);
  /* USER CODE END USBPD_USBIF_Swap2Device */

  /* Set Host state */
  USB_DRD_Event.DeviceState = START_USB_DEVICE;
  USB_DRD_Event.HostState = STOP_USB_HOST;

  /* Send message to start device */
  if (tx_queue_send(&ux_app_Host_MsgQueue_UCPD, &USB_DRD_Event, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    Error_Handler();
  }

  /* USER CODE END USBPD_USBIF_DeviceStart */

  /* USER CODE END USBPD_USBIF_Swap2Device */
}

void USBPD_USBIF_DeviceSetBOSInfo(uint32_t PortNum, void *DataPtr)
{
  /* USER CODE BEGIN USBPD_USBIF_DeviceSetBOSInfo */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF set BOS info", 18);
  /* USER CODE END USBPD_USBIF_DeviceSetBOSInfo */
}

void USBPD_USBIF_DeviceSetVDMInfo(uint32_t PortNum, void *DataPtr)
{
  /* USER CODE BEGIN USBPD_USBIF_DeviceSetVDMInfo */
  USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *) "USBIF set VDM info", 18);
  /* USER CODE END USBPD_USBIF_DeviceSetVDMInfo */
}

/* USER CODE BEGIN Exported functions */

/* USER CODE END Exported functions */

/**
* @}
*/

/** @addtogroup USBPD_CORE_USBIF_Private_Functions
* @{
*/

/* USER CODE BEGIN Private functions */

/* USER CODE END Private functions */

/**
* @}
*/

