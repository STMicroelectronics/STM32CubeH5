/**
  ******************************************************************************
  * @file           : m2_mx25lm51245g.h
  * @brief          : Definitions for Macronix MX25LM51245G NOR FLash memory.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __M2_MX25LM51245G_H
#define __M2_MX25LM51245G_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* M2 board LDO Setup time (ms) */
#define M2_MX25LM51245G_LDO_SETUP_TIME                     200U

/* Memory NOR Flash commands */

/* Write Enable command */
#define M2_MX25LM51245G_WRITE_ENABLE_CMD                   0x06U
#define M2_MX25LM51245G_WRITE_DISABLE_CMD                  0x04U
#define M2_MX25LM51245G_WRITE_ENABLE_DISABLE_ACCESS_MODE   (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS)
#define M2_MX25LM51245G_HIGH_PERF_WRITE_ENABLE_CMD         0x06F9U
#define M2_MX25LM51245G_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_16_BITS | HAL_XSPI_INSTRUCTION_DTR_ENABLE)
/* Autopolling for WEL and WIP bits in 1-line mode */
#define M2_MX25LM51245G_WRITE_ENABLE_MATCH_VALUE           0x02U
#define M2_MX25LM51245G_WRITE_ENABLE_MASK_VALUE            0x02U
#define M2_MX25LM51245G_WIP_STATUS_MATCH_VALUE             0x00U
#define M2_MX25LM51245G_WIP_STATUS_MASK_VALUE              0x01U
/* Status Register access commands */
#define M2_MX25LM51245G_REG_SR_READ_CMD                    0x05U
#define M2_MX25LM51245G_REG_SR_READ_DUMMY_CYCLE            0U
#define M2_MX25LM51245G_REG_SR_READ_ACCESS_MODE            (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_DATA_1_LINE)
#define M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_CMD          0x05FAU
#define M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_DUMMY_CYCLE  20U
#define M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_ACCESS_MODE  (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_16_BITS | HAL_XSPI_INSTRUCTION_DTR_ENABLE | \
                                                            HAL_XSPI_ADDRESS_8_LINES     | HAL_XSPI_ADDRESS_32_BITS     | HAL_XSPI_ADDRESS_DTR_ENABLE     | \
                                                            HAL_XSPI_DATA_8_LINES                                       | HAL_XSPI_DATA_DTR_ENABLE        | \
                                                            HAL_XSPI_DQS_ENABLE)
#define M2_MX25LM51245G_REG_SR_WRITE_CMD                   0x01U
#define M2_MX25LM51245G_REG_SR_WRITE_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_DATA_1_LINE)
/* Configuration Register access commands */
#define M2_MX25LM51245G_REG_CR_READ_CMD                    0x15U
#define M2_MX25LM51245G_REG_CR_READ_ACCESS_MODE            (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_DATA_1_LINE)
#define M2_MX25LM51245G_REG_CR_READ_DUMMY_CYCLE            0U
#define M2_MX25LM51245G_REG_CR_WRITE_CMD                   0x01U
#define M2_MX25LM51245G_REG_CR_WRITE_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_DATA_1_LINE)
/* Configuration Register 2 access commands */
#define M2_MX25LM51245G_REG_CR2_READ_CMD                   0x71U
#define M2_MX25LM51245G_REG_CR2_READ_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_ADDRESS_1_LINE | HAL_XSPI_ADDRESS_32_BITS | HAL_XSPI_DATA_1_LINE)
#define M2_MX25LM51245G_REG_CR2_READ_DUMMY_CYCLE           0U
#define M2_MX25LM51245G_REG_CR2_WRITE_CMD                  0x72U
#define M2_MX25LM51245G_REG_CR2_WRITE_ACCESS_MODE          (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_ADDRESS_1_LINE | HAL_XSPI_ADDRESS_32_BITS | HAL_XSPI_DATA_1_LINE)
/* JEDEC ID and SFDP content reading commands */
#define M2_MX25LM51245G_JEDEC_READ_CMD                     0x9FU
#define M2_MX25LM51245G_READ_JEDEC_ID_ACCESS_MODE          (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_DATA_1_LINE)
#define M2_MX25LM51245G_JEDEC_ID_VALUE                     {0xC2U,0x85U,0x3AU}
#define M2_MX25LM51245G_READ_SFDP_CMD                      0x5AU
#define M2_MX25LM51245G_READ_SFDP_ACCESS_MODE              (HAL_XSPI_INSTRUCTION_1_LINE | HAL_XSPI_INSTRUCTION_8_BITS | HAL_XSPI_ADDRESS_1_LINE | HAL_XSPI_ADDRESS_24_BITS | HAL_XSPI_DATA_1_LINE)
#define M2_MX25LM51245G_SFDP_DUMMY_CYCLES                  8U
/* Read and Page Program commands (always executed when in OPI-DTR mode) */
#define M2_MX25LM51245G_OCTAL_DTR_READ_CMD                 0xEE11U
#define M2_MX25LM51245G_OCTAL_DTR_READ_ACCESS_MODE         (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_16_BITS | HAL_XSPI_INSTRUCTION_DTR_ENABLE | \
                                                            HAL_XSPI_ADDRESS_8_LINES     | HAL_XSPI_ADDRESS_32_BITS     | HAL_XSPI_ADDRESS_DTR_ENABLE     | \
                                                            HAL_XSPI_DATA_8_LINES                                       | HAL_XSPI_DATA_DTR_ENABLE        | \
                                                            HAL_XSPI_DQS_ENABLE)
#define M2_MX25LM51245G_OCTAL_DTR_READ_DUMMY_CYCLE         20U
#define M2_MX25LM51245G_OCTAL_DTR_PAGE_PROG_CMD            0x12EDU
#define M2_MX25LM51245G_OCTAL_DTR_PAGE_PROG_ACCESS_MODE    (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_16_BITS | HAL_XSPI_INSTRUCTION_DTR_ENABLE | \
                                                            HAL_XSPI_ADDRESS_8_LINES     | HAL_XSPI_ADDRESS_32_BITS     | HAL_XSPI_ADDRESS_DTR_ENABLE     | \
                                                            HAL_XSPI_DATA_8_LINES                                       | HAL_XSPI_DATA_DTR_ENABLE)
/* Sector Erase command (always executed when in OPI-DTR mode) */
#define M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_CMD         0x21DEU
#define M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_ACCESS_MODE (HAL_XSPI_INSTRUCTION_8_LINES | HAL_XSPI_INSTRUCTION_16_BITS | HAL_XSPI_INSTRUCTION_DTR_ENABLE | \
                                                            HAL_XSPI_ADDRESS_8_LINES     | HAL_XSPI_ADDRESS_32_BITS     | HAL_XSPI_ADDRESS_DTR_ENABLE)
#define M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_TIMEOUT     500U    /* Max 400ms in spec */
/* Page Size */
#define M2_MX25LM51245G_PAGE_SIZE                          256U
/* Macro for retrieving xSPI peripheral frequency */
#define M2_MX25LM51245G_PERIPH_CLOCK_VALUE                 HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_OSPI);


/* Define used to configure memory registers (operations performed in 1-Line, before switching to Octal mode) */
#define M2_MX25LM51245G_REG_CR_DRIVE_STRENGH_VALUE         0x07U  /* value 24 ohms */
#define M2_MX25LM51245G_REG_CR_DRIVE_STRENGH_MASK          0x07U
/* CR2 register commands */
#define M2_MX25LM51245G_REG_CR2_HIGH_PERF_ADDRESS          0U
#define M2_MX25LM51245G_REG_CR2_HIGH_PERF_VALUE            0x02U
#define M2_MX25LM51245G_REG_CR2_HIGH_PERF_MASK             0x02U
#define M2_MX25LM51245G_REG_CR2_DUMMY_CYCLE_ADDRESS        0x300U
#define M2_MX25LM51245G_REG_CR2_DUMMY_CYCLE_VALUE          0U
#define M2_MX25LM51245G_REG_CR2_DUMMY_CYCLE_MASK           0x07U


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __M2_MX25LM51245G_H */
