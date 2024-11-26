/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_dpm_user.c
  * @author  MCD Application Team
  * @brief   USBPD DPM user code
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

#define USBPD_DPM_USER_C
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbpd_core.h"
#include "usbpd_dpm_user.h"
#include "usbpd_pdo_defs.h"
#include "usbpd_dpm_core.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_vdm_user.h"
#include "usbpd_pwr_if.h"
#include "usbpd_pwr_user.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include "string.h"
#include "stdio.h"
#endif /* _TRACE */
/* USER CODE BEGIN Includes */
#include "usbpd_usb_if.h"
/* USER CODE END Includes */

/** @addtogroup STM32_USBPD_APPLICATION
* @{
*/

/** @addtogroup STM32_USBPD_APPLICATION_DPM_USER
* @{
*/

#define DPM_NO_SRC_PDO_FOUND              0xFFU                     /*!< No match found between Received SRC PDO and SNK capabilities    */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */
typedef struct
{
  uint32_t RequestedVoltageInmVunits;              /*!< Sink request operating voltage in mV units       */
  uint32_t MaxOperatingCurrentInmAunits;           /*!< Sink request Max operating current in mA units   */
  uint32_t OperatingCurrentInmAunits;              /*!< Sink request operating current in mA units       */
  uint32_t MaxOperatingPowerInmWunits;             /*!< Sink request Max operating power in mW units     */
  uint32_t OperatingPowerInmWunits;                /*!< Sink request operating power in mW units         */
} USBPD_DPM_SNKPowerRequestDetails_TypeDef;
/* USER CODE END Private_Typedef */

/* Private define ------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_DEFINES USBPD USER Private Defines
* @{
*/
/* USER CODE BEGIN Private_Define */
static USBPD_StatusTypeDef DPM_TurnOnPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role);
static USBPD_StatusTypeDef DPM_TurnOffPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role);
static void DPM_AssertRp(uint8_t PortNum);
static void DPM_AssertRd(uint8_t PortNum);
uint8_t AlertPower;
uint8_t AlertRole;
extern uint8_t VDM_Mode_On[USBPD_PORT_COUNT];
/* USER CODE END Private_Define */
/** @brief  Sink Request characteristics Structure definition
*
*/

/**
* @}
*/

/* Private macro -------------------------------------------------------------*/
/** @defgroup USBPD_USER_PRIVATE_MACROS USBPD USER Private Macros
* @{
*/
#if defined(_TRACE)
#define DPM_USER_DEBUG_TRACE_SIZE       50u
#define DPM_USER_DEBUG_TRACE(_PORT_, ...)  do {                                                                \
char _str[DPM_USER_DEBUG_TRACE_SIZE];                                                                    \
  uint8_t _size = snprintf(_str, DPM_USER_DEBUG_TRACE_SIZE, __VA_ARGS__);                                  \
    if (_size < DPM_USER_DEBUG_TRACE_SIZE)                                                                   \
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (uint8_t)(_PORT_), 0, (uint8_t*)_str, strlen(_str));                \
    else                                                                                                     \
      USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (uint8_t)(_PORT_), 0, (uint8_t*)_str, DPM_USER_DEBUG_TRACE_SIZE);   \
  } while(0)

#define DPM_USER_ERROR_TRACE(_PORT_, _STATUS_, ...)  do {                                                      \
    if (USBPD_OK != _STATUS_) {                                                                                \
      char _str[DPM_USER_DEBUG_TRACE_SIZE];                                                                  \
        uint8_t _size = snprintf(_str, DPM_USER_DEBUG_TRACE_SIZE, __VA_ARGS__);                                \
          if (_size < DPM_USER_DEBUG_TRACE_SIZE)                                                                 \
            USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (uint8_t)(_PORT_), 0, (uint8_t*)_str, strlen(_str));              \
          else                                                                                                   \
            USBPD_TRACE_Add(USBPD_TRACE_DEBUG, (uint8_t)(_PORT_), 0, (uint8_t*)_str, DPM_USER_DEBUG_TRACE_SIZE); \
    }                                                                                                          \
  } while(0)
#else
#define DPM_USER_DEBUG_TRACE(_PORT_, ...)
#define DPM_USER_ERROR_TRACE(_PORT_, _STATUS_, ...)
#endif /* _TRACE */
    /* USER CODE BEGIN Private_Macro */

    /* USER CODE END Private_Macro */
    /**
    * @}
    */

    /* Private variables ---------------------------------------------------------*/
    /** @defgroup USBPD_USER_PRIVATE_VARIABLES USBPD USER Private Variables
    * @{
    */
#if defined(_GUI_INTERFACE)
GUI_NOTIFICATION_POST         DPM_GUI_PostNotificationMessage   = NULL;
GUI_NOTIFICATION_FORMAT_SEND  DPM_GUI_FormatAndSendNotification = NULL;
GUI_SAVE_INFO                 DPM_GUI_SaveInfo                  = NULL;
#endif /* GUI_INTERFACE */

    /* USER CODE BEGIN Private_Variables */
    /* USER CODE END Private_Variables */
    /**
    * @}
    */

    /* Private function prototypes -----------------------------------------------*/
    /** @defgroup USBPD_USER_PRIVATE_FUNCTIONS USBPD USER Private Functions
    * @{
    */
    /* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */

    /* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */
    /**
    * @}
    */

    /* Exported functions ------- ------------------------------------------------*/
    /** @defgroup USBPD_USER_EXPORTED_FUNCTIONS USBPD USER Exported Functions
    * @{
    */
    /* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS */

    /* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS */

    /** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 USBPD USER Exported Functions called by DPM CORE
    * @{
    */
    /* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

    /* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS_GROUP1 */

    /**
    * @brief  Initialize DPM (port power role, PWR_IF, CAD and PE Init procedures)
    * @retval USBPD Status
    */
    USBPD_StatusTypeDef USBPD_DPM_UserInit(void)
    {
      /* USER CODE BEGIN USBPD_DPM_UserInit */
      //  return USBPD_OK;
      /* PWR SET UP */
      if(USBPD_OK !=  USBPD_PWR_IF_Init())
      {
        return USBPD_ERROR;
      }

      return USBPD_OK;
      /* USER CODE END USBPD_DPM_UserInit */
    }

#if defined(_GUI_INTERFACE)
/**
  * @brief  Function to set the function ptr linked to GUI interface
  * @param  PtrFormatSend Pointer on function to format and send GUI notifications
  * @param  PtrPost       Pointer on function to send GUI notifications
  * @param  PtrSaveInfo   Pointer on function to save information from Port Partner
  * @retval None
  */
void USBPD_DPM_SetNotification_GUI(GUI_NOTIFICATION_FORMAT_SEND PtrFormatSend, GUI_NOTIFICATION_POST PtrPost, GUI_SAVE_INFO PtrSaveInfo)
{
  DPM_GUI_PostNotificationMessage   = PtrPost;
  DPM_GUI_FormatAndSendNotification = PtrFormatSend;
  DPM_GUI_SaveInfo                  = PtrSaveInfo;
}
#endif /* GUI_INTERFACE */

/**
* @brief  User delay implementation which is OS dependent
* @param  Time time in ms
* @retval None
*/
void USBPD_DPM_WaitForTime(uint32_t Time)
{
  HAL_Delay(Time);
}

/**
* @brief  User processing time, it is recommended to avoid blocking task for long time
* @param  argument  DPM User event
* @retval None
*/
void USBPD_DPM_UserExecute(void const *argument)
{
  /* USER CODE BEGIN USBPD_DPM_UserExecute */

  /* USER CODE END USBPD_DPM_UserExecute */
#if defined(_RTOS) || defined(USBPD_THREADX)
#else
#if defined(_GUI_INTERFACE)
  GUI_Execute();
#endif /* _GUI_INTERFACE */
#endif /* _RTOS || USBPD_THREADX */
}

/**
* @brief  UserCableDetection reporting events on a specified port from CAD layer.
* @param  PortNum The handle of the port
* @param  State CAD state
* @retval None
*/
void USBPD_DPM_UserCableDetection(uint8_t PortNum, USBPD_CAD_EVENT State)
{
  /* USER CODE BEGIN USBPD_DPM_UserCableDetection */
  switch(State)
  {
  case USBPD_CAD_EVENT_ATTEMC:
#if defined(_VCONN_SUPPORT)
    /* Save flag to keep information that cable is a EMC. */
    DPM_Ports[PortNum].DPM_CablePDCapable = USBPD_TRUE;
#endif /* _VCONN_SUPPORT */
  case USBPD_CAD_EVENT_ATTACHED:
    {
      if (USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
      {
        if (USBPD_OK != USBPD_PWR_IF_VBUSEnable(PortNum))
        {
          /* Should not occur */
          NVIC_SystemReset();
        }
      }
      DPM_Ports[PortNum].DPM_IsConnected = 1;
      break;
    }

  case USBPD_CAD_EVENT_DETACHED :
  case USBPD_CAD_EVENT_EMC :
  default :
    {

      /* reset all values received from port partner */
      memset(&DPM_Ports[PortNum], 0, sizeof(DPM_Ports[PortNum]));

      if(USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole)
      {
        if (USBPD_OK != USBPD_PWR_IF_VBUSDisable(PortNum))
        {
          /* Should not occur */
          while(1);
        }
      }

#if defined(_VCONN_SUPPORT)
      if(USBPD_TRUE == DPM_Params[PortNum].VconnStatus)
      {
        /* Switch Off Vconn */
        USBPD_DPM_PE_VconnPwr(PortNum, USBPD_DISABLE);
      }
#endif /* _VCONN_SUPPORT */
      break;
    }
  }
  /* USER CODE END USBPD_DPM_UserCableDetection */
}

/**
* @brief  function used to manage user timer.
* @param  PortNum Port number
* @retval None
*/
void USBPD_DPM_UserTimerCounter(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_UserTimerCounter */

  /* USER CODE END USBPD_DPM_UserTimerCounter */
}

/**
* @}
*/

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP2 USBPD USER Exported Callbacks functions called by PE
* @{
*/

/**
* @brief  Callback function called by PE to inform DPM about PE event.
* @param  PortNum The current port number
* @param  EventVal @ref USBPD_NotifyEventValue_TypeDef
* @retval None
*/
void USBPD_DPM_Notification(uint8_t PortNum, USBPD_NotifyEventValue_TypeDef EventVal)
{

#if defined(_GUI_INTERFACE)
    /* Forward PE notifications to GUI if enabled */
  if (NULL != DPM_GUI_PostNotificationMessage)
  {
    DPM_GUI_PostNotificationMessage(PortNum, EventVal);
  }
#endif /* GUI_INTERFACE */

  /* USER CODE BEGIN USBPD_DPM_Notification */


  /* Manage event notified by the stack? */

  switch(EventVal)
  {
    /***************************************************************************
    Power Notification
    */

  case USBPD_NOTIFY_POWER_SWAP_NOT_SUPPORTED :
    AlertPower = PowerNotSupported;
    break;

  case USBPD_NOTIFY_POWER_SWAP_REJ :
    AlertPower = PowerRejected;
    break;

  case USBPD_NOTIFY_POWER_EXPLICIT_CONTRACT :
    /* Power ready means an explicit contract has been establish and Power is available */
    /* Request VDM identify only if not already entered in VDM mode */
    if ((0 == VDM_Mode_On[PortNum]) && (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole))
    {
      USBPD_PE_SVDM_RequestIdentity(PortNum, USBPD_SOPTYPE_SOP);
    }
    if ((USBPD_SPECIFICATION_REV3 == DPM_Params[PortNum].PE_SpecRevision)
        && (USBPD_PORTPOWERROLE_SRC == DPM_Params[PortNum].PE_PowerRole))
    {

    }
    break;
    /*
    End Power Notification
    ***************************************************************************/

    /***************************************************************************
    REQUEST ANSWER NOTIFICATION
    */
  case USBPD_NOTIFY_REQUEST_ACCEPTED:
    /* Update the VBUS threshold according the new request */
    USBPD_PWR_IF_UpdateVbusThreshold(PortNum);

    /* Update DPM_RDOPosition only if current role is SNK */
    if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
    {
      USBPD_SNKRDO_TypeDef rdo;
      rdo.d32 = DPM_Ports[PortNum].DPM_RequestDOMsg;
      DPM_Ports[PortNum].DPM_RDOPosition = rdo.GenericRDO.ObjectPosition;
    }
    break;

    /*
    End REQUEST ANSWER NOTIFICATION
    ***************************************************************************/

  case USBPD_NOTIFY_REQUEST_REJECTED:
  case USBPD_NOTIFY_REQUEST_WAIT:
    /* Requested rejected by the source */
    break;

  case USBPD_NOTIFY_POWER_SWAP_TO_SNK_DONE:
    USBPD_PWR_IF_ResetVbusThreshold(PortNum);
    break;
  case USBPD_NOTIFY_HARDRESET_RX:
  case USBPD_NOTIFY_HARDRESET_TX:
    USBPD_PWR_IF_ResetVbusThreshold(PortNum);
    if (USBPD_PORTPOWERROLE_SNK == DPM_Params[PortNum].PE_PowerRole)
    {
      USBPD_VDM_UserReset(PortNum);
    }
    break;


  case USBPD_NOTIFY_POWER_SWAP_TO_SRC_DONE :
    {
      USBPD_PWR_IF_UpdateVbusThreshold(PortNum);
    }
    break;

  case USBPD_NOTIFY_STATE_SNK_READY:
    {
      /* Send GET_SRC_CAPA_EXT only if current revision is PD3.0 */
      if ((USBPD_SPECIFICATION_REV3 == DPM_Params[PortNum].PE_SpecRevision)
          && (1 == DPM_Ports[PortNum].FlagSendGetSrcCapaExtended))
      {

        DPM_Ports[PortNum].FlagSendGetSrcCapaExtended = 0;
      }
    }
    break;

  case USBPD_NOTIFY_STATE_SRC_DISABLED:
    {
      /* SINK Port Partner is not PD capable. Legacy cable may have been connected
      In this state, VBUS is set to 5V */
    }
    break;
  case USBPD_NOTIFY_CABLERESET_REQUESTED :
    {
      /* CABLE_RESET is requested by the PE but port is not in the correct configuration */
      /* Only a DFP Shall generate Cable Reset Signaling. A DFP Shall only generate Cable Reset Signaling within an Explicit Contract. */
      if ((USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole) && (USBPD_FALSE == DPM_Params[PortNum].VconnStatus))
      {
        /* DFP is not VCONN source. Should request a VCONN Swap */
        USBPD_DPM_RequestVconnSwap(PortNum);
        DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_TRUE;
      }
    }
    break;
  case USBPD_NOTIFY_VCONN_SWAP_COMPLETE :
    {
      /* VCONN Swap received in DFP. Now DFP is VCONN Source. Can send the CABLE_RESET  */
      if ((USBPD_TRUE == DPM_Ports[PortNum].DPM_CableResetOnGoing) && (USBPD_TRUE == DPM_Params[PortNum].VconnStatus))
      {
        /* DFP is now VCONN source. Cable Reset request could be processed */
        USBPD_DPM_RequestCableReset(PortNum);
        DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_FALSE;
      }
    }
    break;

    /*
    *  USB management
    */
  case USBPD_NOTIFY_USBSTACK_START:
    {
      /* start host/device stack */
      if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
      {
        USBPD_USBIF_HostStart(PortNum);
      }
      else
      {
        USBPD_USBIF_DeviceStart(PortNum);
      }
      break;
    }
  case USBPD_NOTIFY_USBSTACK_STOP:
    {
      /* stop host/device stack */
      if (USBPD_PORTDATAROLE_DFP == DPM_Params[PortNum].PE_DataRole)
      {
        USBPD_USBIF_HostStop(PortNum);
      }
      else
      {
        USBPD_USBIF_DeviceStop(PortNum);
      }
      break;
    }
  case USBPD_NOTIFY_DATAROLESWAP_DFP :
    {
      /* switch or start host stack */
      USBPD_USBIF_Swap2Host(PortNum);
      break;
    }
  case USBPD_NOTIFY_DATAROLESWAP_UFP :
    {
      /* switch or start the device stack */
      USBPD_USBIF_Swap2Device(PortNum);
      break;
    }

    /*
    Answer DatRoleSwap NOTIFICATION
    ***************************************************************************/
  case USBPD_NOTIFY_DATAROLESWAP_NOT_SUPPORTED :
    AlertRole = RoleNotSupported;
    break;

  case USBPD_NOTIFY_DATAROLESWAP_REJECTED :
    AlertRole = RoleRejected;
    break;

    /*
    End Answer DatRoleSwap NOTIFICATION
    ***************************************************************************/

    /*************************** end USB management *****************************/

  default:
    break;
  }
  /* USER CODE END USBPD_DPM_Notification */
}

/**
* @brief  Callback function called by PE layer when HardReset message received from PRL
* @param  PortNum The current port number
* @param  CurrentRole the current role
* @param  Status status on hard reset event
* @retval None
*/
void USBPD_DPM_HardReset(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_HR_Status_TypeDef Status)
{
  /* USER CODE BEGIN USBPD_DPM_HardReset */
  USBPD_VDM_UserReset(PortNum);

  /* Reset flag DPM_CableResetOnGoing */
  DPM_Ports[PortNum].DPM_CableResetOnGoing = USBPD_FALSE;

  switch (Status)
  {
  case USBPD_HR_STATUS_WAIT_VBUS_VSAFE0V:
    if (USBPD_PORTPOWERROLE_SRC == CurrentRole)
    {
      /* Reset the power supply */
      DPM_TurnOffPower(PortNum, USBPD_PORTPOWERROLE_SRC);
    }
    break;
  case USBPD_HR_STATUS_WAIT_VBUS_VSAFE5V:
    if (CurrentRole == USBPD_PORTPOWERROLE_SRC)
    {
      /* Power on the power supply */
      DPM_TurnOnPower(PortNum, CurrentRole);
    }
    break;
  case USBPD_HR_STATUS_COMPLETED:
    break;
  case USBPD_HR_STATUS_FAILED:
    break;
  default:
    break;
  }
  /* USER CODE END USBPD_DPM_HardReset */
}

/**
* @brief  Request the DPM to setup the new power level.
* @param  PortNum The current port number
* @retval USBPD status
*/
USBPD_StatusTypeDef USBPD_DPM_SetupNewPower(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_SetupNewPower */
  return USBPD_PWR_IF_SetProfile(PortNum);
  /* USER CODE END USBPD_DPM_SetupNewPower */
}

/**
* @brief  DPM callback to allow PE to retrieve information from DPM/PWR_IF.
* @param  PortNum Port number
* @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
* @param  Ptr     Pointer on address where DPM data should be written (u8 pointer)
* @param  Size    Pointer on nb of u8 written by DPM
* @retval None
*/
void USBPD_DPM_GetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t *Size)
{
  /* USER CODE BEGIN USBPD_DPM_GetDataInfo */
  /* Check type of information targeted by request */
  switch (DataId)
  {
    /* Case Port Source PDO Data information :
    Case Port SINK PDO Data information :
    Call PWR_IF PDO reading request.
    */
  case USBPD_CORE_DATATYPE_SRC_PDO :
  case USBPD_CORE_DATATYPE_SNK_PDO :
    USBPD_PWR_IF_GetPortPDOs(PortNum, DataId, Ptr, Size);
    *Size *= 4;
    break;

    /* Case Requested voltage value Data information */
  case USBPD_CORE_DATATYPE_REQ_VOLTAGE :
    *Size = 4;
    (void)memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_Ports[PortNum].DPM_RequestedVoltage, *Size);
    break;

  case USBPD_CORE_PPS_STATUS :
    {
      /* Get current drawn by sink */
      USBPD_PPSSDB_TypeDef pps_status = {0};

      /* Disable VBUS & IBUS Measurements */
      pps_status.fields.OutputVoltageIn20mVunits  = 0xFFFF;
      pps_status.fields.OutputCurrentIn50mAunits  = 0xFF;
      pps_status.fields.RealTimeFlags             = USBPD_PPS_REALTIMEFLAGS_PTF_NOT_SUPPORTED | USBPD_PPS_REALTIMEFLAGS_OMF_DISABLED;

      *Size = 4;
      (void)memcpy((uint8_t*)Ptr, (uint8_t *)&pps_status.d32, *Size);
    }
    break;

  case USBPD_CORE_EXTENDED_CAPA :
    {
      *Size = sizeof(USBPD_SCEDB_TypeDef);
      memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_USER_Settings[PortNum].DPM_SRCExtendedCapa, *Size);
    }
    break;

  case USBPD_CORE_SNK_EXTENDED_CAPA :
    {
      *Size = sizeof(USBPD_SKEDB_TypeDef);
      memcpy((uint8_t*)Ptr, (uint8_t *)&DPM_USER_Settings[PortNum].DPM_SNKExtendedCapa, *Size);
    }
    break;

  case USBPD_CORE_REVISION:
    {
      *Size = sizeof(USBPD_RevisionDO_TypeDef);
      USBPD_RevisionDO_TypeDef rev =
      {
        /* Hardcoded values as example, user may want to use a global USBPD_RevisionDO_TypeDef variable */
        .b.Revision_major = 3,                  /*!< Major revision */
        .b.Revision_minor = 0,                  /*!< Minor revision */
        .b.Version_major  = 2,                  /*!< Major version  */
        .b.Version_minor  = 0,                  /*!< Minor version  */
      };
      memcpy((uint8_t *)Ptr, &rev, *Size);
    }
    break;

  default :
    DPM_USER_DEBUG_TRACE(PortNum, "ADVICE: update USBPD_DPM_GetDataInfo:%d", DataId);
    *Size = 0;
    break;
  }
  /* USER CODE END USBPD_DPM_GetDataInfo */
}

/**
* @brief  DPM callback to allow PE to update information in DPM/PWR_IF.
* @param  PortNum Port number
* @param  DataId  Type of data to be updated in DPM based on @ref USBPD_CORE_DataInfoType_TypeDef
* @param  Ptr     Pointer on the data
* @param  Size    Nb of bytes to be updated in DPM
* @retval None
*/
void USBPD_DPM_SetDataInfo(uint8_t PortNum, USBPD_CORE_DataInfoType_TypeDef DataId, uint8_t *Ptr, uint32_t Size)
{
  /* USER CODE BEGIN USBPD_DPM_SetDataInfo */
  uint32_t index;

  /* Check type of information targeted by request */
  switch (DataId)
  {
    /* Case requested DO position Data information :
    */
  case USBPD_CORE_DATATYPE_RDO_POSITION :
    if (Size == 4)
    {
      uint8_t* temp;
      temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPosition;
      (void)memcpy(temp, Ptr, Size);
      DPM_Ports[PortNum].DPM_RDOPositionPrevious = *Ptr;
      temp = (uint8_t*)&DPM_Ports[PortNum].DPM_RDOPositionPrevious;
      (void)memcpy(temp, Ptr, Size);
    }
    break;

    /* Case Received Source PDO values Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_SRC_PDO :
    if (Size <= (USBPD_MAX_NB_PDO * 4))
    {
      uint8_t* rdo;
      DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO = (Size / 4);
      /* Copy PDO data in DPM Handle field */
      for (index = 0; index < (Size / 4); index++)
      {
        rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index];
        (void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
      }
    }
    break;

    /* Case Received Sink PDO values Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_SNK_PDO :
    if (Size <= (USBPD_MAX_NB_PDO * 4))
    {
      uint8_t* rdo;
      DPM_Ports[PortNum].DPM_NumberOfRcvSNKPDO = (Size / 4);
      /* Copy PDO data in DPM Handle field */
      for (index = 0; index < (Size / 4); index++)
      {
        rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_ListOfRcvSNKPDO[index];
        (void)memcpy(rdo, (Ptr + (index * 4u)), (4u * sizeof(uint8_t)));
      }
    }
    break;

    /* Case Received Request PDO Data information :
    */
  case USBPD_CORE_DATATYPE_RCV_REQ_PDO :
    if (Size == 4)
    {
      uint8_t* rdo;
      rdo = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvRequestDOMsg;
      (void)memcpy(rdo, Ptr, Size);
    }
    break;

  case USBPD_CORE_SNK_EXTENDED_CAPA :
    {
      uint8_t*  _snk_ext_capa;
      _snk_ext_capa = (uint8_t*)&DPM_Ports[PortNum].DPM_RcvSNKExtendedCapa;
      memcpy(_snk_ext_capa, Ptr, Size);
    }
    break;

    /* In case of unexpected data type (Set request could not be fulfilled) :
    */
  default :
    DPM_USER_DEBUG_TRACE(PortNum, "ADVICE: update USBPD_DPM_SetDataInfo:%d", DataId);
    break;
  }

  /* USER CODE END USBPD_DPM_SetDataInfo */
}

/**
* @brief  Evaluate received Request Message from Sink port
* @param  PortNum Port number
* @param  PtrPowerObject  Pointer on the power data object
* @retval USBPD status : USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT, USBPD_GOTOMIN
*/
USBPD_StatusTypeDef USBPD_DPM_EvaluateRequest(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
  /* USER CODE BEGIN USBPD_DPM_EvaluateRequest */
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_PDO_TypeDef pdo;
  uint32_t pdomaxcurrent = 0;
  uint32_t rdomaxcurrent = 0, rdoopcurrent = 0, rdoobjposition = 0;
  uint32_t rdovoltage;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];

  rdo.d32 = pdhandle->DPM_RcvRequestDOMsg;
  rdoobjposition  = rdo.GenericRDO.ObjectPosition;

  /* Set unchuncked bit if supported by ports */
  DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
  if ((USBPD_TRUE == rdo.GenericRDO.UnchunkedExtendedMessage)
      && (USBPD_TRUE == DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport))
  {
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
  }

  pdhandle->DPM_RDOPosition = 0;

  /* Check if RDP can be met within the supported PDOs by the Source port */
  /* USBPD_DPM_EvaluateRequest: Evaluate Sink Request\r */
  /* USBPD_DPM_EvaluateRequest: Check if RDP can be met within the supported PDOs by the Source port\r */

  /* Search PDO in Port Source PDO list, that corresponds to Position provided in Request RDO */
  if (USBPD_PWR_IF_SearchRequestedPDO(PortNum, rdoobjposition, &pdo.d32) != USBPD_OK)
  {
    /* Invalid PDO index */
    /* USBPD_DPM_EvaluateRequest: Invalid PDOs index */
    return USBPD_REJECT;
  }

  switch(pdo.GenericPDO.PowerObject)
  {
  case USBPD_CORE_PDO_TYPE_FIXED:
    {
      pdomaxcurrent = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
      rdomaxcurrent = rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits;
      rdoopcurrent  = rdo.FixedVariableRDO.OperatingCurrentIn10mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 10;
      rdovoltage    = pdo.SRCFixedPDO.VoltageIn50mVunits * 50;

      if(rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        /* USBPD_DPM_EvaluateRequest: Sink requests too much operating current*/
        return USBPD_REJECT;
      }

      if(rdomaxcurrent > pdomaxcurrent)
      {
        /* Sink requests too much maximum operating current */
        /* USBPD_DPM_EvaluateRequest: Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
  case USBPD_CORE_PDO_TYPE_APDO:
    {
      uint32_t pdominvoltage, pdomaxvoltage;
      pdomaxcurrent                           = pdo.SRCSNKAPDO.MaxCurrentIn50mAunits;
      rdoopcurrent                            = rdo.ProgRDO.OperatingCurrentIn50mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 50;
      if(rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        return USBPD_REJECT;
      }
      pdominvoltage = pdo.SRCSNKAPDO.MinVoltageIn100mV * 100;
      pdomaxvoltage = pdo.SRCSNKAPDO.MaxVoltageIn100mV * 100;
      rdovoltage    = rdo.ProgRDO.OutputVoltageIn20mV * 20;

      if ((rdovoltage < pdominvoltage) || (rdovoltage > pdomaxvoltage))
      {
        /* Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
  case USBPD_CORE_PDO_TYPE_BATTERY:
  case USBPD_CORE_PDO_TYPE_VARIABLE:
  default:
    {
      return USBPD_REJECT;
    }
  }

  /* Set RDO position and requested voltage in DPM port structure */
  DPM_Ports[PortNum].DPM_RequestedVoltage = rdovoltage;
  pdhandle->DPM_RDOPositionPrevious = pdhandle->DPM_RDOPosition;
  pdhandle->DPM_RDOPosition = rdoobjposition;

  /* Save the power object */
  *PtrPowerObject = pdo.GenericPDO.PowerObject;

  /* Accept the requested power */
  /* USBPD_DPM_EvaluateRequest: Sink requested %d mV %d mA for operating current from %d to %d mA\r",
  pdo.SRCFixedPDO.VoltageIn50mVunits * 50, pdo.SRCFixedPDO.MaxCurrentIn10mAunits * 10,
  rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10, rdo.FixedVariableRDO.OperatingCurrentIn10mAunits * 10 */
  /* USBPD_DPM_EvaluateRequest: Source accepts the requested power */
  return USBPD_ACCEPT;
  /* USER CODE END USBPD_DPM_EvaluateRequest */
}

/**
* @brief  Callback to be used by PE to evaluate a Vconn swap
* @param  PortNum Port number
* @retval USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT
*/
USBPD_StatusTypeDef USBPD_DPM_EvaluateVconnSwap(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_EvaluateVconnSwap */
  USBPD_StatusTypeDef status = USBPD_REJECT;
  if (USBPD_TRUE == DPM_USER_Settings[PortNum].PE_VconnSwap)
  {
    status = USBPD_ACCEPT;
  }

  return status;
  /* USER CODE END USBPD_DPM_EvaluateVconnSwap */
}

/**
* @brief  Callback to be used by PE to manage VConn
* @param  PortNum Port number
* @param  State Enable or Disable VConn on CC lines
* @retval USBPD_ACCEPT, USBPD_REJECT
*/
USBPD_StatusTypeDef USBPD_DPM_PE_VconnPwr(uint8_t PortNum, USBPD_FunctionalState State)
{
  /* USER CODE BEGIN USBPD_DPM_PE_VconnPwr */
  return USBPD_ERROR;
  /* USER CODE END USBPD_DPM_PE_VconnPwr */
}

/**
* @brief  DPM callback to allow PE to forward extended message information.
* @param  PortNum Port number
* @param  MsgType Type of message to be handled in DPM
*         This parameter can be one of the following values:
*           @arg @ref USBPD_EXT_SECURITY_REQUEST Security Request extended message
*           @arg @ref USBPD_EXT_SECURITY_RESPONSE Security Response extended message
* @param  ptrData   Pointer on address Extended Message data could be read (u8 pointer)
* @param  DataSize  Nb of u8 that compose Extended message
* @retval None
*/
void USBPD_DPM_ExtendedMessageReceived(uint8_t PortNum, USBPD_ExtendedMsg_TypeDef MsgType, uint8_t *ptrData, uint16_t DataSize)
{
  /* USER CODE BEGIN USBPD_DPM_ExtendedMessageReceived */

  /* USER CODE END USBPD_DPM_ExtendedMessageReceived */
}

/**
* @brief  DPM callback to allow PE to enter ERROR_RECOVERY state.
* @param  PortNum Port number
* @retval None
*/
void USBPD_DPM_EnterErrorRecovery(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_EnterErrorRecovery */
  /* Inform CAD to enter recovery mode */
  USBPD_CAD_EnterErrorRecovery(PortNum);
  /* USER CODE END USBPD_DPM_EnterErrorRecovery */
}

/**
* @brief  Callback used to ask application the reply status for a DataRoleSwap request
* @note   if the callback is not set (ie NULL) the stack will automatically reject the request
* @param  PortNum Port number
* @retval Returned values are:
@ref USBPD_ACCEPT if DRS can be accepted
@ref USBPD_REJECT if DRS is not accepted in one data role (DFP or UFP) or in PD2.0 config
@ref USBPD_NOTSUPPORTED if DRS is not supported at all by the application (in both data roles) - P3.0 only
*/
USBPD_StatusTypeDef USBPD_DPM_EvaluateDataRoleSwap(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_EvaluateDataRoleSwap */
  return USBPD_ACCEPT;
  /* USER CODE END USBPD_DPM_EvaluateDataRoleSwap */
}

/**
* @brief  Callback to be used by PE to check is VBUS is ready or present
* @param  PortNum Port number
* @param  Vsafe   Vsafe status based on @ref USBPD_VSAFE_StatusTypeDef
* @retval USBPD_DISABLE or USBPD_ENABLE
*/
USBPD_FunctionalState USBPD_DPM_IsPowerReady(uint8_t PortNum, USBPD_VSAFE_StatusTypeDef Vsafe)
{
  /* USER CODE BEGIN USBPD_DPM_IsPowerReady */
  return ((USBPD_OK == USBPD_PWR_IF_SupplyReady(PortNum, Vsafe)) ? USBPD_ENABLE : USBPD_DISABLE);
  /* USER CODE END USBPD_DPM_IsPowerReady */
}

/**
* @}
*/

/** @defgroup USBPD_USER_EXPORTED_FUNCTIONS_GROUP3 USBPD USER Functions PD messages requests
* @{
*/

/**
* @brief  Request the PE to send a hard reset
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestHardReset(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_HardReset(PortNum);
  DPM_USER_ERROR_TRACE(PortNum, _status, "HARD RESET not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a cable reset.
* @note   Only a DFP Shall generate Cable Reset Signaling. A DFP Shall only generate Cable Reset Signaling within an Explicit Contract.
The DFP has to be supplying VCONN prior to a Cable Reset
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestCableReset(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CableReset(PortNum);
  DPM_USER_ERROR_TRACE(PortNum, _status, "CABLE RESET not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GOTOMIN message
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGotoMin(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GOTOMIN, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GOTOMIN not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a PING message
* @note   In USB-PD stack, only ping management for P3.0 is implemented.
*         If PD2.0 is used, PING timer needs to be implemented on user side.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestPing(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_PING, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "PING not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a request message.
* @param  PortNum     The current port number
* @param  IndexSrcPDO Index on the selected SRC PDO (value between 1 to 7)
* @param  RequestedVoltage Requested voltage (in MV and use mainly for APDO)
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestMessageRequest(uint8_t PortNum, uint8_t IndexSrcPDO, uint16_t RequestedVoltage)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;
  /* USER CODE BEGIN USBPD_DPM_RequestMessageRequest */
  /* To be adapted to call the PE function */
  /*       _status = USBPD_PE_Send_Request(PortNum, rdo.d32, pdo_object);*/
  DPM_USER_DEBUG_TRACE(PortNum, "ADVICE: update USBPD_DPM_RequestMessageRequest");
  /* USER CODE END USBPD_DPM_RequestMessageRequest */
  DPM_USER_ERROR_TRACE(PortNum, _status, "REQUEST not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GET_SRC_CAPA message
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapability(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_SRC_CAPA not accepted by the stack");

  return _status;
}

/**
* @brief  Request the PE to send a GET_SNK_CAPA message
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapability(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SNK_CAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_SINK_CAPA not accepted by the stack");

  return _status;
}

/**
* @brief  Request the PE to perform a Data Role Swap.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestDataRoleSwap(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_DR_SWAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "DRS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a Power Role Swap.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestPowerRoleSwap(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_PR_SWAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "PRS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a VCONN Swap.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVconnSwap(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_VCONN_SWAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VCS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a soft reset
* @param  PortNum The current port number
* @param  SOPType SOP Type based on @ref USBPD_SOPType_TypeDef
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestSoftReset(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_SOFT_RESET, SOPType);
  DPM_USER_ERROR_TRACE(PortNum, _status, "SOFT_RESET not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a Source Capability message.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestSourceCapability(uint8_t PortNum)
{
  /* PE will directly get the PDO saved in structure @ref PWR_Port_PDO_Storage */
  USBPD_StatusTypeDef _status = USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_SRC_CAPABILITIES, NULL);
  DPM_USER_ERROR_TRACE(PortNum, _status, "SRC_CAPA not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a VDM discovery identity
* @param  PortNum The current port number
* @param  SOPType SOP Type
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryIdentify(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;
  /* USER CODE BEGIN USBPD_DPM_RequestVDM_DiscoveryIdentify */
  if (USBPD_SOPTYPE_SOP == SOPType)
  {
    _status = USBPD_PE_SVDM_RequestIdentity(PortNum, SOPType);
  }
  /* USER CODE END USBPD_DPM_RequestVDM_DiscoveryIdentify */
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM Discovery Ident not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a VDM discovery SVID
* @param  PortNum The current port number
* @param  SOPType SOP Type
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoverySVID(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SVDM_RequestSVID(PortNum, SOPType);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM discovery SVID not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a VDM Discovery mode message on one SVID.
* @param  PortNum The current port number
* @param  SOPType SOP Type
* @param  SVID    SVID used for discovery mode message
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_DiscoveryMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SVDM_RequestMode(PortNum, SOPType, SVID);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM Discovery mode not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a VDM mode enter.
* @param  PortNum   The current port number
* @param  SOPType   SOP Type
* @param  SVID      SVID used for discovery mode message
* @param  ModeIndex Index of the mode to be entered
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_EnterMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SVDM_RequestModeEnter(PortNum, SOPType, SVID, ModeIndex);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM mode enter not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a VDM mode exit.
* @param  PortNum   The current port number
* @param  SOPType   SOP Type
* @param  SVID      SVID used for discovery mode message
* @param  ModeIndex Index of the mode to be exit
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestVDM_ExitMode(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint8_t ModeIndex)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SVDM_RequestModeExit(PortNum, SOPType, SVID, ModeIndex);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM mode exit not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a Display Port status
* @param  PortNum   The current port number
* @param  SOPType   SOP Type
* @param  SVID      Used SVID
* @param  pDPStatus Pointer on DP Status data (32 bit)
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortStatus(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPStatus)
{
  USBPD_StatusTypeDef _status;
  /* USER CODE BEGIN USBPD_DPM_RequestDisplayPortStatus */
  /*USBPD_VDM_FillDPStatus(PortNum, (USBPD_DPStatus_TypeDef*)pDPStatus);*/
  /* USER CODE END USBPD_DPM_RequestDisplayPortStatus */
  _status = USBPD_PE_SVDM_RequestSpecific(PortNum, SOPType, SVDM_SPECIFIC_1, SVID);
  DPM_USER_ERROR_TRACE(PortNum, _status, "Display Port status not accepted by the stack");
  return _status;
}
/**
* @brief  Request the PE to send a Display Port Config
* @param  PortNum   The current port number
* @param  SOPType   SOP Type
* @param  SVID      Used SVID
* @param  pDPConfig Pointer on DP Config data (32 bit)
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestDisplayPortConfig(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID, uint32_t *pDPConfig)
{
  USBPD_StatusTypeDef _status;
  /* USER CODE BEGIN USBPD_DPM_RequestDisplayPortConfig */
  /*USBPD_VDM_FillDPConfig(PortNum, (USBPD_DPConfig_TypeDef*)pDPConfig);*/
  /* USER CODE END USBPD_DPM_RequestDisplayPortConfig */
  _status = USBPD_PE_SVDM_RequestSpecific(PortNum, SOPType, SVDM_SPECIFIC_2, SVID);
  DPM_USER_ERROR_TRACE(PortNum, _status, "Display Port Config not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a VDM Attention.
* @param  PortNum The current port number
* @param  SOPType SOP Type
* @param  SVID    Used SVID
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestAttention(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint16_t SVID)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SVDM_RequestAttention(PortNum, SOPType, SVID);
  DPM_USER_ERROR_TRACE(PortNum, _status, "VDM ATTENTION not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send an ALERT to port partner
* @param  PortNum The current port number
* @param  Alert   Alert based on @ref USBPD_ADO_TypeDef
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestAlert(uint8_t PortNum, USBPD_ADO_TypeDef Alert)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_ALERT, (uint32_t*)&Alert.d32);
  DPM_USER_ERROR_TRACE(PortNum, _status, "ALERT not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to get a source capability extended
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetSourceCapabilityExt(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_SRC_CAPEXT, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_SRC_CAPA_EXT not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to get a sink capability extended
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetSinkCapabilityExt(uint8_t PortNum)
{
  DPM_USER_ERROR_TRACE(PortNum, USBPD_ERROR, "GET_SINK_CAPA_EXT not accepted by the stack");
  return USBPD_ERROR;
}

/**
* @brief  Request the PE to get a manufacturer info
* @param  PortNum The current port number
* @param  SOPType SOP Type
* @param  pManuInfoData Pointer on manufacturer info based on @ref USBPD_GMIDB_TypeDef
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetManufacturerInfo(uint8_t PortNum, USBPD_SOPType_TypeDef SOPType, uint8_t* pManuInfoData)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;
  if (USBPD_SOPTYPE_SOP == SOPType)
  {
    _status = USBPD_PE_SendExtendedMessage(PortNum, SOPType, USBPD_EXT_GET_MANUFACTURER_INFO, (uint8_t*)pManuInfoData, sizeof(USBPD_GMIDB_TypeDef));
  }
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_MANU_INFO not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to request a GET_PPS_STATUS
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetPPS_Status(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_PPS_STATUS, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_PPS_STATUS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to request a GET_STATUS
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetStatus(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_STATUS, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_STATUS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to perform a Fast Role Swap.
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestFastRoleSwap(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_FR_SWAP, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "FRS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GET_COUNTRY_CODES message
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryCodes(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_CtrlMessage(PortNum, USBPD_CONTROLMSG_GET_COUNTRY_CODES, USBPD_SOPTYPE_SOP);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_COUNTRY_CODES not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GET_COUNTRY_INFO message
* @param  PortNum     The current port number
* @param  CountryCode Country code (1st character and 2nd of the Alpha-2 Country)
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetCountryInfo(uint8_t PortNum, uint16_t CountryCode)
{
  USBPD_StatusTypeDef _status = USBPD_PE_Request_DataMessage(PortNum, USBPD_DATAMSG_GET_COUNTRY_INFO, (uint32_t*)&CountryCode);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_COUNTRY_INFO not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GET_BATTERY_CAPA
* @param  PortNum         The current port number
* @param  pBatteryCapRef  Pointer on the Battery Capability reference
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryCapability(uint8_t PortNum, uint8_t *pBatteryCapRef)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_GET_BATTERY_CAP, (uint8_t*)pBatteryCapRef, 1);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_BATTERY_CAPA not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a GET_BATTERY_STATUS
* @param  PortNum           The current port number
* @param  pBatteryStatusRef Pointer on the Battery Status reference
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestGetBatteryStatus(uint8_t PortNum, uint8_t *pBatteryStatusRef)
{
  USBPD_StatusTypeDef _status = USBPD_PE_SendExtendedMessage(PortNum, USBPD_SOPTYPE_SOP, USBPD_EXT_GET_BATTERY_STATUS, (uint8_t*)pBatteryStatusRef, 1);
  DPM_USER_ERROR_TRACE(PortNum, _status, "GET_BATTERY_STATUS not accepted by the stack");
  return _status;
}

/**
* @brief  Request the PE to send a SECURITY_REQUEST
* @param  PortNum The current port number
* @retval USBPD Status
*/
USBPD_StatusTypeDef USBPD_DPM_RequestSecurityRequest(uint8_t PortNum)
{
  USBPD_StatusTypeDef _status = USBPD_ERROR;
  DPM_USER_ERROR_TRACE(PortNum, _status, "SECURITY_REQUEST not accepted by the stack");
  return _status;
}




/**
* @brief  Evaluate received Capabilities Message from Source port and prepare the request message
* @param  PortNum         Port number
* @param  PtrRequestData  Pointer on selected request data object
* @param  PtrPowerObjectType  Pointer on the power data object
* @retval None
*/

void USBPD_DPM_SNK_EvaluateCapabilities(uint8_t PortNum, uint32_t *PtrRequestData, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType)
{
  /* USER CODE BEGIN USBPD_DPM_SNK_EvaluateCapabilities */
  USBPD_PDO_TypeDef  pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  uint32_t size;
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* Initialize RDO */
  rdo.d32 = 0;

  /* Select the first PDO to get a 5V aligned with sink PDO */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];

  /* Read the sink PDO */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)snkpdolist, &size);

  /* Initialise sinkdpo with the first PDO*/
  snk_fixed_pdo.d32 = snkpdolist[0];

  if(USBPD_CORE_PDO_TYPE_FIXED == pdo.GenericPDO.PowerObject)
  {
    /* Prepare the requested pdo */
    rdo.FixedVariableRDO.ObjectPosition = 1;
    if (snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits > pdo.SRCFixedPDO.MaxCurrentIn10mAunits)
    {
      rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
      rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
      rdo.FixedVariableRDO.CapabilityMismatch = 1;
    }
    else
    {
      rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits;
      rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = snk_fixed_pdo.SNKFixedPDO.OperationalCurrentIn10mAunits;
      rdo.FixedVariableRDO.CapabilityMismatch = 0;
    }

    rdo.FixedVariableRDO.USBCommunicationsCapable = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;

#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
    if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
    {
      rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
      DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
      /* Set unchuncked bit if supported by port partner;*/
      if (USBPD_TRUE == fixed_pdo.SRCFixedPDO.UnchunkedExtendedMessage)
      {
        DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
      }
    }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */

    *PtrPowerObjectType = USBPD_CORE_PDO_TYPE_FIXED;
    *PtrRequestData = rdo.d32;
    pdhandle->DPM_RequestDOMsg = rdo.d32;
    pdhandle->DPM_RequestedVoltage = 5000;
  }
  else {
    /* This case shall never occurs because any source must present a first PDO with 5V */
  }

  DPM_USER_DEBUG_TRACE(PortNum, "ADVICE: update USBPD_DPM_SNK_EvaluateCapabilities");
  /* USER CODE END USBPD_DPM_SNK_EvaluateCapabilities */
}





/**
* @brief  Evaluate the swap request from PE.
* @param  PortNum The current port number
* @retval USBPD_ACCEPT, USBPD_WAIT, USBPD_REJECT
*/
USBPD_StatusTypeDef USBPD_DPM_EvaluatePowerRoleSwap(uint8_t PortNum)
{
  /* USER CODE BEGIN USBPD_DPM_EvaluatePowerRoleSwap */
  return USBPD_ACCEPT;
  /* USER CODE END USBPD_DPM_EvaluatePowerRoleSwap */
}




/**
* @brief  Power role swap status update
* @param  PortNum Port number
* @param  CurrentRole the current role
* @param  Status status on power role swap event
* @retval None
*/
void USBPD_DPM_PowerRoleSwap(uint8_t PortNum, USBPD_PortPowerRole_TypeDef CurrentRole, USBPD_PRS_Status_TypeDef Status)
{
  /* USER CODE BEGIN USBPD_DPM_PowerRoleSwap */
  switch (Status)
  {
  case USBPD_PRS_STATUS_VBUS_OFF:
    if (CurrentRole == USBPD_PORTPOWERROLE_SRC)
    {
      /* This variable is used to avoid VCONN disable during HR procedure */
      uint32_t _vconnstate = DPM_Params[PortNum].VconnStatus;
      DPM_Params[PortNum].VconnStatus = USBPD_FALSE;

      /* In case of power role swap keep VCONN On */
      DPM_TurnOffPower(PortNum, CurrentRole);
      /* restore vconn status */
      DPM_Params[PortNum].VconnStatus = _vconnstate;
    }
    break;
  case USBPD_PRS_STATUS_SRC_RP2RD:
    DPM_AssertRd(PortNum);
    break;
  case USBPD_PRS_STATUS_SNK_RD2RP:
    DPM_AssertRp(PortNum);
    break;
  case USBPD_PRS_STATUS_VBUS_ON:
    DPM_TurnOnPower(PortNum, CurrentRole);
    break;
  default:
    break;
  }
  /* USER CODE END USBPD_DPM_PowerRoleSwap */
}


/**
* @brief  Turn Off power supply.
* @param  PortNum The current port number
* @param  Role    Port power role
* @retval USBPD_OK, USBPD_ERROR
*/
static USBPD_StatusTypeDef DPM_TurnOffPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role)
{
  USBPD_StatusTypeDef status = USBPD_OK;

  status = USBPD_PWR_IF_VBUSDisable(PortNum);
  return status;
}


/**
* @brief  Assert Rd resistor.
* @param  PortNum The current port number
* @retval None
*/
static void DPM_AssertRd(uint8_t PortNum)
{
  USBPD_CAD_AssertRd(PortNum);
}


/**
* @brief  Assert Rp resistor.
* @param  PortNum The current port number
* @retval None
*/
static void DPM_AssertRp(uint8_t PortNum)
{
  USBPD_CAD_AssertRp(PortNum);
}



/**
* @brief  Turn On power supply.
* @param  PortNum The current port number
* @param  Role    Port power role
* @retval USBPD_ACCEPT, USBPD_WAIT, USBPD_REJECT
*/
static USBPD_StatusTypeDef DPM_TurnOnPower(uint8_t PortNum, USBPD_PortPowerRole_TypeDef Role)
{
  USBPD_StatusTypeDef status = USBPD_OK;
  /* Enable the output */
  status = USBPD_PWR_IF_VBUSEnable(PortNum);
  if(USBPD_PORTPOWERROLE_SRC == Role)
  {
    /* Enable the output */
    USBPD_DPM_WaitForTime(30);
  }
  else
  {
    /* stop current sink */
  }

  return status;
}

/**
* @brief  Examinate a given SRC PDO to check if matching with SNK capabilities.
* @param  PortNum Port number
* @param  SrcPDO              Selected SRC PDO (32 bits)
* @param  PtrRequestedVoltage Pointer on Voltage value that could be reached if SRC PDO is requested (only valid if USBPD_TRUE is returned) in mV
* @param  PtrRequestedPower   Pointer on Power value that could be reached if SRC PDO is requested (only valid if USBPD_TRUE is returned) in mW
* @retval USBPD_FALSE of USBPD_TRUE (USBPD_TRUE returned in SRC PDO is considered matching with SNK profile)
*/
uint32_t USBPD_DPM_SNK_EvaluateMatchWithSRCPDO(uint8_t PortNum, uint32_t SrcPDO, uint32_t* PtrRequestedVoltage, uint32_t* PtrRequestedPower)
{
  USBPD_PDO_TypeDef  srcpdo, snkpdo;
  uint32_t match = USBPD_FALSE;
  uint32_t nbsnkpdo;
  uint32_t snkpdo_array[USBPD_MAX_NB_PDO];
  uint16_t i, srcvoltage50mv, srcmaxvoltage50mv, srcminvoltage50mv, srcmaxcurrent10ma;
  uint16_t snkvoltage50mv, snkmaxvoltage50mv, snkminvoltage50mv, snkopcurrent10ma;
  uint32_t maxrequestedpower, currentrequestedpower;
  uint32_t maxrequestedvoltage, currentrequestedvoltage;
  uint32_t snkoppower250mw, srcmaxpower250mw;

  /* Retrieve SNK PDO list from PWR_IF storage : PDO values + nb of u32 written by PWR_IF (nb of PDOs) */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t*)snkpdo_array, &nbsnkpdo);

  if (0 == nbsnkpdo)
  {
    return(USBPD_FALSE);
  }

  /* Set default output values */
  maxrequestedpower    = 0;
  maxrequestedvoltage  = 0;

  /* Check SRC PDO value according to its type */
  srcpdo.d32 = SrcPDO;
  switch(srcpdo.GenericPDO.PowerObject)
  {
    /* SRC Fixed Supply PDO */
  case USBPD_CORE_PDO_TYPE_FIXED:
    srcvoltage50mv = srcpdo.SRCFixedPDO.VoltageIn50mVunits;
    srcmaxcurrent10ma = srcpdo.SRCFixedPDO.MaxCurrentIn10mAunits;

    /* Loop through SNK PDO list */
    for (i=0; i<nbsnkpdo; i++)
    {
      currentrequestedpower = 0;
      currentrequestedvoltage = 0;

      /* Retrieve SNK PDO value according to its type */
      snkpdo.d32 = snkpdo_array[i];
      switch(snkpdo.GenericPDO.PowerObject)
      {
        /* SNK Fixed Supply PDO */
      case USBPD_CORE_PDO_TYPE_FIXED:
        {
          snkvoltage50mv = snkpdo.SNKFixedPDO.VoltageIn50mVunits;
          snkopcurrent10ma = snkpdo.SNKFixedPDO.OperationalCurrentIn10mAunits;

          /* Match if :
          SNK Voltage = SRC Voltage
          &&
          SNK Op Current <= SRC Max Current

          Requested Voltage : SNK Voltage
          Requested Op Current : SNK Op Current
          Requested Max Current : SNK Op Current
          */
          if (  (snkvoltage50mv == srcvoltage50mv)
              &&(snkopcurrent10ma <= srcmaxcurrent10ma))
          {
            currentrequestedpower = (snkvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
            currentrequestedvoltage = snkvoltage50mv;
          }
          break;
        }
        /* SNK Variable Supply (non-battery) PDO */
      case USBPD_CORE_PDO_TYPE_VARIABLE:
        snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
        snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

        /* Match if :
        SNK Max voltage >= SRC Voltage
        &&
        SNK Min voltage <= SRC Voltage
        &&
        SNK Op current <= SRC Max current

        Requested Voltage : SRC Voltage
        Requested Op Current : SNK Op Current
        Requested Max Current : SNK Op Current
        */
        if (  (snkmaxvoltage50mv >= srcvoltage50mv)
            &&(snkminvoltage50mv <= srcvoltage50mv)
              &&(snkopcurrent10ma  <= srcmaxcurrent10ma))
        {
          currentrequestedpower = (srcvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
          currentrequestedvoltage = srcvoltage50mv;
        }
        break;

        /* SNK Battery Supply PDO */
      case USBPD_CORE_PDO_TYPE_BATTERY:
        snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
        snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

        /* Match if :
        SNK Max voltage >= SRC Voltage
        &&
        SNK Min voltage <= SRC Voltage
        &&
        SNK Op power <= SRC Max current * SRC Voltage

        Requested Voltage : SRC Voltage
        Requested Op Current : SNK Op Power/ SRC Voltage
        Requested Max Current : SNK Op Power/ SRC Voltage
        */
        if (  (snkmaxvoltage50mv >= srcvoltage50mv)
            &&(snkminvoltage50mv <= srcvoltage50mv)
              &&(snkoppower250mw <= ((srcvoltage50mv * srcmaxcurrent10ma)/500)))  /* to get value in 250 mw units */
        {
          currentrequestedvoltage = srcvoltage50mv;
          currentrequestedpower = snkoppower250mw;
        }
        break;

        /* SNK Augmented Power Data Object (APDO) */
      case USBPD_CORE_PDO_TYPE_APDO:
        break;

      default:
        break;
      }

      if (currentrequestedpower > maxrequestedpower)
      {
        match = USBPD_TRUE;
        maxrequestedpower   = currentrequestedpower;
        maxrequestedvoltage = currentrequestedvoltage;
      }
    }
    break;

    /* SRC Variable Supply (non-battery) PDO */
  case USBPD_CORE_PDO_TYPE_VARIABLE:
    srcmaxvoltage50mv = srcpdo.SRCVariablePDO.MaxVoltageIn50mVunits;
    srcminvoltage50mv = srcpdo.SRCVariablePDO.MinVoltageIn50mVunits;
    srcmaxcurrent10ma = srcpdo.SRCVariablePDO.MaxCurrentIn10mAunits;

    /* Loop through SNK PDO list */
    for (i=0; i<nbsnkpdo; i++)
    {
      currentrequestedpower = 0;
      currentrequestedvoltage = 0;

      /* Retrieve SNK PDO value according to its type */
      snkpdo.d32 = snkpdo_array[i];
      switch(snkpdo.GenericPDO.PowerObject)
      {
        /* SNK Fixed Supply PDO */
      case USBPD_CORE_PDO_TYPE_FIXED:
        /* No match */
        break;

        /* SNK Variable Supply (non-battery) PDO */
      case USBPD_CORE_PDO_TYPE_VARIABLE:
        snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
        snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

        /* Match if :
        SNK Max voltage >= SRC Max Voltage
        &&
        SNK Min voltage <= SRC Min Voltage
        &&
        SNK Op current <= SRC Max current

        Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage
        Requested Op Current : SNK Op Current
        Requested Max Current : SNK Op Current
        */
        if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
            &&(snkminvoltage50mv <= srcminvoltage50mv)
              &&(snkopcurrent10ma <= srcmaxcurrent10ma))
        {
          currentrequestedpower = (srcmaxvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
          currentrequestedvoltage = srcmaxvoltage50mv;
        }
        break;

        /* SNK Battery Supply PDO */
      case USBPD_CORE_PDO_TYPE_BATTERY:
        snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
        snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

        /* Match if :
        SNK Max voltage >= SRC Max Voltage
        &&
        SNK Min voltage <= SRC Min Voltage
        &&
        SNK Op power <= SRC Max current * SRC Max Voltage

        Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage, that fulfill
        SNK Op power <= Voltage * SRC Max Current
        Requested Op Current : SNK Op Power/ SRC Voltage
        Requested Max Current : SNK Op Power/ SRC Voltage
        */
        if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
            &&(snkminvoltage50mv <= srcminvoltage50mv)
              &&(snkoppower250mw <= ((srcmaxvoltage50mv * srcmaxcurrent10ma)/500)))  /* to get value in 250 mw units */
        {
          currentrequestedpower   = snkoppower250mw * 250; /* to get value in mw */
          currentrequestedvoltage = srcmaxvoltage50mv;
        }
        break;

        /* SNK Augmented Power Data Object (APDO) */
      case USBPD_CORE_PDO_TYPE_APDO:
        break;

      default:
        break;
      }

      if (currentrequestedpower > maxrequestedpower)
      {
        match = USBPD_TRUE;
        maxrequestedpower   = currentrequestedpower;
        maxrequestedvoltage = currentrequestedvoltage;
      }
    }
    break;

    /* SRC Battery Supply PDO */
  case USBPD_CORE_PDO_TYPE_BATTERY:
    srcmaxvoltage50mv = srcpdo.SRCBatteryPDO.MaxVoltageIn50mVunits;
    srcminvoltage50mv = srcpdo.SRCBatteryPDO.MinVoltageIn50mVunits;
    srcmaxpower250mw  = srcpdo.SRCBatteryPDO.MaxAllowablePowerIn250mWunits;

    /* Loop through SNK PDO list */
    for (i=0; i<nbsnkpdo; i++)
    {
      currentrequestedpower = 0;
      currentrequestedvoltage = 0;

      /* Retrieve SNK PDO value according to its type */
      snkpdo.d32 = snkpdo_array[i];
      switch(snkpdo.GenericPDO.PowerObject)
      {
        /* SNK Fixed Supply PDO */
      case USBPD_CORE_PDO_TYPE_FIXED:
        /* No match */
        break;

        /* SNK Variable Supply (non-battery) PDO */
      case USBPD_CORE_PDO_TYPE_VARIABLE:
        snkmaxvoltage50mv = snkpdo.SNKVariablePDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKVariablePDO.MinVoltageIn50mVunits;
        snkopcurrent10ma  = snkpdo.SNKVariablePDO.OperationalCurrentIn10mAunits;

        /* Match if :
        SNK Max voltage >= SRC Max Voltage
        &&
        SNK Min voltage <= SRC Min Voltage
        &&
        SNK Op current * SRC Max Voltage <= SRC Max Power

        Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage : SRC Max Voltage
        Requested Op Current : SNK Op Current
        Requested Max Current : SNK Op Current
        */
        if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
            &&(snkminvoltage50mv <= srcminvoltage50mv)
              &&(srcmaxvoltage50mv * snkopcurrent10ma <= srcmaxpower250mw))
        {
          currentrequestedpower = (srcmaxvoltage50mv * snkopcurrent10ma) / 2; /* to get value in mw */
          currentrequestedvoltage = srcmaxvoltage50mv;
        }
        break;

        /* SNK Battery Supply PDO */
      case USBPD_CORE_PDO_TYPE_BATTERY:
        snkmaxvoltage50mv = snkpdo.SNKBatteryPDO.MaxVoltageIn50mVunits;
        snkminvoltage50mv = snkpdo.SNKBatteryPDO.MinVoltageIn50mVunits;
        snkoppower250mw   = snkpdo.SNKBatteryPDO.OperationalPowerIn250mWunits;

        /* Match if :
        SNK Max voltage >= SRC Max Voltage
        &&
        SNK Min voltage <= SRC Min Voltage
        &&
        SNK Op power <= SRC Max power

        Requested Voltage : Any value between SRC Min Voltage and SRC Max Voltage, that fulfill
        SNK Op power <= Voltage * SRC Max Current
        Requested Op Current : SNK Op Power/ SRC Voltage
        Requested Max Current : SNK Op Power/ SRC Voltage
        */
        if (  (snkmaxvoltage50mv >= srcmaxvoltage50mv)
            &&(snkminvoltage50mv <= srcminvoltage50mv)
              &&(snkoppower250mw <= srcmaxpower250mw))
        {
          currentrequestedpower   = snkoppower250mw * 250; /* to get value in mw */
          currentrequestedvoltage = srcmaxvoltage50mv;
        }
        break;

        /* SNK Augmented Power Data Object (APDO) */
      case USBPD_CORE_PDO_TYPE_APDO:
        break;

      default:
        break;
      }

      if (currentrequestedpower > maxrequestedpower)
      {
        match = USBPD_TRUE;
        maxrequestedpower   = currentrequestedpower;
        maxrequestedvoltage = currentrequestedvoltage;
      }
    }
    break;

    /* Augmented Power Data Object (APDO) */
  case USBPD_CORE_PDO_TYPE_APDO:
    {
      uint16_t srcmaxvoltage100mv, srcmaxcurrent50ma;
      srcmaxvoltage100mv = srcpdo.SRCSNKAPDO.MaxVoltageIn100mV;
      srcmaxcurrent50ma = srcpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

      /* Loop through SNK PDO list */
      for (i=0; i<nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch(snkpdo.GenericPDO.PowerObject)
        {
        case USBPD_CORE_PDO_TYPE_FIXED:
        case USBPD_CORE_PDO_TYPE_VARIABLE:
        case USBPD_CORE_PDO_TYPE_BATTERY:
          /* No match */
          break;
          /* SNK Augmented Power Data Object (APDO) */
        case USBPD_CORE_PDO_TYPE_APDO:
          {
            uint16_t snkmaxvoltage100mv, snkminvoltage100mv, snkmaxcurrent50ma;

            snkminvoltage100mv = snkpdo.SRCSNKAPDO.MinVoltageIn100mV;
            snkmaxvoltage100mv = snkpdo.SRCSNKAPDO.MaxVoltageIn100mV;
            snkmaxcurrent50ma = snkpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

            /* Match if voltage matches with the APDO voltage range */
            if ((PWR_DECODE_100MV(snkminvoltage100mv) <= (*PtrRequestedVoltage))
                && ((*PtrRequestedVoltage) <= PWR_DECODE_100MV(snkmaxvoltage100mv))
                  && (snkmaxcurrent50ma <= srcmaxcurrent50ma))
            {
              if (0 != *PtrRequestedPower)
              {
                currentrequestedpower = (*PtrRequestedVoltage * PWR_DECODE_50MA(snkmaxcurrent50ma)) / 1000; /* to get value in mw */
                currentrequestedvoltage = (*PtrRequestedVoltage / 50);
              }
              else
              {
                *PtrRequestedVoltage = MIN(PWR_DECODE_100MV(srcmaxvoltage100mv), PWR_DECODE_100MV(snkmaxvoltage100mv));
                currentrequestedpower = (*PtrRequestedVoltage * PWR_DECODE_50MA(snkmaxcurrent50ma)) / 1000; /* to get value in mw */
                currentrequestedvoltage = (*PtrRequestedVoltage / 50);
              }
            }
          }
          break;

        default:
          break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
    }
    break;

  default:
    return(USBPD_FALSE);
  }

  if (maxrequestedpower > 0)
  {
    *PtrRequestedPower   = maxrequestedpower;
    *PtrRequestedVoltage = maxrequestedvoltage * 50; /* value in mV */
  }
  return(match);
}

/**
* @}
*/

/** @addtogroup USBPD_USER_PRIVATE_FUNCTIONS
* @{
*/

/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS */

/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS */

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/


