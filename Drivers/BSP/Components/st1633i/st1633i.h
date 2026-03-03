/**
  ******************************************************************************
  * @file    st1633i.h
  * @author  MCD Application Team
  * @brief   This file contains all the constants parameters for the ST1633I-CTG
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST1633I_H
#define __ST1633I_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "st1633i_reg.h"
#include <stddef.h>
#include "st1633i_conf.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup ST1633I
  * @{
  */

/** @defgroup ST1633I_Exported_Constants
  * @{
  */
#define ST1633I_OK      (0)
#define ST1633I_ERROR   (-1)

/**
  * @brief  ST1633I Size
  */
#define  ST1633I_WIDTH    ((uint16_t)480)          /* LCD PIXEL WIDTH            */
#define  ST1633I_HEIGHT   ((uint16_t)272)          /* LCD PIXEL HEIGHT           */

/**
  * @brief  ST1633I Timing
  */
#define  ST1633I_HSYNC            ((uint16_t)4)   /* Horizontal synchronization */
#define  ST1633I_HBP              ((uint16_t)43)  /* Horizontal back porch      */
#define  ST1633I_HFP              ((uint16_t)0)   /* Horizontal front porch     */
#define  ST1633I_VSYNC            ((uint16_t)4)   /* Vertical synchronization   */
#define  ST1633I_VBP              ((uint16_t)7)   /* Vertical back porch        */
#define  ST1633I_VFP              ((uint16_t)8)   /* Vertical front porch       */

/* Max detectable simultaneous touches */
#define ST1633I_MAX_NB_TOUCH             5U

/* Possible values of ST1633I_GEST_ID_REG */
#define ST1633I_GEST_ID_NO_GESTURE        0x00U
#define ST1633I_GEST_ID_DOUBLE_TAP        0x01U
#define ST1633I_GEST_ID_ZOOM_IN           0x02U
#define ST1633I_GEST_ID_ZOOM_OUT          0x03U
#define ST1633I_GEST_ID_MOVE_RIGHT        0x04U
#define ST1633I_GEST_ID_MOVE_LEFT         0x05U
#define ST1633I_GEST_ID_MOVE_DOWN         0x06U
#define ST1633I_GEST_ID_MOVE_UP           0x07U

/**
  * @}
  */

/** @defgroup ST1633I_Exported_Types ST1633I Exported Types
 * @{
 */
typedef struct
{
  uint32_t  Radian;
  uint32_t  OffsetLeftRight;
  uint32_t  OffsetUpDown;
  uint32_t  DistanceLeftRight;
  uint32_t  DistanceUpDown;
  uint32_t  DistanceZoom;
}ST1633I_Gesture_Init_t;

typedef int32_t (*ST1633I_Init_Func)    (void);
typedef int32_t (*ST1633I_DeInit_Func)  (void);
typedef int32_t (*ST1633I_GetTick_Func) (void);
#if (ST1633I_AUTO_CALIBRATION_ENABLED == 1)
typedef int32_t (*ST1633I_Delay_Func)   (uint32_t);
#endif /* ST1633I_AUTO_CALIBRATION_ENABLED == 1 */
typedef int32_t (*ST1633I_WriteReg_Func)(uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*ST1633I_ReadReg_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  ST1633I_Init_Func          Init;
  ST1633I_DeInit_Func        DeInit;
  uint16_t                   Address;
  ST1633I_WriteReg_Func      WriteReg;
  ST1633I_ReadReg_Func       ReadReg;
  ST1633I_GetTick_Func       GetTick;
} ST1633I_IO_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX;
  uint32_t  TouchY;
} ST1633I_State_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX[ST1633I_MAX_NB_TOUCH];
  uint32_t  TouchY[ST1633I_MAX_NB_TOUCH];
} ST1633I_MultiTouch_State_t;

typedef struct
{
  ST1633I_IO_t         IO;
  st1633i_ctx_t        Ctx;
  uint8_t             IsInitialized;
} ST1633I_Object_t;

typedef struct
{
  uint8_t   MultiTouch;
  uint8_t   Gesture;
  uint8_t   MaxTouch;
  uint32_t  MaxXl;
  uint32_t  MaxYl;
} ST1633I_Capabilities_t;

 typedef struct
{
  int32_t ( *Init                 ) ( ST1633I_Object_t *);
  int32_t ( *DeInit               ) ( ST1633I_Object_t * );
  int32_t ( *GestureConfig        ) ( ST1633I_Object_t *, ST1633I_Gesture_Init_t* );
  int32_t ( *ReadID               ) ( ST1633I_Object_t *, uint32_t * );
  int32_t ( *GetState             ) ( ST1633I_Object_t *, ST1633I_State_t* );
  int32_t ( *GetMultiTouchState   ) ( ST1633I_Object_t *, ST1633I_MultiTouch_State_t* );
  int32_t ( *GetGesture           ) ( ST1633I_Object_t *, uint8_t* );
  int32_t ( *GetCapabilities      ) ( const ST1633I_Object_t *, ST1633I_Capabilities_t * );
  int32_t ( *EnableIT             ) ( ST1633I_Object_t * );
  int32_t ( *DisableIT            ) ( ST1633I_Object_t * );
  int32_t ( *ClearIT              ) ( const ST1633I_Object_t * );
  int32_t ( *ITStatus             ) ( const ST1633I_Object_t * );
} ST1633I_TS_Drv_t;
/**
 * @}
 */

/** @addtogroup ST1633I_Exported_Variables
  * @{
  */
extern ST1633I_TS_Drv_t ST1633I_TS_Driver;


/** @defgroup ST1633I_Exported_Functions
  * @{
  */
int32_t ST1633I_RegisterBusIO (ST1633I_Object_t *pObj, ST1633I_IO_t *pIO);
int32_t ST1633I_Init(ST1633I_Object_t *pObj);
int32_t ST1633I_DeInit(ST1633I_Object_t *pObj);
int32_t ST1633I_GestureConfig(ST1633I_Object_t *pObj, ST1633I_Gesture_Init_t *GestureInit);
int32_t ST1633I_ReadID(ST1633I_Object_t *pObj, uint32_t *Id);
int32_t ST1633I_GetState(ST1633I_Object_t *pObj, ST1633I_State_t *State);
int32_t ST1633I_GetMultiTouchState(ST1633I_Object_t *pObj, ST1633I_MultiTouch_State_t *State);
int32_t ST1633I_GetGesture(ST1633I_Object_t *pObj, uint8_t *GestureId);
int32_t ST1633I_EnableIT(ST1633I_Object_t *pObj);
int32_t ST1633I_DisableIT(ST1633I_Object_t *pObj);
int32_t ST1633I_ITStatus(const ST1633I_Object_t *pObj);
int32_t ST1633I_ClearIT(const ST1633I_Object_t *pObj);
int32_t ST1633I_GetCapabilities(const ST1633I_Object_t *pObj, ST1633I_Capabilities_t *Capabilities);
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

#ifdef __cplusplus
}
#endif

#endif /* __ST1633I_H */
