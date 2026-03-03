/**
  ******************************************************************************
  * @file    mbedtls_alt_config.h
  * @author  GPM Application Team
  * @brief   Header for mbedtls_alt middleware
  *
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
#ifndef MBEDTLS_ALT_CONFIG_H
#define MBEDTLS_ALT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/** @defgroup MBEDTLS_HAL_XXX_ALT
  * @brief    Alternative cryptography implementation based on STM32 hardware
  *           cryptographic accelerator
  * @{
  */

/**
  * @brief MBEDTLS_HAL_AES_ALT Enables ST AES alternative module to replace mbed
  *        TLS AES module by ST AES alternative implementation based on STM32
  *        AES hardware accelerator.
  *
  *        Uncomment a macro to enable ST AES hardware alternative module.
  *        Requires: MBEDTLS_AES_C, MBEDTLS_AES_ALT.
  */
#define MBEDTLS_HAL_AES_ALT

/**
  * @brief HW_CRYPTO_DPA_AES Allows DPA resistance for AES modes by using secure
  *        crypto processor (SAES), when this option is enabled,
  *        AES-ECB/CBC modes become DPA-protected.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_AES_ALT
  */
//#define HW_CRYPTO_DPA_AES


/**
  * @brief MBEDTLS_HAL_GCM_ALT Enables ST GCM alternative module to replace mbed
  *        TLS GCM module by ST GCM alternative implementation based on STM32
  *        AES hardware accelerator.
  *
  *        Uncomment a macro to enable ST GCM hardware alternative module.
  *        Requires: MBEDTLS_AES_C, MBEDTLS_GCM_C, MBEDTLS_GCM_ALT.
  */
//#define MBEDTLS_HAL_GCM_ALT

/**
  * @brief HW_CRYPTO_DPA_GCM Allows DPA resistance for GCM by using secure crypto
  *        processor (SAES) when this option is enabled, GCM becomes DPA-protected.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_GCM_ALT.
 */
//#define HW_CRYPTO_DPA_GCM

/**
  * @brief HW_CRYPTO_DPA_CTR_FOR_GCM Allows DPA resistance for GCM through CTR by
  *        using secure crypto processor (SAES) when this option is enabled,
  *        CTR becomes DPA-protected.
  *        CTR protected mode is mixed with software to create GCM protected mode.
  *        This option can be enabled when the hardware don't support protected GCM.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_GCM_ALT, HW_CRYPTO_DPA_GCM.
 */
//#define HW_CRYPTO_DPA_CTR_FOR_GCM

#if defined(HW_CRYPTO_DPA_GCM) && defined(HW_CRYPTO_DPA_CTR_FOR_GCM)
#error "HW_CRYPTO_DPA_GCM and HW_CRYPTO_DPA_CTR_FOR_GCM cannot be defined simultaneously"
#endif /* HW_CRYPTO_DPA_GCM && HW_CRYPTO_DPA_CTR_FOR_GCM */

/**
  * @brief MBEDTLS_HAL_SHA256_ALT Enables ST SHA-224 and SHA-256 alternative
  *        modules to replace mbed TLS SHA-224 and SHA-256 modules by ST SHA-224
  *        and SHA-256 alternative implementation based on STM32 HASH hardware
  *        accelerator.
  *
  *        Uncomment a macro to enable ST SHA256 hardware alternative module.
  *        Requires: MBEDTLS_SHA256_C, MBEDTLS_SHA256_ALT.
  */
//#define MBEDTLS_HAL_SHA256_ALT

/**
  * @brief ST_HW_CONTEXT_SAVING Enables ST HASH save context
  *        The HASH context of the interrupted task can be saved from the HASH
  *        registers to memory, and then be restored from memory to the HASH
  *        registers.
  *
  *        Uncomment a macro to enable ST HASH save context.
  *        Requires: MBEDTLS_SHA256_ALT.
  */
//#define ST_HW_CONTEXT_SAVING

#if defined(ST_HW_CONTEXT_SAVING) && (USE_HAL_HASH_SUSPEND_RESUME != 1U)
#error "Enable USE_HAL_HASH_SUSPEND_RESUME flag to save HASH context"
#endif /* ST_HW_CONTEXT_SAVING && USE_HAL_HASH_SUSPEND_RESUME */

/**
  * @brief MBEDTLS_HAL_ECDSA_ALT Enables ST ECDSA alternative module to replace
  *        mbed TLS ECDSA sign and  verify functions by ST ECDSA alternative
  *        implementation based on STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECDSA hardware alternative module.
  *        Requires: MBEDTLS_ECDSA_C, MBEDTLS_ECDSA_SIGN_ALT,
  *                  MBEDTLS_ECDSA_VERIFY_ALT, MBEDTLS_ECP_ALT.
  */
// #define MBEDTLS_HAL_ECDSA_ALT

/**
  * @brief MBEDTLS_HAL_ECDH_ALT Enables ST ECDH alternative module to replace
  *        mbed TLS Compute shared secret module by ST Compute shared secret
  *        alternative implementation based on STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECDH hardware alternative module.
  *        Requires: MBEDTLS_ECDH_C, MBEDTLS_ECDH_COMPUTE_SHARED_ALT,
  *                  MBEDTLS_ECP_ALT!!!!!.
  */
// #define MBEDTLS_HAL_ECDH_ALT

/**
  * @brief MBEDTLS_HAL_ECP_ALT Enables ST ECP alternative module to replace
  *        mbed TLS ECP module by ST ECP alternative implementation based on
  *        STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECP hardware alternative module.
  *        Requires: MBEDTLS_ECP_C, MBEDTLS_ECP_ALT.
  */
// #define MBEDTLS_HAL_ECP_ALT

/**
  * @brief MBEDTLS_HAL_RSA_ALT Enables ST RSA alternative module to replace
  *        mbed TLS RSA module by ST RSA alternative implementation based on
  *        STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST RSA hardware alternative module.
  *        Requires: MBEDTLS_RSA_C, MBEDTLS_RSA_ALT.
  */
//#define MBEDTLS_HAL_RSA_ALT

/**
  * @brief MBEDTLS_HAL_ENTROPY_HARDWARE_ALT Enables ST entropy source module
  *        to replace mbed TLS entropy module by ST entropy implementation
  *        based on STM32 RNG hardware accelerator.
  *
  *        Uncomment a macro to enable ST entropy hardware alternative module.
  *        Requires: MBEDTLS_ENTROPY_C, MBEDTLS_ENTROPY_HARDWARE_ALT.
  */
#define MBEDTLS_HAL_ENTROPY_HARDWARE_ALT

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_ALT_CONFIG_H */
