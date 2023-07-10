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
#include "low_level_flash.h"
#include "low_level_security.h"
#include "boot_hal_flowcontrol.h"
#include "boot_hal_cfg.h"
#include "flash_layout.h"
#include "config-boot.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

/**
  * Arm Flash device structure.
  */
struct arm_obk_flash_dev_t
{
  OBK_LowLevelDevice *dev;
  ARM_FLASH_INFO *data;       /*!< OBK FLASH memory device data */
};

/* Private defines -----------------------------------------------------------*/
#define SBS_EXT_EPOCHSELCR_EPOCH_SEL_S_EPOCH    (1U << SBS_EPOCHSELCR_EPOCH_SEL_Pos)
#define MAX_SIZE_CFG                         OBK_HDPL1_CFG_SIZE
#define ST_SHA256_TIMEOUT                       (3U)

/* config for OBK flash driver */
#define OBK_FLASH0_TOTAL_SIZE                   (0x2000U)
#define OBK_FLASH0_PROG_UNIT                    (OBK_FLASH_PROG_UNIT)
#define OBK_FLASH0_ERASED_VAL                   (0xFF)

/* Private variables ---------------------------------------------------------*/
static HASH_HandleTypeDef hhash;
static ARM_FLASH_INFO ARM_OBK_FLASH0_DEV_DATA =
{
  .sector_info    = NULL,     /* Uniform sector layout */
  .sector_count   = OBK_FLASH0_TOTAL_SIZE / OBK_FLASH0_PROG_UNIT,
  .sector_size    = OBK_FLASH0_PROG_UNIT,
  .page_size      = OBK_FLASH0_PROG_UNIT,
  .program_unit   = OBK_FLASH0_PROG_UNIT, /* Minimum write size in bytes */
  .erased_value   = OBK_FLASH0_ERASED_VAL
};

static struct arm_obk_flash_dev_t ARM_OBK_FLASH0_DEV =
{
  .dev    = &(OBK_FLASH0_DEV),
  .data   = &(ARM_OBK_FLASH0_DEV_DATA)
};

/* Global variables ----------------------------------------------------------*/
#if defined(__ICCARM__)
#pragma location=".ram_init_ro"
#elif defined(__GNUC__)
__attribute__((section(".ram_init_ro")))
#endif /* __ICCARM__ */

OBK_Hdpl1Config OBK_Hdpl1_Cfg;
extern __IO uint32_t DoubleECC_Error_Counter;

/* Private function prototypes -----------------------------------------------*/
int32_t OBK_Flash_WriteEncrypted(uint32_t Offset, const void *pData, uint32_t Length);
/* Functions Definition ------------------------------------------------------*/
/**
  * \brief      Check if the Flash memory boundaries are not violated.
  * \param[in]  flash_dev  Flash device structure \ref arm_obk_flash_dev_t
  * \param[in]  offset     Highest Flash memory address which would be accessed.
  * \return     Returns true if Flash memory boundaries are not violated, false
  *             otherwise.
  */
static bool is_range_valid(struct arm_obk_flash_dev_t *Flash_dev, uint32_t Offset)
{
  return (Offset <= OBK_HDPL1_END) ? (true) : (false);
}

/**
  * \brief  Check if the parameter is aligned to program_unit.
  * @parma  Param Any number that can be checked against the
  *               program_unit, e.g. Flash memory address or
  *               data length in bytes.
  * @retval Returns true if param is aligned to program_unit, false
  *               otherwise.
  */
static bool is_write_aligned(struct arm_obk_flash_dev_t *Flash_dev, uint32_t Param)
{
  return ((Param % Flash_dev->data->program_unit) != 0U) ? (false) : (true);
}

/**
  * @brief  Control if the length is 16 bytes multiple (QUADWORD)
  * @param  Length: Number of bytes (multiple of 16 bytes)
  * @retval None
  */
static bool is_write_allowed(struct arm_obk_flash_dev_t *Flash_dev, uint32_t Length)
{
  return ((Length % Flash_dev->data->program_unit) != 0U) ? (false) : (true);
}

/**
  * @brief  Read encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 16 bytes)
  * @param  pData Data buffer to be filled (aligned on 4 bytes)
  * @param  Length Number of bytes (multiple of 4 bytes)
  * @retval ARM_DRIVER error status
  */
static int32_t OBK_Flash_ReadEncrypted(uint32_t Offset, void *pData, uint32_t Length)
{

  uint8_t *p_source = (uint8_t *) (FLASH_OBK_BASE_S + Offset);
  uint8_t *p_destination = (uint8_t *) pData;

  /* Check OBKeys  boundaries */
  if (is_range_valid(&ARM_OBK_FLASH0_DEV, Offset + Length -1U) != true)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Do not use memcpy from lib to manage properly ECC error */
  DoubleECC_Error_Counter = 0U;
  memcpy(p_destination, p_source, Length);
  if (DoubleECC_Error_Counter != 0U)
  {
    BOOT_LOG_ERR("Double ECC error detected: FLASH_ECCDETR=0x%x", (int)FLASH->ECCDETR);
    memset(p_destination, 0x00, Length);
  }

  return ARM_DRIVER_OK;
}

/**
  * @brief  Write encrypted OBkeys
  * @param  Offset Offset in the OBKeys area (aligned on 16 bytes)
  * @param  pData Data buffer to be programmed encrypted (aligned on 4 bytes)
  * @param  Length Number of bytes (multiple of 16 bytes)
  * @retval ARM_DRIVER error status
  */
int32_t OBK_Flash_WriteEncrypted(uint32_t Offset, const void *pData, uint32_t Length)
{
  uint32_t i = 0U;
  uint32_t destination = FLASH_OBK_BASE_S + Offset;
  FLASH_EraseInitTypeDef FLASH_EraseInitStruct = {0U};
  uint32_t sector_error = 0U;

  /* Check parameters */
  if ((is_range_valid(&ARM_OBK_FLASH0_DEV, Offset + Length - 1U) != true) ||
      (is_write_aligned(&ARM_OBK_FLASH0_DEV, Offset) != true) ||
      (is_write_allowed(&ARM_OBK_FLASH0_DEV, Length) != true) ||
      (Length > MAX_SIZE_CFG))
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Unlock  Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASHEx_OBK_Unlock();

  /* Erase OBKeys */
  FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_OBK_ALT;
  if (HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &sector_error) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /* Program OBKeys */
  for (i = 0U; i < Length; i += OBK_FLASH_PROG_UNIT)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD_OBK_ALT, (destination + i), (uint32_t)&((uint32_t*)pData)[i / 4U]) != HAL_OK)
    {
      return ARM_DRIVER_ERROR_SPECIFIC;
    }
  }

  /* Swap all OBKeys */
  if (HAL_FLASHEx_OBK_Swap(ALL_OBKEYS) != HAL_OK)
  {
      return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_Lock();
  (void) HAL_FLASHEx_OBK_Lock();

  return ARM_DRIVER_OK;
}

/**
  * @brief  Read non-encrypted OBkeys
  * @param  Offset: Offset in the OBKeys area (aligned on 16 bytes)
  * @param  pData Data buffer to be filled (aligned on 4 bytes)
  * @param  Length: Number of bytes (multiple of 4 bytes)
  * @retval ARM_DRIVER error status
  */
static int32_t OBK_Read(uint32_t Offset, void *pData, uint32_t Length)
{
  uint8_t *p_source = (uint8_t *) (FLASH_OBK_BASE_S + Offset);
  uint8_t *p_destination = (uint8_t *) pData;

  /* Check parameters */
  if (is_range_valid(&ARM_OBK_FLASH0_DEV, Offset + Length - 1U) != true)
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Do not use memcpy from lib to manage properly ECC error */
  DoubleECC_Error_Counter = 0U;
  memcpy(p_destination, p_source, Length);
  if (DoubleECC_Error_Counter != 0U)
  {
    BOOT_LOG_ERR("Double ECC error detected: FLASH_ECCDETR=0x%x", (int)FLASH->ECCDETR);
    memset(p_destination, 0x00, Length);
  }

  return ARM_DRIVER_OK;
}

/**
  * @brief  Write OBkeys
  * @param  Offset: Offset in the OBKeys area (aligned on 16 bytes)
  * @param  Length: Number of bytes (multiple of 4 bytes)
  * @param  DataAddress Data buffer to be encrypted (aligned on 4 bytes)
  * @retval ARM_DRIVER error status
  */
static int32_t OBK_Write(uint32_t Offset, const void *pData, uint32_t Length)
{
  uint32_t i = 0U;
  uint32_t destination  = FLASH_OBK_BASE_S + Offset;
  FLASH_EraseInitTypeDef FLASH_EraseInitStruct = { 0U };;
  uint32_t sector_error = 0U;
  uint32_t data_address = (uint32_t) pData;

  /* Check parameters */
  if ((is_range_valid(&ARM_OBK_FLASH0_DEV, Offset + Length - 1U) != true) ||
      (is_write_aligned(&ARM_OBK_FLASH0_DEV, Offset) != true) ||
      (is_write_allowed(&ARM_OBK_FLASH0_DEV, Length) != true))
  {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Unlock  Flash area */
  (void) HAL_FLASH_Unlock();
  (void) HAL_FLASHEx_OBK_Unlock();

  /* Erase OBKeys */
  FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_OBK_ALT;
  if (HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &sector_error) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /* Program OBKeys */
  for (i = 0U; i < Length; i += OBK_FLASH_PROG_UNIT)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD_OBK_ALT, (destination + i), (data_address + i)) != HAL_OK)
    {
    return ARM_DRIVER_ERROR_SPECIFIC;
    }
  }

  /* Swap all OBKeys */
  if (HAL_FLASHEx_OBK_Swap(ALL_OBKEYS) != HAL_OK)
  {
    return ARM_DRIVER_ERROR_SPECIFIC;
  }

  /* Lock the User Flash area */
  (void) HAL_FLASH_Lock();
  (void) HAL_FLASHEx_OBK_Lock();

  return ARM_DRIVER_OK;
}

/**
  * @brief  Memory compare with constant time execution.
  * @note   Objective is to avoid basic attacks based on time execution
  * @param  pAdd1 Address of the first buffer to compare
  * @param  pAdd2 Address of the second buffer to compare
  * @param  Size Size of the comparison
  * @retval SFU_ SUCCESS if equal, a SFU_error otherwise.
  */
static uint32_t MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size)
{
  uint8_t result = 0x00U;
  uint32_t i = 0U;

  for (i = 0U; i < Size; i++)
  {
    result |= pAdd1[i] ^ pAdd2[i];
  }
  return result;
}

/**
  * @brief  Compute SHA256
  * @param  pBuffer: pointer to the input buffer to be hashed
  * @param  Length: length of the input buffer in bytes
  * @param  pSHA256: pointer to the compuyed digest
  * @retval None
  */
static HAL_StatusTypeDef Compute_SHA256(uint8_t *pBuffer, uint32_t Length, uint8_t *pSHA256)
{
  /* Enable HASH clock */
  __HAL_RCC_HASH_CLK_ENABLE();

  hhash.Instance = HASH;
  /* HASH Configuration */
  if (HAL_HASH_DeInit(&hhash) != HAL_OK)
  {
    return HAL_ERROR;
  }
  hhash.Init.DataType = HASH_BYTE_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA256;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* HASH computation */
  if (HAL_HASH_Start(&hhash, pBuffer, Length, pSHA256, ST_SHA256_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  Read data in OBkeys HDPL1
  * @param  pOBK_Hdpl1Data: pointer on HDPL1 data
  * @retval None
  */
HAL_StatusTypeDef OBK_ReadHdpl1Data(OBK_Hdpl1Data *pOBK_Hdpl1Data)
{
  uint8_t sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl1Data;

  /* Read configuration in OBKeys */
  if (OBK_Read(OBK_HDPL1_DATA_OFFSET, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data)) != ARM_DRIVER_OK)
  {
    return HAL_ERROR;
  }

  /* Verif SHA256 on the whole Hdpl 1 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *) (Address + SHA256_LENGTH), sizeof(OBK_Hdpl1Data) - SHA256_LENGTH, sha256) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (MemoryCompare(pOBK_Hdpl1Data->SHA256, sha256, SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL1 data");
    return HAL_ERROR;
  }
  return HAL_OK;
}

/**
  * @brief  Update data in OBkeys Hdpl1 1
  * @param  pOBK_Hdpl1Data: pointer on Hdpl 1 data
  * @retval ARM_DRIVER error status
  */
HAL_StatusTypeDef OBK_UpdateHdpl1Data(OBK_Hdpl1Data *pOBK_Hdpl1Data)
{
  uint8_t sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl1Data;

  /* Verif SHA256 on the whole Hdpl 1 data except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *) (Address + SHA256_LENGTH), sizeof(OBK_Hdpl1Data) - SHA256_LENGTH, sha256) != HAL_OK)
  {
    BOOT_LOG_ERR("Wrong OBK HDPL1 data");
    return HAL_ERROR;
  }
  (void) memcpy(&pOBK_Hdpl1Data->SHA256[0], &sha256[0], SHA256_LENGTH);

  /* Write configuration in OBKeys */
  if (OBK_Write(OBK_HDPL1_DATA_OFFSET, (void *) pOBK_Hdpl1Data, sizeof(OBK_Hdpl1Data)) != ARM_DRIVER_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}


/* Public Functions Definition ------------------------------------------------------*/

/**
  * @brief  Read configuration in OBkeys Hdpl 1
  * @param  pOBK_Hdpl1Cfg : pointer on Hdpl 1 configuration
  * @retval None
  */
void OBK_ReadHdpl1Config(OBK_Hdpl1Config *pOBK_Hdpl1Cfg)
{
  uint8_t sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl1Cfg;

  /* Read configuration in OBKeys */
  if (OBK_Flash_ReadEncrypted(OBK_HDPL1_CFG_OFFSET,(void *) pOBK_Hdpl1Cfg,  sizeof(OBK_Hdpl1Config)) != ARM_DRIVER_OK)
  {
    Error_Handler();
  }

  /* Verif SHA256 on the whole Hdpl 1 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *) (Address + SHA256_LENGTH), sizeof(OBK_Hdpl1Config) - SHA256_LENGTH, sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl1Cfg->SHA256[0], &sha256[0], SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("read: Wrong OBK HDPL1 cfg");
    Error_Handler();
  }
}

/**
  * @brief  Verify configuration in OBkeys Hdpl1 1
  * @param  pOBK_Hdpl1Cfg : pointer on Hdpl 1 configuration
  * @retval None
  */
void OBK_VerifyHdpl1Config(OBK_Hdpl1Config *pOBK_Hdpl1Cfg)
{
  uint8_t sha256[SHA256_LENGTH] = { 0U };
  uint32_t Address = (uint32_t) pOBK_Hdpl1Cfg;

  /* Verif SHA256 on the whole Hdpl 1 config except first 32 bytes of SHA256 */
  if (Compute_SHA256((uint8_t *) (Address + SHA256_LENGTH), sizeof(OBK_Hdpl1Config) - SHA256_LENGTH, sha256) != HAL_OK)
  {
    Error_Handler();
  }
  if (MemoryCompare(&pOBK_Hdpl1Cfg->SHA256[0], &sha256[0], SHA256_LENGTH) != 0U)
  {
    BOOT_LOG_ERR("verify: Wrong OBK HDPL1 cfg");
    Error_Handler();
  }
}

/**
  * @brief  Get counter
  * @param  pCounter: pointer on current Fw version
  * @retval None
  */
HAL_StatusTypeDef OBK_GetNVCounter(enum tfm_nv_counter_t CounterId, uint32_t *pCounter)
{
  OBK_Hdpl1Data OBK_data = { 0U };

  /* Read all OBK hdpl1/2 data & control SHA256 */
  if (OBK_ReadHdpl1Data(&OBK_data) != HAL_OK)
  {
    return HAL_ERROR;
  }
  *pCounter = 0xFFFFFFFFU;
  switch (CounterId)
  {
    case PLAT_NV_COUNTER_3 :
      if (OBK_data.Image0CurVersion >= OBK_data.Image0PrevVersion)
      {
        *pCounter = OBK_data.Image0CurVersion;
        return HAL_OK;
      }
      break;
    case PLAT_NV_COUNTER_4 :
      if (OBK_data.Image1CurVersion >= OBK_data.Image1PrevVersion)
      {
        *pCounter = OBK_data.Image1CurVersion;
        return HAL_OK;
      }
      break;
    case PLAT_NV_COUNTER_5 :
      if (OBK_data.Image2CurVersion >= OBK_data.Image2PrevVersion)
      {
        *pCounter = OBK_data.Image2CurVersion;
        return HAL_OK;
      }
      break;
    case PLAT_NV_COUNTER_6 :
      if (OBK_data.Image3CurVersion >= OBK_data.Image3PrevVersion)
      {
        *pCounter = OBK_data.Image3CurVersion;
        return HAL_OK;
      }
      break;
    default :
      break;
  }
  return HAL_ERROR;
}

/**
  * @brief  Update counter in OBKeys
  * @param  Counter: Fw version
  * @retval None
  */
HAL_StatusTypeDef OBK_UpdateNVCounter(enum tfm_nv_counter_t CounterId, uint32_t Counter)
{
  OBK_Hdpl1Data OBK_data = { 0U };

  /* Read all OBK hdpl1/2 data & control SHA256 */
  if (OBK_ReadHdpl1Data(&OBK_data) != HAL_OK)
  {
    return HAL_ERROR;
  }

  switch (CounterId)
  {
    case PLAT_NV_COUNTER_3 :
      OBK_data.Image0PrevVersion = OBK_data.Image0CurVersion;
      OBK_data.Image0CurVersion = Counter;
      break;
    case PLAT_NV_COUNTER_4 :
      OBK_data.Image1PrevVersion = OBK_data.Image1CurVersion;
      OBK_data.Image1CurVersion = Counter;
      break;
    case PLAT_NV_COUNTER_5 :
      OBK_data.Image2PrevVersion = OBK_data.Image2CurVersion;
      OBK_data.Image2CurVersion = Counter;
      break;
    case PLAT_NV_COUNTER_6 :
      OBK_data.Image3PrevVersion = OBK_data.Image3CurVersion;
      OBK_data.Image3CurVersion = Counter;
      break;
    default :
      return HAL_ERROR;
      break;
  }
  /* Update all OBK hdpl1 data with associated SHA256 */
  if (OBK_UpdateHdpl1Data(&OBK_data) != HAL_OK)
  {
    return HAL_ERROR;
  }
  else
  {
    return HAL_OK;
  }
}

/**
  * @brief  Init DHUK
  * @param  pBuffer pointer to the input buffer to be hashed
  * @param  Length length of the input buffer in bytes
  * @param  pSHA256 pointer to the compuyed digest
  * @retval None
  */
void OBK_InitDHUK(void)
{

}
