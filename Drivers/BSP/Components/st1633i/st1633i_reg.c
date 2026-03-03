/**
  ******************************************************************************
  * @file    st1633i_reg.c
  * @author  MCD Application Team
  * @brief   This file provides unitary register function to control the ST1633i Touch
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

/* Includes ------------------------------------------------------------------*/
#include "st1633i_reg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @defgroup ST1633I
  * @{
  */

/*******************************************************************************
* Function Name : st1633i_read_reg
* Description   : Generic Reading function. It must be full-filled with either
*                 I2C or SPI reading functions
* Input         : Register Address, length of buffer
* Output        : pdata Read
*******************************************************************************/
int32_t st1633i_read_reg(const st1633i_ctx_t *ctx, uint8_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->ReadReg(ctx->handle, reg, pdata, length);
}

/*******************************************************************************
* Function Name : st1633i_write_reg
* Description   : Generic Writing function. It must be full-filled with either
*                 I2C or SPI writing function
* Input         : Register Address, pdata to be written, length of buffer
* Output        : None
*******************************************************************************/
int32_t st1633i_write_reg(const st1633i_ctx_t *ctx, uint8_t reg, uint8_t *pdata, uint16_t length)
{
  return ctx->WriteReg(ctx->handle, reg, pdata, length);
}

/**************** Base Function  *******************/
/*******************************************************************************
* Function Name  : st1633i_gest_id
* Description    : Read Gesture ID
* Input          : Pointer to uint8_t
* Output         : Status of GEST_ID register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_gest_id(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_GEST_ID_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_td_status
* Description    : Read Number of Touch Points
* Input          : Pointer to uint8_t
* Output         : Status of TD_STATUS register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_td_status(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;
  uint8_t data[19] = {0};

  ret = st1633i_read_reg(ctx, ST1633I_X0_H_REG, (uint8_t *)&data, 19);

  if (ret == 0)
  {
    if (((data[0] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK))  && \
        ((data[4] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK))  && \
        ((data[8] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK))  && \
        ((data[12] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
        ((data[16] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)))
    {
      *value = 5;
    }
    else if (((data[0] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[4] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[8] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[12] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)))
    {
      *value = 4;
    }
    else if (((data[0] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[4] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[8] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)))
    {
      *value = 3;
    }
    else if (((data[0] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)) && \
             ((data[4] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)))
    {
      *value = 2;
    }
    else if (((data[0] & ST1633I_TD_STATUS_BIT_MASK) == (ST1633I_TD_STATUS_BIT_MASK)))
    {
      *value = 1;
    }
    else
    {
      *value = 0;
    }
  }

  return ret;

}

/*******************************************************************************
* Function Name  : st1633i_x0_h_tp
* Description    : Read First X High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X0_H_REG[6:4] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x0_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_X0_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_X0_H_BIT_MASK;
    *value = *value >> ST1633I_X0_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_x0_l_tp
* Description    : Read First X Low Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X0_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x0_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_X0_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_y0_h_tp
* Description    : Read First Touch ID
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y0_H_REG[2:0] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y0_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_Y0_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_Y0_H_BIT_MASK;
    *value = *value >> ST1633I_Y0_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_y0_l_tp
* Description    : Read First Y High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y0_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y0_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_Y0_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_x1_h_tp
* Description    : Read First X High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X1_H_REG[6:4] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x1_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_X1_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_X1_H_BIT_MASK;
    *value = *value >> ST1633I_X1_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_x1_l_tp
* Description    : Read First X Low Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X1_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x1_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_X1_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_y1_h_tp
* Description    : Read First Touch ID
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y1_H_REG[2:0] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y1_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_Y1_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_Y1_H_BIT_MASK;
    *value = *value >> ST1633I_Y1_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_y1_l_tp
* Description    : Read First Y High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y1_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y1_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_Y1_L_REG, value, 1);
}


/*******************************************************************************
* Function Name  : st1633i_x2_h_tp
* Description    : Read First X High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X2_H_REG[6:4] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x2_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_X2_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_X2_H_BIT_MASK;
    *value = *value >> ST1633I_X2_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_x2_l_tp
* Description    : Read First X Low Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X2_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x2_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_X2_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_y2_h_tp
* Description    : Read First Touch ID
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y2_H_REG[2:0] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y2_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_Y2_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_Y2_H_BIT_MASK;
    *value = *value >> ST1633I_Y2_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_y2_l_tp
* Description    : Read First Y High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y2_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y2_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_Y2_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_x3_h_tp
* Description    : Read First X High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X3_H_REG[6:4] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x3_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_X3_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_X3_H_BIT_MASK;
    *value = *value >> ST1633I_X3_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_x3_l_tp
* Description    : Read First X Low Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X3_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x3_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_X3_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_y3_h_tp
* Description    : Read First Touch ID
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y3_H_REG[2:0] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y3_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_Y3_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_Y3_H_BIT_MASK;
    *value = *value >> ST1633I_Y3_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_y3_l_tp
* Description    : Read First Y High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y3_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y3_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_Y3_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_x4_h_tp
* Description    : Read First X High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X4_H_REG[6:4] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x4_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_X4_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_X4_H_BIT_MASK;
    *value = *value >> ST1633I_X4_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_x4_l_tp
* Description    : Read First X Low Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_X4_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_x4_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_X4_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_y4_h_tp
* Description    : Read First Touch ID
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y4_H_REG[2:0] bits
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y4_h_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  int32_t ret;

  ret = st1633i_read_reg(ctx, ST1633I_Y4_H_REG, (uint8_t *)value, 1);

  if(ret == 0)
  {
    *value &= ST1633I_Y4_H_BIT_MASK;
    *value = *value >> ST1633I_Y4_H_BIT_POSITION;
  }

  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_y4_l_tp
* Description    : Read First Y High Touch Position
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_Y4_L register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_y4_l_tp(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_Y4_L_REG, value, 1);
}

/*******************************************************************************
* Function Name  : ST1633I_CTRL
* Description    : Control the Switch between Active and Monitoring Mode
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_ctrl(const st1633i_ctx_t *ctx, uint8_t value)
{
  return st1633i_write_reg(ctx, ST1633I_CTRL_REG, &value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_time_enter_monitor
* Description    : Set the time period of switching from Active mode to Monitor
*                  mode when there is no touching.
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_time_enter_monitor(const st1633i_ctx_t *ctx, uint8_t value)
{
  return st1633i_write_reg(ctx, ST1633I_TIMEOUTTOIDLE_REG, &value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_pwr_mode
* Description    : Set the power down bit to enter power down mode.
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_pwr_mode(const st1633i_ctx_t *ctx, uint8_t value)
{
  uint32_t ret;
  uint8_t tmp;

  ret = st1633i_read_reg(ctx, ST1633I_CTRL_REG, &tmp, 1);

  if (ret == 0)
  {
    tmp &= ~ST1633I_PWR_MODE_BIT_MASK;
    tmp |= value << ST1633I_PWR_MODE_BIT_POSITION;

    ret = st1633i_write_reg(ctx, ST1633I_CTRL_REG, &tmp, 1);
  }
  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_deep_pwr_mode
* Description    : Set the deep power down bit to enter deep power down mode.
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_deep_pwr_mode(const st1633i_ctx_t *ctx, uint8_t value)
{
  uint32_t ret;
  uint8_t tmp;

  ret = st1633i_read_reg(ctx, ST1633I_CTRL_REG, &tmp, 1);

  if (ret == 0)
  {
    tmp &= ~ST1633I_DEEP_PWR_MODE_BIT_MASK;
    tmp |= value << ST1633I_DEEP_PWR_MODE_BIT_POSITION;

    ret = st1633i_write_reg(ctx, ST1633I_CTRL_REG, &tmp, 1);
  }
  return ret;
}

/*******************************************************************************
* Function Name  : st1633i_firm_id
* Description    : Firmware Version
* Input          : Pointer to uint8_t
* Output         : Status of ST1633I_FIRMID register
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t  st1633i_firm_id(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_FIRMID_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_state
* Description    : Select Current Device Status
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t st1633i_state(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_STATUS_REG, value, 1);
}

/*******************************************************************************
* Function Name  : st1633i_error
* Description    : Select Current Error code
* Input          : uint8_t
* Output         : None
* Return         : Status [ST1633I_ERROR, ST1633I_OK]
*******************************************************************************/
int32_t st1633i_error(const st1633i_ctx_t *ctx, uint8_t *value)
{
  return st1633i_read_reg(ctx, ST1633I_ERROR_REG, value, 1);
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
