/**
  ******************************************************************************
  * @file    ccm_alt.c
  * @author  GPM Application Team
  * @brief   Implementation of mbedtls_alt Middleware CCM module
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
  *  This file implements ST CCM HW services based on API from mbed TLS
  *
  * Definition of CCM:
  * http://csrc.nist.gov/publications/nistpubs/800-38C/SP800-38C_updated-July20_2007.pdf
  * RFC 3610 "Counter with CBC-MAC (CCM)"
  *
  * Related:
  * RFC 5116 "An Interface and Algorithms for Authenticated Encryption"
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

#if defined(MBEDTLS_CCM_C)
#if defined(MBEDTLS_CCM_ALT)

#include "mbedtls/ccm.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/constant_time.h"

#include <string.h>
#include "mbedtls/platform.h"

#if defined(MBEDTLS_HAL_CCM_ALT)

/* Parameter validation macros */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST_CRYP_TIMEOUT   1000  /* timeout (in ms) for the crypto processor   */

#define H_LENGTH 2 /*!< Formatting of the Associated Data
                        If 0 < a < 2e16-2e8,
                        then a is encoded as [a]16, i.e., two octets */

/* Private macro -------------------------------------------------------------*/
/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
  do {                                                  \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
          | ( (uint32_t) (b)[(i) + 1] << 16 )           \
          | ( (uint32_t) (b)[(i) + 2] <<  8 )           \
          | ( (uint32_t) (b)[(i) + 3]       );          \
  } while( 0 )
#endif /* !GET_UINT32_BE */

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*
 * Initialize context
 */
void mbedtls_ccm_init(mbedtls_ccm_context *ctx)
{
#if defined(MBEDTLS_THREADING_C)
  __disable_irq();
  /* mutex cannot be initialized twice */
  if (!cryp_mutex_started)
  {
    mbedtls_mutex_init(&cryp_mutex);
    cryp_mutex_started = 1;
  }
  cryp_context_count++;
  __enable_irq();
#endif /* MBEDTLS_THREADING_C */

  memset((void *)ctx, 0, sizeof(mbedtls_ccm_context));
}

int mbedtls_ccm_setkey(mbedtls_ccm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
  unsigned int i;
  int ret = 0;

  /* Protect context access                                  */
  /* (it may occur at a same time in a threaded environment) */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_lock(&cryp_mutex) != 0)
  {
    return (MBEDTLS_ERR_THREADING_MUTEX_ERROR);
  }
#endif /* MBEDTLS_THREADING_C */

  switch (keybits)
  {
    case 128:
      ctx->hcryp_ccm.Init.KeySize = CRYP_KEYSIZE_128B;;
      break;

    case 192:
#if ( USE_AES_KEY192 == 1 )
      ctx->hcryp_ccm.Init.KeySize = CRYP_KEYSIZE_192B;
      break;
#else
      ret = MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
      goto exit;
#endif /* USE_AES_KEY192 */

    case 256:
      ctx->hcryp_ccm.Init.KeySize = CRYP_KEYSIZE_256B;
      break;

    default :
      ret = MBEDTLS_ERR_CCM_BAD_INPUT;
      goto exit;
  }

  /* Format and fill AES key  */
  for (i = 0; i < (keybits / 32); i++)
  {
    GET_UINT32_BE(ctx->ccm_key[i], key, 4 * i);
  }

  ctx->hcryp_ccm.Init.pKey = ctx->ccm_key;
  ctx->hcryp_ccm.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_BYTE;
  ctx->hcryp_ccm.Init.DataType = CRYP_BYTE_SWAP;
  ctx->hcryp_ccm.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  ctx->hcryp_ccm.Init.Algorithm  = CRYP_AES_CCM;

#if defined(HW_CRYPTO_DPA_AES)
  ctx->hcryp_ccm.Instance = SAES;
  if (keybits == 0)
  {
    ctx->hcryp_ccm.Init.KeyProtection = CRYP_KEYSEL_HW;
  }
  else
  {
    ctx->hcryp_ccm.Init.KeyProtection = CRYP_KEYSEL_NORMAL;
  }
#else
  ctx->hcryp_ccm.Instance = AES;
#endif /* HW_CRYPTO_DPA_AES */

  ctx->hcryp_ccm.Init.pInitVect  = NULL;
  ctx->hcryp_ccm.Init.Header     = NULL;
  ctx->hcryp_ccm.Init.HeaderSize = 0;
  ctx->hcryp_ccm.Init.B0 = NULL;
  ctx->hcryp_ccm.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ONCE;
  ctx->hcryp_ccm.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_BYTE;
  if (HAL_CRYP_Init(&ctx->hcryp_ccm) != HAL_OK)
  {
    ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    goto exit;
  }

#if defined(HW_CRYPTO_DPA_AES)
  /* Enable SAES clock */
  __HAL_RCC_SAES_CLK_ENABLE();
#else
  /* Enable AES clock */
  __HAL_RCC_AES_CLK_ENABLE();
#endif /* HW_CRYPTO_DPA_AES */

  if (HAL_CRYP_Init(&ctx->hcryp_ccm) != HAL_OK)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

  /* allow multi-context of CRYP : save context */
  ctx->ctx_save_cr = ctx->hcryp_ccm.Instance->CR;

exit :
  /* Free context access */
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&cryp_mutex) != 0)
  {
    ret = MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  return (ret);
}

/*
 * Free context
 */
void mbedtls_ccm_free(mbedtls_ccm_context *ctx)
{
  if (ctx == NULL)
  {
    return;
  }

#if defined(MBEDTLS_THREADING_C)
  __disable_irq();
  if (cryp_context_count > 0)
  {
    cryp_context_count--;
  }

  if (cryp_mutex_started)
  {
    mbedtls_mutex_free(&cryp_mutex);
    cryp_mutex_started = 0;
  }
  __enable_irq();

  /* Shut down CRYP on last context */
  if (cryp_context_count == 0)
#endif /* MBEDTLS_THREADING_C */
  {
    HAL_CRYP_DeInit(&ctx->hcryp_ccm);
  }
  mbedtls_platform_zeroize(ctx, sizeof(mbedtls_ccm_context));
}

#define CCM_STATE__CLEAR                0
#define CCM_STATE__STARTED              (1 << 0)
#define CCM_STATE__LENGTHS_SET          (1 << 1)
#define CCM_STATE__AUTH_DATA_STARTED    (1 << 2)
#define CCM_STATE__AUTH_DATA_FINISHED   (1 << 3)
#define CCM_STATE__ERROR                (1 << 4)

static void mbedtls_ccm_clear_state(mbedtls_ccm_context *ctx)
{
  ctx->state = CCM_STATE__CLEAR;
  memset(ctx->y, 0, 16);
}

static int ccm_calculate_first_block_if_ready(mbedtls_ccm_context *ctx)
{
  unsigned char i = 0;
  size_t len_left = 0;
  uint32_t *iv_p = (uint32_t *)(ctx->y);

  /* length calculation can be done only after both
   * mbedtls_ccm_starts() and mbedtls_ccm_set_lengths() have been executed
   */
  if (!(ctx->state & CCM_STATE__STARTED) || !(ctx->state & CCM_STATE__LENGTHS_SET))
  {
    return 0;
  }

  /* CCM expects non-empty tag.
   * CCM* allows empty tag. For CCM* without tag, ignore plaintext length.
   */
  if (ctx->tag_len == 0)
  {
    if (ctx->mode == MBEDTLS_CCM_STAR_ENCRYPT || ctx->mode == MBEDTLS_CCM_STAR_DECRYPT)
    {
      ctx->plaintext_len = 0;
    }
    else
    {
      return MBEDTLS_ERR_CCM_BAD_INPUT;
    }
  }

  /*
   * First block:
   * 0        .. 0        flags
   * 1        .. iv_len   nonce (aka iv)  - set by: mbedtls_ccm_starts()
   * iv_len+1 .. 15       length
   *
   * With flags as (bits):
   * 7        0
   * 6        add present?
   * 5 .. 3   (t - 2) / 2
   * 2 .. 0   q - 1
   */
  ctx->y[0] |= (ctx->add_len > 0) << 6;
  ctx->y[0] |= ((ctx->tag_len - 2) / 2) << 3;
  ctx->y[0] |= ctx->q - 1;

  for (i = 0, len_left = ctx->plaintext_len; i < ctx->q; i++, len_left >>= 8)
  {
    ctx->y[15 - i] = MBEDTLS_BYTE_0(len_left);
  }

  if (len_left > 0)
  {
    ctx->state |= CCM_STATE__ERROR;
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  /* first authentication block */
  for (i = 0; i < 4; i++)
  {
    GET_UINT32_BE(iv_p[i], ctx->y, 4U * i);
  }
  ctx->hcryp_ccm.Init.B0 = (uint32_t *)ctx->y;

  return 0;
}

int mbedtls_ccm_starts(mbedtls_ccm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len)
{
  /* Also implies q is within bounds */
  if (iv_len < 7 || iv_len > 13)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  ctx->mode = mode;
  ctx->q = 16 - 1 - (unsigned char) iv_len;

  /*
   * See ccm_calculate_first_block_if_ready() for block layout description
   */
  memcpy(ctx->y + 1, iv, iv_len);

  ctx->state |= CCM_STATE__STARTED;
  return ccm_calculate_first_block_if_ready(ctx);
}

int mbedtls_ccm_set_lengths(mbedtls_ccm_context *ctx,
                            size_t total_ad_len,
                            size_t plaintext_len,
                            size_t tag_len)
{
  /*
   * Check length requirements: SP800-38C A.1
   * Additional requirement: a < 2^16 - 2^8 to simplify the code.
   * 'length' checked later (when writing it to the first block)
   *
   * Also, loosen the requirements to enable support for CCM* (IEEE 802.15.4).
   */
  if (tag_len == 2 || tag_len > 16 || tag_len % 2 != 0)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  if (total_ad_len >= 0xFF00)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  ctx->plaintext_len = plaintext_len;
  ctx->add_len = total_ad_len;
  ctx->tag_len = tag_len;

  ctx->state |= CCM_STATE__LENGTHS_SET;
  return ccm_calculate_first_block_if_ready(ctx);
}

int mbedtls_ccm_update_ad(mbedtls_ccm_context *ctx,
                          const unsigned char *add,
                          size_t add_len)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  unsigned char *b1_padded_addr = NULL;            /* Formatting of B1   */
  size_t b1_length = 0;                            /* B1 with padding    */
  uint8_t b1_padding = 0;                          /* B1 word alignment  */

  if (ctx->state & CCM_STATE__ERROR)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  if (add_len > 0)
  {
    if (ctx->state & CCM_STATE__AUTH_DATA_FINISHED)
    {
      return MBEDTLS_ERR_CCM_BAD_INPUT;
    }

    if (!(ctx->state & CCM_STATE__AUTH_DATA_STARTED))
    {
      if (add_len > ctx->add_len)
      {
        return MBEDTLS_ERR_CCM_BAD_INPUT;
      }

      /* Extra bytes to deal with data padding such that
       * the resulting string can be partitioned into words
       */
      b1_padding = ((add_len + H_LENGTH) % 4);
      b1_length = add_len + H_LENGTH + b1_padding;

      b1_padded_addr = mbedtls_calloc(1, b1_length);

      if (b1_padded_addr == NULL)
      {
        return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
      }

      /* Header length */
      b1_padded_addr[0] = (unsigned char)((add_len >> 8) & 0xFF);
      b1_padded_addr[1] = (unsigned char)((add_len) & 0xFF);

      /* data concatenation */
      memcpy(b1_padded_addr + H_LENGTH, add, add_len);

      /* blocks (B) associated to the Associated Data (A) */
      ctx->hcryp_ccm.Init.Header     = (uint32_t *)b1_padded_addr;

      ctx->hcryp_ccm.Init.HeaderSize = b1_length;

      ctx->state |= CCM_STATE__AUTH_DATA_STARTED;

      /* allow multi-context of CRYP use: restore context */
      ctx->hcryp_ccm.Instance->CR = ctx->ctx_save_cr;

      if (ctx->mode == MBEDTLS_CCM_ENCRYPT || \
          ctx->mode == MBEDTLS_CCM_STAR_ENCRYPT)
      {
        if (HAL_CRYP_Encrypt(&ctx->hcryp_ccm,
                             (uint32_t *)NULL,
                             0,
                             (uint32_t *)NULL,
                             ST_CRYP_TIMEOUT) != HAL_OK)
        {
          ctx->state |= CCM_STATE__ERROR;
          ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
          goto exit;
        }
      }

      if (ctx->mode == MBEDTLS_CCM_DECRYPT || \
          ctx->mode == MBEDTLS_CCM_STAR_DECRYPT)
      {
        if (HAL_CRYP_Decrypt(&ctx->hcryp_ccm,
                             (uint32_t *)NULL,
                             0,
                             (uint32_t *)NULL,
                             ST_CRYP_TIMEOUT) != HAL_OK)
        {
          ctx->state |= CCM_STATE__ERROR;
          ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
          goto exit;
        }
      }
      /* allow multi-context of CRYP : save context */
      ctx->ctx_save_cr = ctx->hcryp_ccm.Instance->CR;

    }
  }
  else
  {
    ctx->hcryp_ccm.Init.Header = NULL;
    ctx->hcryp_ccm.Init.HeaderSize = 0;
  }

  ctx->state |= CCM_STATE__AUTH_DATA_FINISHED;
  ret = 0;

exit:
  if (add_len > 0)
  {
    mbedtls_free(b1_padded_addr);
  }
  return ret;
}

int mbedtls_ccm_update(mbedtls_ccm_context *ctx,
                       const unsigned char *input, size_t input_len,
                       unsigned char *output, size_t output_size,
                       size_t *output_len)
{
  if (ctx->state & CCM_STATE__ERROR)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  /* Check against plaintext length only if performing operation with
   * authentication
   */
  if (ctx->tag_len != 0 && input_len > ctx->plaintext_len)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  if (output_size < input_len)
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  *output_len = input_len;

  /* allow multi-context of CRYP use: restore context */
  ctx->hcryp_ccm.Instance->CR = ctx->ctx_save_cr;

  /* blocks (B) associated to the plaintext message (P) */
  if (ctx->mode == MBEDTLS_CCM_ENCRYPT || \
      ctx->mode == MBEDTLS_CCM_STAR_ENCRYPT)
  {
    if (HAL_CRYP_Encrypt(&ctx->hcryp_ccm,
                         (uint32_t *)input,
                         input_len,
                         (uint32_t *)output,
                         ST_CRYP_TIMEOUT) != HAL_OK)
    {
      ctx->state |= CCM_STATE__ERROR;
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
  }

  if (ctx->mode == MBEDTLS_CCM_DECRYPT || \
      ctx->mode == MBEDTLS_CCM_STAR_DECRYPT)
  {
    if (HAL_CRYP_Decrypt(&ctx->hcryp_ccm,
                         (uint32_t *)input,
                         input_len,
                         (uint32_t *)output,
                         ST_CRYP_TIMEOUT) != HAL_OK)
    {
      ctx->state |= CCM_STATE__ERROR;
      return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
    }
  }

  /* allow multi-context of CRYP : save context */
  ctx->ctx_save_cr = ctx->hcryp_ccm.Instance->CR;

  return 0;
}

int mbedtls_ccm_finish(mbedtls_ccm_context *ctx,
                       unsigned char *tag, size_t tag_len)
{
  __ALIGN_BEGIN uint8_t mac[16]      __ALIGN_END;  /* temporary mac */

  if (ctx->state & CCM_STATE__ERROR)
  {
    return MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  }

  if (ctx->add_len > 0 && !(ctx->state & CCM_STATE__AUTH_DATA_FINISHED))
  {
    return MBEDTLS_ERR_CCM_BAD_INPUT;
  }

  /* Tag has a variable length */
  memset(mac, 0, sizeof(mac));
  /* allow multi-context of CRYP use: restore context */
  ctx->hcryp_ccm.Instance->CR = ctx->ctx_save_cr;

  /* Generate the authentication TAG */
  if (HAL_CRYPEx_AESCCM_GenerateAuthTAG(&ctx->hcryp_ccm,
                                        (uint32_t *)mac,
                                        ST_CRYP_TIMEOUT) != HAL_OK)
  {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }

  if (tag != NULL)
  {
    memcpy(tag, mac, tag_len);
  }

  /* allow multi-context of CRYP : save context */
  ctx->ctx_save_cr = ctx->hcryp_ccm.Instance->CR;

  mbedtls_ccm_clear_state(ctx);
  mbedtls_platform_zeroize(ctx->ccm_key, sizeof(ctx->ccm_key));

  return 0;
}

/*
 * Authenticated encryption or decryption
 */
static int ccm_auth_crypt(mbedtls_ccm_context *ctx, int mode, size_t length,
                          const unsigned char *iv, size_t iv_len,
                          const unsigned char *add, size_t add_len,
                          const unsigned char *input, unsigned char *output,
                          unsigned char *tag, size_t tag_len)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t olen;

  if ((ret = mbedtls_ccm_starts(ctx, mode, iv, iv_len)) != 0)
  {
    return ret;
  }

  if ((ret = mbedtls_ccm_set_lengths(ctx, add_len, length, tag_len)) != 0)
  {
    return ret;
  }

  if ((ret = mbedtls_ccm_update_ad(ctx, add, add_len)) != 0)
  {
    return ret;
  }

  if ((ret = mbedtls_ccm_update(ctx, input, length,
                                output, length, &olen)) != 0)
  {
    return ret;
  }

  if ((ret = mbedtls_ccm_finish(ctx, tag, tag_len)) != 0)
  {
    return ret;
  }

  return 0;
}

/*
 * Authenticated encryption
 */
int mbedtls_ccm_star_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
                                     const unsigned char *iv, size_t iv_len,
                                     const unsigned char *add, size_t add_len,
                                     const unsigned char *input, unsigned char *output,
                                     unsigned char *tag, size_t tag_len)
{
  return ccm_auth_crypt(ctx, MBEDTLS_CCM_STAR_ENCRYPT, length, iv, iv_len,
                        add, add_len, input, output, tag, tag_len);
}

int mbedtls_ccm_encrypt_and_tag(mbedtls_ccm_context *ctx, size_t length,
                                const unsigned char *iv, size_t iv_len,
                                const unsigned char *add, size_t add_len,
                                const unsigned char *input, unsigned char *output,
                                unsigned char *tag, size_t tag_len)
{
  return ccm_auth_crypt(ctx, MBEDTLS_CCM_ENCRYPT, length,
                        iv, iv_len, add, add_len, input, output, tag, tag_len);
}

/*
 * Authenticated decryption
 */
static int ccm_auth_decrypt(mbedtls_ccm_context *ctx, int mode, size_t length,
                            const unsigned char *iv, size_t iv_len,
                            const unsigned char *add, size_t add_len,
                            const unsigned char *input, unsigned char *output,
                            const unsigned char *tag, size_t tag_len)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  unsigned char check_tag[16];
  unsigned char i = 0;
  int diff = 0;

  if ((ret = ccm_auth_crypt(ctx, mode, length,
                            iv, iv_len, add, add_len,
                            input, output, check_tag, tag_len)) != 0)
  {
    return ret;
  }

  /* Check tag in "constant-time" */
  for (diff = 0, i = 0; i < tag_len; i++)
  {
    diff |= tag[i] ^ check_tag[i];
  }

  if (diff != 0)
  {
    mbedtls_platform_zeroize(output, length);
    return (MBEDTLS_ERR_CCM_AUTH_FAILED);
  }

  return 0;
}

int mbedtls_ccm_star_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
                                  const unsigned char *iv, size_t iv_len,
                                  const unsigned char *add, size_t add_len,
                                  const unsigned char *input, unsigned char *output,
                                  const unsigned char *tag, size_t tag_len)
{
  return ccm_auth_decrypt(ctx, MBEDTLS_CCM_STAR_DECRYPT, length,
                          iv, iv_len, add, add_len,
                          input, output, tag, tag_len);
}

int mbedtls_ccm_auth_decrypt(mbedtls_ccm_context *ctx, size_t length,
                             const unsigned char *iv, size_t iv_len,
                             const unsigned char *add, size_t add_len,
                             const unsigned char *input, unsigned char *output,
                             const unsigned char *tag, size_t tag_len)
{
  return (ccm_auth_decrypt(ctx, MBEDTLS_CCM_DECRYPT, length,
                           iv, iv_len, add, add_len,
                           input, output, tag, tag_len));
}


#endif /* MBEDTLS_HAL_CCM_ALT */

#endif /*MBEDTLS_CCM_ALT*/
#endif /*MBEDTLS_CCM_C*/
