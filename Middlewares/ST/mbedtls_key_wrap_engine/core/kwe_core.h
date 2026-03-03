/**
  ******************************************************************************
  * @file    kwe_core.h
  * @author  MCD Application Team
  * @brief   Header for kwe_core.c module
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
#ifndef KWE_CORE_H
#define KWE_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "kwe_config.h"
#include "kwe_types.h"

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup CORE
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup CORE_Exported_Constants CORE Exported Constants
  * @{
  */

/**
  * @brief CORE Middleware version number
  */
#define KWE_VERSION_MAIN   (0x00U) /*!< [31:24] main version */
#define KWE_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define KWE_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define KWE_VERSION_RC     (0x01U) /*!< [7:0]  release candidate */
#define KWE_VERSION        ((KWE_VERSION_MAIN << 24U) | \
                            (KWE_VERSION_SUB1 << 16U) | \
                            (KWE_VERSION_SUB2 <<  8U) | \
                            (KWE_VERSION_RC))

/**
  * @}
  */

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/** @addtogroup CORE_Exported_Functions
  * @{
  */

/** @addtogroup CORE_Exported_Functions_Group1
  * @{
  */

/**
  * @brief CORE General Purpose functions
  */
KWE_StatusTypeDef KWE_Init(void);

int32_t KWE_GetVersion(void);

/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group2
  * @{
  */

/**
  * @brief CORE Key Wrap functions
  */
KWE_StatusTypeDef KWE_GenerateWrappedEccKey(
  KWE_EcpTypeDef *p_ecp,
  KWE_AlgTypeDef ecc_alg,
  uint8_t *p_key_buffer,
  size_t key_buffer_size,
  size_t *p_key_buffer_length);

KWE_StatusTypeDef KWE_WrapEccKey(
  KWE_EcpTypeDef *p_ecp,
  KWE_AlgTypeDef ecc_alg,
  const uint8_t *p_data,
  uint8_t *p_key_buffer,
  size_t key_buffer_size,
  size_t *p_key_buffer_length);

KWE_StatusTypeDef KWE_WrapRsaKey(
  KWE_RsaTypeDef *p_rsa,
  uint8_t *p_key_buffer,
  size_t *p_key_buffer_length);

/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group3
  * @{
  */

/**
  * @brief CORE Asymmetric Key functions
  */
KWE_StatusTypeDef KWE_EcdsaPublicKeyExport(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  uint8_t *p_public_key,
  size_t public_key_size,
  size_t *p_public_key_length);

KWE_StatusTypeDef KWE_EcdhKeyAgreement(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  const uint8_t *p_peer_key,
  size_t peer_key_length,
  uint8_t *p_shared_secret,
  size_t shared_secret_size,
  size_t *p_shared_secret_length);

/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group4
  * @{
  */

/**
  * @brief CORE Asymmetric Cryptography functions
  */
KWE_StatusTypeDef KWE_EcdsaSignHash(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  const uint8_t *p_hash,
  uint8_t *p_signature,
  size_t signature_size,
  size_t *p_signature_length);


KWE_StatusTypeDef KWE_RsaModularExp(
  KWE_RsaTypeDef *p_rsa,
  const uint8_t *p_key_buffer,
  const uint8_t *p_input,
  uint8_t *p_output);
/**
  * @}
  */

/** @addtogroup CORE_Exported_Functions_Group5
  * @{
  */

/**
  * @brief CORE Symmetric Cryptography functions
  */
KWE_StatusTypeDef KWE_WrapAESKey(
  const uint8_t *data,
  size_t data_length,
  uint8_t *key_buffer,
  size_t key_buffer_size,
  size_t *key_buffer_length);

KWE_StatusTypeDef KWE_AesAeadEncrypt(
  KWE_AlgTypeDef aes_alg,
  const uint8_t *key_buffer, size_t key_buffer_size,
  const uint8_t *nonce, size_t nonce_length,
  const uint8_t *additional_data, size_t additional_data_length,
  const uint8_t *plaintext, size_t plaintext_length,
  uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length,
  uint8_t tag_length);

KWE_StatusTypeDef KWE_AesAeadDecrypt(
  KWE_AlgTypeDef aes_alg,
  const uint8_t *key_buffer, size_t key_buffer_size,
  const uint8_t *nonce, size_t nonce_length,
  const uint8_t *additional_data, size_t additional_data_length,
  const uint8_t *ciphertext, size_t ciphertext_length,
  uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length,
  uint8_t tag_length);

KWE_StatusTypeDef KWE_AesEncrypt(
  KWE_AlgTypeDef aes_alg,
  const uint8_t *key_buffer, size_t key_buffer_size,
  const uint8_t *nonce, size_t nonce_length,
  const uint8_t *plaintext, size_t plaintext_length,
  uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length);

KWE_StatusTypeDef KWE_AesDecrypt(
  KWE_AlgTypeDef aes_alg,
  const uint8_t *key_buffer, size_t key_buffer_size,
  const uint8_t *input, size_t input_length,
  uint8_t *output, size_t output_size, size_t *output_length);

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

#endif /*KWE_CORE_H */
