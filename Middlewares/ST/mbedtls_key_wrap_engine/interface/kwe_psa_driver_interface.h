/**
  ******************************************************************************
  * @file    kwe_psa_driver_interface.h
  * @author  MCD Application Team
  * @brief   Header for kwe_psa_driver_interface.c module
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
#ifndef KWE_PSA_DRIVER_INTERFACE_H
#define KWE_PSA_DRIVER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "crypto.h"
#include "bignum_core.h"
#include "mbedtls/error.h"
#include "psa_crypto_ecp.h"
#include "psa_crypto_rsa.h"
#include "psa_util.h"
#include "md_psa.h"
#include "kwe_core.h"
#include "kwe_psa_driver_key_management.h"

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup INTERFACE
  * @{
  */
#if defined(PSA_KWE_DRIVER_ENABLED)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/** @addtogroup INTERFACE_Exported_Functions
  * @{
  */

/** @addtogroup INTERFACE_Exported_Functions_Group1
  * @{
  */
psa_status_t kwe_to_psa_error(KWE_StatusTypeDef state);

/**
  * @}
  */

/** @addtogroup INTERFACE_Exported_Functions_Group2
  * @{
  */
psa_status_t mbedtls_kwe_opaque_generate_key(
  const psa_key_attributes_t *p_attributes,
  uint8_t *p_key_buffer, size_t key_buffer_size,
  size_t *p_key_buffer_length);

psa_status_t mbedtls_kwe_opaque_import_key(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_data, size_t data_length,
  uint8_t *p_key_buffer, size_t key_buffer_size,
  size_t *p_key_buffer_length, size_t *bits);

/**
  * @}
  */

/** @addtogroup INTERFACE_Exported_Functions_Group3
  * @{
  */
psa_status_t mbedtls_kwe_opaque_export_public_key(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key, size_t key_length,
  uint8_t *p_data, size_t data_size, size_t *p_data_length);

psa_status_t mbedtls_kwe_opaque_key_agreement(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_peer_key, size_t peer_key_length,
  uint8_t *p_shared_secret, size_t shared_secret_size,
  size_t *p_shared_secret_length);

/**
  * @}
  */

/** @addtogroup INTERFACE_Exported_Functions_Group4
  * @{
  */
psa_status_t mbedtls_kwe_opaque_signature_sign_hash(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_hash, size_t hash_length,
  uint8_t *p_signature, size_t signature_size, size_t *p_signature_length);

psa_status_t mbedtls_kwe_opaque_signature_sign_message(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg, const uint8_t *p_input, size_t input_length,
  uint8_t *p_signature, size_t signature_size, size_t *p_signature_length);

psa_status_t mbedtls_kwe_opaque_asymmetric_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_input, size_t input_length,
  const uint8_t *p_salt, size_t salt_length,
  uint8_t *p_output, size_t output_size, size_t *p_output_length);

/**
  * @}
  */

/** @addtogroup INTERFACE_Exported_Functions_Group5
  * @{
  */
psa_status_t mbedtls_kwe_opaque_aead_encrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length);

psa_status_t mbedtls_kwe_opaque_aead_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_ciphertext, size_t ciphertext_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length);

psa_status_t mbedtls_kwe_opaque_cipher_encrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_iv, size_t iv_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length);

psa_status_t mbedtls_kwe_opaque_cipher_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_ciphertext, size_t input_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length);

/**
  * @}
  */

/**
  * @}
  */
#endif /* PSA_KWE_DRIVER_ENABLED */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif /*KWE_PSA_DRIVER_INTERFACE_H */
