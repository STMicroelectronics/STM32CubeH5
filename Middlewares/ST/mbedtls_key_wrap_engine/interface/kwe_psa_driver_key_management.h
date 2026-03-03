/**
  ******************************************************************************
  * @file    kwe_psa_driver_key_Management.h
  * @author  MCD Application Team
  * @brief   Header for kwe_psa_driver_key_Management.c module
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
#ifndef KWE_PSA_DRIVER_KEY_MANAGEMENT_H
#define KWE_PSA_DRIVER_KEY_MANAGEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <psa/crypto_driver_common.h>

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup INTERFACE
  * @brief
  * @{
  */
#if defined(PSA_KWE_DRIVER_ENABLED)

#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup INTERFACE_Exported_Constants INTERFACE Exported Constants
  * @{
  */

/** @defgroup KWE_DRIVER_Constants KWE Driver Constants
  * @{
  */
/*!< STM32 Key Wrap Engine PSA Opaque Driver Location */
#define PSA_CRYPTO_KWE_DRIVER_LOCATION   ((psa_key_location_t)0x00800001)
/*!< STM32 Key Wrap Engine IV size: 16 bytes */
#define PSA_KWE_DRIVER_IV_BASE_SIZE          (16U)
/*!< STM32 Key Wrap Engine TAG size: 16 bytes */
#define PSA_KWE_DRIVER_TAG_BASE_SIZE         (16U)
/*!< STM32 Key Wrap Engine context base size: 16 bytes IV + 16 bytes Tag */
#define PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE (32U)
/**
  * @}
  */

/** @defgroup RSSE_Constants RSSE Constants
  * @{
  */
/*!< RSSE ECC blob offset */
#define PSA_KWE_RSSE_BLOB_IV_OFFSET         (0x00000008)
/*!< RSSE RSA blob offset */
#define PSA_KWE_RSSE_BLOB_KEY_OFFSET        (0x00000050)
/*!< RSSE AES blob offset */
#define PSA_KWE_RSSE_AES_BLOB_OFFSET        (0x00000004)
/*!< The minimum value for a key identifier wrapped by RSSE */
#define PSA_KWE_KEY_ID_RSSE_MIN              ((psa_key_id_t) 0x3f000000)
/*!< The maximum value for a key identifier wrapped by RSSE */
#define PSA_KWE_KEY_ID_RSSE_MAX              ((psa_key_id_t) 0x3fffffff)
/**
  * @}
  */

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/** @defgroup INTERFACE_Exported_macros INTERFACE Exported macros
  * @{
  */
#define PSA_KWE_KEY_ID_IS_RSSE(id) \
  ((((id) >= PSA_KWE_KEY_ID_RSSE_MIN) && ((id) <= PSA_KWE_KEY_ID_RSSE_MAX))? \
   1U: 0U)
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */
/** @addtogroup INTERFACE_Exported_Functions
  * @{
  */

/** @addtogroup INTERFACE_Exported_Functions_Group6
  * @{
  */
size_t mbedtls_kwe_opaque_size_function(
  const psa_key_type_t key_type,
  const size_t key_bits);

psa_status_t mbedtls_kwe_opaque_get_key_buffer_size(
  const psa_key_attributes_t *attributes,
  size_t *key_buffer_size);

psa_status_t mbedtls_kwe_opaque_get_key_buffer_size_from_key_data(
  const psa_key_attributes_t *attributes,
  const uint8_t *data,
  size_t data_length,
  size_t *key_buffer_size);
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

#endif /*KWE_PSA_DRIVER_KEY_MANAGEMENT_H */
