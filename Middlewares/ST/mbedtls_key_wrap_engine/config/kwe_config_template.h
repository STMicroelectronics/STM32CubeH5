/**
  ******************************************************************************
  * @file    kwe_config_template.h
  * @author  GPM Application Team
  * @brief   Config file for STM32 Key Wrap Engine middleware
  *          This file should be copied to the application folder and renamed
  *          to kwe_config.h
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
#ifndef KWE_CONFIG_H
#define KWE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u3xx_hal.h"

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup CONFIG
  * @brief
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * \def KWE_DRIVER_ENABLED
  *
  * Enables STM32 KWE Driver core an implementation to use ST hardware crypto
  * accelerator that manages asymmetric and symmetric keys in wrapped forme.
  *
  * Uncomment a macro to enable the STM32 KWE PSA Driver core for ST key
  * wrap engine.
  *
  * Requires HAL_MODULE_ENABLED.
  */
#define KWE_DRIVER_ENABLED

/**
  * \def PSA_KWE_DRIVER_ENABLED
  *
  * Enables STM32 KWE PSA Driver Interface an implementation of PSA opaque
  * driver for the ST hardware crypto accelerator that manages
  * asymmetric and symmetric keys in wrapped forme.
  *
  * Uncomment a macro to enable the STM32 KWE PSA Driver Interface for ST
  * key wrap engine.
  *
  * Requires KWE_DRIVER_ENABLED.
  */
#define PSA_KWE_DRIVER_ENABLED

/**
  * \def KWE_ASYMMETRIC_KEY_WRAP_ENABLED
  *
  * Enables STM32 KWE PSA Driver that manages asymmetric keys in wrapped forme.
  *
  * Uncomment a macro to enable asymmetric key wrap operations on STM32 key
  * key wrap engine.
  *
  * Requires PSA_KWE_DRIVER_ENABLED.
  */
#define KWE_ASYMMETRIC_KEY_WRAP_ENABLED

/**
  * \def KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY
  *
  * Enables symmetric key wrap using AES-CBC algorithm, by default symmetric
  * keys are wrapped using AES-ECB algorithm.
  *
  * Uncomment a macro to enable symmetric key wrap operations using AES-CBC
  * algorithm.
  *
  * Requires PSA_KWE_DRIVER_ENABLED.
  */
#define KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY

/**
  * \def KWE_ECP_SHORT_WEIERSTRASS_ENABLED
  *
  * Enables Short Weierstrass elliptic curve forme.
  *
  * Uncomment a macro to enable ECC public key export in Short Weierstrass
  * elliptic curve forme.
  *
  * Requires PSA_KWE_DRIVER_ENABLED.
  */
#define KWE_ECP_SHORT_WEIERSTRASS_ENABLED

#ifdef __cplusplus
}
#endif

#endif /* KWE_CONFIG_H */
/**
  * @}
  */

/**
  * @}
  */
