/**
  ******************************************************************************
  * @file  : memory_toolbox.c
  * @brief : This file provides basic functions to achieve below services on the
             STM32 XPSI - Memory interface.
              + JEDEC ID reading function
              + SFDP Table reading function
              + Status/Configuration Registers management (read/write operations)
              + Write Enable command execution
              + Write In Progress flag check function (Auto-polling mode)
              + Memory Sector erase function
              + Page program function
              + Data read function (Indirect mode)
              + Memory-mapped mode management (enable/disable)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "memory_toolbox.h"

/* Private includes ----------------------------------------------------------*/
#include <string.h>
#if defined(EXAMPLE_TRACE_ENABLE)
#include <stdio.h>
#include <stdint.h>
#endif /* EXAMPLE_TRACE_ENABLE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Size of JEDEC ID value */
#define MEMTOOLBOX_JEDEC_ID_SIZE                              3U
/* Size of SFDP Header */
#define MEMTOOLBOX_SFDP_HEADER_SIZE                           8U
/* Size of SFDP Header pattern */
#define MEMTOOLBOX_SFDP_HEADER_START_SIZE                     4U
/* SFDP Header pattern */
#define MEMTOOLBOX_SFDP_HEADER_START                          ((uint8_t *) "SFDP")
/* Size of SFDP Parameter Header */
#define MEMTOOLBOX_SFDP_PARAMETER_HEADER_SIZE                 8U
/* Size of SFDP Parameter content */
#define MEMTOOLBOX_SFDP_PARAMETER_SIZE                        512U
/* Index of Nb of SFDP Parameters in SFDP Header */
#define MEMTOOLBOX_SFDP_HEADER_NB_PARAMETER_INDEX             6U
/* Index of Size of SFDP Parameter Table in SFDP Parameter Header */
#define MEMTOOLBOX_SFDP_PARAM_HEADER_PARAM_TABLE_SIZE_INDEX   3U
/* Timeout duration for completion of basic write operations (Register updates, Page program, ...) */
#define MEMTOOLBOX_DEFAULT_AUTOPOLLING_TIMEOUT                1000U   /* 1 sec */

#if defined(EXAMPLE_TRACE_ENABLE)
/* Nb of buffer data sent to trace output per line */
#define EXAMPLE_TRACE_NB_DATA_PER_LINE               16U
/* Size of line array storing data to be output : should be enough for nb of bytes per line bytes :
   "  " + "0xXX," * EXAMPLE_TRACE_NB_DATA_PER_LINE + null terminator */
#define EXAMPLE_TRACE_DATA_LINE_STRING_SIZE          83U
#endif /* EXAMPLE_TRACE_ENABLE */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief Extract SFDP Parameter size from SFDP Parameter Header
  */
#define MEMTOOLBOX_EXTRACT_SFDP_PARAM_TABLE_SIZE_FROM_PARAM_HEADER(_PARAM_HEADER_)                          \
  ((uint32_t)_PARAM_HEADER_[4]) | ((uint32_t)_PARAM_HEADER_[5] << 8) | ((uint32_t)_PARAM_HEADER_[6] << 16); \

#if defined(EXAMPLE_TRACE_ENABLE)
/**
  * @brief Trace macro for a string
  */
#define EXAMPLE_TRACE_STR(_STR_)                                                                      \
  {                                                                                                   \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "  ");                             \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) _STR_);                            \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "\n\r");                           \
  }

/**
  * @brief Trace macro for a string and a value (hex)
  */
#define EXAMPLE_TRACE_STR_HEX(_STR_, _HEX_)                                                           \
  {                                                                                                   \
    char str[15];                                                                                     \
    (void)sprintf(str, " 0x%x", (unsigned int)_HEX_);                                                 \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "  ");                             \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) _STR_);                            \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) str);                              \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "\n\r");                           \
  }

/**
  * @brief Trace macro for a string and a command (hex)
  */
#define EXAMPLE_TRACE_STR_CMD(_STR_, _CMD_, _WIDTH_)                                                  \
  {                                                                                                   \
    char str[10];                                                                                     \
    if ((_WIDTH_) == HAL_XSPI_INSTRUCTION_8_BITS)                                                     \
      (void)sprintf(str, " [0x%02X]", (unsigned int)_CMD_);                                           \
    else                                                                                              \
      (void)sprintf(str, " [0x%04X]", (unsigned int)_CMD_);                                           \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "  ");                             \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) _STR_);                            \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) str);                              \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "\n\r");                           \
  }

/**
  * @brief Debug macro for a string and a value (int)
  */
#define EXAMPLE_TRACE_STR_INT(_STR_, _INT_)                                                           \
  {                                                                                                   \
    char str[10];                                                                                     \
    (void)sprintf(str, " %u", (unsigned int)_INT_);                                                   \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "  ");                             \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) _STR_);                            \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) str);                              \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "\n\r");                           \
  }

/**
  * @brief Debug macro for a string and a data buffer
  */
#define EXAMPLE_TRACE_BUFFER(_STR_, _BUF_, _SIZE_)                                                    \
  {                                                                                                   \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_CMD_INFO, (uint8_t *) "  ");                             \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_ALL, (uint8_t *) _STR_);                                 \
    Memory_Toolbox_Trace(EXAMPLE_TRACE_LEVEL_ALL, (uint8_t *) "\n\r");                                \
    Memory_Toolbox_TraceBuffer(EXAMPLE_TRACE_LEVEL_ALL, (const uint8_t *)(_BUF_), (uint16_t)(_SIZE_));\
  }

#else

/**
  * @brief Empty trace macro definitions
  */
#define EXAMPLE_TRACE_STR(_STR_)
#define EXAMPLE_TRACE_STR_HEX(_STR_, _HEX_)
#define EXAMPLE_TRACE_STR_CMD(_STR_, _CMD_, _WIDTH_)
#define EXAMPLE_TRACE_STR_INT(_STR_, _INT_)
#define EXAMPLE_TRACE_BUFFER(_STR_, _BUF_, _SIZE_)

#endif /* EXAMPLE_TRACE_ENABLE */

/* Private variables ---------------------------------------------------------*/
#if defined(EXAMPLE_TRACE_ENABLE)
static Memory_Toolbox_TraceCallback_t memory_toolbox_trace_cb = NULL;
static Memory_Toolbox_TraceLevel_t memory_toolbox_current_trace_level = EXAMPLE_TRACE_LEVEL_CMD_INFO;

/* Messages to be displayed in logs according to execution status */
const char *memory_toolbox_status_msgs[MEMORY_MAX_STATUS_NB] =
{
  "Execution OK",
  "ERROR : Command Operation failed",
  "ERROR : Transmit Operation failed",
  "ERROR : Receive Operation failed",
  "ERROR : MemoryMap Operation failed",
  "ERROR : AutoPolling Operation failed",
  "ERROR : Abort Operation failed",
  "ERROR : Timeout Operation",
  "ERROR : Buffer comparison failed",
  "ERROR : Unexpected JEDEC ID value",
  "ERROR : Unexpected SFDP Header",
  "ERROR : Error while reading SR",
  "ERROR : Error while checking WEL bit in SR after Write Enable command",
  "ERROR : Error while checking WEL bit in SR after Write Disable command",
  "ERROR : Error while updating RCC configuration for reaching max perf",
  "ERROR : Error while updating DLYB configuration",
  /* ... add more messages as needed */
};
#endif /* EXAMPLE_TRACE_ENABLE */

/* Register name table definition.
   (depends on selected memory according to project compilation option.) */
#if defined(M2_M95P32)
/* Memory Device STMicrolectronics M95P32-IXMNT/E :
    - Status Register,
    - Configuration Register */
const char *memory_toolbox_register_names[MEMORY_REG_ID_COUNT] = { "SR", "CR" };
#elif defined(M2_IS25LP032D)
/* Memory Device ISSI IS25LP032DJNLE-TR  :
    - Status Register,
    - Read Parameters Register,
    - Extended Register */
const char *memory_toolbox_register_names[MEMORY_REG_ID_COUNT] = { "SR", "RR", "ER" };
#elif defined(M2_W25Q16JV)
/* Memory Device Winbond W25Q16JVSNIQ  :
    - Status Register,
    - Status Register 2,
    - Status Register 3  */
const char *memory_toolbox_register_names[MEMORY_REG_ID_COUNT] = { "SR", "SR2", "SR3" };
#elif defined(M2_MX25UW25645G)
/* Memory Device Macronix MX25UW25645GXDI00  :
    - Status Register,
    - Configuration Register,
    - Configuration Register 2 */
const char *memory_toolbox_register_names[MEMORY_REG_ID_COUNT] = { "SR", "CR", "CR2" };
#elif defined(M2_MX25LM51245G)
/* Memory Device Macronix MX25LM51245GXDI00  :
    - Status Register,
    - Configuration Register,
    - Configuration Register 2 */
const char *memory_toolbox_register_names[MEMORY_REG_ID_COUNT] = { "SR", "CR", "CR2" };
#else
#warning "Please define names corresponding to registers managed in your memory"
#endif /* M2_M95P32 */

uint8_t memory_toolbox_register_dump[MEMORY_REG_ID_COUNT] = {0};

/* Private function prototypes -----------------------------------------------*/


/* Private user code ---------------------------------------------------------*/

/**
  * @brief  Reads the JEDEC ID from the memory.
  * @param  hxspi: XSPI handle
  * @param  JedecID: Pointer to buffer to store JEDEC ID
  * @retval MEMORY_OK             JEDEC ID Read operation performed successfully
  * @retval MEMORY_CMD_KO         Error detected during command configuration
  * @retval MEMORY_WRONG_JEDEC_ID Unexpected JEDEC ID Value
  */
Memory_Toolbox_Status_t Memory_Toolbox_Read_JedecID(XSPI_HandleTypeDef *hxspi, uint8_t *JedecID)
{
  uint8_t expected_JedecId[MEMTOOLBOX_JEDEC_ID_SIZE] = MEMORY_READ_JEDEC_ID_VALUE;
  XSPI_RegularCmdTypeDef  sCommand = {0};

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read JEDEC ID", MEMORY_READ_JEDEC_ID_CMD, HAL_XSPI_INSTRUCTION_8_BITS);

  /* Read operation ------------------------------------------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = MEMORY_READ_JEDEC_ID_CMD;
  sCommand.InstructionMode    = (MEMORY_READ_JEDEC_ID_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_JEDEC_ID_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.DataMode           = (MEMORY_READ_JEDEC_ID_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataLength         = MEMTOOLBOX_JEDEC_ID_SIZE;
  sCommand.DummyCycles        = 0U;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  if (HAL_XSPI_Receive(hxspi, JedecID, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  EXAMPLE_TRACE_BUFFER("  => JEDEC ID value :", JedecID, MEMTOOLBOX_JEDEC_ID_SIZE);

  /* Check if read JEDEC ID is corresponding to expected one */
  if (Memory_Toolbox_MemCmp(JedecID, expected_JedecId, MEMTOOLBOX_JEDEC_ID_SIZE) != MEMORY_OK)
  {
    EXAMPLE_TRACE_BUFFER("  => Expected JEDEC ID value :", expected_JedecId, MEMTOOLBOX_JEDEC_ID_SIZE);
    return (MEMORY_WRONG_JEDEC_ID);
  }

  return MEMORY_OK;
}


/**
  * @brief  Reads the SFDP table data from the memory.
  * @param  hxspi: XSPI handle
  * @param  SFDP_Data: Pointer to buffer to store SFDP table data
  * @param  SFDP_Data_Addr: Address of SFDP table data
  * @param  SFDP_Data_Size: Size of SFDP table data to read
  * @retval MEMORY_OK           SFDP Data read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during command configuration
  */
Memory_Toolbox_Status_t Memory_Toolbox_Read_SFDP_Data(XSPI_HandleTypeDef *hxspi, uint8_t *SFDP_Data,
                                                      uint32_t SFDP_Data_Addr, uint32_t SFDP_Data_Size)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read SFDP Data", MEMORY_READ_SFDP_CMD, HAL_XSPI_INSTRUCTION_8_BITS);
  EXAMPLE_TRACE_STR_HEX("  Address :", SFDP_Data_Addr);
  EXAMPLE_TRACE_STR_HEX("  Size    :", SFDP_Data_Size);

  /* Read operation ------------------------------------------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = MEMORY_READ_SFDP_CMD;
  sCommand.InstructionMode    = (MEMORY_READ_SFDP_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_SFDP_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode        = (MEMORY_READ_SFDP_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_READ_SFDP_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode           = (MEMORY_READ_SFDP_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.Address            = SFDP_Data_Addr;
  sCommand.DataLength         = SFDP_Data_Size;
  sCommand.DummyCycles        = MEMORY_SFDP_DUMMY_CYCLES;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  if (HAL_XSPI_Receive(hxspi, SFDP_Data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  EXAMPLE_TRACE_BUFFER("  => SFDP Data :", SFDP_Data, SFDP_Data_Size);

  return MEMORY_OK;
}

/**
  * @brief  Reads all SFDP table content from the memory.
  * @param  hxspi: XSPI handle
  * @retval MEMORY_OK                  SFDP table read operation performed successfully
  * @retval MEMORY_CMD_KO              Error detected during command configuration
  * @retval MEMORY_WRONG_SFDP_HEADER   Error detected in SFDP Header data content
  */
Memory_Toolbox_Status_t Memory_Toolbox_Read_All_SFDP_Table(XSPI_HandleTypeDef *hxspi)
{
  uint32_t nb_param      = 0U;
  uint32_t param_address = 0U;
  uint32_t param_size    = 0U;
  uint8_t  SFDP_header[MEMTOOLBOX_SFDP_HEADER_SIZE]            = {0U};
  uint8_t  param_header[MEMTOOLBOX_SFDP_PARAMETER_HEADER_SIZE] = {0U};
  uint8_t  param[MEMTOOLBOX_SFDP_PARAMETER_SIZE]               = {0U};
  Memory_Toolbox_Status_t status      = MEMORY_OK;

  EXAMPLE_TRACE_STR("[MEM Toolbox] Read All SFDP Table");

  /* Read SFDP Header in 1 Line  mode ---------------------------------------- */
  EXAMPLE_TRACE_STR("  READ SFDP Header");
  status = Memory_Toolbox_Read_SFDP_Data(hxspi, SFDP_header, 0, MEMTOOLBOX_SFDP_HEADER_SIZE);

  /* Check if read SFDP header has correct start pattern */
  if (Memory_Toolbox_MemCmp(SFDP_header, MEMTOOLBOX_SFDP_HEADER_START, MEMTOOLBOX_SFDP_HEADER_START_SIZE) != MEMORY_OK)
  {
    return (MEMORY_WRONG_SFDP_HEADER);
  }

  /* Extract nb of SFDP Parameters from SFDP Header */
  nb_param = SFDP_header[MEMTOOLBOX_SFDP_HEADER_NB_PARAMETER_INDEX] + 1U;

  /* Iterate on number of SFDP parameters */
  for (uint8_t i = 0U; i < nb_param; i++)
  {
    /* Read SFDP parameter header in 1 Line  mode ---------------------------------------- */
    EXAMPLE_TRACE_STR_INT("  READ SFDP Parameter Header Nb:", (i + 1));
    status = Memory_Toolbox_Read_SFDP_Data(hxspi, param_header,
                                           MEMTOOLBOX_SFDP_HEADER_SIZE + (i * MEMTOOLBOX_SFDP_PARAMETER_HEADER_SIZE),
                                           MEMTOOLBOX_SFDP_PARAMETER_HEADER_SIZE);

    /* Retrieve address and size of parameter from SFDP table */
    param_size = 4U * param_header[MEMTOOLBOX_SFDP_PARAM_HEADER_PARAM_TABLE_SIZE_INDEX];
    if (param_size > MEMTOOLBOX_SFDP_PARAMETER_SIZE)
    {
      param_size = MEMTOOLBOX_SFDP_PARAMETER_SIZE;
    }
    param_address = MEMTOOLBOX_EXTRACT_SFDP_PARAM_TABLE_SIZE_FROM_PARAM_HEADER(param_header);

    /* Read SFDP parameter content in 1 Line  mode ---------------------------------------- */
    EXAMPLE_TRACE_STR_INT("  READ SFDP Parameter Content Nb:", (i + 1));
    status = Memory_Toolbox_Read_SFDP_Data(hxspi, param, param_address, param_size);
  }

  return status;
}

/**
  * @brief  Sends a Write Enable command and waits until it is effective.
  * @param  hxspi: XSPI handle
  * @param  mode: Performance mode
  * @retval MEMORY_OK                      Write enable performed successfully
  * @retval MEMORY_CMD_KO                  Error detected during command configuration
  * @retval MEMORY_READ_SR_KO              Error detected during Status Register reading
  * @retval MEMORY_WRITE_ENABLE_CHECK_KO   Error while checking WEL bit in SR after Write Enable command
  * @retval MEMORY_WRITE_DISABLE_CHECK_KO  Error while checking WEL bit in SR after Write Disable command
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_Check_Write_Enabling(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef s_config = {0};
  uint8_t RegBufferRead[2]         = {0};

  EXAMPLE_TRACE_STR("[MEM Toolbox] Check Write Enable and Write Disable commands");

  /* Check Write enable Latch bit in SR is Disabled */
  if (Memory_Toolbox_ReadStatusRegister(hxspi, RegBufferRead, 1U, MEMORY_STARTUP_MODE) != MEMORY_OK)
  {
    return (MEMORY_READ_SR_KO);
  }
  else
  {
    if ((RegBufferRead[0] & MEMORY_WRITE_ENABLE_MASK_VALUE) == MEMORY_WRITE_ENABLE_MATCH_VALUE)
    {
      return (MEMORY_WRITE_ENABLE_CHECK_KO);
    }
  }

  EXAMPLE_TRACE_STR_CMD("  Send Write Enable", MEMORY_WRITE_ENABLE_CMD,
                        (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Write Enable command configuration --------------------------------- */
  sCommand.Instruction        = MEMORY_WRITE_ENABLE_CMD;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_WRITE_ENABLE_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = 0U;

  /* Execute Write Enable command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  /* Reconfigure XSPI to automatic polling mode to wait for write enabling */
  sCommand.Instruction        = MEMORY_READ_SR_CMD;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = MEMORY_READ_SR_DUMMY_CYCLE;
  sCommand.Address            = MEMORY_READ_SR_ADDRESS;

  /* If Write Enable is done in DTR, RDSR is also done in DTR (Data included) */
  if (sCommand.InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE)
  {
    sCommand.DataLength    = 2;
  }
  else
  {
    sCommand.DataLength    = 1;
  }

  /* Send the command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  s_config.MatchValue         = MEMORY_WRITE_ENABLE_MATCH_VALUE;
  s_config.MatchMask          = MEMORY_WRITE_ENABLE_MASK_VALUE;
  s_config.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime       = 0x10;
  s_config.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  /* Perform Auto polling on WEL bit */
  if (HAL_XSPI_AutoPolling(hxspi, &s_config, MEMTOOLBOX_DEFAULT_AUTOPOLLING_TIMEOUT) != HAL_OK)
  {
    return (MEMORY_WRITE_ENABLE_CHECK_KO);
  }

  /* Read Status Register and check read value (WEL bit) : Check Write enable Latch bit in SR is Enabled */
  if (Memory_Toolbox_ReadStatusRegister(hxspi, RegBufferRead, 1U, MEMORY_STARTUP_MODE) != MEMORY_OK)
  {
    return (MEMORY_READ_SR_KO);
  }
  else
  {
    if ((RegBufferRead[0] & MEMORY_WRITE_ENABLE_MASK_VALUE) != MEMORY_WRITE_ENABLE_MATCH_VALUE)
    {
      return (MEMORY_WRITE_ENABLE_CHECK_KO);
    }
  }

  EXAMPLE_TRACE_STR_CMD("  Send Write Disable", MEMORY_WRITE_DISABLE_CMD,
                        (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Write Disable command configuration --------------------------------- */
  sCommand.Instruction        = MEMORY_WRITE_DISABLE_CMD;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_WRITE_DISABLE_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = 0U;

  /* Execute Write Disable command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  /* Reconfigure XSPI to automatic polling mode to wait for write disabling */
  sCommand.Instruction        = MEMORY_READ_SR_CMD;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_READ_SR_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = MEMORY_READ_SR_DUMMY_CYCLE;
  sCommand.Address            = MEMORY_READ_SR_ADDRESS;

  /* If Write Enable is done in DTR, RDSR is also done in DTR (Data included) */
  if (sCommand.InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE)
  {
    sCommand.DataLength    = 2;
  }
  else
  {
    sCommand.DataLength    = 1;
  }

  /* Send the command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return (MEMORY_CMD_KO);
  }

  s_config.MatchValue         = (~MEMORY_WRITE_ENABLE_MATCH_VALUE) & MEMORY_WRITE_ENABLE_MASK_VALUE;
  s_config.MatchMask          = MEMORY_WRITE_ENABLE_MASK_VALUE;
  s_config.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime       = 0x10;
  s_config.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  /* Perform Auto polling on WEL bit */
  if (HAL_XSPI_AutoPolling(hxspi, &s_config, MEMTOOLBOX_DEFAULT_AUTOPOLLING_TIMEOUT) != HAL_OK)
  {
    return (MEMORY_WRITE_DISABLE_CHECK_KO);
  }

  /* Read Status Register and check read value (WEL bit) : Check Write enable Latch bit in SR is Disabled */
  if (Memory_Toolbox_ReadStatusRegister(hxspi, RegBufferRead, 1U, MEMORY_STARTUP_MODE) != MEMORY_OK)
  {
    return (MEMORY_READ_SR_KO);
  }
  else
  {
    if ((RegBufferRead[0] & MEMORY_WRITE_ENABLE_MASK_VALUE) == MEMORY_WRITE_ENABLE_MATCH_VALUE)
    {
      return (MEMORY_WRITE_DISABLE_CHECK_KO);
    }
  }

  return MEMORY_OK;
}

/**
  * @brief  Sends a Write Enable command and waits until it is effective.
  * @param  hxspi: XSPI handle
  * @param  mode: Performance mode
  * @retval MEMORY_OK           Write enable performed successfully
  * @retval MEMORY_CMD_KO       Error detected during command configuration
  * @retval MEMORY_AUTOPOLL_KO  Error detected during auto-polling
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteEnable(XSPI_HandleTypeDef *hxspi, Memory_Toolbox_PerfMode_t mode)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef s_config = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;
  uint32_t cmd;
  uint32_t access_mode;
  uint32_t dummy_cycles;
  uint32_t address;

  if (mode == MEMORY_STARTUP_MODE)
  {
    cmd          = MEMORY_WRITE_ENABLE_CMD;
    access_mode  = MEMORY_WRITE_ENABLE_ACCESS_MODE;
  }
  else
  {
    cmd          = MEMORY_HIGH_PERF_WRITE_ENABLE_CMD;
    access_mode  = MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE;
  }

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Enable", cmd, (access_mode & XSPI_CCR_ISIZE_Msk));

  /* Write Enable command configuration --------------------------------- */
  sCommand.Instruction        = cmd;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (access_mode & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (access_mode & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (access_mode & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (access_mode & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (access_mode & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (access_mode & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (access_mode & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (access_mode & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (access_mode & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = 0U;

  /* Execute Write Enable command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (mode == MEMORY_STARTUP_MODE)
  {
    cmd          = MEMORY_READ_SR_CMD;
    access_mode  = MEMORY_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_READ_SR_ADDRESS;
  }
  else
  {
    cmd          = MEMORY_HIGH_PERF_READ_SR_CMD;
    access_mode  = MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_HIGH_PERF_READ_SR_ADDRESS;
  }

  /* Reconfigure XSPI to automatic polling mode to wait for write enabling */
  sCommand.Instruction        = cmd;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (access_mode & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (access_mode & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (access_mode & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (access_mode & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (access_mode & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (access_mode & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (access_mode & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (access_mode & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (access_mode & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = dummy_cycles;
  sCommand.Address            = address;

  /* If Write Enable is done in DTR, RDSR is also done in DTR (Data included) */
  if (sCommand.InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE)
  {
    sCommand.DataLength    = 2;
  }
  else
  {
    sCommand.DataLength    = 1;
  }

  /* Send the command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  s_config.MatchValue         = MEMORY_WRITE_ENABLE_MATCH_VALUE;
  s_config.MatchMask          = MEMORY_WRITE_ENABLE_MASK_VALUE;
  s_config.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime       = 0x10;
  s_config.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hxspi, &s_config, MEMTOOLBOX_DEFAULT_AUTOPOLLING_TIMEOUT) != HAL_OK)
  {
    status = MEMORY_AUTOPOLL_KO;
  }

  return status;
}

/**
  * @brief  Polls the memory until it is ready (not busy).
  * @param  hxspi: XSPI handle
  * @param  mode: Performance mode
  * @param  timeout: Maximum time allocated for reaching ready state
  * @retval MEMORY_OK           Memory is ready
  * @retval MEMORY_CMD_KO       Error detected during command configuration
  * @retval MEMORY_AUTOPOLL_KO  Error detected during auto-polling
  */
Memory_Toolbox_Status_t Memory_Toolbox_AutoPollingMemReady(XSPI_HandleTypeDef *hxspi, Memory_Toolbox_PerfMode_t mode,
                                                           uint32_t timeout)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef s_config = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;
  uint32_t cmd;
  uint32_t access_mode;
  uint32_t dummy_cycles;
  uint32_t address;

  EXAMPLE_TRACE_STR("[MEM Toolbox] AutoPolling MemReady");

  if (mode == MEMORY_STARTUP_MODE)
  {
    cmd          = MEMORY_READ_SR_CMD;
    access_mode  = MEMORY_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_READ_SR_ADDRESS;
  }
  else
  {
    cmd          = MEMORY_HIGH_PERF_READ_SR_CMD;
    access_mode  = MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_HIGH_PERF_READ_SR_ADDRESS;
  }

  /* Configure XSPI to automatic polling mode to wait end of write in progress */
  sCommand.Instruction        = cmd;
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.InstructionMode    = (access_mode & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (access_mode & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (access_mode & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (access_mode & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (access_mode & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (access_mode & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (access_mode & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (access_mode & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (access_mode & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = dummy_cycles;
  sCommand.Address            = address;

  /* If WIP check is done in DTR, RDSR is also done in DTR (Data included) */
  if (sCommand.InstructionDTRMode == HAL_XSPI_INSTRUCTION_DTR_ENABLE)
  {
    sCommand.DataLength    = 2;
  }
  else
  {
    sCommand.DataLength    = 1;
  }

  /* Send the command */
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  s_config.MatchValue         = MEMORY_WIP_STATUS_MATCH_VALUE;
  s_config.MatchMask          = MEMORY_WIP_STATUS_MASK_VALUE;
  s_config.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime       = 0x10;
  s_config.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(hxspi, &s_config, timeout) != HAL_OK)
  {
    status = MEMORY_AUTOPOLL_KO;
  }

  return status;
}


/**
  * @brief  Erases a sector of the memory.
  * @param  hxspi: XSPI handle
  * @param  Sector_Addr: Address of sector to erase
  * @retval MEMORY_OK           Erase operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during erase operation
  */

Memory_Toolbox_Status_t Memory_Toolbox_Erase_Sector(XSPI_HandleTypeDef *hxspi, uint32_t Sector_Addr)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Erase Sector", MEMORY_SECTOR_ERASE_CMD,
                        (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_STR_HEX("  Address :", Sector_Addr);

  /* Configure automatic polling mode to wait for end of any previous Write operation -------- */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_HIGH_PERFORMANCE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_HIGH_PERFORMANCE);

  /* Erasing Sequence --------------------------------------------------- */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = MEMORY_SECTOR_ERASE_CMD;
  sCommand.InstructionMode    = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DQSMode            = (MEMORY_SECTOR_ERASE_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.Address            = Sector_Addr;
  sCommand.DummyCycles        = 0;

  EXAMPLE_TRACE_STR("  Send Erase Sector command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  /* Configure automatic polling mode to wait for end of erase -------- */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_HIGH_PERFORMANCE, MEMORY_SECTOR_ERASE_TIMEOUT);

  return status;
}

/**
  * @brief  Programs a page of the memory.
  * @param  hxspi: XSPI handle
  * @param  aPageBuffer: Buffer containing data to write
  * @param  Page_Addr: Address of the page
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  */
Memory_Toolbox_Status_t Memory_Toolbox_Program_Page(XSPI_HandleTypeDef *hxspi, uint8_t *aPageBuffer, uint32_t Page_Addr)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Program Page", MEMORY_PAGE_PROG_CMD,
                        (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_STR_HEX("  Address :", Page_Addr);
  EXAMPLE_TRACE_BUFFER("  => Data to write:", aPageBuffer, MEMORY_PAGE_SIZE);

  /* Configure automatic polling mode to wait for end of any write/erase operation (if any) -------- */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_HIGH_PERFORMANCE, MEMORY_SECTOR_ERASE_TIMEOUT);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_HIGH_PERFORMANCE);

  /* Writing Sequence ------------------------------------------------- */
  sCommand.Instruction        = MEMORY_PAGE_PROG_CMD;
  sCommand.InstructionMode    = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionWidth   = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.AddressMode        = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressWidth       = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.DataMode           = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DataLength         = MEMORY_PAGE_SIZE;
  sCommand.Address            = Page_Addr;

  EXAMPLE_TRACE_STR("  Send Page Program command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aPageBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  return status;
}

/**
  * @brief  This function reads a data area in memory
  * @param  hxspi: XSPI handle
  * @param  Read_Data_Buffer: buffer who contains the data read from memory
  * @param  Data_Addr: address of the data to be read
  * @param  Data_Size: Size of the data to be read (in nb of bytes)
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during Read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during Read operation execution
  */
Memory_Toolbox_Status_t Memory_Toolbox_Read_MemData(XSPI_HandleTypeDef *hxspi, uint8_t *Read_Data_Buffer,
                                                    uint32_t Data_Addr, uint32_t Data_Size)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Data", MEMORY_READ_CMD,
                        (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_STR_HEX("  Address :", Data_Addr);
  EXAMPLE_TRACE_STR_HEX("  Size    :", Data_Size);

  /* Wait for end of any write/erase operation (if any) -------------- */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_HIGH_PERFORMANCE, MEMORY_SECTOR_ERASE_TIMEOUT);

  /* Read Sequence ---------------------------------------------------- */
  sCommand.Instruction        = MEMORY_READ_CMD;
  sCommand.InstructionMode    = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode        = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth       = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode           = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = MEMORY_READ_DUMMY_CYCLE;
  sCommand.DataLength         = Data_Size;
  sCommand.Address            = Data_Addr;

  EXAMPLE_TRACE_STR("  Send Read command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, Read_Data_Buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }
  EXAMPLE_TRACE_BUFFER("  => Read data :", Read_Data_Buffer, Data_Size);

  return status;
}

/**
  * @brief  This function reads a data area in memory in Memory mapped mode
  * @param  Read_Data_Buffer: buffer who contains the data read from memory
  * @param  Data_Addr: address of the data to be read
  * @param  Data_Size: Size of the data to be read (in nb of bytes)
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during Read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during Read operation execution
  */
Memory_Toolbox_Status_t Memory_Toolbox_Read_MemData_MemoryMapped_Mode(uint8_t *Read_Data_Buffer,
                                                                      uint8_t *Data_Addr, uint32_t Data_Size)
{
  EXAMPLE_TRACE_STR("[MEM Toolbox] Read Data in Memory Mapped mode");
  EXAMPLE_TRACE_STR_HEX("  Address :", Data_Addr);
  EXAMPLE_TRACE_STR_HEX("  Size    :", Data_Size);

  /* Reading operation in Memory Mapped mode is here achieved thanks to a memcpy() call,
     but could also be implemented thanks to DMA Memory to Memory transfers */
  memcpy(Read_Data_Buffer, Data_Addr, Data_Size);

  EXAMPLE_TRACE_BUFFER("  => Read data (MMP) :", Read_Data_Buffer, Data_Size);

  return MEMORY_OK;
}

/**
  * @brief  Enables memory-mapped mode.
  * @param  hxspi: XSPI handle
  * @retval MEMORY_OK           Operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during configuration
  * @retval MEMORY_MMP_MODE_KO  Error detected during memory-mapped mode activation
  */
Memory_Toolbox_Status_t Memory_Toolbox_Enable_MemoryMapped_Mode(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand        = {0};
  XSPI_MemoryMappedTypeDef sMemMappedCfg  = {0};
  Memory_Toolbox_Status_t status          = MEMORY_OK;

  EXAMPLE_TRACE_STR("[MEM Toolbox] Enable Memory Mapped Mode");

  /* Configure automatic polling mode to wait for end of any write/erase operation (if any) -------- */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_HIGH_PERFORMANCE, MEMORY_SECTOR_ERASE_TIMEOUT);

  /* Memory-mapped mode configuration --------------------------------- */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  sCommand.Instruction        = MEMORY_READ_CMD;
  sCommand.InstructionMode    = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth   = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode        = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth       = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode           = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode            = (MEMORY_READ_ACCESS_MODE & XSPI_CCR_DQSE_Msk);
  sCommand.DummyCycles        = MEMORY_READ_DUMMY_CYCLE;
  sCommand.Address            = 0;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  sCommand.OperationType      = HAL_XSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction        = MEMORY_PAGE_PROG_CMD;
  sCommand.InstructionMode    = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth   = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode        = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode     = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth       = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode           = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode        = (MEMORY_PAGE_PROG_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DummyCycles        = 0U;

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  sMemMappedCfg.TimeOutActivation  = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeoutPeriodClock = 0x40;

  if (HAL_XSPI_MemoryMapped(hxspi, &sMemMappedCfg) != HAL_OK)
  {
    status = MEMORY_MMP_MODE_KO;
  }

  return status;
}

/**
  * @brief  Disables memory-mapped mode.
  * @param  hxspi: XSPI handle
  * @retval MEMORY_OK           Operation performed successfully
  * @retval MEMORY_ABORT_KO     Error detected during abort operation
  */
Memory_Toolbox_Status_t Memory_Toolbox_Disable_MemoryMapped_Mode(XSPI_HandleTypeDef *hxspi)
{
  Memory_Toolbox_Status_t status = MEMORY_OK;

  EXAMPLE_TRACE_STR("[MEM Toolbox] Disable Memory Mapped Mode");

  /* Abort OctoSPI driver to stop the memory-mapped mode -------------- */
  if (HAL_XSPI_Abort(hxspi) != HAL_OK)
  {
    status = MEMORY_ABORT_KO;
  }

  return status;
}

/**
  * @brief  Writes the status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Status Register", MEMORY_WRITE_SR_CMD,
                        (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => SR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Status register: */
  sCommand.Instruction         = MEMORY_WRITE_SR_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_SR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.Address             = 0;

  EXAMPLE_TRACE_STR("  Send Write SR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @param  mode: Performance mode
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                          uint8_t RegisterBufferSize, Memory_Toolbox_PerfMode_t mode)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;
  uint32_t cmd;
  uint32_t access_mode;
  uint32_t dummy_cycles;
  uint32_t address;

  if (mode == MEMORY_STARTUP_MODE)
  {
    cmd          = MEMORY_READ_SR_CMD;
    access_mode  = MEMORY_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_READ_SR_ADDRESS;
  }
  else
  {
    cmd          = MEMORY_HIGH_PERF_READ_SR_CMD;
    access_mode  = MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE;
    dummy_cycles = MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE;
    address      = MEMORY_HIGH_PERF_READ_SR_ADDRESS;
  }

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Status Register", cmd, (access_mode & XSPI_CCR_ISIZE_Msk));

  /* Read Status register */
  sCommand.Instruction         = cmd;
  sCommand.InstructionMode     = (access_mode & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (access_mode & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (access_mode & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (access_mode & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (access_mode & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (access_mode & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (access_mode & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (access_mode & XSPI_CCR_DDTR_Msk);
  sCommand.DQSMode             = (access_mode & XSPI_CCR_DQSE_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = dummy_cycles;
  sCommand.Address             = address;

  EXAMPLE_TRACE_STR("  Send Read SR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => SR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR, aRegisterBuffer[0]);

  return status;
}

#if defined(M2_MX25UW25645G) || defined(M2_MX25LM51245G) || defined(M2_M95P32)
/**
  * @brief  Writes the configuration register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  * @note On some memories, Configuration Register corresponds to Function Register.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteConfigRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Configuration Register", MEMORY_WRITE_CR_CMD,
                        (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => CR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Configuration register: */
  sCommand.Instruction         = MEMORY_WRITE_CR_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_CR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;

  EXAMPLE_TRACE_STR("  Send Write CR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_CR, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the configuration register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadConfigRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                          uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Configuration Register", MEMORY_READ_CR_CMD,
                        (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read Configuration register: */
  sCommand.Instruction         = MEMORY_READ_CR_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_CR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = MEMORY_READ_CR_DUMMY_CYCLE;

  EXAMPLE_TRACE_STR("  Send Read CR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => CR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_CR, aRegisterBuffer[0]);

  return status;
}
#endif /* M2_MX25UW25645G || M2_MX25LM51245G || M2_M95P32 */

#if defined(M2_MX25UW25645G) || defined(M2_MX25LM51245G)
/**
  * @brief  Writes the second configuration register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @param  Reg_Addr: Register address
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteConfigRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                            uint8_t RegisterBufferSize, uint32_t Reg_Addr)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Configuration Register 2", MEMORY_WRITE_CR2_CMD,
                        (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => CR2 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Configuration register 2 */
  sCommand.Instruction         = MEMORY_WRITE_CR2_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_CR2_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.Address             = Reg_Addr;

  EXAMPLE_TRACE_STR("  Send Write CR2 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_CR2, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the second configuration register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @param  Reg_Addr: Register address
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadConfigRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize, uint32_t Reg_Addr)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Configuration Register 2", MEMORY_READ_CR2_CMD,
                        (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read Configuration register 2 */
  sCommand.Instruction         = MEMORY_READ_CR2_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_ADMODE_Msk);
  sCommand.AddressDTRMode      = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_ADDTR_Msk);
  sCommand.AddressWidth        = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_ADSIZE_Msk);
  sCommand.DataMode            = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_CR2_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.Address             = Reg_Addr;

  EXAMPLE_TRACE_STR("  Send Read CR2 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => CR2 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_CR2, aRegisterBuffer[0]);

  return status;
}
#endif /* M2_MX25UW25645G || M2_MX25LM51245G */

#if defined(M2_W25Q16JV)
/**
  * @brief  Writes the second Status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister2(XSPI_HandleTypeDef *hxspi,
                                                            uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Status Register 2", MEMORY_WRITE_SR2_CMD,
                        (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => SR2 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Status register2 */
  sCommand.Instruction         = MEMORY_WRITE_SR2_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_SR2_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;

  EXAMPLE_TRACE_STR("  Send Write SR2 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR2, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the second Status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Status Register 2", MEMORY_READ_SR2_CMD,
                        (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read Status register 2 */
  sCommand.Instruction         = MEMORY_READ_SR2_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_SR2_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = MEMORY_READ_SR2_DUMMY_CYCLE;

  EXAMPLE_TRACE_STR("  Send Read SR2 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => SR2 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR2, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Writes the 3rd Status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister3(XSPI_HandleTypeDef *hxspi,
                                                            uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Status Register 3", MEMORY_WRITE_SR3_CMD,
                        (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => SR3 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Status register3 */
  sCommand.Instruction         = MEMORY_WRITE_SR3_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_SR3_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;

  EXAMPLE_TRACE_STR("  Send Write SR3 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR3, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the 3rd Status register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister3(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Status Register 3", MEMORY_READ_SR3_CMD,
                        (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read Status register 3 */
  sCommand.Instruction         = MEMORY_READ_SR3_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_SR3_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = MEMORY_READ_SR3_DUMMY_CYCLE;

  EXAMPLE_TRACE_STR("  Send Read SR3 command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => SR3 Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_SR3, aRegisterBuffer[0]);

  return status;
}
#endif /* M2_W25Q16JV */

#if defined(M2_IS25LP032D)
/**
  * @brief  Writes the Read register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteReadRegister(XSPI_HandleTypeDef *hxspi,
                                                         uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Read Register", MEMORY_WRITE_RR_CMD,
                        (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => RR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write READ register */
  sCommand.Instruction         = MEMORY_WRITE_RR_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_RR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;

  EXAMPLE_TRACE_STR("  Send Write RR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_RR, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the Read register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadReadRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                        uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Read Register", MEMORY_READ_RR_CMD,
                        (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read READ register */
  sCommand.Instruction         = MEMORY_READ_RR_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_RR_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = MEMORY_READ_RR_DUMMY_CYCLE;

  EXAMPLE_TRACE_STR("  Send Read RR command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => RR Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_RR, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Writes the Extended register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer containing data to write
  * @param  RegisterBufferSize: Size of data to write
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @retval MEMORY_TRANSMIT_KO  Error detected during write operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_WriteExtendedRegister(XSPI_HandleTypeDef *hxspi,
                                                             uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Write Extended Register", MEMORY_WRITE_ER_CMD,
                        (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));
  EXAMPLE_TRACE_BUFFER("  => ER Value :", aRegisterBuffer, RegisterBufferSize);

  /* Enable write operations -------------------------------------------- */
  Memory_Toolbox_WriteEnable(hxspi, MEMORY_STARTUP_MODE);

  /* Write Extended register: */
  sCommand.Instruction         = MEMORY_WRITE_ER_CMD;
  sCommand.InstructionMode     = (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_WRITE_ER_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;

  EXAMPLE_TRACE_STR("  Send Write ER command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Transmit(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_TRANSMIT_KO;
  }

  /* Ensure Write operation is completed */
  Memory_Toolbox_AutoPollingMemReady(hxspi, MEMORY_STARTUP_MODE, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_ER, aRegisterBuffer[0]);

  return status;
}

/**
  * @brief  Reads the Extended register inside the memory.
  * @param  hxspi: XSPI handle
  * @param  aRegisterBuffer: Buffer to store read data
  * @param  RegisterBufferSize: Size of data to read
  * @retval MEMORY_OK           Read operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during read operation configuration
  * @retval MEMORY_RECEIVE_KO   Error detected during read operation execution
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_ReadExtendedRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                            uint8_t RegisterBufferSize)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_CMD("[MEM Toolbox] Read Extended Register", MEMORY_READ_ER_CMD,
                        (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_ISIZE_Msk));

  /* Read Extended register */
  sCommand.Instruction         = MEMORY_READ_ER_CMD;
  sCommand.InstructionMode     = (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode            = (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_READ_ER_ACCESS_MODE & XSPI_CCR_DDTR_Msk);
  sCommand.DataLength          = RegisterBufferSize;
  sCommand.DummyCycles         = MEMORY_READ_ER_DUMMY_CYCLE;

  EXAMPLE_TRACE_STR("  Send Read ER command");
  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  if (HAL_XSPI_Receive(hxspi, aRegisterBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_RECEIVE_KO;
  }

  EXAMPLE_TRACE_BUFFER("  => ER Value :", aRegisterBuffer, RegisterBufferSize);

  /* Update register value in Ram variable (1st byte) */
  Memory_Toolbox_Update_Register_Value(MEMORY_REG_ID_ER, aRegisterBuffer[0]);

  return status;
}
#endif /* M2_IS25LP032D */

#if defined(M2_IS25LP032D)
/**
  * @brief  Send Enter Quad command to the memory.
  * @param  hxspi: XSPI handle
  * @retval MEMORY_OK           Write operation performed successfully
  * @retval MEMORY_CMD_KO       Error detected during write operation configuration
  * @note This command is always executed in 1 line mode.
  */
Memory_Toolbox_Status_t Memory_Toolbox_EnterQuad(XSPI_HandleTypeDef *hxspi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  Memory_Toolbox_Status_t status   = MEMORY_OK;

  EXAMPLE_TRACE_STR_HEX("[MEM Toolbox] Enter quad operation", MEMORY_ENTER_QUAD_CMD);

  /* Enter quad command */
  sCommand.Instruction         = MEMORY_ENTER_QUAD_CMD;
  sCommand.InstructionMode     = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_IMODE_Msk);
  sCommand.InstructionDTRMode  = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_IDTR_Msk);
  sCommand.InstructionWidth    = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_ISIZE_Msk);
  sCommand.AddressMode         = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_ADMODE);
  sCommand.DataMode            = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_DMODE_Msk);
  sCommand.DataDTRMode         = (MEMORY_ENTER_QUAD_ACCESS_MODE & XSPI_CCR_DDTR_Msk);

  if (HAL_XSPI_Command(hxspi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    status = MEMORY_CMD_KO;
  }

  return status;
}

#endif /* M2_IS25LP032D */

/**
  * @brief  Compares data at the destination address with source data.
  * @param  Destination_Address: Destination address
  * @param  ptrData: Pointer to source data
  * @param  DataSize: Size of data to compare (in bytes)
  * @retval MEMORY_OK           Data matches
  * @retval MEMORY_COMPARE_KO   Data does not match
  */
Memory_Toolbox_Status_t Memory_Toolbox_MemCmp(uint8_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize)
{
  Memory_Toolbox_Status_t status = MEMORY_OK;
  uint8_t *buffer1 = Destination_Address;
  const uint8_t *buffer2 = ptrData;

  for (uint32_t i = 0U; i < DataSize; i++)
  {
    if (*buffer1 != *buffer2)
    {
      status = MEMORY_COMPARE_KO;
    }
    buffer1++;
    buffer2++;
  }

  return status;
}

/**
  * @brief  Update register value in register dump array.
  * @param  RegId: Register Identifier
  * @param  Value: Register value
  * @retval None
  */
void Memory_Toolbox_Update_Register_Value(Memory_Toolbox_RegisterId_t RegId, uint8_t Value)
{
  if (RegId < MEMORY_REG_ID_COUNT)
  {
    memory_toolbox_register_dump[RegId] = Value;
  }
}

#if defined(EXAMPLE_TRACE_ENABLE)
/**
  * @brief This function allows main program to register the function used for trace output
  * @param TraceCallback: Callback to be executed for trace output
  * @return None
  **/
void Memory_Toolbox_RegisterTraceCallback(Memory_Toolbox_TraceCallback_t TraceCallback)
{
  memory_toolbox_trace_cb = TraceCallback;
}

/**
  * @brief This function allows main program to select appropriate trace level
  * @param TraceLevel Trace level
  * @retval None
  **/
void Memory_Toolbox_SetTraceLevel(Memory_Toolbox_TraceLevel_t TraceLevel)
{
  memory_toolbox_current_trace_level = TraceLevel;
}

/**
  * @brief This function prints message associated to a given status into debug log
  * @param Status Program status to be printed in log
  * @retval None
  **/
void Memory_Toolbox_LogStatus(Memory_Toolbox_Status_t Status)
{
  if (Status < MEMORY_MAX_STATUS_NB)
  {
    EXAMPLE_TRACE_STR(memory_toolbox_status_msgs[Status]);
  }
}

/**
  * @brief This function manages the call of registered trace callback to evacuate trace data
  *        if trace level is within user selection
  * @param TraceLevel Trace level
  * @param Msg Trace data to be evacuated
  * @retval None
  **/
void Memory_Toolbox_Trace(Memory_Toolbox_TraceLevel_t TraceLevel, const uint8_t *Msg)
{
  if (TraceLevel <= memory_toolbox_current_trace_level)
  {
    if (memory_toolbox_trace_cb)
    {
      memory_toolbox_trace_cb((uint8_t *) Msg);
    }
  }
}

/**
  * @brief This function manages the call of registered trace callback to evacuate buffer data in hex format
  *        if trace level is within user selection
  * @param TraceLevel Trace level
  * @param Buffer Ptr on buffer data to be evacuated
  * @param Size Number of buffer data bytes to be evacuated
  * @retval None
  **/
void Memory_Toolbox_TraceBuffer(Memory_Toolbox_TraceLevel_t TraceLevel, const uint8_t *Buffer, uint16_t Size)
{
  char line[EXAMPLE_TRACE_DATA_LINE_STRING_SIZE];
  uint16_t i;
  uint16_t j;
  uint16_t pos;

  if (TraceLevel <= memory_toolbox_current_trace_level)
  {
    /* Buffer data should be output in hexadecimal format, with 16 bytes per line */
    line[0] = ' ';
    line[1] = ' ';
    for (i = 0; i < Size; i += EXAMPLE_TRACE_NB_DATA_PER_LINE)
    {
      pos = 2U;
      for (j = 0; j < EXAMPLE_TRACE_NB_DATA_PER_LINE && (i + j) < Size; ++j)
      {
        pos += sprintf(&line[pos], "0x%02X", Buffer[i + j]);
        if (j < (EXAMPLE_TRACE_NB_DATA_PER_LINE - 1U) && (i + j + 1) < Size)
        {
          pos += sprintf(&line[pos], ",");
        }
      }
      line[pos] = '\0';
      Memory_Toolbox_Trace(TraceLevel, (uint8_t *) "  ");
      Memory_Toolbox_Trace(TraceLevel, (uint8_t *) line);
      Memory_Toolbox_Trace(TraceLevel, (uint8_t *) "\n\r");
    }
  }
}

/**
  * @brief This function dumps content of registers as known in Ram array
  * @retval None
  **/
void Memory_Toolbox_Dump_Registers(void)
{
  EXAMPLE_TRACE_STR("[MEM Toolbox] Register values:");
  for (uint8_t i = 0; i < MEMORY_REG_ID_COUNT; ++i)
  {
    EXAMPLE_TRACE_STR_HEX(memory_toolbox_register_names[i], memory_toolbox_register_dump[i]);
  }
}

/**
  * @brief This function dumps content of registers as known in Ram array
  * @param  hxspi: XSPI handle
  * @retval None
  */
void Memory_Toolbox_LogFrequency(XSPI_HandleTypeDef *hxspi)
{
  uint32_t clockprescaler = hxspi->Init.ClockPrescaler;
  uint32_t frequency      = MEMORY_PERIPH_CLOCK_VALUE;

  EXAMPLE_TRACE_STR_INT("[MEM Toolbox] XSPI Frequency value (Mhz): ", (frequency / (1000000U * (clockprescaler + 1U))));
}
#endif /* EXAMPLE_TRACE_ENABLE */
