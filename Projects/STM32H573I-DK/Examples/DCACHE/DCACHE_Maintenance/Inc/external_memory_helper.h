/**
  ******************************************************************************
* @file    DCACHE/DCACHE_Maintenance/Inc/hal_external_memory_helper.h
* @author  MCD Application Team
* @brief   This file contains the headers of the external memory helper.
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
#ifndef HAL_EXTERNAL_MEM_HELPER
#define HAL_EXTERNAL_MEM_HELPER

#ifdef __cplusplus
extern "C" {
#endif

/* ******************** FMC SRAM **********************************************/
/* Exported constants --------------------------------------------------------*/
  /* USER CODE BEGIN EC */

/* MX25LM512ABA1G12 Macronix memory */
/* Flash commands */
#define OCTAL_IO_READ_CMD           0xEC13
#define OCTAL_PAGE_PROG_CMD         0x12ED
#define OCTAL_READ_STATUS_REG_CMD   0x05FA
#define OCTAL_SECTOR_ERASE_CMD      0xDC23
#define OCTAL_WRITE_ENABLE_CMD      0x06F9
#define READ_STATUS_REG_CMD         0x05
#define WRITE_CFG_REG_2_CMD         0x72
#define WRITE_ENABLE_CMD            0x06

/* Dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ     6
#define DUMMY_CLOCK_CYCLES_READ_REG 4

/* Auto-polling values */
#define WRITE_ENABLE_MATCH_VALUE    0x02
#define WRITE_ENABLE_MASK_VALUE     0x02

#define MEMORY_READY_MATCH_VALUE    0x00
#define MEMORY_READY_MASK_VALUE     0x01

#define AUTO_POLLING_INTERVAL       0x10

/* Memory registers address */
#define CONFIG_REG2_ADDR1           0x0000000
#define CR2_STR_OPI_ENABLE          0x01

#define CONFIG_REG2_ADDR3           0x00000300
#define CR2_DUMMY_CYCLES_66MHZ      0x07

/* Memory delay */
#define MEMORY_REG_WRITE_DELAY      40
#define MEMORY_PAGE_PROG_DELAY      2
#define EXT_MEM_TIMEOUT             0xFFFFU

/* Memory offsets  */
#define MEMORY_SECTOR_1_OFFSET          0
#define MEMORY_SECTOR_2_OFFSET          0x10000U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint32_t extMemory_Config(void);
uint32_t MemoryErase(uint32_t memoryOffset);

#ifdef __cplusplus
}
#endif

#endif /* HAL_EXTERNAL_MEM_HELPER */
