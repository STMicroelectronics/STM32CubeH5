/**
  ******************************************************************************
  * @file           : m2_is25lp032d.h
  * @brief          : Definitions for ISSI IS25LP032D NOR FLash memory.
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
#ifndef __M2_IS25LP032D_H
#define __M2_IS25LP032D_H

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
#define M2_IS25LP032D_LDO_SETUP_TIME                     200U


/* Memory NOR Flash commands */
/* Write Enable command */
#define M2_IS25LP032D_WRITE_ENABLE_CMD                   0x06U
#define M2_IS25LP032D_WRITE_DISABLE_CMD                  0x04U
#define M2_IS25LP032D_WRITE_ENABLE_DISABLE_ACCESS_MODE   (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS)
#define M2_IS25LP032D_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_8_BITS)
/* JEDEC ID and SFDP content reading commands */
#define M2_IS25LP032D_JEDEC_READ_CMD                     0x9FU
#define M2_IS25LP032D_READ_JEDEC_ID_ACCESS_MODE          (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_JEDEC_ID_VALUE                     {0x9DU,0x60U,0x16U}
#define M2_IS25LP032D_READ_SFDP_CMD                      0x5AU
#define M2_IS25LP032D_READ_SFDP_ACCESS_MODE              (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_ADDRESS_1_LINE      | HAL_XSPI_ADDRESS_24_BITS    | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_SFDP_DUMMY_CYCLES                  8U
/* Autopolling commands: WEL, WIP */
#define M2_IS25LP032D_WRITE_ENABLE_MATCH_VALUE           0x02U
#define M2_IS25LP032D_WRITE_ENABLE_MASK_VALUE            0x02U
#define M2_IS25LP032D_WIP_STATUS_MATCH_VALUE             0x00U
#define M2_IS25LP032D_WIP_STATUS_MASK_VALUE              0x01U
/* Configuration Register access commands */
/* Configuration Register 2 access commands */
/* Read and Page Program commands */
#define M2_IS25LP032D_QUAD_READ_CMD                      0xEBU
#define M2_IS25LP032D_QUAD_READ_ACCESS_MODE              (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_ADDRESS_4_LINES     | HAL_XSPI_ADDRESS_24_BITS    | \
                                                          HAL_XSPI_DATA_4_LINES)
#define M2_IS25LP032D_QUAD_READ_DUMMY_CYCLE              10U
#define M2_IS25LP032D_QUAD_PAGE_PROG_CMD                 0x02U
#define M2_IS25LP032D_QUAD_PAGE_PROG_ACCESS_MODE         (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_ADDRESS_4_LINES     | HAL_XSPI_ADDRESS_24_BITS    | \
                                                          HAL_XSPI_DATA_4_LINES)
/* Sector Erase command (always executed when in QSPI mode) */
#define M2_IS25LP032D_QUAD_SECTOR_ERASE_CMD              0x20U
#define M2_IS25LP032D_QUAD_SECTOR_ERASE_ACCESS_MODE      (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_ADDRESS_4_LINES     | HAL_XSPI_ADDRESS_24_BITS)
#define M2_IS25LP032D_QUAD_SECTOR_ERASE_TIMEOUT          500U    /* Max 300ms in spec */
/* Enter quad operation command */
#define M2_IS25LP032D_ENTER_QUAD_CMD                     0x35U
#define M2_IS25LP032D_ENTER_QUAD_ACCESS_MODE             (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS)
/* Page Size */
#define M2_IS25LP032D_PAGE_SIZE                          256U
/* Macro for retrieving xSPI peripheral frequency */
#define M2_IS25LP032D_PERIPH_CLOCK_VALUE                 HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_OSPI);


/* Define use for to configure memory registers (operations performed in 1-Line, before switching to Octal mode) */
/* Status Register register commands */
#define M2_IS25LP032D_REG_SR_READ_CMD                    0x05U
#define M2_IS25LP032D_REG_SR_READ_DUMMY_CYCLE            0U
#define M2_IS25LP032D_REG_SR_READ_ACCESS_MODE            (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_HIGH_PERF_REG_SR_READ_ACCESS_MODE  (HAL_XSPI_INSTRUCTION_4_LINES | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_4_LINES)
#define M2_IS25LP032D_REG_SR_WRITE_CMD                   0x01U
#define M2_IS25LP032D_REG_SR_WRITE_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_REG_SR_QUAD_ENABLE_VALUE           0x40U /* (QE bit enabling) */
#define M2_IS25LP032D_REG_SR_QUAD_ENABLE_MASK            0x40U
/* Read Register commands */
#define M2_IS25LP032D_REG_RR_READ_CMD                    0x61U
#define M2_IS25LP032D_REG_RR_READ_ACCESS_MODE            (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_REG_RR_READ_DUMMY_CYCLE            0U
#define M2_IS25LP032D_REG_RR_WRITE_CMD                   0xC0U
#define M2_IS25LP032D_REG_RR_WRITE_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_REG_RR_DUMMY_CYCLE_VALUE           0x50U  /* 10 Dummy cycles */
#define M2_IS25LP032D_REG_RR_DUMMY_CYCLE_MASK            0x78U
/* Extended Register commands */
#define M2_IS25LP032D_REG_ER_READ_CMD                    0x81U
#define M2_IS25LP032D_REG_ER_READ_ACCESS_MODE            (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_REG_ER_READ_DUMMY_CYCLE            0U
#define M2_IS25LP032D_REG_ER_WRITE_CMD                   0x83U
#define M2_IS25LP032D_REG_ER_WRITE_ACCESS_MODE           (HAL_XSPI_INSTRUCTION_1_LINE  | HAL_XSPI_INSTRUCTION_8_BITS | \
                                                          HAL_XSPI_DATA_1_LINE)
#define M2_IS25LP032D_REG_ER_DRIVE_STRENGTH_VALUE        0xE0U /* Drive Strength 50% (Default value) */
#define M2_IS25LP032D_REG_ER_DRIVE_STRENGTH_MASK         0xE0U

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

#endif /* __M2_IS25LP032D_H */
