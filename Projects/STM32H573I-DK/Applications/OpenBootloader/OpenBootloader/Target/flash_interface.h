/**
  ******************************************************************************
  * @file    flash_interface.h
  * @author  MCD Application Team
  * @brief   Header for flash_interface.c module
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
#ifndef FLASH_INTERFACE_H
#define FLASH_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common_interface.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define FLASH_BUSY_STATE_ENABLED       ((uint32_t)0xAAAA0000)
#define FLASH_BUSY_STATE_DISABLED      ((uint32_t)0x0000DDDD)
#define PROGRAM_TIMEOUT                ((uint32_t)0x000FFFFF)

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void OPENBL_FLASH_JumpToAddress(uint32_t Address);
void OPENBL_FLASH_Lock(void);
void OPENBL_FLASH_OB_Unlock(void);
void OPENBL_FLASH_OB_Launch(void);
uint8_t OPENBL_FLASH_Read(uint32_t Address);
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength);
void OPENBL_FLASH_Unlock(void);
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *pData, uint32_t DataLength);
ErrorStatus OPENBL_FLASH_Erase(uint8_t *pData, uint32_t DataLength);
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *pListOfSectors, uint32_t Length);

uint32_t OPENBL_FLASH_GetProductState(void);
void OPENBL_Enable_BusyState_Flag(void);
void OPENBL_Disable_BusyState_Flag(void);

#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout);
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *pPageError);
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout);
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(
  FLASH_EraseInitTypeDef *pEraseInit, uint32_t *pPageError);
#endif /* (__ICCARM__) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FLASH_INTERFACE_H */
