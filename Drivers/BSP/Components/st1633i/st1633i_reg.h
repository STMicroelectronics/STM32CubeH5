/**
  ******************************************************************************
  * @file    st1633i_reg.h
  * @author  MCD Application Team
  * @brief   Header of st1633i_reg.c
  *
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
#ifndef ST1633I_REG_H
#define ST1633I_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Macros --------------------------------------------------------------------*/

/** @addtogroup BSP
 * @{
 */

/** @addtogroup Component
 * @{
 */

/** @addtogroup ST1633I
 * @{
 */

/* Exported types ------------------------------------------------------------*/

/** @defgroup ST1633I_Exported_Types ST1633I Exported Types
 * @{
 */
/************** Generic Function  *******************/

typedef int32_t (*ST1633I_Write_Func)(const void *, uint8_t, uint8_t*, uint16_t);
typedef int32_t (*ST1633I_Read_Func) (const void *, uint8_t, uint8_t*, uint16_t);

/**
 * @}
 */

/** @defgroup ST1633I_Imported_Globals ST1633I Imported Globals
 * @{
 */
typedef struct
{
  ST1633I_Write_Func   WriteReg;
  ST1633I_Read_Func    ReadReg;
  void                 *handle;
} st1633i_ctx_t;
/**
 * @}
 */

/** @defgroup ST1633I_Exported_Constants ST1633I Exported Constants
 * @{
 */

/* ST1633I firmware version */
#define ST1633I_FIRMID_REG            0x00U

/* Current operating mode the ST1633I system is in (R) */
#define ST1633I_STATUS_REG            0x01U

/* ST1633I error register (R) */
#define ST1633I_ERROR_REG             0x01U

/* Control register */
#define ST1633I_CTRL_REG              0x02U

/* The time period of switching from Active mode to Monitor mode when there is no touching */
#define ST1633I_TIMEOUTTOIDLE_REG     0x03U

/* X and Y resolution registers (R) */
#define ST1633I_XY_RES_H_REG          0x04U
#define ST1633I_X_RES_L_REG           0x05U
#define ST1633I_Y_RES_L_REG           0x06U

/* Sensing counter registers (R) */
#define ST1633I_SENSING_COUNTER_H_REG 0x07U
#define ST1633I_SENSING_COUNTER_L_REG 0x08U

/* FW revision registers (R) */
#define ST1633I_FW_REV3_REG           0x0CU
#define ST1633I_FW_REV2_REG           0x0DU
#define ST1633I_FW_REV1_REG           0x0EU
#define ST1633I_FW_REV0_REG           0x0FU

/* Advance touch information register (R) */
#define ST1633I_ADV_TOUCH_REG         0x10U
#define ST1633I_GEST_ID_REG           0x10U

/* P0 X, Y coordinates (R) */
#define ST1633I_X0_H_REG              0x12U
#define ST1633I_Y0_H_REG              0x12U
#define ST1633I_X0_L_REG              0x13U
#define ST1633I_Y0_L_REG              0x14U

/* P1 X, Y coordinates (R) */
#define ST1633I_X1_H_REG              0x16U
#define ST1633I_Y1_H_REG              0x16U
#define ST1633I_X1_L_REG              0x17U
#define ST1633I_Y1_L_REG              0x18U

/* P2 X, Y coordinates (R) */
#define ST1633I_X2_H_REG              0x1AU
#define ST1633I_Y2_H_REG              0x1AU
#define ST1633I_X2_L_REG              0x1BU
#define ST1633I_Y2_L_REG              0x1CU

/* P3 X, Y coordinates (R) */
#define ST1633I_X3_H_REG              0x1EU
#define ST1633I_Y3_H_REG              0x1EU
#define ST1633I_X3_L_REG              0x1FU
#define ST1633I_Y3_L_REG              0x20U

/* P4 X, Y coordinates (R) */
#define ST1633I_X4_H_REG              0x22U
#define ST1633I_Y4_H_REG              0x22U
#define ST1633I_X4_L_REG              0x23U
#define ST1633I_Y4_L_REG              0x24U

/* Maximum contact number supported register (R) */
#define ST1633I_NB_MAX_TOUCH_REG      0x3FU

/* Miscellaneous information register (R) */
#define ST1633I_MISC_INFO_REG         0xF0U

/* Miscellaneous control register */
#define ST1633I_MISC_CTRL_REG         0xF1U

/* Smart wake up ID register (R) */
#define ST1633I_SMARTWAKEUP_REG       0xF2U

/**
 * @}
 */

/*******************************************************************************
* Register      : Generic - All
* Address       : Generic - All
* Bit Group Name: None
* Permission    : W
*******************************************************************************/
int32_t st1633i_write_reg(const st1633i_ctx_t *ctx, uint8_t reg, uint8_t *pdata, uint16_t length);
int32_t st1633i_read_reg(const st1633i_ctx_t *ctx, uint8_t reg, uint8_t *pdata, uint16_t length);

/**************** Base Function  *******************/
/*******************************************************************************
* Register      : GEST_ID
* Address       : 0X10
* Bit Group Name: Gesture ID
* Permission    : R
*******************************************************************************/
#define   ST1633I_GEST_ID_BIT_MASK        0x0FU
#define   ST1633I_GEST_ID_BIT_POSITION    0U
int32_t st1633i_gest_id(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : TD_STATUS
* Address       : 0X12
* Bit Group Name:
* Permission    : R
*******************************************************************************/
#define   ST1633I_TD_STATUS_BIT_MASK        0x80U
#define   ST1633I_TD_STATUS_BIT_POSITION    7U
int32_t st1633i_td_status(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X0_H
* Address       : 0X12
* Bit Group Name: First Touch X Position
* Permission    : R
* Default value : 0x70U
*******************************************************************************/
#define   ST1633I_X0_H_BIT_MASK        0x70U
#define   ST1633I_X0_H_BIT_POSITION    4U
int32_t st1633i_x0_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X0_L
* Address       : 0X13
* Bit Group Name: First Touch X Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_X0_L_BIT_MASK        0xFFU
#define   ST1633I_X0_L_BIT_POSITION    0U
int32_t st1633i_x0_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y0_H
* Address       : 0X12
* Bit Group Name: First Touch Y Position
* Permission    : R
* Default value : 0x07U
*******************************************************************************/
#define   ST1633I_Y0_H_BIT_MASK        0x07U
#define   ST1633I_Y0_H_BIT_POSITION    0U
int32_t st1633i_y0_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y0_L
* Address       : 0X14
* Bit Group Name: First Touch Y Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_Y0_L_BIT_MASK        0xFFU
#define   ST1633I_Y0_L_BIT_POSITION    0U
int32_t st1633i_y0_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X1_H
* Address       : 0X16
* Bit Group Name: Second Touch X Position
* Permission    : R
* Default value : 0x70U
*******************************************************************************/
#define   ST1633I_X1_H_BIT_MASK        0x70U
#define   ST1633I_X1_H_BIT_POSITION    4U
int32_t st1633i_x1_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X1_L
* Address       : 0X17
* Bit Group Name: Second Touch X Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_X1_L_BIT_MASK        0xFFU
#define   ST1633I_X1_L_BIT_POSITION    0U
int32_t st1633i_x1_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y1_H
* Address       : 0X16
* Bit Group Name: Second Touch Y Position
* Permission    : R
* Default value : 0x07U
*******************************************************************************/
#define   ST1633I_Y1_H_BIT_MASK        0x07U
#define   ST1633I_Y1_H_BIT_POSITION    0U
int32_t st1633i_y1_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y1_L
* Address       : 0X18
* Bit Group Name: Second Touch Y Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_Y1_L_BIT_MASK        0xFFU
#define   ST1633I_Y1_L_BIT_POSITION    0U
int32_t st1633i_y1_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X2_H
* Address       : 0X1A
* Bit Group Name: Third Touch X Position
* Permission    : R
* Default value : 0x70U
*******************************************************************************/
#define   ST1633I_X2_H_BIT_MASK        0x70U
#define   ST1633I_X2_H_BIT_POSITION    4U
int32_t st1633i_x2_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X2_L
* Address       : 0X1B
* Bit Group Name: Third Touch X Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_X2_L_BIT_MASK        0xFFU
#define   ST1633I_X2_L_BIT_POSITION    0U
int32_t st1633i_x2_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y2_H
* Address       : 0X1A
* Bit Group Name: Third Touch Y Position
* Permission    : R
* Default value : 0x07U
*******************************************************************************/
#define   ST1633I_Y2_H_BIT_MASK        0x07U
#define   ST1633I_Y2_H_BIT_POSITION    0U
int32_t st1633i_y2_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y2_L
* Address       : 0X1C
* Bit Group Name: Third Touch Y Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_Y2_L_BIT_MASK        0xFFU
#define   ST1633I_Y2_L_BIT_POSITION    0U
int32_t st1633i_y2_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X3_H
* Address       : 0X1E
* Bit Group Name: Fourth Touch X Position
* Permission    : R
* Default value : 0x70U
*******************************************************************************/
#define   ST1633I_X3_H_BIT_MASK        0x70U
#define   ST1633I_X3_H_BIT_POSITION    4U
int32_t st1633i_x3_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X3_L
* Address       : 0X1F
* Bit Group Name: Fourth Touch X Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_X3_L_BIT_MASK        0xFFU
#define   ST1633I_X3_L_BIT_POSITION    0U
int32_t st1633i_x3_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y3_H
* Address       : 0X1E
* Bit Group Name: Fourth Touch Y Position
* Permission    : R
* Default value : 0x07U
*******************************************************************************/
#define   ST1633I_Y3_H_BIT_MASK        0x07U
#define   ST1633I_Y3_H_BIT_POSITION    0U
int32_t st1633i_y3_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y3_L
* Address       : 0X20
* Bit Group Name: Fourth Touch Y Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_Y3_L_BIT_MASK        0xFFU
#define   ST1633I_Y3_L_BIT_POSITION    0U
int32_t st1633i_y3_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X4_H
* Address       : 0X22
* Bit Group Name: Fifth Touch X Position
* Permission    : R
* Default value : 0x70U
*******************************************************************************/
#define   ST1633I_X4_H_BIT_MASK        0x70U
#define   ST1633I_X4_H_BIT_POSITION    4U
int32_t st1633i_x4_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : X4_L
* Address       : 0X23
* Bit Group Name: Fifth Touch X Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_X4_L_BIT_MASK        0xFFU
#define   ST1633I_X4_L_BIT_POSITION    0U
int32_t st1633i_x4_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y4_H
* Address       : 0X22
* Bit Group Name: Fifth Touch Y Position
* Permission    : R
* Default value : 0x07U
*******************************************************************************/
#define   ST1633I_Y4_H_BIT_MASK        0x07U
#define   ST1633I_Y4_H_BIT_POSITION    0U
int32_t st1633i_y4_h_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : Y4_L
* Address       : 0X24
* Bit Group Name: Fifth Touch Y Position
* Permission    : R
* Default value : 0xFFU
*******************************************************************************/
#define   ST1633I_Y4_L_BIT_MASK        0xFFU
#define   ST1633I_Y4_L_BIT_POSITION    0U
int32_t st1633i_y4_l_tp(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : CTRL
* Address       : 0X02
* Bit Group Name:
* Permission    : RW
*******************************************************************************/
#define   ST1633I_CTRL_BIT_MASK           0xFFU
#define   ST1633I_CTRL_BIT_POSITION       0U
int32_t st1633i_ctrl(const st1633i_ctx_t *ctx, uint8_t value);

/*******************************************************************************
* Register      : TIMEOUT TO IDLE
* Address       : 0X03
* Bit Group Name:
* Permission    : RW
* Default value : 0x00
*******************************************************************************/
#define   ST1633I_TIMEOUTTOIDLE_BIT_MASK           0x02U
#define   ST1633I_TIMEOUTTOIDLE_BIT_POSITION       0U
int32_t st1633i_time_enter_monitor(const st1633i_ctx_t *ctx, uint8_t value);

/*******************************************************************************
* Register      : PWR_MODE
* Address       : 0X02
* Bit Group Name:
* Permission    : RW
* Default value : 0x00
*******************************************************************************/
#define   ST1633I_PWR_MODE_BIT_MASK           0x02U
#define   ST1633I_PWR_MODE_BIT_POSITION       1U
int32_t st1633i_pwr_mode(const st1633i_ctx_t *ctx, uint8_t value);

/*******************************************************************************
* Register      : DEEP_PWR_MODE
* Address       : 0X02
* Bit Group Name:
* Permission    : RW
* Default value : 0x00
*******************************************************************************/
#define   ST1633I_DEEP_PWR_MODE_BIT_MASK           0x04U
#define   ST1633I_DEEP_PWR_MODE_BIT_POSITION       2U
int32_t st1633i_deep_pwr_mode(const st1633i_ctx_t *ctx, uint8_t value);

/*******************************************************************************
* Register      : FIRMID
* Address       : 0X00
* Bit Group Name:
* Permission    : R
* Default value : None
*******************************************************************************/
#define   ST1633I_FIRMID_BIT_MASK           0xFFU
#define   ST1633I_FIRMID_BIT_POSITION       0U
int32_t st1633i_firm_id(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : STATE
* Address       : 0X01
* Bit Group Name:
* Permission    : RW
* Default value : 0x01
*******************************************************************************/
#define   ST1633I_STATE_BIT_MASK           0x0FU
#define   ST1633I_STATE_BIT_POSITION       0U
int32_t st1633i_state(const st1633i_ctx_t *ctx, uint8_t *value);

/*******************************************************************************
* Register      : ERROR
* Address       : 0X01
* Bit Group Name:
* Permission    : RW
* Default value : 0x00
*******************************************************************************/
#define   ST1633I_ERROR_BIT_MASK           0xF0U
#define   ST1633I_ERROR_BIT_POSITION       4U
int32_t st1633i_error(const st1633i_ctx_t *ctx, uint8_t *value);

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

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
#endif /* ST1633I_REG_H */
