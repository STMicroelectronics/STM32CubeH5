/**
  ******************************************************************************
  * @file    gcm_alt.h
  * @author  GPM Application Team
  * @brief   Header for gcm_alt.c module
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  * Copyright The Mbed TLS Contributors
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
  * @brief This file contains GCM definitions and functions based on STM32 AES
  *        hardware crypto accelerator.
  *
  * The Galois/Counter Mode (GCM) for 128-bit block ciphers is defined
  * in <em>D. McGrew, J. Viega, The Galois/Counter Mode of Operation
  * (GCM), Natl. Inst. Stand. Technol.</em>
  *
  * For more information on GCM, see <em>NIST SP 800-38D: Recommendation for
  * Block Cipher Modes of Operation: Galois/Counter Mode (GCM) and GMAC</em>.
  *
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MBEDTLS_GCM_ALT_H
#define MBEDTLS_GCM_ALT_H

#if defined(MBEDTLS_GCM_ALT)
/* Includes ------------------------------------------------------------------*/
#if defined(MBEDTLS_HAL_GCM_ALT)

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief          AES context structure
  *
  */
typedef struct mbedtls_gcm_context
{
  uint32_t gcm_key[8];                  /* Encryption/Decryption key           */
  CRYP_HandleTypeDef hcryp_gcm;         /* HW driver handle                    */
  uint32_t ctx_save_cr;                 /* Saved HW context for multi-instance */
  uint64_t len;                         /* total length of the encrypted data. */
#if defined(HW_CRYPTO_DPA_GCM) || defined(HW_CRYPTO_DPA_CTR_FOR_GCM)
  uint64_t HL[16];                   /*!< Precalculated HTable low. */
  uint64_t HH[16];                   /*!< Precalculated HTable high. */
  uint64_t add_len;                  /*!< The total length of the additional data. */
  unsigned char base_ectr[16];       /*!< The first ECTR for tag. */
#endif /* HW_CRYPTO_DPA_GCM || HW_CRYPTO_DPA_CTR_FOR_GCM */
  unsigned char y[16];               /*!< The Y working value. */
  unsigned char buf[16];             /*!< The buf working value. */
  int mode;                             /* The operation to perform:
                                               #MBEDTLS_GCM_ENCRYPT or
                                               #MBEDTLS_GCM_DECRYPT.             */
}
mbedtls_gcm_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_HAL_GCM_ALT */

#endif /* MBEDTLS_GCM_ALT */
#endif /* MBEDTLS_GCM_ALT_H */
