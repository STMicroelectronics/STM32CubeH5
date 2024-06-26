/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif /* __ICCARM__  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/

#define S_CODE_OFFSET      0x12000 /* This define is updated automatically from ROT_BOOT project */
#define S_CODE_SIZE        0x6000 /* This define is updated automatically from ROT_BOOT project */
#define NS_CODE_OFFSET     (S_CODE_OFFSET + S_CODE_SIZE) /* Non secure code Offset */
#define NS_CODE_SIZE       0xA0000 /* This define is updated automatically from ROT_BOOT project */
#define IMAGE_HEADER_SIZE  (0x400)  /* mcuboot headre size */
#define S_CODE_START       (FLASH_BASE_S + S_CODE_OFFSET + IMAGE_HEADER_SIZE)

#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL(*funcptr)(void);
#endif /* __ICCARM__  */

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;



/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
