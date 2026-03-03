/**
  ******************************************************************************
  * @file    st1633i.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the ST1633I
  *          component.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "st1633i.h"
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup ST1633I ST1633I
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/** @defgroup ST1633I_Exported_Variables ST1633I Exported Variables
  * @{
  */

/* Touch screen driver structure initialization */
ST1633I_TS_Drv_t ST1633I_TS_Driver =
{
  ST1633I_Init,
  ST1633I_DeInit,
  ST1633I_GestureConfig,
  ST1633I_ReadID,
  ST1633I_GetState,
  ST1633I_GetMultiTouchState,
  ST1633I_GetGesture,
  ST1633I_GetCapabilities,
  ST1633I_EnableIT,
  ST1633I_DisableIT,
  ST1633I_ClearIT,
  ST1633I_ITStatus
};
/**
  * @}
  */

/** @defgroup ST1633I_Private_Function_Prototypes ST1633I Private Function Prototypes
  * @{
  */
#if (ST1633I_AUTO_CALIBRATION_ENABLED == 1)
static int32_t ST1633I_TS_Calibration(ST1633I_Object_t *pObj);
static int32_t ST1633I_Delay(ST1633I_Object_t *pObj, uint32_t Delay);
#endif /* ST1633I_AUTO_CALIBRATION_ENABLED == 1 */
static int32_t ST1633I_DetectTouch(ST1633I_Object_t *pObj);
static int32_t ReadRegWrap(const void *handle, uint8_t Reg, uint8_t* Data, uint16_t Length);
static int32_t WriteRegWrap(const void *handle, uint8_t Reg, uint8_t* Data, uint16_t Length);

/**
  * @}
  */

/** @defgroup ST1633I_Exported_Functions ST1633I Exported Functions
  * @{
  */

/**
  * @brief  Register IO bus to component object
  * @param  Component object pointer
  * @retval error status
  */
int32_t ST1633I_RegisterBusIO (ST1633I_Object_t *pObj, ST1633I_IO_t *pIO)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = ST1633I_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.ReadReg  = ReadRegWrap;
    pObj->Ctx.WriteReg = WriteRegWrap;
    pObj->Ctx.handle   = pObj;

    if(pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = ST1633I_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  Get ST1633I sensor capabilities
  * @param  pObj Component object pointer
  * @param  Capabilities pointer to ST1633I sensor capabilities
  * @retval Component status
  */
int32_t ST1633I_GetCapabilities(const ST1633I_Object_t *pObj, ST1633I_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Store component's capabilities */
  Capabilities->MultiTouch = 1;
  Capabilities->Gesture    = 0;  /* Gesture feature is currently not activated on FW chipset */
  Capabilities->MaxTouch   = ST1633I_MAX_NB_TOUCH;
  Capabilities->MaxXl      = ST1633I_MAX_X_LENGTH;
  Capabilities->MaxYl      = ST1633I_MAX_Y_LENGTH;

  return ST1633I_OK;
}

/**
  * @brief  Initialize the ST1633I communication bus
  *         from MCU to ST1633I : ie I2C channel initialization (if required).
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_Init(ST1633I_Object_t *pObj)
{
  int32_t ret = ST1633I_OK;

  if(pObj->IsInitialized == 0U)
  {
    /* Initialize IO BUS layer */
    pObj->IO.Init();

    /* Note TS_INT is active low */
    ret += ST1633I_DisableIT(pObj);

    pObj->IsInitialized = 1;
  }

  if(ret != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }

  return ret;
}

/**
  * @brief  De-Initialize the ST1633I communication bus
  *         from MCU to ST1633I : ie I2C channel initialization (if required).
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_DeInit(ST1633I_Object_t *pObj)
{
  if(pObj->IsInitialized == 1U)
  {
    pObj->IsInitialized = 0;
  }

  return ST1633I_OK;
}

/**
  * @brief  Configure the ST1633I gesture
  *         from MCU to ST1633I : ie I2C channel initialization (if required).
  * @param  pObj  Component object pointer
  * @param  GestureInit Gesture init structure
  * @retval Component status
  */
int32_t ST1633I_GestureConfig(ST1633I_Object_t *pObj, ST1633I_Gesture_Init_t *GestureInit)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @brief  Read the ST1633I device ID, pre initialize I2C in case of need to be
  *         able to read the ST1633I device ID, and verify this is a ST1633I.
  * @param  pObj Component object pointer
  * @param  Id Pointer to component's ID
  * @retval Component status
  */
int32_t ST1633I_ReadID(ST1633I_Object_t *pObj, uint32_t *Id)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @brief  Get the touch screen X and Y positions values
  * @param  pObj Component object pointer
  * @param  State Single Touch structure pointer
  * @retval Component status.
  */
int32_t ST1633I_GetState(ST1633I_Object_t *pObj, ST1633I_State_t *State)
{
  int32_t ret = ST1633I_OK;
  uint8_t  data[3] = {0};

  State->TouchDetected = (uint32_t)ST1633I_DetectTouch(pObj);
  if(st1633i_read_reg(&pObj->Ctx, ST1633I_X0_H_REG, data, (uint16_t)sizeof(data)) != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }
  else
  {
    /* Send back first ready X position to caller */
    State->TouchX = (((uint32_t)data[0] & ST1633I_X0_H_BIT_MASK) << 4) | ((uint32_t)data[1] & ST1633I_X0_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY = (((uint32_t)data[0] & ST1633I_Y0_H_BIT_MASK) << 8) | ((uint32_t)data[2] & ST1633I_Y0_L_BIT_MASK);
  }

  return ret;
}

/**
  * @brief  Get the touch screen Xn and Yn positions values in multi-touch mode
  * @param  pObj Component object pointer
  * @param  State Multi Touch structure pointer
  * @retval Component status.
  */
int32_t ST1633I_GetMultiTouchState(ST1633I_Object_t *pObj, ST1633I_MultiTouch_State_t *State)
{
  int32_t ret = ST1633I_OK;
  uint8_t  data[19] = {0};

  State->TouchDetected = (uint32_t)ST1633I_DetectTouch(pObj);

  if(st1633i_read_reg(&pObj->Ctx, ST1633I_X0_H_REG, data, (uint16_t)sizeof(data)) != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }
  else
  {
    /* Send back first ready X position to caller */
    State->TouchX[0] = (((uint32_t)data[0] & ST1633I_X0_H_BIT_MASK) << 4) | ((uint32_t)data[1] & ST1633I_X0_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY[0] = (((uint32_t)data[0] & ST1633I_Y0_H_BIT_MASK) << 8) | ((uint32_t)data[2] & ST1633I_Y0_L_BIT_MASK);

    /* Send back first ready X position to caller */
    State->TouchX[1] = (((uint32_t)data[4] & ST1633I_X1_H_BIT_MASK) << 4) | ((uint32_t)data[5] & ST1633I_X1_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY[1] = (((uint32_t)data[4] & ST1633I_Y1_H_BIT_MASK) << 8) | ((uint32_t)data[6] & ST1633I_Y1_L_BIT_MASK);

    /* Send back first ready X position to caller */
    State->TouchX[2] = (((uint32_t)data[8] & ST1633I_X2_H_BIT_MASK) << 4) | ((uint32_t)data[9] & ST1633I_X2_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY[2] = (((uint32_t)data[8] & ST1633I_Y2_H_BIT_MASK) << 8) | ((uint32_t)data[10] & ST1633I_Y2_L_BIT_MASK);

    /* Send back first ready X position to caller */
    State->TouchX[3] = (((uint32_t)data[12] & ST1633I_X3_H_BIT_MASK) << 4) | ((uint32_t)data[13] & ST1633I_X3_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY[3] = (((uint32_t)data[12] & ST1633I_Y3_H_BIT_MASK) << 8) | ((uint32_t)data[14] & ST1633I_Y3_L_BIT_MASK);

    /* Send back first ready X position to caller */
    State->TouchX[4] = (((uint32_t)data[16] & ST1633I_X4_H_BIT_MASK) << 4) | ((uint32_t)data[17] & ST1633I_X4_L_BIT_MASK);
    /* Send back first ready Y position to caller */
    State->TouchY[4] = (((uint32_t)data[16] & ST1633I_Y4_H_BIT_MASK) << 8) | ((uint32_t)data[18] & ST1633I_Y4_L_BIT_MASK);
  }

  return ret;
}

/**
  * @brief  Get Gesture ID
  * @param  pObj Component object pointer
  * @param  GestureId gesture ID
  * @retval Component status
  */
int32_t ST1633I_GetGesture(ST1633I_Object_t *pObj, uint8_t *GestureId)
{
  return st1633i_gest_id(&pObj->Ctx, GestureId);
}

/**
  * @brief  Configure the ST1633I device to generate IT on given INT pin
  *         connected to MCU as EXTI.
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_EnableIT(ST1633I_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @brief  Configure the ST1633I device to stop generating IT on the given INT pin
  *         connected to MCU as EXTI.
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_DisableIT(ST1633I_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @brief  Get IT status from ST1633I interrupt status registers
  *         Should be called Following an EXTI coming to the MCU to know the detailed
  *         reason of the interrupt.
  *         @note : This feature is not supported by ST1633I.
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_ITStatus(const ST1633I_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @brief  Clear IT status in ST1633I interrupt status clear registers
  *         Should be called Following an EXTI coming to the MCU.
  *         @note : This feature is not supported by ST1633I.
  * @param  pObj Component object pointer
  * @retval Component status
  */
int32_t ST1633I_ClearIT(const ST1633I_Object_t *pObj)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  /* Always return ST1633I_OK as feature not supported by ST1633I */
  return ST1633I_OK;
}

/**
  * @}
  */

/** @defgroup ST1633I_Private_Functions ST1633I Private Functions
  * @{
  */
#if (ST1633I_AUTO_CALIBRATION_ENABLED == 1)
/**
  * @brief This function provides accurate delay (in milliseconds)
  * @param pObj pointer to component object
  * @param Delay specifies the delay time length, in milliseconds
  * @retval Component status
  */
static int32_t ST1633I_Delay(ST1633I_Object_t *pObj, uint32_t Delay)
{
  uint32_t tickstart;
  tickstart = pObj->IO.GetTick();
  while((pObj->IO.GetTick() - tickstart) < Delay)
  {
  }
  return ST1633I_OK;
}

/**
  * @brief  Start TouchScreen calibration phase
  * @param pObj pointer to component object
  * @retval Component status
  */
static int32_t ST1633I_TS_Calibration(ST1633I_Object_t *pObj)
{
  int32_t ret = ST1633I_OK;
  uint32_t nbr_attempt;
  uint8_t read_data;
  uint8_t end_calibration = 0;

  /* Switch ST1633I back to factory mode to calibrate */
  if(st1633i_dev_mode_w(&pObj->Ctx, ST1633I_DEV_MODE_FACTORY) != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }/* Read back the same register ST1633I_DEV_MODE_REG */
  else if(st1633i_dev_mode_r(&pObj->Ctx, &read_data) != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }
  else
  {
    (void)ST1633I_Delay(pObj, 300); /* Wait 300 ms */

    if(read_data != ST1633I_DEV_MODE_FACTORY )
    {
      /* Return error to caller */
      ret = ST1633I_ERROR;
    }
    else
    {
      /* Start calibration command */
      read_data= 0x04;
      if(st1633i_write_reg(&pObj->Ctx, ST1633I_TD_STAT_REG, &read_data, 1) != ST1633I_OK)
      {
        ret = ST1633I_ERROR;
      }
      else
      {
        (void)ST1633I_Delay(pObj, 300); /* Wait 300 ms */

        /* 100 attempts to wait switch from factory mode (calibration) to working mode */
        for (nbr_attempt=0; ((nbr_attempt < 100U) && (end_calibration == 0U)) ; nbr_attempt++)
        {
          if(st1633i_dev_mode_r(&pObj->Ctx, &read_data) != ST1633I_OK)
          {
            ret = ST1633I_ERROR;
            break;
          }
          if(read_data == ST1633I_DEV_MODE_WORKING)
          {
            /* Auto Switch to ST1633I_DEV_MODE_WORKING : means calibration have ended */
            end_calibration = 1; /* exit for loop */
          }

          (void)ST1633I_Delay(pObj, 200); /* Wait 200 ms */
        }
      }
    }
  }

  return ret;
}
#endif /* ST1633I_AUTO_CALIBRATION_ENABLED == 1 */

/**
  * @brief  Return if there is touches detected or not.
  *         Try to detect new touches and forget the old ones (reset internal global
  *         variables).
  * @param  pObj Component object pointer
  * @retval Number of active touches detected (can be 0, 1 or 2) or ST1633I_ERROR
  *         in case of error
  */
static int32_t ST1633I_DetectTouch(ST1633I_Object_t *pObj)
{
  int32_t ret;
  uint8_t nb_touch = 0;

  /* Read register ST1633I_TD_STAT_REG to check number of touches detection */
  if(st1633i_td_status(&pObj->Ctx, &nb_touch) != ST1633I_OK)
  {
    ret = ST1633I_ERROR;
  }
  else
  {
    if(nb_touch > ST1633I_MAX_NB_TOUCH)
    {
      /* If invalid number of touch detected, set it to zero */
      ret = 0;
    }
    else
    {
      ret = (int32_t)nb_touch;
    }
  }
  return ret;
}

/**
  * @brief  Wrap IO bus read function to component register red function
  * @param  handle Component object handle
  * @param  Reg The target register address to read
  * @param  pData The target register value to be read
  * @param  Length buffer size to be read
  * @retval Component status.
  */
static int32_t ReadRegWrap(const void *handle, uint8_t Reg, uint8_t* pData, uint16_t Length)
{
  const ST1633I_Object_t *pObj = (const ST1633I_Object_t *)handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Wrap IO bus write function to component register write function
  * @param  handle Component object handle
  * @param  Reg The target register address to write
  * @param  pData The target register value to be written
  * @param  Length buffer size to be written
  * @retval Component status.
  */
static int32_t WriteRegWrap(const void *handle, uint8_t Reg, uint8_t* pData, uint16_t Length)
{
  const ST1633I_Object_t *pObj = (const ST1633I_Object_t *)handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

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
