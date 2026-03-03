/**
  ******************************************************************************
  * @file    ccm_alt.h
  * @author  GPM Application Team
  * @brief   Header for ccm_alt.c module
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
  * @brief This file provides an API for the CCM authenticated encryption
  *        mode for block ciphers based on STM32 AES hardware crypto
  *        accelerator.
  *
  * CCM combines Counter mode encryption with CBC-MAC authentication
  * for 128-bit block ciphers.
  *
  * Input to CCM includes the following elements:
  * <ul><li>Payload - data that is both authenticated and encrypted.</li>
  * <li>Associated data (Adata) - data that is authenticated but not
  * encrypted, For example, a header.</li>
  * <li>Nonce - A unique value that is assigned to the payload and the
  * associated data.</li></ul>
  *
  * Definition of CCM:
  * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
  * RFC 3610 "Counter with CBC-MAC (CCM)"
  *
  * Related:
  * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
  *
  * Definition of CCM*:
  * IEEE 802.15.4 - IEEE Standard for Local and metropolitan area networks
  * Integer representation is fixed most-significant-octet-first order and
  * the representation of octets is most-significant-bit-first order. This is
  * consistent with RFC 3610.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MBEDTLS_CCM_ALT_H
#define MBEDTLS_CCM_ALT_H

#if defined(MBEDTLS_CCM_ALT)
/* Includes ------------------------------------------------------------------*/
#if defined(MBEDTLS_HAL_CCM_ALT)
#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/**
  * @brief    The CCM context-type definition. The CCM context is passed
  *           to the APIs called.
  */
typedef struct mbedtls_ccm_context
{
  uint32_t ccm_key[8];                 /* Encryption/Decryption key        */
  CRYP_HandleTypeDef hcryp_ccm;        /* HW driver handle                 */
  uint32_t           ctx_save_cr;      /* save context for multi-instance  */
  unsigned char y[16];                 /* The Y working buffer */
  size_t plaintext_len;                /* Total plaintext length */
  size_t add_len;                      /* Total authentication data length */
  size_t tag_len;                      /* Total tag length */
  unsigned int q;                      /* The Q working value */
  int mode;                            /* The operation to perform:
                                          #MBEDTLS_CCM_ENCRYPT or
                                          #MBEDTLS_CCM_DECRYPT or
                                          #MBEDTLS_CCM_STAR_ENCRYPT or
                                          #MBEDTLS_CCM_STAR_DECRYPT. */
  int state;                           /* Working value holding context's
                                          state. Used for chunked data input */
}
mbedtls_ccm_context;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_CCM_ALT */

#endif /* MBEDTLS_CCM_ALT */
#endif /* MBEDTLS_CCM_ALT_H */
