/**
  ******************************************************************************
  * @file    entropy_hardware_alt.h
  * @author  GPM Application Team
  * @brief   Header for entropy_hardware_alt.c module
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
#ifndef ENTROPY_HARDWARE_ALT_H
#define ENTROPY_HARDWARE_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include "mbedtls/entropy.h"
#include "entropy_poll.h"

#if defined(RCC_RNGCLKSOURCE_HSI48)
#define RNGCLKSOURCE_HSI RCC_RNGCLKSOURCE_HSI48
#else
#define RNGCLKSOURCE_HSI RCC_RNGCLKSOURCE_HSI
#endif /* RCC_RNGCLKSOURCE_HSI48 */

/* Exported functions ------------------------------------------------------- */
int RNG_DeInit(void);
int RNG_Init(void);
int RNG_GetBytes(uint8_t *output, size_t length, size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif /* ENTROPY_HARDWARE_ALT_H */
