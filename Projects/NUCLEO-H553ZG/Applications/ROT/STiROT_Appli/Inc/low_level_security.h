/**
  ******************************************************************************
  * @file    low_level_security.h
  * @author  MCD Application Team
  * @brief   Header for low_level_security.c module
  *
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
#ifndef LOW_LEVEL_SECURITY_H
#define LOW_LEVEL_SECURITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
struct sau_cfg_t
{
  uint32_t RNR;
  uint32_t RBAR;
  uint32_t RLAR;
};


/* Exported macros -----------------------------------------------------------*/
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* Exported functions ------------------------------------------------------- */
void mpu_loader_cfg(void);
void gtzc_loader_cfg(void);
void gpio_loader_cfg(void);
void sau_loader_cfg(void);
void nvic_loader_cfg(void);
void fpu_enable_cfg(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_SECURITY_H */
