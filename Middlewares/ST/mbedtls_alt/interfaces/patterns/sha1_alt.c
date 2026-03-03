/**
  ******************************************************************************
  * @file    sha1_alt.c
  * @author  GPM Application Team
  * @brief   Implementation of mbedtls_alt Middleware SHA1 module
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
  *  This file implements STMicroelectronics SHA1 with HW services based on API
  *  from mbed TLS
  *
  *  The SHA-1 standard was published by NIST in 1993.
  *
  *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

#if defined(MBEDTLS_SHA1_C)
#if defined(MBEDTLS_SHA1_ALT)
#include "mbedtls/sha1.h"
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"

#if defined(MBEDTLS_HAL_SHA1_ALT)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST_HASH_TIMEOUT ((uint32_t) 1000)  /* TO in ms for the hash processor */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
#if defined(MBEDTLS_THREADING_C)
  __disable_irq();
  /* mutex cannot be initialized twice */
  if (!hash_mutex_started)
  {
    mbedtls_mutex_init(&hash_mutex);
    hash_mutex_started = 1;
  }
  hash_context_count++;
  __enable_irq();
#endif /* MBEDTLS_THREADING_C */

  mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha1_context));
  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
  if (ctx == NULL)
  {
    return;
  }

#if defined(MBEDTLS_THREADING_C)
  __disable_irq();
  if (hash_context_count > 0)
  {
    hash_context_count--;
  }

  if (hash_mutex_started)
  {
    mbedtls_mutex_free(&hash_mutex);
    hash_mutex_started = 0;
  }
  __enable_irq();

  /* Shut down HASH on last context */
  if (hash_context_count == 0)
  {
    HAL_HASH_DeInit(&ctx->hhash);
  }
#endif /* MBEDTLS_THREADING_C */

  mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst,
                        const mbedtls_sha1_context *src)
{
  *dst = *src;
}

int mbedtls_sha1_starts(mbedtls_sha1_context *ctx)
{
  int ret = 0;

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&hash_mutex)) != 0)
  {
    return (ret);
  }
#endif /* MBEDTLS_THREADING_C */

  /* HASH Configuration */
  if (HAL_HASH_DeInit(&ctx->hhash) != HAL_OK)
  {
    ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    goto exit;
  }
  ctx->hhash.Instance =  HASH;
  ctx->hhash.Init.DataType = HASH_BYTE_SWAP;
  ctx->hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA1;
  if (HAL_HASH_Init(&ctx->hhash) != HAL_OK)
  {
    ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    goto exit;
  }

  /* first block on 17 words */
  ctx->first = ST_SHA1_EXTRA_BYTES;

  ctx->sbuf_len = 0;

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

exit :
  /* Free context access */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&hash_mutex) != 0)
  {
    ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  return (ret);
}

int mbedtls_internal_sha1_process(mbedtls_sha1_context *ctx,
                                  const unsigned char data[ST_SHA1_BLOCK_SIZE])
{
  int ret = 0;

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&hash_mutex)) != 0)
  {
    return (ret);
  }
#endif /* MBEDTLS_THREADING_C */

#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  if (HAL_HASH_Accumulate(&ctx->hhash,
                          (uint8_t *) data,
                          ST_SHA1_BLOCK_SIZE, ST_HASH_TIMEOUT) != 0)
  {
    ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    goto exit;
  }

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

exit :
  /* Free context access */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&hash_mutex) != 0)
  {
    ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  return (ret);
}

int mbedtls_sha1_update(mbedtls_sha1_context *ctx,
                        const unsigned char *input,
                        size_t ilen)
{
  int ret = 0;
  size_t currentlen = ilen;

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&hash_mutex)) != 0)
  {
    return (ret);
  }
#endif /* MBEDTLS_THREADING_C */

#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  if (currentlen < (ST_SHA1_BLOCK_SIZE + ctx->first - ctx->sbuf_len))
  {
    /* only store input data in context buffer */
    memcpy(ctx->sbuf + ctx->sbuf_len, input, currentlen);
    ctx->sbuf_len += currentlen;
  }
  else
  {
    /* fill context buffer until ST_SHA1_BLOCK_SIZE bytes, and process it */
    memcpy(ctx->sbuf + ctx->sbuf_len,
           input,
           (ST_SHA1_BLOCK_SIZE + ctx->first - ctx->sbuf_len));
    currentlen -= (ST_SHA1_BLOCK_SIZE + ctx->first - ctx->sbuf_len);

    if (HAL_HASH_Accumulate(&ctx->hhash,
                            (uint8_t *)(ctx->sbuf),
                            ST_SHA1_BLOCK_SIZE + ctx->first, ST_HASH_TIMEOUT) != 0)
    {
      ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
      goto exit;
    }

    /* Process following input data
                  with size multiple of ST_SHA1_BLOCK_SIZE bytes */
    size_t iter = currentlen / ST_SHA1_BLOCK_SIZE;
    if (iter != 0)
    {
      if (HAL_HASH_Accumulate(&ctx->hhash,
                              (uint8_t *)(input + ST_SHA1_BLOCK_SIZE + ctx->first - ctx->sbuf_len),
                              (iter * ST_SHA1_BLOCK_SIZE), ST_HASH_TIMEOUT) != 0)
      {
        ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
        goto exit;
      }
    }

    /* following blocks on 16 words */
    ctx->first = 0;

    /* Store only the remaining input data
                    up to (ST_SHA1_BLOCK_SIZE - 1) bytes */
    ctx->sbuf_len = currentlen % ST_SHA1_BLOCK_SIZE;
    if (ctx->sbuf_len != 0)
    {
      memcpy(ctx->sbuf, input + ilen - ctx->sbuf_len, ctx->sbuf_len);
    }
  }

#ifdef ST_HW_CONTEXT_SAVING
  /* save hw context */
  HAL_HASH_Suspend(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

exit :
  /* Free context access */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&hash_mutex) != 0)
  {
    ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  return (ret);
}

int mbedtls_sha1_finish(mbedtls_sha1_context *ctx,
                        unsigned char output[20])
{
  int ret = 0;

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&hash_mutex)) != 0)
  {
    return (ret);
  }
#endif /* MBEDTLS_THREADING_C */

#ifdef ST_HW_CONTEXT_SAVING
  /* restore hw context */
  HAL_HASH_Resume(&ctx->hhash, (uint8_t *)ctx->ctx_save_regs);
#endif /* ST_HW_CONTEXT_SAVING */

  /* Last accumulation for pending bytes in sbuf_len,
                           then trig processing and get digest */
  if (HAL_HASH_AccumulateLast(&ctx->hhash,
                              ctx->sbuf,
                              ctx->sbuf_len,
                              output,
                              ST_HASH_TIMEOUT) != 0)
  {
    ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    goto exit;
  }

  ctx->sbuf_len = 0;

exit :
  /* Free context access */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&hash_mutex) != 0)
  {
    ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  return (ret);
}

#endif /* MBEDTLS_HAL_SHA1_ALT */

#endif /* MBEDTLS_SHA1_ALT*/
#endif /* MBEDTLS_SHA1_C */
