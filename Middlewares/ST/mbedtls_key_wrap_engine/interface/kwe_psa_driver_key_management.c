/**
  ******************************************************************************
  * @file    kwe_psa_driver_key_management.c
  * @author  MCD Application Team
  * @brief   Implementation of STM32 KWE Middleware interface module to PSA API
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

/* Includes ------------------------------------------------------------------*/
#include "psa/crypto.h"
#include "kwe_psa_driver_key_management.h"

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup INTERFACE
  * @brief
  * @{
  */

#if defined(PSA_KWE_DRIVER_ENABLED)

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
/** @addtogroup INTERFACE_Exported_Functions
  * @{
  */

/** @defgroup INTERFACE_Exported_Functions_Group6 Key Management functions
  * @brief   KWE Key Management APIs to interface with Mbed TLS
  *
@verbatim
  ==============================================================================
                      ##### Key Management functions #####
  ==============================================================================
    [..]
      This subsection provides a set of functions allowing to return the size of
      the wrapped private keys.

@endverbatim
  * @{
  */

/**
  * @brief  A function that return size of wrapped key in bytes.
  * @param  key_type : key type (ECC, RSA, AES).
  * @param  key_bits : actual key size in bits.
  * @retval Key size added to the metadata size that gets added to the wrapped
  *         key.
  */
size_t mbedtls_kwe_opaque_size_function(
  const psa_key_type_t key_type,
  const size_t key_bits)
{
  size_t key_buffer_size;

  key_buffer_size = PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits);
  if (key_buffer_size == 0U)
  {
    return 0;
  }
  /* Include spacing for base size overhead over the key size */
  if (PSA_KEY_TYPE_IS_ECC(key_type)) /* KWE wraps only the private key */
  {
    key_buffer_size = key_buffer_size + PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE;
  }
  else if (PSA_KEY_TYPE_IS_RSA(key_type)) /* KWE wraps exponent and phi, the modulus is stored with the blob */
  {
    key_buffer_size = (3U * key_buffer_size) + PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE;
  }
  else if (key_type == PSA_KEY_TYPE_AES)
  {
#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
    key_buffer_size = key_buffer_size + PSA_KWE_DRIVER_IV_BASE_SIZE;
#else
    return key_buffer_size;
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */
  }
  else
  {
    key_buffer_size = 0U;
  }

  return key_buffer_size;
}

/**
  * @brief  A function that get size of wrapped key in bytes from key bits.
  * @param  attributes : The key attributes.
  * @param  key_buffer_size : Minimum buffer size to contain the key material.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_get_key_buffer_size(
  const psa_key_attributes_t *attributes,
  size_t *key_buffer_size)
{
  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits = psa_get_key_bits(attributes);
  *key_buffer_size = mbedtls_kwe_opaque_size_function(key_type,
                                                      key_bits);
  return ((*key_buffer_size != 0U) ?
          PSA_SUCCESS : PSA_ERROR_NOT_SUPPORTED);
}

/**
  * @brief  A function that get size of wrappedkey in bytes from key data.
  * @param  attributes : The key attributes
  * @param  data : The input key data
  * @param  data_length : The input data length
  * @param  key_buffer_size : Minimum buffer size to contain the key material
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_get_key_buffer_size_from_key_data(
  const psa_key_attributes_t *attributes,
  const uint8_t *data,
  size_t data_length,
  size_t *key_buffer_size)
{
  psa_key_type_t key_type = psa_get_key_type(attributes);
  size_t key_bits = psa_get_key_bits(attributes);

  UNUSED(data);

  if (PSA_KWE_KEY_ID_IS_RSSE(attributes->MBEDTLS_PRIVATE(id)) != 0U)
  {
    *key_buffer_size = mbedtls_kwe_opaque_size_function(key_type,
                                                        key_bits);
  }
  else
  {
    *key_buffer_size = mbedtls_kwe_opaque_size_function(key_type,
                                                        PSA_BYTES_TO_BITS(data_length));
  }
  return ((*key_buffer_size != 0U) ?
          PSA_SUCCESS : PSA_ERROR_NOT_SUPPORTED);
}
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
