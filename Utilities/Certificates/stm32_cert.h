/**
  ******************************************************************************
  * @file    stm32_cert.h
  * @author  GPM Application Team
  * @brief   Header file of STM32_CERT module.
  *          This file contains the common declaration for getting certificate.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_CERT_H
#define STM32_CERT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplucplus */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

#ifdef STM32H573xx
#include "stm32h573xx.h"
#define DUA_USE_ECC_CURVE_SECP256R1
#else
#error "Target not defined in the preprocessor setting or target not supported"
#endif /* STM32H573xx */

/* Enable / disable required elliptic curves here (multiple curves support allowed) */
#ifdef DUA_USE_ECC_CURVE_SECP521R1
#define DUA_PRIVATE_KEY_MAX_SIZE_BYTES (68U)
#else
#define DUA_PRIVATE_KEY_MAX_SIZE_BYTES (32U)
#endif /* DUA_USE_ECC_CURVE_SECP521R1 */

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_DISCO
  * @{
  */

/** @addtogroup Common
  * @{
  */

/** @defgroup STM32_CERT CERTIFICATES Utility
  * @{
  */

/** @defgroup UTIL_STM32_CERT_Exported_Constants CERTIFICATES Utility Exported Constants
  * @{
  */

#define DUA_PUBLIC_PART_MAX_SIZE_BYTES (512U)

/**
  * @brief  CERTIFICATES Utility CERT ID definition
  */
typedef enum
{
  DUA_INITIAL_ATTEST = 1U, DUA_USER = 2U
} CERT_Id_t;
/**
  * @}
  */

/**
  * @brief  CERTIFICATES Utility Error definition
  */
typedef enum
{
  CERT_OK = 1U, CERT_ERROR = 2U, CERT_HW_ERROR = 3U
} CERT_Status_t;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup UTIL_STM32_CERT_Exported_Functions
  * @{
  */
CERT_Status_t UTIL_CERT_GetCertificateSize(const CERT_Id_t certificate_id, uint32_t *p_certificate_size);
CERT_Status_t UTIL_CERT_GetCertificate(const CERT_Id_t certificate_id, uint8_t *p_certificate);
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

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* STM32_CERT_H */
