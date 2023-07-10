/**
  ******************************************************************************
* @file    GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/Secure/Src/external_memory_helper.c
* @author  MCD Application Team
* @brief   Helper C file to configure the external memory
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

#include "main.h"
#include "external_memory_helper.h"

#if defined (HAL_XSPI_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern XSPI_HandleTypeDef    hospi1;
XSPI_RegularCmdTypeDef sCommand = {0};
XSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

uint32_t address = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi);
void OSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi);
void OSPI_OctalModeCfg(XSPI_HandleTypeDef *hospi);

/**
* @brief  external memory configuration
* @retval None
*/
uint32_t extMemory_Config(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  HAL_XSPI_DLYB_CfgTypeDef HAL_OSPI_DLYB_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  HAL_OSPI_DLYB_Cfg_Struct.Units = 0;
  HAL_OSPI_DLYB_Cfg_Struct.PhaseSel = 0;
  if (HAL_XSPI_DLYB_SetConfig(&hospi1, &HAL_OSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    return 1;
  }
  /* Configure the memory in octal mode ------------------------------------- */
  OSPI_OctalModeCfg(&hospi1);

  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

  /* Enable write operations ------------------------------------------ */
  OSPI_WriteEnable(&hospi1);

  /* Erasing Sequence ------------------------------------------------- */
  sCommand.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction   = OCTAL_SECTOR_ERASE_CMD;
  sCommand.AddressMode   = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.Address       = address;
  sCommand.DataMode      = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles   = 0;

  if (HAL_XSPI_Command(&hospi1, &sCommand,5000U) != HAL_OK)
  {
    return 2;
  }

  /* Configure automatic polling mode to wait for end of erase ------ */
  OSPI_AutoPollingMemReady(&hospi1);

  /* Enable write operations ---------------------------------------- */
  OSPI_WriteEnable(&hospi1);

  /* Memory-mapped mode configuration ------------------------------- */
  sCommand.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
  sCommand.DataMode      = HAL_XSPI_DATA_8_LINES;
  sCommand.DataLength    = 1;
  sCommand.DQSMode       = HAL_XSPI_DQS_ENABLE;

  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 3;
  }

  sCommand.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.Instruction   = OCTAL_IO_READ_CMD;
  sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;
  sCommand.DQSMode       = HAL_XSPI_DQS_DISABLE;

  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return 4;
  }

  sMemMappedCfg.TimeOutActivation  = HAL_XSPI_TIMEOUT_COUNTER_ENABLE;
  sMemMappedCfg.TimeoutPeriodClock = 0x50;
  if (HAL_XSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
  {
    return 5;
  }

  return 0;
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hospi: OSPI handle
  * @retval None
  */
void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};

  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = OCTAL_WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction    = OCTAL_READ_STATUS_REG_CMD;
  sCommand.Address        = 0x0;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth   = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.DataMode       = HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode    = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DataLength     = 1;
  sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_REG;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue    = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask     = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode     = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime  = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hospi: OSPI handle
  * @retval None
  */
void OSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};


  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = OCTAL_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.Address            = 0x0;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DataLength         = 1;
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue    = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask     = MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode     = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime  = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function configure the memory in Octal mode.
  * @param  hospi: OSPI handle
  * @retval None
  */
void OSPI_OctalModeCfg(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};
  uint8_t reg;

  /* Enable write operations ---------------------------------------- */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
  sCommand.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DataLength  = 1;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue    = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask     = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode     = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime  = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Write Configuration register 2 (with new dummy cycles) --------- */
  sCommand.Instruction    = WRITE_CFG_REG_2_CMD;
  sCommand.Address        = CONFIG_REG2_ADDR3;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth   = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  reg = CR2_DUMMY_CYCLES_66MHZ;

  if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait that the memory is ready ---------------------------------- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_NONE;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask  = MEMORY_READY_MASK_VALUE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable write operations ---------------------------------------- */
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.MatchValue = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask  = WRITE_ENABLE_MASK_VALUE;

  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Write Configuration register 2 (with octal mode) --------------- */
  sCommand.Instruction = WRITE_CFG_REG_2_CMD;
  sCommand.Address     = CONFIG_REG2_ADDR1;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;

  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  reg = CR2_STR_OPI_ENABLE;

  if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Wait that the configuration is effective and check that memory is ready */
  HAL_Delay(MEMORY_REG_WRITE_DELAY);

  /* Wait that the memory is ready ---------------------------------- */
  OSPI_AutoPollingMemReady(hospi);
}

#endif /* HAL_XSPI_MODULE_ENABLED */

/**
* @}
*/

/**
* @}
*/
