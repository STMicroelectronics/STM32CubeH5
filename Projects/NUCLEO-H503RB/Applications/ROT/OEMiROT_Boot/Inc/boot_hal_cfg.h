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
#define OEMIROT_ICACHE_ENABLE /*!< Instruction cache enable */

/* Static protections */
#define OEMIROT_WRP_PROTECT_ENABLE  /*!< Write Protection  */
#define OEMIROT_HDP_PROTECT_ENABLE /*!< HDP protection   */
#define OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET /*!< SRAM2 clear at Reset  */
#define OEMIROT_SECURE_USER_SRAM2_ECC /*!< SRAM2 ECC */
#define OEMIROT_SECURE_USER_SRAM1_ERASE_AT_RESET /*!< SRAM1 clear at Reset  */
#define OEMIROT_SECURE_USER_SRAM1_ECC /*!< SRAM1 ECC */

#ifdef OEMIROT_DEV_MODE
#define OEMIROT_OB_PRODUCT_STATE_VALUE OB_PROD_STATE_OPEN   /*!< Product State */
#else
#define OEMIROT_OB_PRODUCT_STATE_VALUE OB_PROD_STATE_CLOSED /*!< Product State */
#endif /* OEMIROT_DEV_MODE */

#define NO_TAMPER            (0)                /*!< No tamper activated */
#define INTERNAL_TAMPER_ONLY (1)                /*!< Only Internal tamper activated */
#define ALL_TAMPER           (2)                /*!< Internal and External tamper activated, bootloader not available */
#define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY            /*!< TAMPER configuration flag  */
#define OEMIROT_OB_BOOT_LOCK OB_BOOT_LOCK_ENABLE /*!< BOOT Lock expected value  */

#ifdef OEMIROT_DEV_MODE
/*#define OEMIROT_ENABLE_SET_OB*/ /*!< Option bytes are set by OEMiROT_Boot when not correctly set  */
#define OEMIROT_ERROR_HANDLER_STOP_EXEC /*!< Error handler stops execution (else it resets) */
#endif /* OEMIROT_DEV_MODE */

/* Run time protections */
#define OEMIROT_FLASH_PRIVONLY_ENABLE  /*!< Flash Command in Privileged only  */
#define OEMIROT_MPU_PROTECTION    /*!< OEMiROT_Boot uses MPU to prevent execution outside of OEMiROT_Boot code  */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  OEMIROT_SUCCESS = 0U,
  OEMIROT_FAILED
} OEMIROT_ErrorStatus;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Error_Handler(void) __NO_RETURN;
#endif /* BOOT_HAL_CFG_H */
