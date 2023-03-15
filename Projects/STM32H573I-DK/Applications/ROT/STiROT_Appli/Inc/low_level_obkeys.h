/**
  ******************************************************************************
  * @file    low_level_obkeys.h
  * @author  MCD Application Team
  * @brief   Header for low_level_obkeys.c module
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
#ifndef LOW_LEVEL_OBKEYS_H
#define LOW_LEVEL_OBKEYS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#define ARM_DRIVER_OK                 0 /*!< Operation succeeded */
#define ARM_DRIVER_ERROR_SPECIFIC    -6 /*!< Start of driver specific errors */

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/


/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int32_t OBK_Flash_ReadEncrypted(uint32_t Offset, void *pData, uint32_t Length);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_OBKEYS_H */
