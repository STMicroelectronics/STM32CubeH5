/**
  ******************************************************************************
  * @file    sha256_alt.h
  * @author  GPM Application Team
  * @brief   Header for sha256_alt.c module
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
  * @brief This file contains SHA-224 and SHA-256 definitions and functions
  *        based on STM32 HASH hardware crypto accelerator.
  *
  * The Secure Hash Algorithms 224 and 256 (SHA-224 and SHA-256) cryptographic
  * hash functions are defined in <em>FIPS 180-4: Secure Hash Standard (SHS)</em>.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#if defined (MBEDTLS_SHA256_ALT)

#if defined(MBEDTLS_HAL_SHA256_ALT)

#define ST_SHA256_BLOCK_SIZE  ((size_t)  64)        /*!< HW handles 512 bits, ie 64 bytes */
#define ST_SHA256_EXTRA_BYTES ((size_t)  4)         /*!< One supplementary word on first block */
#define ST_SHA256_NB_HASH_REG ((uint32_t)106)        /*!< Number of HASH HW context Registers:
                                                         CR + STR + IMR + CSR[103] */
#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief          SHA-256 context structure
  *
  *                 The structure is used both for SHA-256 and for SHA-224
  *                 checksum calculations. The choice between these two is
  *                 made in the call to mbedtls_sha256_starts_ret().
  */
typedef struct mbedtls_sha256_context
{
  int is224;                                      /*!< 0 = use SHA256, 1 = use SHA224 */
  HASH_HandleTypeDef hhash;                       /*!< Handle of HASH HAL */
  uint8_t sbuf[ST_SHA256_BLOCK_SIZE + ST_SHA256_EXTRA_BYTES];
  /*!< Buffer to store input data
      (first block with its extra bytes,
       intermediate blocks,
       or last input block) */
  uint8_t sbuf_len;                               /*!< Number of bytes stored in sbuf */
  uint8_t ctx_save_regs[ST_SHA256_NB_HASH_REG * 4];
  uint8_t first;                                  /*!< Extra-bytes on first computed block */
}
mbedtls_sha256_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_HAL_SHA256_ALT */

#endif /* MBEDTLS_SHA256_ALT */
#endif /* MBEDTLS_SHA256_ALT_H */
