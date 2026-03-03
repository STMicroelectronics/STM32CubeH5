/**
  ******************************************************************************
  * @file    kwe_core.c
  * @author  MCD Application Team
  * @brief   Implementation of STM32 KWE Middleware Core module
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
#include "kwe_core.h"
#include <string.h>
#include <stdlib.h>

/** @defgroup KWE_MODULES KWE MODULES
  * @{
  */

/** @defgroup CORE CORE
  * @{
  */
#if defined(KWE_DRIVER_ENABLED)

/* Global variables ----------------------------------------------------------*/
/** @defgroup CORE_Exported_Variables CORE Exported Variables
  * @{
  */
#if defined(KWE_ASYMMETRIC_KEY_WRAP_ENABLED)
/**
  * CCB handler to set CCB
  */
CCB_HandleTypeDef hccb;

/**
  * PKA handler to set PKA and reset PKA RAM
  */
PKA_HandleTypeDef hpka;
#endif /* KWE_ASYMMETRIC_KEY_WRAP_ENABLED */

/**
  * CRYP handler to set SAES
  */
CRYP_HandleTypeDef hcryp;
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/** @defgroup CORE_Private_Macros CORE Private Macros
  * @{
  */

/**
  * 32-bit integer manipulation macros (big endian)
  */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                         \
  do {                                               \
    (n) = ((uint32_t) (b)[(i)    ] << 24 ) |         \
          ((uint32_t) (b)[(i) + 1] << 16 ) |         \
          ((uint32_t) (b)[(i) + 2] <<  8 ) |         \
          ((uint32_t) (b)[(i) + 3]       );          \
  } while(0)
#endif /* !GET_UINT32_BE */
/**
  * @}
  */

/* Private defines -----------------------------------------------------------*/
/** @defgroup CORE_Private_Constants CORE Private Constants
  * @{
  */
#define KWE_TIMEOUT_VALUE     (0xFFF)
#define KWE_IV_MAX_SIZE       (16U)
#define KWE_BLOB_IV_OFFSET    0x00000000
#define KWE_BLOB_TAG_OFFSET   0x00000004
#define KWE_BLOB_KEY_OFFSET   0x00000008

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/** @defgroup CORE_Exported_Functions CORE Exported Functions
  * @{
  */

/** @defgroup CORE_Exported_Functions_Group1 General Purpose functions
  *  @brief    KWE APIs related to General Purpose functions.
  *
@verbatim
 ===============================================================================
              ##### General Purpose functions #####
 ===============================================================================
[..]
    (+) Init
    (+) Get Version

@endverbatim
  * @{
  */

/**
  * @brief   KWE Initialization
  * @note    This API is used to initialize hardware IPs and setup system
  *          clock for KWE security engine.
  * @param   None
  * @retval  KWE_SUCCESS if success, an error code otherwise.
  */
KWE_StatusTypeDef KWE_Init(void)
{
  KWE_StatusTypeDef status = KWE_ERROR;

#if defined(KWE_ASYMMETRIC_KEY_WRAP_ENABLED)
  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* De-Initialize KWE engine */
  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }
#endif /* KWE_ASYMMETRIC_KEY_WRAP_ENABLED */

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  KWE_GetVersion
  *         Returns the KWE Middleware revision
  *
  * @param  None
  * @retval int32_t version in the format 0xXYZR (8bits for each XYZ decimal, R
  *         for release candidate)\n
  *                         [31:24] main version \n
  *                         [23:16] sub1 version \n
  *                         [15:8]  sub2 version \n
  *                         [7:0]  release candidate
  */
int32_t KWE_GetVersion(void)
{
  return (int32_t)KWE_VERSION;
}
/**
  * @}
  */

/** @defgroup CORE_Exported_Functions_Group2 Key Wrap functions
  *  @brief    KWE APIs related to Key Wrap Operations.
  *
@verbatim
 ===============================================================================
              ##### Key Wrap functions #####
 ===============================================================================
[..]
    (+) GenerateWrappedEccKey
    (+) WrapEccKey
    (+) RsaKeyWrap

@endverbatim
  * @{
  */

#if defined(KWE_ASYMMETRIC_KEY_WRAP_ENABLED)
/**
  * @brief  A function that generates ECC private key in wrapped form using
  *         KWE hardware accelerator.
  * @note   The key generated can be used for ECC digital signature or
  *         ECC ECDH key exchange.
  * @param  p_ecp : pointer to elliptic curve parameters.
  * @param  ecc_alg : elliptic curve algorithm takes value
  *         KWE_ALG_ECC_ECDSA or KWE_ALG_ECC_ECDH.
  * @param  p_key_buffer : a pointer to be filled with the wrapped key, IV,
  *         and the tag associate to the wrapped key.
  * @param  key_buffer_size : size of the key in bytes.
  * @param  p_key_buffer_length : a pointer to the size of the wrapped key in
  *         bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise.
  */
KWE_StatusTypeDef KWE_GenerateWrappedEccKey(
  KWE_EcpTypeDef *p_ecp,
  KWE_AlgTypeDef ecc_alg,
  uint8_t *p_key_buffer,
  size_t key_buffer_size,
  size_t *p_key_buffer_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_ECDSACurveParamTypeDef ecdsa_param;
  CCB_ECDSAKeyBlobTypeDef ecdsa_blob;
  CCB_ECCMulCurveParamTypeDef ecdh_param;
  CCB_ECCMulKeyBlobTypeDef ecdh_blob;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType        = HAL_CCB_USER_KEY_HW;

  ecdsa_param.primeOrderSizeByte           = p_ecp->order_size;
  ecdsa_param.modulusSizeByte              = p_ecp->modulus_size;
  ecdsa_param.pModulus                     = p_ecp->p_prime;
  ecdsa_param.coefSignA                    = p_ecp->a_sign;
  ecdsa_param.pAbsCoefA                    = p_ecp->p_a_abs;
  ecdsa_param.pCoefB                       = p_ecp->p_b;
  ecdsa_param.pPointX                      = p_ecp->p_gx;
  ecdsa_param.pPointY                      = p_ecp->p_gy;
  ecdsa_param.pPrimeOrder                  = p_ecp->p_n;

  ecdsa_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  ecdsa_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  ecdsa_blob.pWrappedKey = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;

  if (ecc_alg == KWE_ALG_ECC_ECDSA)
  {
    if (HAL_CCB_ECDSA_GenerateWrapPrivateKey(&hccb, &ecdsa_param, &wrapping_key_conf, &ecdsa_blob) != HAL_OK)
    {
      return status;
    }
  }
  else if (ecc_alg == KWE_ALG_ECC_ECDH)
  {
    ecdh_param               = ecdsa_param;
    ecdh_blob                = ecdsa_blob;

    if (HAL_CCB_ECC_GenerateWrapPrivateKey(&hccb, &ecdh_param, &wrapping_key_conf, &ecdh_blob) != HAL_OK)
    {
      return status;
    }
  }
  else
  {
    return KWE_ERROR_NOT_SUPPORTED;
  }

  *p_key_buffer_length = key_buffer_size;

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  A function that wrap ECC private key using KWE hardware
  *         accelerator.
  * @note   The wrapped key can be used for ECC digital signature or
  *         ECC ECDH key exchange.
  * @param  p_ecp : a pointer to elliptic curve parameters.
  * @param  ecc_alg : elliptic curve algorithm takes value
  *         KWE_ALG_ECC_ECDSA or KWE_ALG_ECC_ECDH.
  * @param  p_data : a pointer to the transparent key to be wrapped.
  * @param  p_key_buffer : a pointer to buffer to be filled with the wrapped key,
  *         IV, and the tag associate to the wrapped key.
  * @param  key_buffer_size : size of the key buffer in bytes.
  * @param  p_key_buffer_length : a pointer to the actual size of the wrapped
  *         key in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise.
  */
KWE_StatusTypeDef KWE_WrapEccKey(
  KWE_EcpTypeDef *p_ecp,
  KWE_AlgTypeDef ecc_alg,
  const uint8_t *p_data,
  uint8_t *p_key_buffer,
  size_t key_buffer_size,
  size_t *p_key_buffer_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_ECDSACurveParamTypeDef ecdsa_param;
  CCB_ECDSAKeyBlobTypeDef ecdsa_blob;
  CCB_ECCMulCurveParamTypeDef ecdh_param;
  CCB_ECCMulKeyBlobTypeDef ecdh_blob;

  uint8_t *p_key_data = (uint8_t *)p_data;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key : DHUK */
  wrapping_key_conf.WrappingKeyType        = HAL_CCB_USER_KEY_HW;

  /* Fill ECDSA In parameters */
  ecdsa_param.primeOrderSizeByte           = p_ecp->order_size;
  ecdsa_param.modulusSizeByte              = p_ecp->modulus_size;
  ecdsa_param.pModulus                     = p_ecp->p_prime;
  ecdsa_param.coefSignA                    = p_ecp->a_sign;
  ecdsa_param.pAbsCoefA                    = p_ecp->p_a_abs;
  ecdsa_param.pCoefB                       = p_ecp->p_b;
  ecdsa_param.pPointX                      = p_ecp->p_gx;
  ecdsa_param.pPointY                      = p_ecp->p_gy;
  ecdsa_param.pPrimeOrder                  = p_ecp->p_n;

  ecdsa_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  ecdsa_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  ecdsa_blob.pWrappedKey = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;

  if (ecc_alg == KWE_ALG_ECC_ECDSA)
  {
    if (HAL_CCB_ECDSA_WrapPrivateKey(&hccb, &ecdsa_param, p_key_data, &wrapping_key_conf, &ecdsa_blob) != HAL_OK)
    {
      return status;
    }
  }
  else if (ecc_alg == KWE_ALG_ECC_ECDH)
  {
    ecdh_param               = ecdsa_param;
    ecdh_blob                = ecdsa_blob;
    if (HAL_CCB_ECC_WrapPrivateKey(&hccb, &ecdh_param, p_key_data, &wrapping_key_conf, &ecdh_blob) != HAL_OK)
    {
      return status;
    }
  }
  else
  {
    return KWE_ERROR_NOT_SUPPORTED;
  }

  *p_key_buffer_length = key_buffer_size;

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  A function that can wrap RSA private key (Exponent and phi
  *         parameters) using KWE hardware accelerator.
  * @param  p_rsa : a pointer to RSA parameters.
  * @param  p_key_buffer : a pointer to buffer where the wrapped private
  *         key is to be written.
  * @param  p_key_buffer_length : a pointer to the actual size of outputted
  *         wrapped private key in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_WrapRsaKey(
  KWE_RsaTypeDef *p_rsa,
  uint8_t *p_key_buffer,
  size_t *p_key_buffer_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  uint32_t *p_modulus = NULL;

  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_RSAParamTypeDef rsa_mod_exp_param;
  CCB_RSAKeyBlobTypeDef rsa_mod_exp_blob;
  CCB_RSAClearKeyTypeDef rsa_key;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType         = HAL_CCB_USER_KEY_HW;

  /* Fill RSA Modular exponentiation In parameters */
  rsa_mod_exp_param.expSizeByte             = p_rsa->exponent_size;
  rsa_mod_exp_param.modulusSizeByte         = p_rsa->modulus_size;
  rsa_mod_exp_param.pMod                    = p_rsa->p_modulus;

  rsa_key.pExp                              = p_rsa->p_exponent;
  rsa_key.pPhi                              = p_rsa->p_phi;

  /* Prepare RSA Modular exponentiation parameters  */
  rsa_mod_exp_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  rsa_mod_exp_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  rsa_mod_exp_blob.pWrappedExp = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;
  rsa_mod_exp_blob.pWrappedPhi = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET
                                 + (rsa_mod_exp_param.expSizeByte / 4U);

  if (HAL_CCB_RSA_WrapPrivateKey(&hccb, &rsa_mod_exp_param, &rsa_key,
                                 &wrapping_key_conf,  &rsa_mod_exp_blob)
      != HAL_OK)
  {
    return status;
  }

  p_modulus = rsa_mod_exp_blob.pWrappedPhi + (rsa_mod_exp_param.modulusSizeByte / 4U);

  memcpy(p_modulus, rsa_mod_exp_param.pMod, rsa_mod_exp_param.modulusSizeByte);

  *p_key_buffer_length = (rsa_mod_exp_param.modulusSizeByte + rsa_mod_exp_param.expSizeByte
                          + p_rsa->phi_size + 4 * sizeof(rsa_mod_exp_blob.pIV) + 4 * sizeof(rsa_mod_exp_blob.pTag));

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @}
  */

/** @defgroup CORE_Exported_Functions_Group3 Asymmetric Key functions
  *  @brief    KWE APIs related to Asymmetric Key Operations.
  *
@verbatim
 ===============================================================================
              ##### Asymmetric Key functions #####
 ===============================================================================
[..]  This section provides functions allowing:
    (+) EcdsaPublicKeyExport
    (+) EcdhKeyAgreement

@endverbatim
  * @{
  */

/**
  * @brief  A function that export ECDSA public key from wrapped ECDSA
  *         private key through KWE hardware accelerator.
  * @param  p_ecp : a pointer to elliptic curve parameters.
  * @param  p_key_buffer : a pointer to the wrapped ECDSA private key.
  * @param  p_public_key : a pointer to buffer where the public key data is
  *         to be written.
  * @param  public_key_size : Size of the public_key buffer in bytes.
  * @param  p_public_key_length : a pointer to the actual size of outputted
  *         public key in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_EcdsaPublicKeyExport(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  uint8_t *p_public_key,
  size_t public_key_size,
  size_t *p_public_key_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;

  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_ECDSACurveParamTypeDef ecdsa_param;
  CCB_ECDSAKeyBlobTypeDef ecdsa_blob;
  CCB_ECCMulPointTypeDef publickey;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType         = HAL_CCB_USER_KEY_HW;

  ecdsa_param.primeOrderSizeByte            = p_ecp->order_size;
  ecdsa_param.modulusSizeByte               = p_ecp->modulus_size;
  ecdsa_param.pModulus                      = p_ecp->p_prime;
  ecdsa_param.coefSignA                     = p_ecp->a_sign;
  ecdsa_param.pAbsCoefA                     = p_ecp->p_a_abs;
  ecdsa_param.pCoefB                        = p_ecp->p_b;
  ecdsa_param.pPointX                       = p_ecp->p_gx;
  ecdsa_param.pPointY                       = p_ecp->p_gy;
  ecdsa_param.pPrimeOrder                   = p_ecp->p_n;

  ecdsa_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  ecdsa_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  ecdsa_blob.pWrappedKey = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;

#if defined(KWE_ECP_SHORT_WEIERSTRASS_ENABLED)
  *p_public_key_length               = 1U + 2U * ecdsa_param.primeOrderSizeByte;

  if (public_key_size < *p_public_key_length)
  {
    return status;
  }

  p_public_key[0]                    = 0x04;
  publickey.pPointX                  = p_public_key + 1;
  publickey.pPointY                  = p_public_key + 1 + (public_key_size / 2U);

#else /* To Do */
  *p_public_key_length               = ecdsa_param.primeOrderSizeByte;
  publickey.pPointX                  = p_public_key;
#endif /* KWE_ECP_SHORT_WEIERSTRASS_ENABLED */

  if (HAL_CCB_ECDSA_ComputePublicKey(&hccb, &ecdsa_param, &wrapping_key_conf, &ecdsa_blob, &publickey) != HAL_OK)
  {
    return status;
  }

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;


  return status;
}

/**
  * @brief  A function that performs key agreement using an asymmetric
  *         wrapped private key through KWE hardware accelerator
  *         and a peer public key.
  * @param  p_ecp : a pointer to elliptic curve parameters.
  * @param  p_key_buffer : a pointer to the wrapped ECDH private key.
  * @param  p_peer_key : a pointer to the peer public key.
  * @param  peer_key_length : size of the peer public key in bytes.
  * @param  p_shared_secret : a pointer to buffer where the shared secret
  *         data is to be written.
  * @param  shared_secret_size : Size of shared secret buffer in bytes.
  * @param  p_shared_secret_length : a pointer to the actual size of outputted
  *         shared secret in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_EcdhKeyAgreement(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  const uint8_t *p_peer_key,
  size_t peer_key_length,
  uint8_t *p_shared_secret,
  size_t shared_secret_size,
  size_t *p_shared_secret_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_ECCMulCurveParamTypeDef ecdh_param;
  CCB_ECCMulKeyBlobTypeDef ecdh_blob;
  CCB_ECCMulPointTypeDef ecdh_peer_pubkey;
  CCB_ECCMulPointTypeDef ecdh_shared_secret;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                           = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType       = HAL_CCB_USER_KEY_HW;

  /* Fill ECDSA In parameters */
  ecdh_param.primeOrderSizeByte           = p_ecp->order_size;
  ecdh_param.modulusSizeByte              = p_ecp->modulus_size;
  ecdh_param.pModulus                     = p_ecp->p_prime;
  ecdh_param.coefSignA                    = p_ecp->a_sign;
  ecdh_param.pAbsCoefA                    = p_ecp->p_a_abs;
  ecdh_param.pCoefB                       = p_ecp->p_b;
  ecdh_param.pPrimeOrder                  = p_ecp->p_n;

  ecdh_peer_pubkey.pPointX = (uint8_t *)p_peer_key + 1U;
  ecdh_peer_pubkey.pPointY = (uint8_t *)p_peer_key + 1U + peer_key_length / 2U;

  ecdh_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  ecdh_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  ecdh_blob.pWrappedKey = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;

  ecdh_shared_secret.pPointX  = p_shared_secret;
  ecdh_shared_secret.pPointY  = p_shared_secret + ecdh_param.primeOrderSizeByte;

  if (HAL_CCB_ECC_ComputeScalarMul(&hccb, &ecdh_param, &wrapping_key_conf,
                                   &ecdh_blob, &ecdh_peer_pubkey,
                                   &ecdh_shared_secret) != HAL_OK)
  {
    return status;
  }

  *p_shared_secret_length = (2U * ecdh_param.primeOrderSizeByte);

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @}
  */

/** @defgroup CORE_Exported_Functions_Group4 Asymmetric Cryptography functions
  *  @brief    KWE APIs related to Asymmetric Cryptography functions.
  *
@verbatim
 ===============================================================================
              ##### Asymmetric Cryptography functions #####
 ===============================================================================
    [..]
        (+) EcdsaSignHash
        (+) RsaModularExp

@endverbatim
  * @{
  */

/**
  * @brief  A function that sign a hash using an asymmetric
  *         wrapped private key through KWE hardware accelerator.
  * @param  p_ecp : a pointer to elliptic curve parameters.
  * @param  p_key_buffer : a pointer to the wrapped ECC private key.
  * @param  p_hash : pointer to the hash to be signed.
  * @param  p_signature : pointer to signature buffer to store signature.
  * @param  signature_size : size of signature buffer in bytes.
  * @param  p_signature_length : a pointer to the actual size of outputted
  *         signature in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_EcdsaSignHash(
  KWE_EcpTypeDef *p_ecp,
  const uint8_t *p_key_buffer,
  const uint8_t *p_hash,
  uint8_t *p_signature,
  size_t signature_size,
  size_t *p_signature_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  uint8_t *p_hash_tmp = (uint8_t *)p_hash;

  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_ECDSACurveParamTypeDef ecdsa_param;
  CCB_ECDSAKeyBlobTypeDef ecdsa_blob;
  CCB_ECDSASignTypeDef ecdsa_result;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType         = HAL_CCB_USER_KEY_HW;

  ecdsa_param.primeOrderSizeByte           = p_ecp->order_size;
  ecdsa_param.modulusSizeByte              = p_ecp->modulus_size;
  ecdsa_param.pModulus                     = p_ecp->p_prime;
  ecdsa_param.coefSignA                    = p_ecp->a_sign;
  ecdsa_param.pAbsCoefA                    = p_ecp->p_a_abs;
  ecdsa_param.pCoefB                       = p_ecp->p_b;
  ecdsa_param.pPointX                      = p_ecp->p_gx;
  ecdsa_param.pPointY                      = p_ecp->p_gy;
  ecdsa_param.pPrimeOrder                  = p_ecp->p_n;

  ecdsa_blob.pIV         = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  ecdsa_blob.pTag        = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  ecdsa_blob.pWrappedKey = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;

  ecdsa_result.pRSign                      = p_signature;
  ecdsa_result.pSSign                      = p_signature + signature_size / 2U;

  if (HAL_CCB_ECDSA_Sign(&hccb, &ecdsa_param, &wrapping_key_conf, &ecdsa_blob, p_hash_tmp, &ecdsa_result) != HAL_OK)
  {
    return status;
  }

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }
  *p_signature_length = 2U * ecdsa_param.primeOrderSizeByte;

  status = KWE_SUCCESS;


  return status;
}

/**
  * @brief  A function that performs modular exponentiation using wrapped RSA
  *         private key (Exponent and phi) through KWE hardware
  *         accelerator.
  * @param  p_rsa : a pointer to RSA parameters.
  * @param  p_key_buffer : a pointer to wrapped RSA private key.
  * @param  p_input : a pointer to the operand.
  * @param  p_output : a pointer to buffer where the modular exponentiation
  *         result is to be written
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_RsaModularExp(
  KWE_RsaTypeDef *p_rsa,
  const uint8_t *p_key_buffer,
  const uint8_t *p_input,
  uint8_t *p_output)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CCB_WrappingKeyTypeDef wrapping_key_conf;
  CCB_RSAParamTypeDef rsa_mod_exp_param;
  CCB_RSAKeyBlobTypeDef rsa_mod_exp_blob;

  /* Initialize KWE engine and set default configuration */
  hccb.Instance                        = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    return status;
  }

  /* Configure Wrapping Key */
  wrapping_key_conf.WrappingKeyType         = HAL_CCB_USER_KEY_HW;

  /* Fill RSA Modular exponentiation In parameters */
  rsa_mod_exp_param.expSizeByte               = p_rsa->exponent_size;
  rsa_mod_exp_param.modulusSizeByte           = p_rsa->modulus_size;
  rsa_mod_exp_param.pMod                      = (uint8_t *)p_key_buffer + (KWE_BLOB_KEY_OFFSET * 4U)
                                                + (2U * rsa_mod_exp_param.modulusSizeByte);

  /* Prepare RSA Modular exponentiation parameters  */
  rsa_mod_exp_blob.pIV                        = (uint32_t *)p_key_buffer + KWE_BLOB_IV_OFFSET;
  rsa_mod_exp_blob.pTag                       = (uint32_t *)p_key_buffer + KWE_BLOB_TAG_OFFSET;
  rsa_mod_exp_blob.pWrappedExp                = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET;
  rsa_mod_exp_blob.pWrappedPhi                = (uint32_t *)p_key_buffer + KWE_BLOB_KEY_OFFSET
                                                + rsa_mod_exp_param.expSizeByte / 4U;

  if (HAL_CCB_RSA_ComputeModularExp(&hccb, &rsa_mod_exp_param,
                                    &wrapping_key_conf, &rsa_mod_exp_blob,
                                    (uint8_t *)p_input, p_output)
      != HAL_OK)
  {
    return status;
  }

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}
#endif /* KWE_ASYMMETRIC_KEY_WRAP_ENABLED */

/**
  * @}
  */

/** @defgroup CORE_Exported_Functions_Group5 Symmetric Cryptography functions
  *  @brief    KWE APIs related to Symmetric Cryptography functions.
  *
@verbatim
 ===============================================================================
              ##### Symmetric Cryptography functions #####
 ===============================================================================
[..]
    (+) WrapAESKey
    (+) AesAeadEncrypt
    (+) AesAeadDecrypt
    (+) AesEncrypt
    (+) AesDecrypt

@endverbatim
  * @{
  */

/**
  * KWE AES hardware engine is Compliant with NIST FIPS
  * publication 197 "Advanced encryption standard (AES)"
  * (November 2001)
  */

/**
  * @brief  A function that wrap AES key using KWE hardware
  *         accelerator.
  * @note   The wrapped key can be used for AES and AES AEAD encryption or
  *         decryption operations.
  * @param  p_data : a pointer to the transparent key to be wrapped.
  * @param  data_length : size of the transparent key in bytes.
  * @param  p_key_buffer : a pointer to be filled with the wrapped key, IV,
  *         and the tag associate to the wrapped key.
  * @param  key_buffer_size : size of the key in bytes.
  * @param  p_key_buffer_length : a pointer to the size of the wrapped key IV,
  *         and the tag associate to the wrapped key in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise.
  */
KWE_StatusTypeDef KWE_WrapAESKey(
  const uint8_t *p_data,
  size_t data_length,
  uint8_t *p_key_buffer,
  size_t key_buffer_size,
  size_t *p_key_buffer_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  uint32_t key_le[8] = {0};
  uint32_t *p_key = NULL;
  uint32_t i = 0;
  CRYP_ConfigTypeDef conf;
  (void) memset(&hcryp, 0, sizeof(hcryp));
  (void) memset(&conf, 0, sizeof(conf));

  if (((key_buffer_size) % 16U) != 0U)
  {
    return status;
  }

  hcryp.Instance = SAES;
  hcryp.Init.DataType = CRYP_NO_SWAP;
  if (data_length == 16U)
  {
    hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  }
  else if (data_length == 32U)
  {
    hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
  }
  else
  {
    return KWE_ERROR_NOT_SUPPORTED;
  }

  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  hcryp.Init.KeyMode = CRYP_KEYMODE_WRAPPED;
  hcryp.Init.KeySelect = CRYP_KEYSEL_HW;
  hcryp.Init.KeyProtection = CRYP_KEYPROT_ENABLE;
  hcryp.Init.DataWidthUnit     = CRYP_DATAWIDTHUNIT_WORD;

#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  p_key = (uint32_t *)(p_key_buffer + KWE_IV_MAX_SIZE);
#else
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  p_key = (uint32_t *)p_key_buffer;
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    return status;
  }

#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
  /* Reconfigure the SAES */
  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }
  conf.pInitVect = (uint32_t *)p_key_buffer;
  /* Reconfigure the SAES */
  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }
#endif /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

  /* Set key data in Little endian format */
  for (i = 0; i < (data_length / 4U); i++)
  {
    GET_UINT32_BE(key_le[i], p_data, 4U * i);
  }

  if (HAL_CRYPEx_WrapKey(&hcryp, key_le, p_key, KWE_TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    return status;
  }

  *p_key_buffer_length = key_buffer_size;

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  A function that unwrap AES key using KWE hardware
  *         accelerator.
  * @note   The unwrapped key can be used for AES and AES AEAD encryption or
  *         decryption operations.
  * @param  p_key_buffer : a pointer to the wrapped key and IV associate to
  *         the wrapped key.
  * @param  key_buffer_size : size of the wrapped key and IV in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise.
  */
static KWE_StatusTypeDef KWE_UnwrapAESKey(
  const uint8_t *p_key_buffer,
  size_t key_buffer_size)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  uint32_t *p_key = NULL;
  size_t key_size = 0U;
  CRYP_ConfigTypeDef conf;
  (void) memset(&hcryp, 0, sizeof(hcryp));
  (void) memset(&conf, 0, sizeof(conf));

#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
  key_size = key_buffer_size - KWE_IV_MAX_SIZE;
#else
  key_size = key_buffer_size;
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

  if (((key_size) % 16U) != 0U)
  {
    return status;
  }

  hcryp.Instance               = SAES;
  hcryp.Init.DataType          = CRYP_NO_SWAP;

  if (key_size == 16U)
  {
    hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  }
  else if (key_size == 32U)
  {
    hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
  }
  else
  {
    return KWE_ERROR_NOT_SUPPORTED;
  }

  hcryp.Init.KeyIVConfigSkip   = CRYP_KEYIVCONFIG_ALWAYS;
  hcryp.Init.KeyMode           = CRYP_KEYMODE_WRAPPED;
  hcryp.Init.KeySelect         = CRYP_KEYSEL_HW;
  hcryp.Init.KeyProtection     = CRYP_KEYPROT_ENABLE;
  hcryp.Init.DataWidthUnit     = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.Algorithm         = CRYP_AES_ECB;
#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  p_key = (uint32_t *)(p_key_buffer + KWE_IV_MAX_SIZE);
#else
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  p_key = (uint32_t *)p_key_buffer;
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    return status;
  }

#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
  /* Reconfigure the SAES */
  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }
  conf.pInitVect = (uint32_t *)p_key_buffer;
  /* Reconfigure the SAES */
  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }
#endif /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

  if (HAL_CRYPEx_UnwrapKey(&hcryp, (uint32_t *)p_key, KWE_TIMEOUT_VALUE) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  A function that performs AES authenticated encryption operation
  *         using KWE hardware accelerator.
  * @param  alg : an AES AEAD algorithm.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.

  * @param  p_nonce : a pointer to nonce or initialization vectors (IV).
  * @param  nonce_length : Size of the nonce or IV buffer in bytes.
  * @param  p_additional_data : a pointer to additional data that will be
  *                             authenticated but not encrypted.
  * @param  additional_data_length : Size of additional data in bytes.
  * @param  p_plaintext : pointer data that will be authenticated and encrypted.
  * @param  plaintext_length: Size of data in bytes.
  * @param  p_ciphertext : Output buffer for the authenticated tag and encrypted
  *                        data.
  * @param  ciphertext_size : Size of the Output buffer in bytes.
  * @param  p_ciphertext_length : The size of the actual output cipher in bytes.
  * @param  tag_length : The size of the authentication tag in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_AesAeadEncrypt(
  KWE_AlgTypeDef alg,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length,
  uint8_t tag_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CRYP_ConfigTypeDef conf;
  uint8_t init_vect[16] = {0};
  uint32_t *iv_p = (uint32_t *)init_vect;
  uint32_t i = 0;
  size_t len_left = 0;
  unsigned int q = 0;
  
  unsigned char *b1_padded_addr = NULL;            /* Formatting of B1   */
  size_t b1_length = 0;                                /* B1 with padding    */
  uint8_t b1_padding = 0;                              /* B1 word alignment  */
  __ALIGN_BEGIN uint8_t tag[16]      __ALIGN_END;  /* temporary tag */

  (void) memset(&hcryp, 0, sizeof(hcryp));
  (void) memset(&conf, 0, sizeof(conf));

  if (((uint64_t) additional_data_length >> 61U != 0U) || (nonce_length > 16U))
  {
    return status;
  }

  if (KWE_UnwrapAESKey(p_key_buffer, key_buffer_size) != KWE_SUCCESS)
  {
    return status;
  }

  /* Reconfigure the SAES */
  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if ((alg == KWE_ALG_AES_GCM) || (alg == KWE_ALG_AES_CCM))
  {
    /* additional authentication data limited to 2^64 bits */
    conf.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
    if (additional_data_length != 0U)
    {
      if (alg == KWE_ALG_AES_GCM) 
      {
        /* Set Initialization vector (IV) in Little endian format */
        for (i = 0; i < (nonce_length / 4U); i++)
        {
          GET_UINT32_BE(iv_p[i], p_nonce, 4U * i);
        }

        /* counter value must be set to 2 when processing the first block of payload */
        iv_p[3] = 0x00000002;
        conf.pInitVect = iv_p;
        conf.HeaderSize = additional_data_length;
        conf.Header = (uint32_t *)p_additional_data;
      }
      else if (alg == KWE_ALG_AES_CCM)
      {
        /* Also implies q is within bounds */
        if (nonce_length < 7 || nonce_length > 13)
        {
          return status;
        }

        (void) memcpy(init_vect + 1, p_nonce, nonce_length);

        q = 16 - 1 - (unsigned char)nonce_length;
        init_vect[0] |= (additional_data_length > 0U) << 6U;
        init_vect[0] |= ((tag_length - 2U) / 2U) << 3U;
        init_vect[0] |= q - 1U;
        
        for (i = 0, len_left = plaintext_length; i < q; i++, len_left >>= 8U)
        {
          init_vect[15 - i] = (uint8_t)((len_left)& 0xff);
        }

        if (len_left > 0U)
        {
          return status;
        }
        /* first authentication block */
        for (i = 0U; i < 4U; i++)
        {
          GET_UINT32_BE(iv_p[i], init_vect, 4U * i);
        }
        conf.B0 = iv_p;
        /* Extra bytes to deal with data padding such that
         * the resulting string can be partitioned into words
         */
        b1_padding = ((additional_data_length + 2U) % 4U);
        b1_length = additional_data_length + 2U + b1_padding;

        b1_padded_addr = calloc(1, b1_length);

        if (b1_padded_addr == NULL)
        {
          return status;
        }

        /* Header length */
        b1_padded_addr[0] = (unsigned char)((additional_data_length >> 8) & 0xFF);
        b1_padded_addr[1] = (unsigned char)((additional_data_length) & 0xFF);

        /* data concatenation */
        (void) memcpy(b1_padded_addr + 2U, p_additional_data, additional_data_length);

        /* blocks (B) associated to the Associated Data (A) */
        conf.Header     = (uint32_t *)b1_padded_addr;

        conf.HeaderSize = b1_length;
      }
    }
    else
    {
      conf.HeaderSize = 0U;
      conf.Header = NULL;
    }
  }
  else
  {
    return KWE_ERROR_NOT_SUPPORTED;
  }

  conf.DataWidthUnit     = CRYP_DATAWIDTHUNIT_BYTE;
  conf.DataType          = CRYP_BYTE_SWAP;
  if (alg == KWE_ALG_AES_CCM)
  {
    conf.Algorithm       = CRYP_AES_CCM;
  }
  else
  {
    conf.Algorithm       = CRYP_AES_GCM_GMAC;
  }
  conf.KeyMode           = CRYP_KEYMODE_NORMAL;
  conf.KeySelect         = CRYP_KEYSEL_NORMAL;
  conf.KeyIVConfigSkip   = CRYP_KEYIVCONFIG_ONCE;

  /* Reconfigure the SAES */
  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)p_plaintext, plaintext_length/* ((plaintext_length % 16) * 16U)*/,
                       (uint32_t *)p_ciphertext, KWE_TIMEOUT_VALUE) != HAL_OK)
  {
    goto exit;
  }

  if (alg == KWE_ALG_AES_GCM)
  {
    /* Compute the authentication TAG */
    if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t *)(p_ciphertext + plaintext_length),
                                          KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      return status;
    }
  }
  else if (alg == KWE_ALG_AES_CCM)
  {
    /* Tag has a variable length */
    (void) memset(tag, 0, sizeof(tag));
    /* Compute the authentication TAG */
    if (HAL_CRYPEx_AESCCM_GenerateAuthTAG(&hcryp, (uint32_t *)tag, KWE_TIMEOUT_VALUE))
    {
      goto exit;
    }
    (void) memcpy((p_ciphertext + plaintext_length), tag, tag_length);
  }
  else
  {
    return status;
  }

  *p_ciphertext_length = ciphertext_size;

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

exit:
  if (alg == KWE_ALG_AES_CCM)
  {
    free(b1_padded_addr);
  }
  return status;
}

/**
  * @brief  A function that performs AES authenticated decryption operation
  *         using KWE hardware accelerator.
  * @param  alg : an AES AEAD algorithm.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.

  * @param  p_nonce : a pointer to nonce or initialization vectors (IV).
  * @param  nonce_length : Size of the nonce or IV buffer in bytes.
  * @param  p_additional_data : Additional data that has been authenticated
  *                             but not encrypted
  * @param  additional_data_length : Size of additional data in bytes.
  * @param  p_ciphertext : a pointer to encrypted data and authenticated tag.
  * @param  ciphertext_length : Size of encrypted data and authenticated tag
  *                             buffer in bytes.
  * @param  p_plaintext : a pointer to output buffer for the decrypted data.
  * @param  plaintext_size : Size of the output buffer in bytes.
  * @param  p_plaintext_length : the size of the actual decrypted data in bytes.
  * @param  tag_length : The size of the authentication tag in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_AesAeadDecrypt(
  KWE_AlgTypeDef alg,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_ciphertext, size_t ciphertext_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length,
  uint8_t tag_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CRYP_ConfigTypeDef conf;
  uint8_t init_vect[16] = {0};
  uint32_t *iv_p = (uint32_t *)init_vect;

  __ALIGN_BEGIN uint8_t work_buf[16] __ALIGN_END;
  uint8_t check_tag[16] = {0};
  size_t cipher_length = 0U;
  size_t cipher_block_size = 0U;
  size_t last_bytes = 0;
  uint32_t i = 0;
  size_t len_left = 0;
  unsigned int q = 0;
  
  unsigned char *b1_padded_addr = NULL;            /* Formatting of B1   */
  size_t b1_length = 0;                                /* B1 with padding    */
  uint8_t b1_padding = 0;                              /* B1 word alignment  */

  (void) memset(&hcryp, 0, sizeof(hcryp));
  (void) memset(&conf, 0, sizeof(conf));

  if ((((uint64_t) additional_data_length >> 61U) != 0U) || (nonce_length > 16U))
  {
    return status;
  }

  if (KWE_UnwrapAESKey(p_key_buffer, key_buffer_size) != KWE_SUCCESS)
  {
    return status;
  }
  /* Reconfigure the SAES */
  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if ((alg == KWE_ALG_AES_GCM) || (alg == KWE_ALG_AES_CCM))
  {
    /* Additional authentication data limited to 2^64 bits */
    conf.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
    if (additional_data_length != 0U)
    {
      if (alg == KWE_ALG_AES_GCM) 
      {
        /* Set Initialization vector (IV) in Little endian format */
        for (i = 0; i < (nonce_length / 4U); i++)
        {
          GET_UINT32_BE(iv_p[i], p_nonce, 4U * i);
        }

        /* Counter value must be set to 2 when processing the first block of payload */
        iv_p[3] = 0x00000002;
        conf.pInitVect = iv_p;
        conf.HeaderSize = (uint32_t)additional_data_length;
        conf.Header = (uint32_t *)p_additional_data;
      }
      else if (alg == KWE_ALG_AES_CCM)
      {
        /* Also implies q is within bounds */
        if ((nonce_length < 7U) || (nonce_length > 13U))
        {
          return status;
        }

        (void) memcpy(init_vect + 1U, p_nonce, nonce_length);

        q = 16U - 1U - (unsigned char)nonce_length;
        init_vect[0] |= (additional_data_length > 0U) << 6U;
        init_vect[0] |= ((tag_length - 2U) / 2U) << 3U;
        init_vect[0] |= q - 1U;
        
        for (i = 0U, len_left = (ciphertext_length - tag_length); i < q; i++, len_left >>= 8U)
        {
          init_vect[15U - i] = (uint8_t)((len_left)& 0xff);
        }

        if (len_left > 0U)
        {
          return status;
        }
        /* first authentication block */
        for (i = 0U; i < 4U; i++)
        {
          GET_UINT32_BE(iv_p[i], init_vect, 4U * i);
        }
        conf.B0 = iv_p;
        /* Extra bytes to deal with data padding such that
         * the resulting string can be partitioned into words
         */
        b1_padding = ((additional_data_length + 2U) % 4U);
        b1_length = additional_data_length + 2U + b1_padding;

        b1_padded_addr = calloc(1, b1_length);

        if (b1_padded_addr == NULL)
        {
          return status;
        }

        /* Header length */
        b1_padded_addr[0] = (unsigned char)((additional_data_length >> 8U) & 0xFF);
        b1_padded_addr[1] = (unsigned char)((additional_data_length) & 0xFF);

        /* data concatenation */
        (void) memcpy(b1_padded_addr + 2U, p_additional_data, additional_data_length);

        /* blocks (B) associated to the Associated Data (A) */
        conf.Header     = (uint32_t *)b1_padded_addr;
        conf.HeaderSize = b1_length;
      }
    }
    else
    {
      conf.HeaderSize = 0U;
      conf.Header = NULL;
    }
  }

  conf.DataWidthUnit     = CRYP_DATAWIDTHUNIT_BYTE;
  conf.DataType          = CRYP_BYTE_SWAP;
  if (alg == KWE_ALG_AES_CCM)
  {
    conf.Algorithm       = CRYP_AES_CCM;
  }
  else
  {
    conf.Algorithm       = CRYP_AES_GCM_GMAC;
  }
  conf.KeyMode           = CRYP_KEYMODE_NORMAL;
  conf.KeySelect         = CRYP_KEYSEL_NORMAL;
  conf.KeyIVConfigSkip   = CRYP_KEYIVCONFIG_ONCE;

  /* Reconfigure the SAES */
  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  cipher_length = ciphertext_length - tag_length;
  cipher_block_size = (cipher_length / 16U) * 16U;

  if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)p_ciphertext, cipher_block_size, (uint32_t *)p_plaintext,
                       KWE_TIMEOUT_VALUE) != HAL_OK)
  {
    goto exit;
  }

  /* decrypt last bytes */
  last_bytes = cipher_length % 16U;

  if (last_bytes)
  {
    (void) memset(work_buf, 0, sizeof(work_buf));
    (void) memcpy(work_buf, p_ciphertext + cipher_block_size, last_bytes);

    if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)work_buf, last_bytes, (uint32_t *)(p_plaintext + cipher_block_size),
                         KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      goto exit;
    }
  }

  if (alg == KWE_ALG_AES_GCM)
  {
    /* Compute the authentication TAG */
    if (HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t *)(check_tag), KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      return status;
    }
  }
  else if (alg == KWE_ALG_AES_CCM)
  {
    /* Compute the authentication TAG */
    if (HAL_CRYPEx_AESCCM_GenerateAuthTAG(&hcryp, (uint32_t *)(check_tag), KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      goto exit;
    }
  }
  else
  {
    return status;
  }

  if (memcmp(check_tag, p_ciphertext + plaintext_size, tag_length) != 0)
  {
    goto exit;
  }

  *p_plaintext_length = plaintext_size;

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

exit:
  if (alg == KWE_ALG_AES_CCM)
  {
    free(b1_padded_addr);
  }
  return status;
}

/**
  * @brief  A function that performs AES encryption cipher using KWE
  *         hardware accelerator.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.
  * @param  alg : a cipher algorithm.
  * @param  p_iv : a pointer to initialization vectors (IV).
  * @param  iv_length : Size of the IV buffer in bytes.
  * @param  p_plaintext : pointer data that will be encrypted.
  * @param  plaintext_length: Size of data in bytes.
  * @param  p_ciphertext : Output buffer for the encrypted data.
  * @param  ciphertext_size : Size of the Output buffer in bytes.
  * @param  p_ciphertext_length : The size of the actual output cipher in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_AesEncrypt(
  KWE_AlgTypeDef alg,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  const uint8_t *p_iv, size_t iv_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  CRYP_ConfigTypeDef conf;
  uint32_t init_vect[4] = {0};
  uint32_t i = 0;
  (void) memset(&hcryp, 0, sizeof(hcryp));
  (void) memset(&conf, 0, sizeof(conf));

  if (KWE_UnwrapAESKey(p_key_buffer, key_buffer_size) != KWE_SUCCESS)
  {
    return status;
  }

  /* Reconfigure the SAES */
  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  conf.DataWidthUnit     = CRYP_DATAWIDTHUNIT_BYTE;
  conf.DataType          = CRYP_BYTE_SWAP;
  conf.KeyMode           = CRYP_KEYMODE_NORMAL;
  conf.KeySelect         = CRYP_KEYSEL_NORMAL;
  if (alg == KWE_ALG_AES_CBC)
  {
    conf.Algorithm       = CRYP_AES_CBC;
  }
  else
  {
    conf.Algorithm       = CRYP_AES_ECB;
  }

  if (alg == KWE_ALG_AES_CBC)
  {
    /* Set Initialization vector (IV) in Little endian format */
    for (i = 0; i < (iv_length / 4U); i++)
    {
      GET_UINT32_BE(init_vect[i], p_iv, 4U * i);
    }
    conf.pInitVect = init_vect;
  }

  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)p_plaintext, plaintext_length, (uint32_t *)p_ciphertext,
                       KWE_TIMEOUT_VALUE) != HAL_OK)
  {
    return status;
  }

  *p_ciphertext_length = ciphertext_size;

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;
}

/**
  * @brief  A function that performs AES decryption operation using KWE
  *         hardware accelerator.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.
  * @param  alg : a cipher algorithm.

  * @param  p_ciphertext : a pointer to encrypted data and IV.
  * @param  ciphertext_length : Size of encrypted data and IV.
  * @param  p_plaintext : a pointer to output buffer for the decrypted data.
  * @param  plaintext_size : Size of the output buffer in bytes.
  * @param  p_plaintext_length : the size of the actual decrypted data in bytes.
  * @retval KWE_SUCCESS if success, an error code otherwise
  */
KWE_StatusTypeDef KWE_AesDecrypt(
  KWE_AlgTypeDef alg,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  const uint8_t *p_ciphertext, size_t ciphertext_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length)
{
  KWE_StatusTypeDef status = KWE_ERROR;
  uint32_t init_vect[4] = {0};
  uint32_t i = 0;
  CRYP_ConfigTypeDef conf;
  (void) memset(&conf, 0, sizeof(conf));
  (void) memset(&hcryp, 0, sizeof(hcryp));
  *p_plaintext_length = 0U;

  if (KWE_UnwrapAESKey(p_key_buffer, key_buffer_size) != KWE_SUCCESS)
  {
    return status;
  }

  if (HAL_CRYP_GetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if (alg == KWE_ALG_AES_CBC)
  {
    /* Set Initialization vector (IV) in Little endian format */
    for (i = 0; i < (KWE_IV_MAX_SIZE / 4U); i++)
    {
      GET_UINT32_BE(init_vect[i], p_ciphertext, 4U * i);
    }
    conf.pInitVect = init_vect;
  }

  conf.DataWidthUnit     = CRYP_DATAWIDTHUNIT_BYTE;
  conf.DataType          = CRYP_BYTE_SWAP;
  conf.KeyMode           = CRYP_KEYMODE_NORMAL;
  conf.KeySelect         = CRYP_KEYSEL_NORMAL;
  if (alg == KWE_ALG_AES_CBC)
  {
    conf.Algorithm       = CRYP_AES_CBC;
  }
  else
  {
    conf.Algorithm       = CRYP_AES_ECB;
  }

  if (HAL_CRYP_SetConfig(&hcryp, &conf) != HAL_OK)
  {
    return status;
  }

  if (alg == KWE_ALG_AES_CBC)
  {
    if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)(p_ciphertext + KWE_IV_MAX_SIZE), ciphertext_length - KWE_IV_MAX_SIZE,
                         (uint32_t *)p_plaintext, KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      return status;
    }

    *p_plaintext_length = ciphertext_length - KWE_IV_MAX_SIZE;
  }
  else
  {
    if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)p_ciphertext, ciphertext_length, (uint32_t *)p_plaintext,
                         KWE_TIMEOUT_VALUE) != HAL_OK)
    {
      return status;
    }

    *p_plaintext_length = ciphertext_length;
  }

  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return status;
  }

  status = KWE_SUCCESS;

  return status;

}

/**
  * @}
  */

/**
  * @}
  */

#endif /* KWE_DRIVER_ENABLED */

/**
  * @}
  */

/**
  * @}
  */
