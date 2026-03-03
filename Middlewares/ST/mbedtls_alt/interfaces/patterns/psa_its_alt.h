/**
  ******************************************************************************
  * @file    psa_its_alt.h
  * @author  MCD Application Team
  * @brief   Header for psa_its_alt.c module
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
#ifndef PSA_ITS_ALT_H
#define PSA_ITS_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "psa_crypto_its.h"
#include "storage_interface.h"

#if defined(PSA_USE_ITS_ALT)

#define ITS_INVALID_UID           (0)
#if defined(MBEDTLS_HAL_GCM_ALT) && defined(MBEDTLS_GCM_ALT)
#define ITS_IV_SIZE               (12U)
#else
#define ITS_IV_SIZE               (16U)
#endif /* MBEDTLS_HAL_GCM_ALT */
#define ITS_TAG_SIZE              (16U)
#define ITS_MAX_OBJECT_DATA_SIZE  (1024U)

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  uint8_t obj_id[sizeof(psa_storage_uid_t)];         /*!< Object ID */
  size_t size[sizeof(uint8_t)];                      /*!< The size of the object (in Bytes) */
  uint8_t flags[sizeof(psa_storage_create_flags_t)]; /*!< Object flag indicate object state Set = 1, Reset = 0 */
} its_obj_info_t;

typedef struct
{
  its_obj_info_t obj_info;               /*!< object info: ID, size, flags */
  uint8_t obj_iv[ITS_IV_SIZE];           /*!< Initial vectors that used to encrypt the object */
  uint8_t obj_tag[ITS_TAG_SIZE];         /*!< Tag used to authenticate the object */
  uint8_t obj[ITS_MAX_OBJECT_DATA_SIZE]; /*!< The object buffer */
} its_obj_t;

/* Exported constants --------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* PSA_USE_ITS_ALT */

#ifdef __cplusplus
}
#endif

#endif /* PSA_ITS_ALT_H */
