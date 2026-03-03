/**
  ******************************************************************************
  * @file    kwe_psa_driver_interface.c
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
#include "psa_crypto_driver_wrappers.h"
#include "kwe_psa_driver_interface.h"


/** @addtogroup KWE_MODULES
  * @{
  */

/** @defgroup INTERFACE INTERFACE
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
/** @defgroup INTERFACE_Private_Functions INTERFACE Private Functions
  * @{
  */

/** @defgroup INTERFACE_Private_Functions_Group1 Digest Preprocessing functions
  *  @brief   INTERFACE private function to prepare the digest in accordance
  *           with prime order.
  *
@verbatim
  ==============================================================================
                      ##### Digest Preprocessing function #####
  ==============================================================================

@endverbatim
  * @{
  */

#if defined(MBEDTLS_ECDSA_C)
/**
  * @brief      Prepare the digest for respecting digest length = Prime order length
  * @param[in]  P_pDigestIn        Input digest
  * @param[in]  P_DigestInLength   Input digest length
  * @param[out] P_pDigestOut       Output digest
  * @param[in]  P_DigestOutLength  Output digest length
  * @param[out] P_pPrimeOrder      Prime order of the selected curve
  * @retval     none
  */
static void ecc_hal_prepare_digest(const uint8_t *P_pDigestIn, uint8_t P_DigestInLength,
                                   uint8_t *P_pDigestOut, uint8_t P_DigestOutLength, uint8_t *P_pPrimeOrder)
{

  uint8_t i;                                            /* Counter */
  uint8_t propagation = 0;                              /* For internal use */

  /* Prepare P_pDigestIn for being equal to P_DigestOutLength */
  if (P_DigestInLength < P_DigestOutLength)
  {
    /* Padding of P_pDigestIn at the beginning */
    for (i = 0; i < (P_DigestOutLength - P_DigestInLength); i++)
    {
      P_pDigestOut[i] = 0x00;
    }
    /* Copy the digest */
    (void) memcpy(&P_pDigestOut[P_DigestOutLength - P_DigestInLength], P_pDigestIn, P_DigestInLength);
  }
  else if (P_DigestInLength == P_DigestOutLength)
  {
    if (memcmp(P_pDigestIn, P_pPrimeOrder, P_DigestOutLength) < 0) /* P_pDigestIn < P_pPrimeOrder */
    {
      /* Use digest as provide by user */
      (void) memcpy(P_pDigestOut, P_pDigestIn, P_DigestInLength);
    }
    else /* P_pDigestIn => P_pPrimeOrder */
    {
      /* Copy P_pDigestIn in final digest, temporary operation */
      (void) memcpy(P_pDigestOut, P_pDigestIn, P_DigestInLength);

      /* Reduce digest. digest = digest - prime order */
      for (i = (P_DigestOutLength - 1u); i >= 1u; i--)
      {
        /* Manage the restrains */
        if (propagation == 1u)
        {
          if (P_pDigestOut[i] == 0x00u)
          {
            propagation = 1; /* Keep the restrains */
            P_pDigestOut[i] = 0xFFu;
          }
          else
          {
            propagation = 0u;
            P_pDigestOut[i] = P_pDigestOut[i] - 1u;
          }
        }
        if (P_pDigestOut[i] >= P_pPrimeOrder[i])
        {
          /* P_pDigestIn byte bigger than P_pPrimeOrder, easy to subtract */
          P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
        }
        else
        {
          /* P_pDigestIn byte is smaller than P_pPrimeOrder */
          P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
          propagation = 1;
        }
      }
      /* Case of last byte outside the loop for misra */
      if (propagation == 1u)
      {
        P_pDigestOut[0] = P_pDigestOut[0] - 1u;
      }
      /* P_pDigestOut is obligatory bigger than P_pPrimeOrder */
      P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
    } /* P_pDigestIn => P_pPrimeOrder */
  }
  else /* P_DigestInLength > P_DigestOutLength */
  {
    /* Keep only first P_DigestOutLength bit of the digest */
    (void) memcpy(P_pDigestOut, P_pDigestIn, P_DigestOutLength);

    if (memcmp(P_pDigestOut, P_pPrimeOrder, P_DigestOutLength) < 0) /* P_pDigestOut < P_pPrimeOrder */
    {
      /* Nothing to do */
    }
    else /* P_pDigestOut >=  P_pPrimeOrder */
    {
      /* Reduce digest. digest = digest - prime order */
      for (i = (P_DigestOutLength - 1u); i >= 1u; i--)
      {
        /* Manage the restrains */
        if (propagation == 1u)
        {
          if (P_pDigestOut[i] == 0x00u)
          {
            propagation = 1u; /* Keep the restrains */
            P_pDigestOut[i] = 0xFFu;
          }
          else
          {
            propagation = 0u;
            P_pDigestOut[i] = P_pDigestOut[i] - 1u;
          }
        }
        if (P_pDigestOut[i] >= P_pPrimeOrder[i])
        {
          /* P_pDigestIn byte bigger than P_pPrimeOrder, easy to subtract */
          P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
        }
        else
        {
          /* P_pDigestIn byte is smaller than P_pPrimeOrder */
          P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
          propagation = 1;
        }
        /* Case of last byte outside the loop for misra */
        if (propagation == 1u)
        {
          P_pDigestOut[0] = P_pDigestOut[0] - 1u;
        }
        /* P_pDigestOut is obligatory bigger than P_pPrimeOrder */
        P_pDigestOut[i] = P_pDigestOut[i] - P_pPrimeOrder[i];
      }
    }  /* P_pDigestOut >=  P_pPrimeOrder */
  } /* end digest preparation */
}
#endif /* MBEDTLS_ECDSA_C */

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup INTERFACE_Exported_Functions INTERFACE Exported Functions
  * @{
  */

/** @defgroup INTERFACE_Exported_Functions_Group1 General Purpose functions
  *  @brief   INTERFACE error code translate from KWE to Mbed TLS.
  *
@verbatim
  ==============================================================================
                      ##### General Purposec functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  KWE driver error code translate to PSA error code
  * @param  state : error code from KWE hardware crypto accelerator
  *         function.
  * @retval PSA error code.
  */
psa_status_t kwe_to_psa_error(KWE_StatusTypeDef state)
{
  psa_status_t status;

  switch (state)
  {
    case KWE_SUCCESS:
      status = PSA_SUCCESS;
      break;
    case KWE_ERROR:
      status = PSA_ERROR_HARDWARE_FAILURE;
      break;
    default:
      status = PSA_ERROR_HARDWARE_FAILURE;
      break;
  }

  return status;
}

/**
  * @}
  */

/** @defgroup INTERFACE_Exported_Functions_Group2 Key Wrap functions
  *  @brief   KWE key wrap APIs to interface with Mbed TLS
  *
@verbatim
  ==============================================================================
                      ##### Key Wrap functions #####
  ==============================================================================
    [..]
      This subsection provides a set of functions allowing to wrap or generate
      wrapped private keys.


@endverbatim
  * @{
  */

/**
  * @brief  This function generates a private key in wrapped form using
  *         KWE hardware crypto accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer where the wrapped key is to be
  *         stored.
  * @param  key_buffer_size : size of the wrapped key buffer in bytes.
  * @param  p_key_buffer_length : a pointer to the actual size of the wrapped
  *         key in bytes.
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_generate_key(
  const psa_key_attributes_t *p_attributes,
  uint8_t *p_key_buffer, size_t key_buffer_size,
  size_t *p_key_buffer_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  psa_key_type_t key_type = psa_get_key_type(p_attributes);
  psa_algorithm_t key_alg = psa_get_key_algorithm(p_attributes);
  mbedtls_ecp_group ecp_group;
  KWE_EcpTypeDef ecp_tmp = {0};
  KWE_AlgTypeDef alg;
  size_t curve_bits = 0U;

#endif /* MBEDTLS_ECDSA_C || MBEDTLS_ECDH_C */

#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  if (PSA_KEY_TYPE_IS_ECC(key_type))
  {
    if PSA_ALG_IS_ECDSA(key_alg)
    {
      alg = KWE_ALG_ECC_ECDSA;
    }
    else if (PSA_ALG_IS_ECDH(key_alg))
    {
      alg = KWE_ALG_ECC_ECDH;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    mbedtls_ecp_group_init(&ecp_group);
    curve_bits = psa_get_key_bits(p_attributes);

    ecp_group.id = mbedtls_ecc_group_from_psa(PSA_KEY_TYPE_ECC_GET_FAMILY(key_type),
                                              curve_bits);

    if (ecp_group.id == MBEDTLS_ECP_DP_NONE)
    {
      return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = mbedtls_to_psa_error(mbedtls_ecp_group_load(&ecp_group,
                                                         ecp_group.id));

    if (status != PSA_SUCCESS)
    {
      return status;
    }

    ecp_tmp.modulus_size = ecp_group.st_modulus_size;
    ecp_tmp.order_size   = ecp_group.st_order_size;
    ecp_tmp.p_prime      = ecp_group.st_p;
    ecp_tmp.a_sign       = ecp_group.st_a_sign;
    ecp_tmp.p_a_abs      = ecp_group.st_a_abs;
    ecp_tmp.p_b          = ecp_group.st_b;
    ecp_tmp.p_gx         = ecp_group.st_gx;
    ecp_tmp.p_gy         = ecp_group.st_gy;
    ecp_tmp.p_n          = ecp_group.st_n;

    status = kwe_to_psa_error(KWE_GenerateWrappedEccKey(&ecp_tmp, alg,
                                                        p_key_buffer, key_buffer_size,
                                                        p_key_buffer_length));
    if (status != PSA_SUCCESS)
    {
      return status;
    }

  }
  else
#endif /* MBEDTLS_ECDSA_C || MBEDTLS_ECDH_C */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  This function imports a transparent key and wraps it using KWE
  *         hardware crypto accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_data : a pointer to data buffer contain the transparent key.
  * @param  data_length : size of transparent key in bytes.
  * @param  p_key_buffer : a pointer to buffer where the wrapped key is to be
  *         stored.
  * @param  key_buffer_size : size of the wrapped key buffer in bytes.
  * @param  p_key_buffer_length : a pointer to the actual size of the wrapped
  *         key in bytes.
  * @param  bits : the actual size of wrapped key in bits.
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_import_key(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_data, size_t data_length,
  uint8_t *p_key_buffer, size_t key_buffer_size,
  size_t *p_key_buffer_length, size_t *bits)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
  psa_key_type_t key_type = psa_get_key_type(p_attributes);
  psa_algorithm_t key_alg = psa_get_key_algorithm(p_attributes);
#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_AES_C)
  KWE_AlgTypeDef alg;
#endif /* MBEDTLS_ECDSA_C || MBEDTLS_AES_C */
  
#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  mbedtls_ecp_keypair *ecp = NULL;
  KWE_EcpTypeDef ecp_tmp = {0};
  size_t curve_bits = 0U;
#endif /* MBEDTLS_ECDSA_C || MBEDTLS_ECDH_C */

#if defined(MBEDTLS_RSA_C)
  mbedtls_rsa_context *rsa = NULL;
  KWE_RsaTypeDef rsa_tmp = {0};
#endif /* MBEDTLS_RSA_C */

  /* key type shall not be RAW_DATA */
  if (key_type == PSA_KEY_TYPE_RAW_DATA)
  {
    return PSA_ERROR_NOT_SUPPORTED;
  }

#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  if (PSA_KEY_TYPE_IS_ECC(key_type))
  {
    if PSA_ALG_IS_ECDSA(key_alg)
    {
      alg = KWE_ALG_ECC_ECDSA;
    }
    else if (PSA_ALG_IS_ECDH(key_alg))
    {
      alg = KWE_ALG_ECC_ECDH;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    /* RSSE user key is already wrapped using STM32 Key Wrap Engine (KWE) */
    if (PSA_KWE_KEY_ID_IS_RSSE(p_attributes->id) != 0U)
    {
      /* Get IV and Tag from the RSSE blob */
      (void) memcpy((void *)p_key_buffer, (void *)(p_data + PSA_KWE_RSSE_BLOB_IV_OFFSET), PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE);

      /* Get wrapped key from the RSSE blob */
      (void) memcpy((void *)(p_key_buffer + PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE),
                    (void *)(p_data + PSA_KWE_RSSE_BLOB_KEY_OFFSET), (key_buffer_size - PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE));

      *p_key_buffer_length = key_buffer_size;

      if (bits != NULL)
      {
        if (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_MONTGOMERY)
        {
          *bits = psa_get_key_bits(p_attributes) + 1U;
        }
        else
        {
          *bits = psa_get_key_bits(p_attributes);
        }
      }

      status = PSA_SUCCESS;
    }
    else
    {
      curve_bits = psa_get_key_bits(p_attributes);

      status = mbedtls_to_psa_error(mbedtls_psa_ecp_load_representation(key_type, curve_bits,
                                                                        p_data, data_length, &ecp));
      if (status != PSA_SUCCESS)
      {
        return status;
      }

      ecp_tmp.modulus_size = ecp->MBEDTLS_PRIVATE(grp).st_modulus_size;
      ecp_tmp.order_size   = ecp->MBEDTLS_PRIVATE(grp).st_order_size;
      ecp_tmp.p_prime      = ecp->MBEDTLS_PRIVATE(grp).st_p;
      ecp_tmp.a_sign       = ecp->MBEDTLS_PRIVATE(grp).st_a_sign;
      ecp_tmp.p_a_abs      = ecp->MBEDTLS_PRIVATE(grp).st_a_abs;
      ecp_tmp.p_b          = ecp->MBEDTLS_PRIVATE(grp).st_b;
      ecp_tmp.p_gx         = ecp->MBEDTLS_PRIVATE(grp).st_gx;
      ecp_tmp.p_gy         = ecp->MBEDTLS_PRIVATE(grp).st_gy;
      ecp_tmp.p_n          = ecp->MBEDTLS_PRIVATE(grp).st_n;

      status = kwe_to_psa_error(KWE_WrapEccKey(&ecp_tmp, alg, p_data,
                                               p_key_buffer, key_buffer_size,
                                               p_key_buffer_length));
      if (status != PSA_SUCCESS)
      {
        return status;
      }

      if (bits != NULL)
      {
        if (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_MONTGOMERY)
        {
          *bits = ecp->MBEDTLS_PRIVATE(grp).nbits + 1U;
        }
        else
        {
          *bits = ecp->MBEDTLS_PRIVATE(grp).nbits;
        }
      }
    }

    /* Free ECP context */
    mbedtls_ecp_keypair_free(ecp);
    mbedtls_free(ecp);
  }
  else
#endif /* MBEDTLS_ECDSA_C || MBEDTLS_ECDH_C */
#if defined(MBEDTLS_RSA_C)
    if (PSA_KEY_TYPE_IS_RSA(key_type))
    {
      /* RSSE user key is already wrapped using STM32 Key Wrap Engine (KWE) */
      if (PSA_KWE_KEY_ID_IS_RSSE(p_attributes->id) != 0U)
      {
        if (bits != NULL)
        {
          *bits = psa_get_key_bits(p_attributes);
        }

        /* Get IV and Tag from the RSSE blob */
        (void) memcpy(p_key_buffer, (p_data + PSA_KWE_RSSE_BLOB_IV_OFFSET), PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE);

        /* Get wrapped key exponent from the RSSE blob */
        (void) memcpy((void *)(p_key_buffer + PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE),
                      (void *)(p_data + PSA_KWE_RSSE_BLOB_KEY_OFFSET), PSA_BITS_TO_BYTES(*bits));

        /* Get wrapped key phi from the RSSE blob */
        (void) memcpy((void *)(p_key_buffer + PSA_KWE_DRIVER_KEY_CONTEXT_BASE_SIZE + PSA_BITS_TO_BYTES(*bits)),
                      (void *)(p_data + PSA_KWE_RSSE_BLOB_KEY_OFFSET + PSA_BITS_TO_BYTES(*bits)), PSA_BITS_TO_BYTES(*bits));

        *p_key_buffer_length = key_buffer_size;

        status = PSA_SUCCESS;
      }
      else /* RSA transparent key wrap using STM32 Key Wrap Engine (KWE) */
      {
        status = mbedtls_to_psa_error(mbedtls_psa_rsa_load_representation(key_type, p_data,
                                                                          data_length, &rsa));
        if (status != PSA_SUCCESS)
        {
          return status;
        }

        /* Temporaries holding P-1, Q-1 */
        mbedtls_mpi p1;
        mbedtls_mpi q1;
        mbedtls_mpi phi;
        mbedtls_mpi_init(&p1);
        mbedtls_mpi_init(&q1);
        mbedtls_mpi_init(&phi);

        /* P1 = p - 1 */
        status = mbedtls_to_psa_error(mbedtls_mpi_sub_int(&p1, &rsa->MBEDTLS_PRIVATE(P), 1));
        if (status != PSA_SUCCESS)
        {
          goto cleanup;
        }

        /* Q1 = q - 1 */
        status = mbedtls_to_psa_error(mbedtls_mpi_sub_int(&q1, &rsa->MBEDTLS_PRIVATE(Q), 1));
        if (status != PSA_SUCCESS)
        {
          goto cleanup;
        }

        /* phi = ( p - 1 ) * ( q - 1 ) */
        status = mbedtls_to_psa_error(mbedtls_mpi_mul_mpi(&phi, &p1, &q1));
        if (status != PSA_SUCCESS)
        {
          goto cleanup;
        }
        mbedtls_mpi_core_bigendian_to_host(phi.MBEDTLS_PRIVATE(p), phi.MBEDTLS_PRIVATE(n));
        mbedtls_mpi_core_bigendian_to_host(rsa->MBEDTLS_PRIVATE(D).MBEDTLS_PRIVATE(p),
                                           rsa->MBEDTLS_PRIVATE(D).MBEDTLS_PRIVATE(n));
        mbedtls_mpi_core_bigendian_to_host(rsa->MBEDTLS_PRIVATE(N).MBEDTLS_PRIVATE(p),
                                           rsa->MBEDTLS_PRIVATE(N).MBEDTLS_PRIVATE(n));

        rsa_tmp.phi_size = phi.MBEDTLS_PRIVATE(n);
        rsa_tmp.p_phi = (uint8_t *)phi.MBEDTLS_PRIVATE(p);

        rsa_tmp.modulus_size = 4U * rsa->MBEDTLS_PRIVATE(N).MBEDTLS_PRIVATE(n);
        rsa_tmp.p_modulus = (uint8_t *)rsa->MBEDTLS_PRIVATE(N).MBEDTLS_PRIVATE(p);

        rsa_tmp.exponent_size = 4U * rsa->MBEDTLS_PRIVATE(D).MBEDTLS_PRIVATE(n);
        rsa_tmp.p_exponent = (uint8_t *)rsa->MBEDTLS_PRIVATE(D).MBEDTLS_PRIVATE(p);

        status = kwe_to_psa_error(KWE_WrapRsaKey(&rsa_tmp, p_key_buffer,
                                                 p_key_buffer_length));
        if (status != PSA_SUCCESS)
        {
          return status;
        }

        if (bits != NULL)
        {
          *bits = PSA_BYTES_TO_BITS(rsa_tmp.modulus_size);
        }

cleanup:
        mbedtls_mpi_free(&p1);
        mbedtls_mpi_free(&q1);
        mbedtls_mpi_free(&phi);
      }
    }
    else
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_AES_C)
      if (key_type == PSA_KEY_TYPE_AES)
      {
        if (key_alg == PSA_ALG_ECB_NO_PADDING)
        {
          alg = KWE_ALG_AES_ECB;
        }
        else if (key_alg == PSA_ALG_CBC_NO_PADDING)
        {
          alg = KWE_ALG_AES_CBC;
        }
        else if (key_alg == PSA_ALG_GCM)
        {
          alg = KWE_ALG_AES_GCM;
        }
        else if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(key_alg, 0)== PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0))
        {
          alg = KWE_ALG_AES_CCM;
        }
        else
        {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        UNUSED(alg);

        /* RSSE user key is already wrapped using STM32 Key Wrap Engine (KWE) */
        if (PSA_KWE_KEY_ID_IS_RSSE(p_attributes->id) != 0U)
        {
          if (bits != NULL)
          {
            *bits = psa_get_key_bits(p_attributes);
          }
#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
          /* Get IV from the RSSE blob */
          (void) memcpy((void *)p_key_buffer, (void *)(p_data + PSA_KWE_RSSE_BLOB_IV_OFFSET), PSA_KWE_DRIVER_IV_BASE_SIZE);
          /* Get wrapped key from the RSSE blob */
          (void) memcpy((void *)(p_key_buffer + PSA_KWE_DRIVER_IV_BASE_SIZE),
                        (void *)(p_data + PSA_KWE_RSSE_BLOB_KEY_OFFSET), PSA_BITS_TO_BYTES(*bits));
#else
          /* Get wrapped key  from the RSSE blob */
          (void) memcpy((p_key_buffer), (void *)(p_data + PSA_KWE_RSSE_BLOB_KEY_OFFSET), key_buffer_size);
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

          *p_key_buffer_length = key_buffer_size;

          status = PSA_SUCCESS;
        }
        else /* AES transparent key wrap using STM32 Key Wrap Engine (KWE) */
        {
#if defined (KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY)
          status = psa_generate_random(p_key_buffer, 16U);
          if (status != PSA_SUCCESS)
          {
            return status;
          }
#endif  /* KWE_USE_CBC_TO_WRAP_SYMMETRIC_KEY */

          status = kwe_to_psa_error(KWE_WrapAESKey(p_data, data_length,
                                                   p_key_buffer, key_buffer_size,
                                                   p_key_buffer_length));

          if (status != PSA_SUCCESS)
          {
            return status;
          }

          if (bits != NULL)
          {
            *bits =  PSA_BYTES_TO_BITS(data_length);
          }
        }
      }
      else
#endif /* MBEDTLS_AES_C */
      {
        status = PSA_ERROR_NOT_SUPPORTED;
      }

  return status;
}

/**
  * @}
  */

/** @defgroup INTERFACE_Exported_Functions_Group3 Asymmetric Key functions
  *  @brief   KWE Asymmetric Key APIs to interface with Mbed TLS
  *
@verbatim
  ==============================================================================
                      ##### Asymmetric Key functions #####
  ==============================================================================
    [..]
      This subsection provides a set of functions allowing to export a public
      key or calcule a shared secret from a wrapped private key.


@endverbatim
  * @{
  */

/**
  * @brief  This function exports a public key from a wrapped private key
  *         using KWE hardware crypto accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key : a pointer to buffer that contain wrapped private key.
  * @param  key_length : size of the wrapped key buffer in bytes.
  * @param  p_data : Buffer where the public key data is to be written
  * @param  data_size : Size of the public key buffer in bytes.
  * @param  p_data_length : the actual size of the public key.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_export_public_key(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key, size_t key_length,
  uint8_t *p_data, size_t data_size, size_t *p_data_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  psa_key_type_t key_type = psa_get_key_type(p_attributes);
  mbedtls_ecp_keypair *ecp = NULL;
  KWE_EcpTypeDef ecp_tmp = {0};
  size_t curve_bytes = 0U;
  size_t curve_bits = 0U;

#endif /* MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_ECDH_C)
  if (PSA_KEY_TYPE_IS_ECC(key_type))
  {
    curve_bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));
    curve_bits = psa_get_key_bits(p_attributes);

    status = mbedtls_psa_ecp_load_representation(key_type, curve_bits, p_key + 1U,
                                                 curve_bytes, &ecp);
    if (status != PSA_SUCCESS)
    {
      return status;
    }

    ecp_tmp.modulus_size = ecp->MBEDTLS_PRIVATE(grp).st_modulus_size;
    ecp_tmp.order_size   = ecp->MBEDTLS_PRIVATE(grp).st_order_size;
    ecp_tmp.p_prime      = ecp->MBEDTLS_PRIVATE(grp).st_p;
    ecp_tmp.a_sign       = ecp->MBEDTLS_PRIVATE(grp).st_a_sign;
    ecp_tmp.p_a_abs      = ecp->MBEDTLS_PRIVATE(grp).st_a_abs;
    ecp_tmp.p_b          = ecp->MBEDTLS_PRIVATE(grp).st_b;
    ecp_tmp.p_gx         = ecp->MBEDTLS_PRIVATE(grp).st_gx;
    ecp_tmp.p_gy         = ecp->MBEDTLS_PRIVATE(grp).st_gy;
    ecp_tmp.p_n          = ecp->MBEDTLS_PRIVATE(grp).st_n;

    status = kwe_to_psa_error(KWE_EcdsaPublicKeyExport(&ecp_tmp, p_key,
                                                       p_data, data_size, p_data_length));
    /* Free ECP context */
    mbedtls_ecp_keypair_free(ecp);
    mbedtls_free(ecp);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_ECDSA_C */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  This function performs key agreement using an asymmetric
  *         wrapped private key through KWE hardware accelerator
  *         and a peer public key.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped private
  *         key.
  * @param  key_buffer_size : size of the wrapped private key buffer in bytes.
  * @param  alg : key agreement algorithm.
  * @param  p_peer_key : a pointer to the peer public key.
  * @param  peer_key_length : size of the peer public key in bytes.
  * @param  p_shared_secret : a pointer to buffer where the shared secret
  *         data is to be written.
  * @param  shared_secret_size : Size of shared secret buffer in bytes.
  * @param  p_shared_secret_length : a pointer to the actual size of outputted
  *         shared secret in bytes.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_key_agreement(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_peer_key, size_t peer_key_length,
  uint8_t *p_shared_secret, size_t shared_secret_size,
  size_t *p_shared_secret_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
  psa_key_type_t key_type = psa_get_key_type(p_attributes);

#if defined(MBEDTLS_ECDH_C)
  mbedtls_ecp_keypair *ecp = NULL;
  KWE_EcpTypeDef ecp_tmp = {0};
  size_t curve_bytes = 0;
  uint8_t *p_shared_secret_xy = NULL;
  size_t p_shared_secret_length_xy = 0;
#endif /* MBEDTLS_ECDH_C */

  if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(key_type) ||
      !PSA_ALG_IS_ECDH(alg))
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

#if defined(MBEDTLS_ECDH_C)
  if (PSA_ALG_IS_ECDH(alg))
  {
    curve_bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));

    status = mbedtls_psa_ecp_load_representation(key_type, 0U, p_key_buffer,
                                                 curve_bytes,
                                                 &ecp);

    if (status != PSA_SUCCESS)
    {
      return status;
    }

    ecp_tmp.modulus_size = ecp->MBEDTLS_PRIVATE(grp).st_modulus_size;
    ecp_tmp.order_size   = ecp->MBEDTLS_PRIVATE(grp).st_order_size;
    ecp_tmp.p_prime      = ecp->MBEDTLS_PRIVATE(grp).st_p;
    ecp_tmp.a_sign       = ecp->MBEDTLS_PRIVATE(grp).st_a_sign;
    ecp_tmp.p_a_abs      = ecp->MBEDTLS_PRIVATE(grp).st_a_abs;
    ecp_tmp.p_b          = ecp->MBEDTLS_PRIVATE(grp).st_b;
    ecp_tmp.p_gx         = ecp->MBEDTLS_PRIVATE(grp).st_gx;
    ecp_tmp.p_gy         = ecp->MBEDTLS_PRIVATE(grp).st_gy;
    ecp_tmp.p_n          = ecp->MBEDTLS_PRIVATE(grp).st_n;

    /* Allocate memory space for computed secret */
    p_shared_secret_xy = mbedtls_calloc((2U * ecp_tmp.order_size), sizeof(uint8_t));

    status = kwe_to_psa_error(KWE_EcdhKeyAgreement(&ecp_tmp, p_key_buffer,
                                                   p_peer_key, peer_key_length,
                                                   p_shared_secret_xy, shared_secret_size,
                                                   &p_shared_secret_length_xy));
    /* Only shared secret X part is exported */
    *p_shared_secret_length = p_shared_secret_length_xy / 2U;

    /* Copy the shared secret X part */
    (void) memcpy(p_shared_secret, p_shared_secret_xy, *p_shared_secret_length);

    /* Free the p_shared_secret_xy buffer */
    mbedtls_zeroize_and_free(p_shared_secret_xy, (2U * ecp_tmp.order_size));

    if (status != PSA_SUCCESS)
    {
      return status;
    }

    if (curve_bytes != *p_shared_secret_length)
    {
      status = PSA_ERROR_CORRUPTION_DETECTED;
    }

    /* Free ECP context */
    mbedtls_ecp_keypair_free(ecp);
    mbedtls_free(ecp);

  }
  else
#endif /* MBEDTLS_ECDH_C */

  {
    status =  PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}
/**
  * @}
  */

/** @defgroup INTERFACE_Exported_Functions_Group4 Asymmetric Cryptography functions
  *  @brief   KWE Asymmetric Cryptography APIs to interface with Mbed TLS
  *
@verbatim
  ==============================================================================
                      ##### Asymmetric Cryptography functions #####
  ==============================================================================
    [..]
      This subsection provides a set of functions allowing to sign a message or
      hash, decryp a message using a wrapped private key.


@endverbatim
  * @{
  */

/**
  * @brief  A function that signs a hash with asymmetric wrapped private key
  *         through KWE hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped private
  *                        key.
  * @param  key_buffer_size : size of the wrapped private key buffer in bytes.
  * @param  alg : a signature algorithm (ECDSA or RSA).
  * @param  p_hash : a pointer to the hash to sign.
  * @param  hash_length : size of hash buffer in bytes.
  * @param  p_signature : a pointer to signature buffer to store signature.
  * @param  signature_size : size of signature buffer in bytes.
  * @param  p_signature_length : a pointer to actual size of outputted signature
                                 in bytes.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_signature_sign_hash(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_hash, size_t hash_length,
  uint8_t *p_signature, size_t signature_size, size_t *p_signature_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
#if defined(MBEDTLS_ECDSA_C) || defined(MBEDTLS_RSA_C)
  psa_key_type_t key_type = psa_get_key_type(p_attributes);
#endif /* MBEDTLS_ECDSA_C || MBEDTLS_RSA_C */

#if defined(MBEDTLS_ECDSA_C)
  mbedtls_ecp_keypair *ecp = NULL;
  KWE_EcpTypeDef ecp_tmp = {0};
  size_t curve_bytes = 0;
  uint8_t *p_digest = NULL;
#endif /* MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_RSA_C)
  /* mbedtls_rsa_context *rsa = NULL; */
  mbedtls_md_type_t md_alg;
  unsigned char md_size;
  unsigned char *tmp_sig = NULL;
  KWE_RsaTypeDef rsa_tmp = {0};
#if defined(MBEDTLS_PKCS1_V21)
  const mbedtls_md_info_t *md_info;
  size_t slen = 0;
  size_t min_slen = 0;
  size_t hlen = 0;
  unsigned char *p = NULL;
  unsigned char *salt = NULL;
  mbedtls_md_context_t md_ctx;
  size_t offset = 0;
  size_t msb = 0;
#endif /* MBEDTLS_PKCS1_V21 */
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_ECDSA_C)
  if (PSA_KEY_TYPE_IS_ECC(key_type))
  {
    curve_bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));
    status = mbedtls_psa_ecp_load_representation(key_type, 0u, p_key_buffer,
                                                 curve_bytes,
                                                 &ecp);
    if (status != PSA_SUCCESS)
    {
      return status;
    }

    ecp_tmp.modulus_size = ecp->MBEDTLS_PRIVATE(grp).st_modulus_size;
    ecp_tmp.order_size   = ecp->MBEDTLS_PRIVATE(grp).st_order_size;
    ecp_tmp.p_prime      = ecp->MBEDTLS_PRIVATE(grp).st_p;
    ecp_tmp.a_sign       = ecp->MBEDTLS_PRIVATE(grp).st_a_sign;
    ecp_tmp.p_a_abs      = ecp->MBEDTLS_PRIVATE(grp).st_a_abs;
    ecp_tmp.p_b          = ecp->MBEDTLS_PRIVATE(grp).st_b;
    ecp_tmp.p_gx         = ecp->MBEDTLS_PRIVATE(grp).st_gx;
    ecp_tmp.p_gy         = ecp->MBEDTLS_PRIVATE(grp).st_gy;
    ecp_tmp.p_n          = ecp->MBEDTLS_PRIVATE(grp).st_n;

    p_digest = mbedtls_calloc((ecp_tmp.order_size), sizeof(uint8_t));

    ecc_hal_prepare_digest(p_hash, hash_length,
                           p_digest, ecp_tmp.order_size, ecp_tmp.p_n);

    status = KWE_EcdsaSignHash(&ecp_tmp, p_key_buffer, p_digest, p_signature,
                               signature_size, p_signature_length);

    mbedtls_zeroize_and_free(p_digest, (ecp_tmp.order_size));

    /* Free ECP context */
    mbedtls_ecp_keypair_free(ecp);
    mbedtls_free(ecp);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_ECDSA_C */

#if defined(MBEDTLS_RSA_C)
    if (PSA_KEY_TYPE_IS_RSA(key_type))
    {
      md_alg = mbedtls_md_type_from_psa_alg(PSA_ALG_SIGN_GET_HASH(alg));
      md_size = mbedtls_md_get_size_from_type(md_alg);
      memset(p_signature, 0, signature_size);

      /* For signatures using a hash, the hash length must be correct. */
      if (alg != PSA_ALG_RSA_PKCS1V15_SIGN_RAW)
      {
        if (md_alg == MBEDTLS_MD_NONE)
        {
          return PSA_ERROR_NOT_SUPPORTED;
        }
        if (md_size != hash_length)
        {
          return PSA_ERROR_INVALID_ARGUMENT;
        }
      }

      rsa_tmp.modulus_size = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));
      rsa_tmp.exponent_size = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));

      tmp_sig = mbedtls_calloc(1, signature_size);
      if (tmp_sig == NULL)
      {
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
      }
#if defined(MBEDTLS_PKCS1_V15)
      if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg))
      {
        if ((md_alg != MBEDTLS_MD_NONE || hash_length != 0) && p_hash == NULL)
        {
          return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        }
        /*
         * Prepare PKCS1-v1.5 encoding (padding and hash identifier)
         */
        status = mbedtls_to_psa_error(rsa_rsassa_pkcs1_v15_encode(md_alg, hash_length,
                                                                  p_hash, signature_size, tmp_sig));
        if (status != PSA_SUCCESS)
        {
          goto cleanup;
        }
        /* Private key operation
         *
         * In order to prevent Lenstra's attack, make the signature in a
         * temporary buffer and check it before returning it.
         */

        status = KWE_RsaModularExp(&rsa_tmp, p_key_buffer,
                                   tmp_sig, p_signature);

        if (status != PSA_SUCCESS)
        {
          goto cleanup;
        }

        *p_signature_length = signature_size;
      }
      else
#endif  /* MBEDTLS_PKCS1_V15 */
#if defined(MBEDTLS_PKCS1_V21)
        if (PSA_ALG_IS_RSA_PSS(alg))
        {
          md_info = mbedtls_md_info_from_type(md_alg);
          /* Just make sure this hash is supported in this build. */
          if (md_info == NULL)
          {
            return MBEDTLS_ERR_RSA_INVALID_PADDING;
          }
          hlen = mbedtls_md_get_size(md_info);
          /* Calculate the largest possible salt length, up to the hash size.
           * Normally this is the hash length, which is the maximum salt length
           * according to FIPS 185-4 5.5 (e) and common practice. If there is not
           * enough room, use the maximum salt length that fits. The constraint is
           * that the hash length plus the salt length plus 2 bytes must be at most
           * the key length. This complies with FIPS 186-4 par. 5.5 (e) and RFC 8017
           * (PKCS#1 v2.2) par. 9.1.1 step 3. */
          min_slen = hlen - 2;
          if (signature_size < (hlen + min_slen + 2))
          {
            return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
          }
          else if (signature_size >= (hlen + hlen + 2))
          {
            slen = hlen;
          }
          else
          {
            slen = signature_size - hlen - 2;
          }

          p = tmp_sig;
          p += signature_size - hlen - slen - 2;
          *p++ = 0x01;

          /* Generate salt of length slen in place in the encoded message */
          salt = p;
          status = psa_generate_random(salt, slen);
          if (status != PSA_SUCCESS)
          {
            goto cleanup;
          }

          p += slen;

          mbedtls_md_init(&md_ctx);
          status = mbedtls_to_psa_error(mbedtls_md_setup(&md_ctx, md_info, 0));
          if (status != 0)
          {
            goto cleanup;
          }

          /* Generate H = Hash( M' ) */
          status = mbedtls_to_psa_error(mbedtls_md_starts(&md_ctx));
          if (status != 0)
          {
            goto cleanup;
          }
          status = mbedtls_to_psa_error(mbedtls_md_update(&md_ctx, p, 8));
          if (status != 0)
          {
            goto cleanup;
          }
          status = mbedtls_to_psa_error(mbedtls_md_update(&md_ctx, p_hash, hash_length));
          if (status != 0)
          {
            goto cleanup;
          }
          status = mbedtls_to_psa_error(mbedtls_md_update(&md_ctx, salt, slen));
          if (status != 0)
          {
            goto cleanup;
          }
          status = mbedtls_to_psa_error(mbedtls_md_finish(&md_ctx, p));
          if (status != 0)
          {
            goto cleanup;
          }

          /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
          /* msb = mbedtls_mpi_bitlen(&ctx->N) - 1; */
          msb = psa_get_key_bits(p_attributes) - 1;

          /* Compensate for boundary condition when applying mask */
          if (msb % 8 == 0)
          {
            offset = 1;
          }

          /* maskedDB: Apply dbMask to DB */
          status = mbedtls_to_psa_error(mgf_mask((tmp_sig + offset),
                                                 (signature_size - hlen - 1 - offset),
                                                 p, hlen, md_alg));
          if (status != 0)
          {
            goto cleanup;
          }

          tmp_sig[0] &= 0xFF >> (signature_size * 8 - msb);

          p += hlen;
          *p++ = 0xBC;

          status = KWE_RsaModularExp(&rsa_tmp, p_key_buffer,
                                     tmp_sig, p_signature);

          if (status != PSA_SUCCESS)
          {
            goto cleanup;
          }

          *p_signature_length = signature_size;
        }
        else
#endif  /* MBEDTLS_PKCS1_V21 */
        {
          status = PSA_ERROR_INVALID_ARGUMENT;
        }

cleanup:
      mbedtls_zeroize_and_free(tmp_sig, signature_size);
#if defined(MBEDTLS_PKCS1_V21)
      mbedtls_md_free(&md_ctx);
#endif  /* MBEDTLS_PKCS1_V21 */

    }
    else
#endif /* MBEDTLS_RSA_C */
    {
      status = PSA_ERROR_NOT_SUPPORTED;
    }

  return status;
}

/**
  * @brief  A function that signs a message with asymmetric wrapped private key
  *         through KWE hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped private
  *                        key.
  * @param  key_buffer_size : size of the wrapped private key buffer in bytes.
  * @param  alg : a signature algorithm.
  * @param  p_input : a pointer to the message to sign.
  * @param  input_length : size of message buffer in bytes.
  * @param  p_signature : a pointer to signature buffer to store signature.
  * @param  signature_size : size of signature buffer in bytes.
  * @param  p_signature_length : a pointer to actual size of outputted signature
                                 in bytes.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_signature_sign_message(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg, const uint8_t *p_input, size_t input_length,
  uint8_t *p_signature, size_t signature_size, size_t *p_signature_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

  if (PSA_ALG_IS_SIGN_HASH(alg))
  {
    size_t hash_length = 0;
    uint8_t hash[PSA_HASH_MAX_SIZE];

    status = psa_driver_wrapper_hash_compute(PSA_ALG_SIGN_GET_HASH(alg),
                                             p_input, input_length,
                                             hash, sizeof(hash), &hash_length);

    if (status != PSA_SUCCESS)
    {
      return status;
    }

    status =  mbedtls_kwe_opaque_signature_sign_hash(p_attributes,
                                                     p_key_buffer, key_buffer_size,
                                                     alg, hash, hash_length, p_signature,
                                                     signature_size, p_signature_length);
    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
  {
    status = PSA_ERROR_INVALID_ARGUMENT;
  }

  return status;
}

/**
  * @brief  A function that performs asymmetric decryption operation using an
  *         asymmetric wrapped private key through KWE hardware
  *          accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped private
  *                        key.
  * @param  key_buffer_size : size of the wrapped private key buffer in bytes.
  * @param  alg : asymmetric key algorithm.
  * @param  p_input : The message to decrypt.
  * @param  input_length : Size of the message buffer in bytes.
  * @param  p_salt : A pointer to salt or label, if supported by the decryption
  *                  algorithm.
  * @param  salt_length : Size of the salt buffer in bytes.
  * @param  p_output : Buffer where the decrypted message is to be written.
  * @param  output_size : Size of the decrypted message buffer in bytes.
  * @param  p_output_length : the number of bytes that make up the returned
  *                           output.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t mbedtls_kwe_opaque_asymmetric_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_input, size_t input_length,
  const uint8_t *p_salt, size_t salt_length,
  uint8_t *p_output, size_t output_size, size_t *p_output_length)
{
#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PKCS1_V21)
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;
  psa_key_type_t type = psa_get_key_type(p_attributes);

  int ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  KWE_RsaTypeDef rsa_tmp = {0};
  size_t ilen = 0;
  size_t i = 0;
  size_t pad_len = 0;
  unsigned char *p = NULL;
  mbedtls_ct_condition_t bad = 0;
  mbedtls_ct_condition_t in_padding = MBEDTLS_CT_TRUE;
  unsigned char buf[MBEDTLS_MPI_MAX_SIZE] = {0};
  unsigned char lhash[MBEDTLS_MD_MAX_SIZE] = {0};
  unsigned int hlen = 0;
  const mbedtls_md_info_t *md_info = NULL;
  mbedtls_md_context_t md_ctx;
  mbedtls_md_init(&md_ctx);
  mbedtls_md_type_t md_alg;

  if (PSA_KEY_TYPE_IS_RSA(type))
  {
    rsa_tmp.modulus_size = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));
    rsa_tmp.exponent_size = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));

    status = kwe_to_psa_error(KWE_RsaModularExp(&rsa_tmp, p_key_buffer,
                                                p_input, buf));

    if (status != PSA_SUCCESS)
    {
      return status;
    }

    ilen = PSA_BITS_TO_BYTES(psa_get_key_bits(p_attributes));

    if (ilen < 16  ||  ilen > sizeof(buf))
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    md_alg = mbedtls_md_type_from_psa_alg(PSA_ALG_RSA_OAEP_GET_HASH(alg));
    md_info = mbedtls_md_info_from_type(md_alg);
    if (md_info == NULL)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    hlen = mbedtls_md_get_size(md_info);

    /* checking for integer underflow */
    if (2 * hlen + 2 > ilen)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    /*
     * Unmask data and generate lHash
     */
    if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0)
    {
      mbedtls_md_free(&md_ctx);
      goto cleanup;
    }

    /* seed: Apply seedMask to maskedSeed */
    if ((ret = mgf_mask(buf + 1, hlen, buf + hlen + 1, ilen - hlen - 1,
                        md_alg)) != 0 ||
        /* DB: Apply dbMask to maskedDB */
        (ret = mgf_mask(buf + hlen + 1, ilen - hlen - 1, buf + 1, hlen,
                        md_alg)) != 0)
    {
      mbedtls_md_free(&md_ctx);
      goto cleanup;
    }

    mbedtls_md_free(&md_ctx);

    /* Generate lHash */
    if ((ret = mbedtls_md(md_info, p_salt, salt_length, lhash)) != 0)
    {
      goto cleanup;
    }

    /*
     * Check contents, in "constant-time"
     */
    p = buf;

    bad = mbedtls_ct_bool(*p++); /* First byte must be 0 */

    p += hlen; /* Skip seed */


    /* Check lHash */
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_bool(mbedtls_ct_memcmp(lhash, p, hlen)));
    p += hlen;

    /* Get zero-padding len, but always read till end of buffer
     * (minus one, for the 01 byte) */
    pad_len = 0;
    for (i = 0; i < ilen - 2 * hlen - 2; i++)
    {
      in_padding = mbedtls_ct_bool_and(in_padding, mbedtls_ct_uint_eq(p[i], 0));
      pad_len += mbedtls_ct_uint_if_else_0(in_padding, 1);
    }

    p += pad_len;
    bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_ne(*p++, 0x01));

    /*
     * The only information "leaked" is whether the padding was correct or not
     * (eg, no data is copied if it was not correct). This meets the
     * recommendations in PKCS#1 v2.2: an opponent cannot distinguish between
     * the different error conditions.
     */
    if (bad != MBEDTLS_CT_FALSE)
    {
      ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
      goto cleanup;
    }

    if (ilen - (p - buf) > output_size)
    {
      ret = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
      goto cleanup;
    }

    *p_output_length = ilen - (p - buf);
    if (*p_output_length != 0U)
    {
      (void) memcpy(p_output, p, *p_output_length);
    }

cleanup:
    mbedtls_platform_zeroize(buf, sizeof(buf));
    mbedtls_platform_zeroize(lhash, sizeof(lhash));
    return mbedtls_to_psa_error(ret);
  }
  else
#endif /* MBEDTLS_RSA_C && MBEDTLS_PKCS1_V21 */
  {
    return PSA_ERROR_NOT_SUPPORTED;
  }
}

/**
  * @}
  */

/** @defgroup INTERFACE_Exported_Functions_Group5 Symmetric Cryptography functions
  *  @brief   KWE Symmetric Cryptography APIs to interface with Mbed TLS
  *
@verbatim
  ==============================================================================
                      ##### Symmetric Cryptography functions #####
  ==============================================================================
    [..]
      This subsection provides a set of functions allowing symmetric
     encryption or decryption operation using a wrapped private key.


@endverbatim
  * @{
  */

/**
  * @brief  A function that performs AES authenticated encryption operation
  *         using KWE hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.
  * @param  alg : a AEAD algorithm.
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
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_aead_encrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_AES_C) && (defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CCM_C))
  uint8_t tag_length = 0;
  KWE_AlgTypeDef alg_tmp;

  if (PSA_ALG_IS_AEAD(alg))
  {
    if (alg == PSA_ALG_GCM)
    {
      alg_tmp = KWE_ALG_AES_GCM;
    }
    else if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)==PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0))
    {
      alg_tmp = KWE_ALG_AES_CCM;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);

    status = KWE_AesAeadEncrypt(alg_tmp, p_key_buffer, key_buffer_size,
                                p_nonce, nonce_length,
                                p_additional_data, additional_data_length,
                                p_plaintext, plaintext_length,
                                p_ciphertext, ciphertext_size, p_ciphertext_length,
                                tag_length);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  A function that performs AES authenticated decryption operation
  *         using KWE hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.
  * @param  alg : a AEAD algorithm.
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
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_aead_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_nonce, size_t nonce_length,
  const uint8_t *p_additional_data, size_t additional_data_length,
  const uint8_t *p_ciphertext, size_t ciphertext_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_AES_C) && (defined(MBEDTLS_GCM_C) || defined(MBEDTLS_CCM_C))
  uint8_t tag_length = 0;
  KWE_AlgTypeDef alg_tmp;

  if (PSA_ALG_IS_AEAD(alg))
  {
    if (alg == PSA_ALG_GCM)
    {
      alg_tmp = KWE_ALG_AES_GCM;
    }
    else if (PSA_ALG_AEAD_WITH_SHORTENED_TAG(alg, 0)==PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0))
    {
      alg_tmp = KWE_ALG_AES_CCM;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);

    status = KWE_AesAeadDecrypt(alg_tmp, p_key_buffer, key_buffer_size,
                                p_nonce, nonce_length,
                                p_additional_data, additional_data_length,
                                p_ciphertext, ciphertext_length,
                                p_plaintext, plaintext_size, p_plaintext_length,
                                tag_length);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_GCM_C || MBEDTLS_CCM_C */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  A function that performs AES encryption cipher using KWE
  *         hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
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
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_cipher_encrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_iv, size_t iv_length,
  const uint8_t *p_plaintext, size_t plaintext_length,
  uint8_t *p_ciphertext, size_t ciphertext_size, size_t *p_ciphertext_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_CIPHER_C) && (defined(MBEDTLS_CIPHER_MODE_ECB) || defined(MBEDTLS_CIPHER_MODE_CBC))
  KWE_AlgTypeDef alg_tmp;

  if (PSA_ALG_IS_CIPHER(alg))
  {
    if (alg == PSA_ALG_ECB_NO_PADDING)
    {
      alg_tmp = KWE_ALG_AES_ECB;
    }
    else if (alg == PSA_ALG_CBC_NO_PADDING)
    {
      alg_tmp = KWE_ALG_AES_CBC;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    status = KWE_AesEncrypt(alg_tmp, p_key_buffer, key_buffer_size,
                            p_iv, iv_length,
                            p_plaintext, plaintext_length,
                            p_ciphertext, ciphertext_size, p_ciphertext_length);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_CIPHER_C && MBEDTLS_CIPHER_MODE_ECB || MBEDTLS_CIPHER_MODE_CBC */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
}

/**
  * @brief  A function that performs AES decryption operation using KWE
  *         hardware accelerator.
  * @param  p_attributes : a pointer to key attributes.
  * @param  p_key_buffer : a pointer to buffer that contain the wrapped key.
  * @param  key_buffer_size : size of the wrapped  key buffer in bytes.
  * @param  alg : a cipher algorithm.
  * @param  p_ciphertext : a pointer to encrypted data and IV.
  * @param  input_length : Size of encrypted data and IV.
  * @param  p_plaintext : a pointer to output buffer for the decrypted data.
  * @param  plaintext_size : Size of the output buffer in bytes.
  * @param  p_plaintext_length : the size of the actual decrypted data in bytes.
  * @retval PSA_SUCCESS if success, an error code otherwise
  */
psa_status_t mbedtls_kwe_opaque_cipher_decrypt(
  const psa_key_attributes_t *p_attributes,
  const uint8_t *p_key_buffer, size_t key_buffer_size,
  psa_algorithm_t alg,
  const uint8_t *p_ciphertext, size_t input_length,
  uint8_t *p_plaintext, size_t plaintext_size, size_t *p_plaintext_length)
{
  psa_status_t status = PSA_ERROR_HARDWARE_FAILURE;

#if defined(MBEDTLS_CIPHER_C) && (defined(MBEDTLS_CIPHER_MODE_ECB) || defined(MBEDTLS_CIPHER_MODE_CBC))
  KWE_AlgTypeDef alg_tmp;

  if (PSA_ALG_IS_CIPHER(alg))
  {
    if (alg == PSA_ALG_ECB_NO_PADDING)
    {
      alg_tmp = KWE_ALG_AES_ECB;
    }
    else if (alg == PSA_ALG_CBC_NO_PADDING)
    {
      alg_tmp = KWE_ALG_AES_CBC;
    }
    else
    {
      return PSA_ERROR_NOT_SUPPORTED;
    }

    status = KWE_AesDecrypt(alg_tmp, p_key_buffer, key_buffer_size,
                            p_ciphertext, input_length,
                            p_plaintext, plaintext_size, p_plaintext_length);

    if (status != PSA_SUCCESS)
    {
      return status;
    }
  }
  else
#endif /* MBEDTLS_CIPHER_C && MBEDTLS_CIPHER_MODE_ECB || MBEDTLS_CIPHER_MODE_CBC */
  {
    status = PSA_ERROR_NOT_SUPPORTED;
  }

  return status;
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
