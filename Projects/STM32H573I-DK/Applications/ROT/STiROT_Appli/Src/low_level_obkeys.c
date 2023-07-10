/**
  ******************************************************************************
  * @file    low_level_obkeys.c
  * @author  MCD Application Team
  * @brief   Low Level Interface module to access OKB area in FLASH
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

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "low_level_obkeys.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define SBS_EXT_EPOCHSELCR_EPOCH_SEL_S_EPOCH    (1U << SBS_EPOCHSELCR_EPOCH_SEL_Pos)
#define MAX_SIZE                                (0x100)

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Read encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 16 bytes)
  * @param  pData Data buffer to be filled (aligned on 4 bytes)
  * @param  Length Number of bytes (multiple of 4 bytes)
  * @retval ARM_DRIVER error status
  */
int32_t OBK_Flash_ReadEncrypted(uint32_t Offset, void *pData, uint32_t Length)
{
  uint32_t i = 0U;
  CRYP_HandleTypeDef hcryp = { 0U };
  uint32_t SaesTimeout = 100U;
  uint8_t DataEncrypted[MAX_SIZE] = {0U};
  uint8_t *p_source = (uint8_t *) (FLASH_OBK_BASE_S + Offset);
  uint8_t *p_destination = (uint8_t *) DataEncrypted;
  uint32_t a_aes_iv[4] = {0x8001D1CEU, 0xD1CED1CEU, 0xD1CE8001U, 0xCED1CED1U};

  /* Check Length param */
  if (Length > MAX_SIZE)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  for (i = 0U; (i < Length); i++, p_destination++, p_source++)
  {
    *p_destination = *p_source;
  }

  __HAL_RCC_SBS_CLK_ENABLE();
  __HAL_RCC_SAES_CLK_ENABLE();

  /* Force use of EPOCH_S value for DHUK */
  WRITE_REG(SBS_S->EPOCHSELCR, SBS_EXT_EPOCHSELCR_EPOCH_SEL_S_EPOCH);

  /* Configure SAES parameters */
  hcryp.Instance = SAES_S;
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }
  hcryp.Init.DataType  = CRYP_NO_SWAP;
  hcryp.Init.KeySelect = CRYP_KEYSEL_HW;  /* Hardware unique key (256-bits) */
  hcryp.Init.Algorithm = CRYP_AES_CBC;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL ;
  hcryp.Init.KeySize = CRYP_KEYSIZE_256B; /* 256 bits AES Key*/
  hcryp.Init.pInitVect = a_aes_iv;

  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /*Size is n words*/
  if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)&DataEncrypted[0U], Length / 4U, (uint32_t *)pData, SaesTimeout) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }
  if (HAL_CRYP_DeInit(&hcryp) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }
  __HAL_RCC_SAES_CLK_DISABLE();

  return ARM_DRIVER_OK;
}

