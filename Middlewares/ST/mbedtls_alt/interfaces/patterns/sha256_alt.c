/**
  ******************************************************************************
  * @file    sha256_alt.c
  * @author  GPM Application Team
  * @brief   Implementation of mbedtls_alt SHA256 module
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
  * FIPS-180-2 compliant SHA-256 implementation
  *
  * This file implements SHA256 based on STM32 HASH hardware crypto accelerator.
  *
  *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
  *
  *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_SHA256_ALT)
#include "mbedtls/sha256.h"
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_HAL_SHA256_ALT)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST_SHA256_TIMEOUT     ((uint32_t) 3)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *value, size_t size)
{
  volatile unsigned char *p = (unsigned char *)value;
  while (size--)
  {
    *p++ = 0;
  }
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
  mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));

  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
  if (ctx == NULL)
  {
    return;
  }

  mbedtls_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                          const mbedtls_sha256_context *src)
{
  *dst = *src;
}

int mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
  /* HASH Configuration */
  if (HAL_HASH_DeInit(&ctx->hhash) != HAL_OK)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

  ctx->is224 = is224;
  ctx->hhash.Instance =  HASH;
  if (ctx->is224 == 0)
  {
    ctx->hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA256;
  }
  else
  {
    ctx->hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA224;
  }
  ctx->hhash.Init.DataType = HASH_BYTE_SWAP;
  if (HAL_HASH_Init(&ctx->hhash) != HAL_OK)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

  /* first block on 17 words */
  ctx->first = ST_SHA256_EXTRA_BYTES;

  ctx->sbuf_len = 0;

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  return 0;
}

int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[ST_SHA256_BLOCK_SIZE])
{
#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  if (HAL_HASH_Accumulate(&ctx->hhash, (uint8_t *) data, ST_SHA256_BLOCK_SIZE, ST_SHA256_TIMEOUT) != 0)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  return 0;
}

int mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
  size_t currentlen = ilen;

#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  if (currentlen < (ST_SHA256_BLOCK_SIZE + ctx->first - ctx->sbuf_len))
  {
    /* only store input data in context buffer */
    memcpy(ctx->sbuf + ctx->sbuf_len, input, currentlen);
    ctx->sbuf_len += currentlen;
  }
  else
  {
    /* fill context buffer until ST_SHA256_BLOCK_SIZE bytes, and process it */
    memcpy(ctx->sbuf + ctx->sbuf_len, input, (ST_SHA256_BLOCK_SIZE + ctx->first - ctx->sbuf_len));
    currentlen -= (ST_SHA256_BLOCK_SIZE + ctx->first - ctx->sbuf_len);

    if (HAL_HASH_Accumulate(&ctx->hhash, (uint8_t *)(ctx->sbuf),
                            ST_SHA256_BLOCK_SIZE + ctx->first, ST_SHA256_TIMEOUT) != 0)
    {
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }

    /* Process following input data with size multiple of ST_SHA256_BLOCK_SIZE bytes */
    size_t iter = currentlen / ST_SHA256_BLOCK_SIZE;
    if (iter != 0)
    {
      if (HAL_HASH_Accumulate(&ctx->hhash,
                              (uint8_t *)(input + ST_SHA256_BLOCK_SIZE + ctx->first - ctx->sbuf_len),
                              (iter * ST_SHA256_BLOCK_SIZE), ST_SHA256_TIMEOUT) != 0)
      {
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
      }
    }

    /* following blocks on 16 words */
    ctx->first = 0;

    /* Store only the remaining input data up to (ST_SHA256_BLOCK_SIZE - 1) bytes */
    ctx->sbuf_len = currentlen % ST_SHA256_BLOCK_SIZE;
    if (ctx->sbuf_len != 0)
    {
      memcpy(ctx->sbuf, input + ilen - ctx->sbuf_len, ctx->sbuf_len);
    }
  }

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */
  return 0;
}

int mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char *output)
{
#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  /* Last accumulation for pending bytes in sbuf_len, then trig processing and get digest */
  if (HAL_HASH_AccumulateLast(&ctx->hhash, (uint8_t *)(ctx->sbuf), ctx->sbuf_len, output, ST_SHA256_TIMEOUT) != 0)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

  ctx->sbuf_len = 0;

  return 0;
}

#endif /* MBEDTLS_HAL_SHA256_ALT */

#endif /* MBEDTLS_SHA256_ALT*/
#endif /* MBEDTLS_SHA256_C */
