/**
  ******************************************************************************
  * @file    sha1_alt.h
  * @author  GPM Application Team
  * @brief   Header for sha1_alt.c module
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

/**
  * @brief This file contains SHA-1 definitions and functions based on STM32 HASH
  *        hardware crypto accelerator.
  *
  * The Secure Hash Algorithm 1 (SHA-1) cryptographic hash function is defined in
  * <em>FIPS 180-4: Secure Hash Standard (SHS)</em>.
  *
  * @warning   SHA-1 is considered a weak message digest and its use constitutes
  *            a security risk. We recommend considering stronger message
  *            digests instead.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MBEDTLS_SHA1_ALT_H
#define MBEDTLS_SHA1_ALT_H

#if defined (MBEDTLS_SHA1_ALT)
/* Includes ------------------------------------------------------------------*/
#if defined(MBEDTLS_HAL_SHA1_ALT)

#ifdef __cplusplus
extern "C" {
#endif

#define ST_SHA1_BLOCK_SIZE  ((size_t)  64)          /*!< HW handles 512 bits, ie 64 bytes */
#define ST_SHA1_EXTRA_BYTES ((size_t)  4)           /*!< One supplementary word on first block */
#define ST_SHA1_NB_HASH_REG ((uint32_t)57)          /*!< Number of HASH HW context Registers:
                                                         CR + STR + IMR + CSR[54] */

/**
  * @brief          SHA-1 context structure
  */
typedef struct mbedtls_sha1_context
{
  HASH_HandleTypeDef hhash;                                  /*!< Handle of HASH HAL */
  uint8_t sbuf[ST_SHA1_BLOCK_SIZE + ST_SHA1_EXTRA_BYTES];    /*!< Buffer to store input data
                                                                  (first block with its extra bytes,
                                                                  intermediate blocks,
                                                                  or last input block) */
  uint8_t sbuf_len;                                           /*!< Number of bytes stored in sbuf */
  uint32_t ctx_save_regs[ST_SHA1_NB_HASH_REG];
  uint8_t first;                                  /*!< Extra-bytes on first computed block */
}
mbedtls_sha1_context;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_HAL_SHA1_ALT */

#endif /* MBEDTLS_SHA1_ALT */
#endif /* MBEDTLS_SHA1_ALT_H */
