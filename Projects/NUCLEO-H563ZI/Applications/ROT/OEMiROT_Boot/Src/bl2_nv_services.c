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
#include "platform/include/tfm_plat_nv_counters.h"
#include "low_level_obkeys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private configuration  -----------------------------------------------*/
enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
  return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t CounterId,
                                            uint32_t Data)
{
  uint32_t current = 0U;

  if (OBK_GetNVCounter(CounterId,(uint32_t *) &current) != HAL_OK)
  {
    return TFM_PLAT_ERR_SYSTEM_ERR;
  }
   
  /* never decrement the counter */ 
  if (current > Data)
  {
    return TFM_PLAT_ERR_SYSTEM_ERR;
  }
  else if (Data > current)
  {  
    if (OBK_UpdateNVCounter(CounterId, Data) != HAL_OK)
    {
      return TFM_PLAT_ERR_SYSTEM_ERR;
    }
  }
  else 
  {
   return TFM_PLAT_ERR_SUCCESS;
  }
  
  return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t CounterId,
                                             uint32_t size, uint8_t *val)
{
  /* counter is encoded uint32_t */ 
  if (OBK_GetNVCounter(CounterId,(uint32_t *) val) == HAL_OK)
  {
    if (*(uint32_t *)val != 0xFFFFFFFFU)
    {
      return TFM_PLAT_ERR_SUCCESS;
    }
  }
  return TFM_PLAT_ERR_SYSTEM_ERR;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
