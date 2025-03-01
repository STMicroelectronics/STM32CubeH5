/**
  ******************************************************************************
  * @file    stm32h573i_discovery_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32H573I_DK_CONF_H
#define STM32H573I_DK_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H573I_DK
  * @{
  */

/** @defgroup STM32H573I_DK_CONFIG Config
  * @{
  */

/** @defgroup STM32H573I_DK_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* COM define */
#define USE_BSP_COM_FEATURE               1U
#define USE_COM_LOG                       1U

/* I2C BUS timing define */
#define I2C_VALID_TIMING_NBR              128U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_CS42L51           1U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE      2048U
#define USE_BSP_CPU_CACHE_MAINTENANCE     0U

/* TS defines */
#define USE_TS_GESTURE                    1U
#define USE_TS_MULTI_TOUCH                1U
#define TS_TOUCH_NBR                      2U

/* IRQn priorities */
#define BSP_BUTTON_USER_IT_PRIORITY       15U
#define BSP_AUDIO_OUT_IT_PRIORITY         14U
#define BSP_AUDIO_IN_IT_PRIORITY          15U
#define BSP_SD_IT_PRIORITY                14U
#define BSP_SD_RX_IT_PRIORITY             14U
#define BSP_SD_TX_IT_PRIORITY             15U
#define BSP_TS_IT_PRIORITY                15U


/* I2C4 Frequencies in Hz  */
#define BUS_I2C4_FREQUENCY                100000UL /* Frequency of I2C4 = 100 KHz*/
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

#endif /* STM32H573I_DK_CONF_H */
