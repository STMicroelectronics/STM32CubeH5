/**
  ******************************************************************************
  * @file    kwe_types.h
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
#ifndef KWE_TYPES_H
#define KWE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup KWE_MODULES
  * @{
  */

/** @addtogroup CORE
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup CORE_Exported_Types CORE Exported Types
  * @{
  */

/** @defgroup CORE_Errors_Code CORE Errors Code
  * @{
  */
typedef enum
{
  KWE_SUCCESS                      = 0x00U,
  KWE_ERROR                        = 0x01U,
  KWE_WRONG_KEY_TYPE               = 0x02U,
  KWE_STORAGE_ERROR                = 0x03U,
  KWE_ERROR_NOT_SUPPORTED          = 0x04U,
} KWE_StatusTypeDef;
/**
  * @}
  */

/** @defgroup CORE_Algorithms CORE Algorithms
  * @{
  */
typedef enum
{
  KWE_ALG_ECC_ECDSA                    = 0x00U,
  KWE_ALG_ECC_ECDH                     = 0x01U,
  KWE_ALG_AES_GCM                      = 0x02U,
  KWE_ALG_AES_CCM                      = 0x03U,
  KWE_ALG_AES_CBC                      = 0x04U,
  KWE_ALG_AES_ECB                      = 0x05U,
} KWE_AlgTypeDef;
/**
  * @}
  */

/** @defgroup CORE_Elliptic_Curve_Group CORE Elliptic Curve Group
  * @{
  */
typedef struct
{
  uint32_t modulus_size;     /*!< Number of bytes in prime modulus */
  uint32_t order_size;       /*!< Number of bytes in prime order */
  uint8_t *p_prime;          /*!< Prime modulus p */
  uint32_t a_sign;           /*!< Sign of A coef */
  uint8_t *p_a_abs;          /*!< abs(A) coef */
  uint8_t *p_b;              /*!< B coef */
  uint8_t *p_gx;             /*!< Gx basepoint */
  uint8_t *p_gy;             /*!< Gy basepoint */
  uint8_t *p_n;              /*!< Prime Order n */
} KWE_EcpTypeDef;
/**
  * @}
  */

/** @defgroup CORE_RSA_Group CORE RSA Group
  * @{
  */
typedef struct
{
  uint32_t modulus_size;     /*!< Number of bytes in modulus */
  uint8_t *p_modulus;        /*!< Pointer to modulus */
  uint32_t exponent_size;    /*!< Number of bytes in exponent */
  uint8_t *p_exponent;       /*!< Pointer to exponent */
  uint32_t phi_size;         /*!< Number of bytes in phi */
  uint8_t *p_phi;            /*!< Pointer to phi */
} KWE_RsaTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*KWE_TYPES_H */
