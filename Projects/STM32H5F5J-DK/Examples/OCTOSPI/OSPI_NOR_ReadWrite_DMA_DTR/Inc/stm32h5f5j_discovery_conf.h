/**
  ******************************************************************************
  * @file    stm32h5f5j_discovery_conf_template.h
  * @author  MCD Application Team
  *          This file should be copied to the application folder and renamed
  *          to stm32h5f5j_discovery_conf.h
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
#ifndef STM32H5F5J_DISCOVERY_CONF_H
#define STM32H5F5J_DISCOVERY_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H5F5J_DK
  * @{
  */

/** @defgroup STM32H5F5J_DK_CONFIG Config
  * @{
  */

/** @defgroup STM32H5F5J_DK_CONFIG_Exported_Constants Exported Constants
  * @{
  */

/* COM defines */
#define USE_COM_LOG                         0U
#define USE_BSP_COM_FEATURE                 0U

/* I2C BUS timing define */
#define I2C_VALID_TIMING_NBR              128U

/* LCD defines */
#define USE_LCD_CTRL_ST1633I              1U
#define LCD_LAYER_0_ADDRESS               0x20060000U /* SRAM1 */
#define LCD_LAYER_1_ADDRESS               0x200C0000U /* SRAM1 */


/* Touch Sensing controller defines */
#define USE_FT5336_TS_CTRL                  1U

/* TS supported features defines */
#define USE_TS_GESTURE                      1U
#define USE_TS_MULTI_TOUCH                  1U
#define TS_TOUCH_NBR                        2U

/* Audio codecs defines */
#define USE_AUDIO_CODEC_CS42L51           1U

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        2048U

/* IRQ priorities (Default is 15 as lowest priority level) */
#define BSP_SDRAM_IT_PRIORITY               15U
#define BSP_BUTTON_USER_IT_PRIORITY         15U
#define BSP_AUDIO_OUT_IT_PRIORITY           14U
#define BSP_AUDIO_IN_IT_PRIORITY            15U
#define BSP_SD_IT_PRIORITY                  14U
#define BSP_SD_RX_IT_PRIORITY               14U
#define BSP_SD_TX_IT_PRIORITY               15U
#define BSP_TS_IT_PRIORITY                  15U
#define BSP_XSPI_RAM_IT_PRIORITY            15U
#define BSP_XSPI_RAM_DMA_IT_PRIORITY        15U

/* I2C4 Frequencies in Hz  */
#define BUS_I2C4_FREQUENCY                100000UL /* Frequency of I2C4 = 100 KHz*/

/* Usage of USBPD PWR TRACE system */
#define USE_BSP_USBPD_PWR_TRACE           0U /* USBPD BSP trace system is disabled */

#define USE_DMA2D_TO_FILL_RGB_RECT        1U /* USE DMA2D */

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

#endif /* STM32H5F5J_DISCOVERY_CONF_H */
