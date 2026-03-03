/**
  ******************************************************************************
  * @file    ecdh_alt.c
  * @author  GPM Application Team
  * @brief   Implementation of mbedtls_alt Middleware ECDH module
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
  * Elliptic curve Diffie-Hellman compute shared functions
  * This file implements Elliptic curve Diffie-Hellman compute shared
  * functions based on STM32 PKA hardware crypto accelerator.
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#if defined(MBEDTLS_ECDH_C)
#if defined(MBEDTLS_ECP_ALT)
#include "mbedtls/ecdh.h"
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_HAL_ECDH_ALT)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST_ECDH_TIMEOUT     (5000U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if defined(MBEDTLS_ECDH_COMPUTE_SHARED_ALT)
/*
 * Compute shared secret (SEC1 3.3.1)
 */
static int ecdh_compute_shared_restartable(mbedtls_ecp_group *grp,
                                           mbedtls_mpi *z,
                                           const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                           int (*f_rng)(void *, unsigned char *, size_t),
                                           void *p_rng,
                                           mbedtls_ecp_restart_ctx *rs_ctx)
{

  PKA_HandleTypeDef hpka = {0};                         /* HAL Pka Handle */
  PKA_ECCMulExInTypeDef ECDH_input = {0};               /* ECDH Curve struct */
  PKA_ECCMulOutTypeDef ECDH_ouput = {0};                /* ECDH point = shared secret */
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;      /* MBED return value */
  uint8_t *d_binary = NULL;                             /* Pointer to private key */
  size_t olen = 0;                                      /* Length of the point, internal use */
  uint8_t *Q_binary = NULL;                             /* Pointer to public key */

  /* Enable HW peripheral clock */
  __HAL_RCC_PKA_CLK_ENABLE();

  /* Select the instance */
  hpka.Instance = PKA;
  /* Erase older context */
  MBEDTLS_MPI_CHK((HAL_PKA_DeInit(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  /* Initialize the PKA peripheral */
  MBEDTLS_MPI_CHK((HAL_PKA_Init(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  /* Set HW peripheral input parameter : private key */
  d_binary = mbedtls_calloc(grp->st_order_size, sizeof(uint8_t));
  MBEDTLS_MPI_CHK((d_binary == NULL) ? MBEDTLS_ERR_ECP_ALLOC_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(d, d_binary, grp->st_order_size));
  ECDH_input.scalarMul = d_binary;

  /* Set HW peripheral Input parameter */
  ECDH_input.modulusSize     = grp->st_modulus_size;
  ECDH_input.coefSign        = grp->st_a_sign;
  ECDH_input.coefA           = grp->st_a_abs;
  ECDH_input.coefB           = grp->st_b;
  ECDH_input.modulus         = grp->st_p;
  ECDH_input.scalarMulSize   = grp->st_order_size;
  ECDH_input.primeOrder      = grp->st_n;
  ECDH_input.primeOrderSize  = grp->st_order_size;

  /* Set the public key, this is the remote data */
  Q_binary = mbedtls_calloc((2U * grp->st_modulus_size) + 1U, sizeof(uint8_t));
  MBEDTLS_MPI_CHK((Q_binary == NULL) ? MBEDTLS_ERR_ECP_ALLOC_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_ecp_point_write_binary(grp, Q, MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, Q_binary,
                                                 (2U * grp->st_modulus_size) + 1U));
  ECDH_input.pointX = Q_binary + 1U;
  ECDH_input.pointY = Q_binary + grp->st_modulus_size + 1U;

  /* Start the ECC scalar multiplication */
  MBEDTLS_MPI_CHK((HAL_PKA_ECCMulEx(&hpka, &ECDH_input, ST_ECDH_TIMEOUT) != HAL_OK)
                  ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  /* Allocate memory space for computed secret */
  ECDH_ouput.ptX = mbedtls_calloc(grp->st_order_size, sizeof(uint8_t));
  MBEDTLS_MPI_CHK((ECDH_ouput.ptX == NULL) ? MBEDTLS_ERR_ECP_ALLOC_FAILED : 0);

  ECDH_ouput.ptY = mbedtls_calloc(grp->st_order_size, sizeof(uint8_t));
  MBEDTLS_MPI_CHK((ECDH_ouput.ptY == NULL) ? MBEDTLS_ERR_ECP_ALLOC_FAILED : 0);

  /* Copy the results to user specified space */
  HAL_PKA_ECCMul_GetResult(&hpka, &ECDH_ouput);

  /* Convert the signature into mpi format */
  MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(z, ECDH_ouput.ptX, grp->st_order_size));

  /* Deinitialize the PKA */
  MBEDTLS_MPI_CHK((HAL_PKA_DeInit(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

cleanup:
  /* De-initialize HW peripheral */
  HAL_PKA_DeInit(&hpka);

  /* Disable HW peripheral clock */
  __HAL_RCC_PKA_CLK_DISABLE();

  /* Free memory */
  if (d_binary != NULL)
  {
    mbedtls_platform_zeroize(d_binary, grp->st_order_size);
    mbedtls_free(d_binary);
  }

  if (Q_binary != NULL)
  {
    mbedtls_platform_zeroize(Q_binary, (2U * grp->st_modulus_size) + 1U);
    mbedtls_free(Q_binary);
  }

  if (ECDH_ouput.ptX != NULL)
  {
    mbedtls_platform_zeroize(ECDH_ouput.ptX, grp->st_order_size);
    mbedtls_free(ECDH_ouput.ptX);
  }

  if (ECDH_ouput.ptY != NULL)
  {
    mbedtls_platform_zeroize(ECDH_ouput.ptY, grp->st_order_size);
    mbedtls_free(ECDH_ouput.ptY);
  }

  return ret;
}

/*
 * Compute shared secret (SEC1 3.3.1)
 */
int mbedtls_ecdh_compute_shared(mbedtls_ecp_group *grp, mbedtls_mpi *z,
                                const mbedtls_ecp_point *Q, const mbedtls_mpi *d,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng)
{
  return ecdh_compute_shared_restartable(grp, z, Q, d, f_rng, p_rng, NULL);
}
#endif /* MBEDTLS_ECDH_COMPUTE_SHARED_ALT */

#endif /* MBEDTLS_HAL_ECDH_ALT */

#endif /* MBEDTLS_ECP_ALT */
#endif /* MBEDTLS_ECDH_C */
