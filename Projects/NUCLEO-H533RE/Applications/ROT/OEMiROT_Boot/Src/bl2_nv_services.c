/**
  ******************************************************************************
  * @file    bl2_nv_service.c
  * @author  MCD Application Team
  * @brief   This file provides all the Non volatile firmware functions.
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
#include "boot_hal_cfg.h"
#include "string.h"
#include "platform/include/plat_nv_counters.h"

#include "low_level_obkeys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private configuration  -----------------------------------------------*/
HAL_StatusTypeDef plat_init_nv_counter(void)
{
  return HAL_OK;
}

HAL_StatusTypeDef plat_set_nv_counter(enum nv_counter_t CounterId,
                                      uint32_t Data, uint32_t *Updated)
{
  uint32_t current = 0U;

  /* reset Updated flag */
  *Updated = 0U;

  if (OBK_GetNVCounter(CounterId,(uint32_t *) &current) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* never decrement the counter */
  if (current > Data)
  {
    return HAL_ERROR;
  }
  else if (Data > current)
  {
    if (OBK_UpdateNVCounter(CounterId, Data) != HAL_OK)
    {
      return HAL_ERROR;
    }
    /* set updated flag */
    *Updated = 1U;
  }
  else
  {
   return HAL_OK;
  }

  return HAL_OK;
}

HAL_StatusTypeDef plat_read_nv_counter(enum nv_counter_t CounterId,
                                             uint32_t size, uint8_t *val)
{
  /* counter is encoded uint32_t */
  if (OBK_GetNVCounter(CounterId,(uint32_t *) val) == HAL_OK)
  {
    if (*(uint32_t *)val != 0xFFFFFFFFU)
    {
      return HAL_OK;
    }
  }
  return HAL_ERROR;
}
