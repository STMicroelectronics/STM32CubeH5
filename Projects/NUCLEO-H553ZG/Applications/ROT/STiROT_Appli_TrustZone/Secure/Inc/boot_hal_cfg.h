/**
  ******************************************************************************
  * @file    boot_hal_cfg.h
  * @author  MCD Application Team
  * @brief   File fixing configuration flag specific for STM32H5xx platform
  *
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOOT_HAL_CFG_H
#define BOOT_HAL_CFG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "flash_layout.h"
/* RTC clock */
#define RTC_CLOCK_SOURCE_LSI
#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00F9
#endif
#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF
#endif

/* ICache */
#define STIROT_ICACHE_ENABLE /*!< Instruction cache enable */

/* Static protections */
#if  !defined(OEMUROT_ENABLE)
#define STIROT_WRP_PROTECT_ENABLE  /*!< Write Protection  */
#define STIROT_HDP_PROTECT_ENABLE /*!< HDP protection   */
#endif
#define STIROT_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */
#define STIROT_SECURE_USER_SRAM2_ECC /*!< SRAM2 ECC */


#ifdef STIROT_DEV_MODE
#define STIROT_OB_BOOT_LOCK OB_BOOT_LOCK_DISABLE /*!< BOOT Lock expected value  */
/*#define STIROT_ENABLE_SET_OB*/ /*!< Option bytes are set by STiROT_Appli_TrustZone when not correctly set  */
#define STIROT_ERROR_HANDLER_STOP_EXEC /*!< Error handler stops execution (else it resets) */
#else
#define STIROT_OB_BOOT_LOCK OB_BOOT_LOCK_DISABLE /*!< FIXME BOOT Lock expected value  */
#endif /* STIROT_DEV_MODE */

/* Run time protections */
#define STIROT_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define STIROT_MPU_PROTECTION    /*!< STiROT_Appli_TrustZone uses MPU to prevent execution outside of STiROT_Appli_TrustZone code  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  STIROT_SUCCESS = 0U,
  STIROT_FAILED
} STIROT_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void) __NO_RETURN;
#endif /* BOOT_HAL_CFG_H */
