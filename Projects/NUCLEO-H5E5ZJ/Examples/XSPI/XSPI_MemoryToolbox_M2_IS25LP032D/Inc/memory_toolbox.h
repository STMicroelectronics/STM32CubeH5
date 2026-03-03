/**
  ******************************************************************************
  * @file  : memory_toolbox.h
  * @brief : Header for memory_toolbox.c file.
  *          This file contains the defines related to memory characteristics
  *          and the headers of the Memory Toolbox service functions.
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
#ifndef __MEMORY_TOOLBOX_H
#define __MEMORY_TOOLBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* File containing memory specific defines is included according to project compilation option. */
#if defined(M2_M95P32)
/* Include file  : "m2_m95p32.h"
   Related board : MB1928-18LA from B-M2MEM-PACK1
   Memory Device Manufacturer : STMicrolectronics
   Memory Device Reference : M95P32-IXMNT/E */
#include "m2_m95p32.h"
#elif defined(M2_IS25LP032D)
/* Include file  : "m2_is25lp032d.h"
   Related board : MB1928-33LA from B-M2MEM-PACK1
   Memory Device Manufacturer : ISSI
   Memory Device Reference : IS25LP032DJNLE-TR */
#include "m2_is25lp032d.h"
#elif defined(M2_W25Q16JV)
/* Include file  : "m2_w25q16jv.h"
   Related board : MB1928-33LB from B-M2MEM-PACK1
   Memory Device Manufacturer : Winbond
   Memory Device Reference : W25Q16JVSNIQ */
#include "m2_w25q16jv.h"
#elif defined(M2_MX25UW25645G)
/* Include file  : "m2_mx25uw25645g.h"
   Related board : MB1927-18BA from B-M2MEM-PACK1
   Memory Device Manufacturer : Macronix
   Memory Device Reference : MX25UW25645GXDI00 */
#include "m2_mx25uw25645g.h"
#elif defined(M2_MX25LM51245G)
/* Include file  : "m2_mx25lm51245g.h"
   Related board : MB1927-33BA from B-M2MEM-PACK1
   Memory Device Manufacturer : Macronix
   Memory Device Reference : MX25LM51245GXDI00 */
#include "m2_mx25lm51245g.h"
#else
#warning "Please include file containing your memory definitions"
#endif /* M2_M95P32 */

/* Exported typedefs ---------------------------------------------------------*/
#if defined(EXAMPLE_TRACE_ENABLE)
/* Trace callback type definition */
typedef void (*Memory_Toolbox_TraceCallback_t)(const uint8_t *msg);

/* Trace level enum definition */
typedef enum
{
  EXAMPLE_TRACE_LEVEL_NONE = 0,    /* No information provided in logs */
  EXAMPLE_TRACE_LEVEL_CMD_INFO,    /* Step descriptions and commands are provided in logs */
  EXAMPLE_TRACE_LEVEL_ALL          /* Step descriptions, commands and buffer contents are provided in logs */
} Memory_Toolbox_TraceLevel_t;
#endif /* EXAMPLE_TRACE_ENABLE */

/* Status values returned by Memory Toolbox functions.
   Indicates success (MEMORY_OK) or failure of an operation. */
typedef enum
{
  MEMORY_OK                     = 0U,   /*!< Operation successful                                            */
  MEMORY_CMD_KO                 = 1U,   /*!< Command Operation failed                                        */
  MEMORY_TRANSMIT_KO            = 2U,   /*!< Transmit Operation failed                                       */
  MEMORY_RECEIVE_KO             = 3U,   /*!< Receive Operation failed                                        */
  MEMORY_MMP_MODE_KO            = 4U,   /*!< MemoryMap Operation failed                                      */
  MEMORY_AUTOPOLL_KO            = 5U,   /*!< AutoPolling Operation failed                                    */
  MEMORY_ABORT_KO               = 6U,   /*!< Abort Operation failed                                          */
  MEMORY_TIMEOUT                = 7U,   /*!< Timeout Operation                                               */
  MEMORY_COMPARE_KO             = 8U,   /*!< Buffer comparison failed                                        */
  MEMORY_WRONG_JEDEC_ID         = 9U,   /*!< Unexpected JEDEC ID value                                       */
  MEMORY_WRONG_SFDP_HEADER      = 10U,  /*!< Unexpected SFDP Header                                          */
  MEMORY_READ_SR_KO             = 11U,  /*!< Error while reading SR                                          */
  MEMORY_WRITE_ENABLE_CHECK_KO  = 12U,  /*!< Error while checking WEL bit in SR after Write Enable command   */
  MEMORY_WRITE_DISABLE_CHECK_KO = 13U,  /*!< Error while checking WEL bit in SR after Write Disable command  */
  MEMORY_RCC_CONFIG_KO          = 14U,  /*!< Error while updating RCC configuration for reaching max perf    */
  MEMORY_DLYB_CONFIG_KO         = 15U,  /*!< Error while updating DLYB configuration                         */
  MEMORY_MAX_STATUS_NB
} Memory_Toolbox_Status_t;

/* Definition of type of memory access */
typedef enum
{
  MEMORY_STARTUP_MODE           = 0U, /*!< Used when memory is in reset state */
  MEMORY_HIGH_PERFORMANCE       = 1U, /*!< Used when memory has been configured to its maximum performance capability */
} Memory_Toolbox_PerfMode_t;

/* Register type enum definition.
   (depends on selected memory according to project compilation option.) */
typedef enum
{
#if defined(M2_M95P32)
  /* Memory Device STMicrolectronics M95P32-IXMNT/E :
      - Status Register,
      - Configuration Register */
  MEMORY_REG_ID_SR = 0,                  /*!< Status Register          */
  MEMORY_REG_ID_CR,                      /*!< Configuration Register   */
#elif defined(M2_IS25LP032D)
  /* Memory Device ISSI IS25LP032DJNLE-TR  :
      - Status Register,
      - Read Parameters Register,
      - Extended Register */
  MEMORY_REG_ID_SR = 0,                  /*!< Status Register                                                */
  MEMORY_REG_ID_RR,                      /*!< Read Parameters Register : used for dummy cycles configuration */
  MEMORY_REG_ID_ER,                      /*!< Extended Register : used for Drive Strength configuration      */
#elif defined(M2_W25Q16JV)
  /* Memory Device Winbond W25Q16JVSNIQ  :
      - Status Register,
      - Status Register 2,
      - Status Register 3  */
  MEMORY_REG_ID_SR = 0,                  /*!< Status Register          */
  MEMORY_REG_ID_SR2,                     /*!< Status Register 2        */
  MEMORY_REG_ID_SR3,                     /*!< Status Register 3        */
#elif defined(M2_MX25UW25645G)
  /* Memory Device Macronix MX25UW25645GXDI00  :
      - Status Register,
      - Configuration Register,
      - Configuration Register 2 */
  MEMORY_REG_ID_SR = 0,                  /*!< Status Register          */
  MEMORY_REG_ID_CR,                      /*!< Configuration Register   */
  MEMORY_REG_ID_CR2,                     /*!< Configuration Register 2 */
#elif defined(M2_MX25LM51245G)
  /* Memory Device Macronix MX25LM51245GXDI00  :
      - Status Register,
      - Configuration Register,
      - Configuration Register 2 */
  MEMORY_REG_ID_SR = 0,                  /*!< Status Register          */
  MEMORY_REG_ID_CR,                      /*!< Configuration Register   */
  MEMORY_REG_ID_CR2,                     /*!< Configuration Register 2 */
#else
#warning "Please define enums corresponding to registers managed in your memory"
#endif /* M2_M95P32 */
  MEMORY_REG_ID_COUNT
} Memory_Toolbox_RegisterId_t;

/* Exported constants --------------------------------------------------------*/

/* Below definitions map generic defines ("MEMORY_xxx") to memory specific ones
   according to project compilation option */

#if defined(M2_M95P32)
/* ###########################################################################
   Include file : "m2_m95p32.h"
   Related board : MB1928-18LA from B-M2MEM-PACK1
   Memory Device Manufacturer : STMicrolectronics
   Memory Device Reference : M95P32-IXMNT/E
   Density : 32 Mbits
   Memory configuration steps : Drive Strength update in Configuration Register
   ###########################################################################*/

/* LDO Setup time (ms) */
#define MEMORY_M2_BOARD_LDO_SETUP_TIME             M2_M95P32_LDO_SETUP_TIME

/* Write Enable command */
#define MEMORY_WRITE_ENABLE_CMD                    M2_M95P32_WRITE_ENABLE_CMD
#define MEMORY_WRITE_DISABLE_CMD                   M2_M95P32_WRITE_DISABLE_CMD
#define MEMORY_WRITE_ENABLE_ACCESS_MODE            M2_M95P32_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_WRITE_DISABLE_ACCESS_MODE           M2_M95P32_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_HIGH_PERF_WRITE_ENABLE_CMD          MEMORY_WRITE_ENABLE_CMD
#define MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE  MEMORY_WRITE_ENABLE_ACCESS_MODE
/* Autopolling for WEL and WIP bits */
#define MEMORY_WRITE_ENABLE_MATCH_VALUE            M2_M95P32_WRITE_ENABLE_MATCH_VALUE
#define MEMORY_WRITE_ENABLE_MASK_VALUE             M2_M95P32_WRITE_ENABLE_MASK_VALUE
#define MEMORY_WIP_STATUS_MATCH_VALUE              M2_M95P32_WIP_STATUS_MATCH_VALUE
#define MEMORY_WIP_STATUS_MASK_VALUE               M2_M95P32_WIP_STATUS_MASK_VALUE
/* Status Register access commands */
#define MEMORY_READ_SR_CMD                         M2_M95P32_REG_SR_READ_CMD
#define MEMORY_READ_SR_ADDRESS                     0U /* No address used in Read Status Register command on M95P32 */
#define MEMORY_READ_SR_ACCESS_MODE                 M2_M95P32_REG_SR_READ_ACCESS_MODE
#define MEMORY_READ_SR_DUMMY_CYCLE                 M2_M95P32_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_HIGH_PERF_READ_SR_CMD               MEMORY_READ_SR_CMD
#define MEMORY_HIGH_PERF_READ_SR_ADDRESS           0U /* No address used in Read Status Register command on M95P32 */
#define MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE       MEMORY_READ_SR_ACCESS_MODE
#define MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE       MEMORY_READ_SR_DUMMY_CYCLE
#define MEMORY_WRITE_SR_CMD                        M2_M95P32_REG_SR_WRITE_CMD
#define MEMORY_WRITE_SR_ACCESS_MODE                M2_M95P32_REG_SR_WRITE_ACCESS_MODE
/* Configuration Register access commands */
#define MEMORY_READ_CR_CMD                         M2_M95P32_REG_CR_READ_CMD
#define MEMORY_READ_CR_ACCESS_MODE                 M2_M95P32_REG_CR_READ_ACCESS_MODE
#define MEMORY_READ_CR_DUMMY_CYCLE                 M2_M95P32_REG_CR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_CR_CMD                        M2_M95P32_REG_CR_WRITE_CMD
#define MEMORY_WRITE_CR_ACCESS_MODE                M2_M95P32_REG_CR_WRITE_ACCESS_MODE
/* JEDEC ID and SFDP content reading commands */
#define MEMORY_READ_JEDEC_ID_CMD                   M2_M95P32_JEDEC_READ_CMD
#define MEMORY_READ_JEDEC_ID_ACCESS_MODE           M2_M95P32_READ_JEDEC_ID_ACCESS_MODE
#define MEMORY_READ_JEDEC_ID_VALUE                 M2_M95P32_JEDEC_ID_VALUE
#define MEMORY_READ_SFDP_CMD                       M2_M95P32_READ_SFDP_CMD
#define MEMORY_READ_SFDP_ACCESS_MODE               M2_M95P32_READ_SFDP_ACCESS_MODE
#define MEMORY_SFDP_DUMMY_CYCLES                   M2_M95P32_SFDP_DUMMY_CYCLES
/* Read and Page Program commands */
#define MEMORY_READ_CMD                            M2_M95P32_QUAD_IO_READ_CMD
#define MEMORY_READ_ACCESS_MODE                    M2_M95P32_QUAD_IO_READ_ACCESS_MODE
#define MEMORY_READ_DUMMY_CYCLE                    M2_M95P32_QUAD_IO_READ_DUMMY_CYCLE
#define MEMORY_PAGE_PROG_CMD                       M2_M95P32_QUAD_PAGE_PROG_CMD
#define MEMORY_PAGE_PROG_ACCESS_MODE               M2_M95P32_QUAD_PAGE_PROG_ACCESS_MODE
/* Sector Erase command */
#define MEMORY_SECTOR_ERASE_CMD                    M2_M95P32_SECTOR_ERASE_CMD
#define MEMORY_SECTOR_ERASE_ACCESS_MODE            M2_M95P32_SECTOR_ERASE_ACCESS_MODE
#define MEMORY_SECTOR_ERASE_TIMEOUT                M2_M95P32_SECTOR_ERASE_TIMEOUT
/* Page Size */
#define MEMORY_PAGE_SIZE                           M2_M95P32_PAGE_SIZE
/* Macro for retrieving xSPI peripheral frequency */
#define MEMORY_PERIPH_CLOCK_VALUE                  M2_M95P32_PERIPH_CLOCK_VALUE


#elif defined(M2_IS25LP032D)

/* ###########################################################################
   Include file  : "m2_is25lp032d.h"
   Related board : MB1928-33LA from B-M2MEM-PACK1
   Memory Device Manufacturer : ISSI
   Memory Device Reference : IS25LP032DJNLE-TR
   Density : 32 Mbits
   Memory configuration steps : QE bit enabling in Status Register
                                Dummy Cycles update in Read Parameters Register
                                Drive Strength update in Extended Register
                                Quad mode enabling with specific instruction
   ###########################################################################*/

/* LDO Setup time (ms) */
#define MEMORY_M2_BOARD_LDO_SETUP_TIME             M2_IS25LP032D_LDO_SETUP_TIME

/* Write Enable command */
#define MEMORY_WRITE_ENABLE_CMD                    M2_IS25LP032D_WRITE_ENABLE_CMD
#define MEMORY_WRITE_DISABLE_CMD                   M2_IS25LP032D_WRITE_DISABLE_CMD
#define MEMORY_WRITE_ENABLE_ACCESS_MODE            M2_IS25LP032D_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_WRITE_DISABLE_ACCESS_MODE           M2_IS25LP032D_WRITE_ENABLE_DISABLE_ACCESS_MODE
/* In Quad mode, there is no specific command for write enable */
#define MEMORY_HIGH_PERF_WRITE_ENABLE_CMD          MEMORY_WRITE_ENABLE_CMD
#define MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE  M2_IS25LP032D_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE
/* Autopolling for WEL and WIP bits */
#define MEMORY_WRITE_ENABLE_MATCH_VALUE            M2_IS25LP032D_WRITE_ENABLE_MATCH_VALUE
#define MEMORY_WRITE_ENABLE_MASK_VALUE             M2_IS25LP032D_WRITE_ENABLE_MASK_VALUE
#define MEMORY_WIP_STATUS_MATCH_VALUE              M2_IS25LP032D_WIP_STATUS_MATCH_VALUE
#define MEMORY_WIP_STATUS_MASK_VALUE               M2_IS25LP032D_WIP_STATUS_MASK_VALUE
/* Status Register access commands */
#define MEMORY_READ_SR_CMD                         M2_IS25LP032D_REG_SR_READ_CMD
#define MEMORY_READ_SR_ADDRESS                     0U /* No address used in Read Status Register on IS25LP032D */
#define MEMORY_READ_SR_ACCESS_MODE                 M2_IS25LP032D_REG_SR_READ_ACCESS_MODE
#define MEMORY_READ_SR_DUMMY_CYCLE                 M2_IS25LP032D_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_HIGH_PERF_READ_SR_CMD               MEMORY_READ_SR_CMD
#define MEMORY_HIGH_PERF_READ_SR_ADDRESS           0U /* No address used in Read Status Register on IS25LP032D */
#define MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE       M2_IS25LP032D_HIGH_PERF_REG_SR_READ_ACCESS_MODE
#define MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE       MEMORY_READ_SR_DUMMY_CYCLE
#define MEMORY_WRITE_SR_CMD                        M2_IS25LP032D_REG_SR_WRITE_CMD
#define MEMORY_WRITE_SR_ACCESS_MODE                M2_IS25LP032D_REG_SR_WRITE_ACCESS_MODE
/* Read Register access commands */
#define MEMORY_READ_RR_CMD                         M2_IS25LP032D_REG_RR_READ_CMD
#define MEMORY_READ_RR_ACCESS_MODE                 M2_IS25LP032D_REG_RR_READ_ACCESS_MODE
#define MEMORY_READ_RR_DUMMY_CYCLE                 M2_IS25LP032D_REG_RR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_RR_CMD                        M2_IS25LP032D_REG_RR_WRITE_CMD
#define MEMORY_WRITE_RR_ACCESS_MODE                M2_IS25LP032D_REG_RR_WRITE_ACCESS_MODE
/* Extended Register access commands */
#define MEMORY_READ_ER_CMD                         M2_IS25LP032D_REG_ER_READ_CMD
#define MEMORY_READ_ER_ACCESS_MODE                 M2_IS25LP032D_REG_ER_READ_ACCESS_MODE
#define MEMORY_READ_ER_DUMMY_CYCLE                 M2_IS25LP032D_REG_ER_READ_DUMMY_CYCLE
#define MEMORY_WRITE_ER_CMD                        M2_IS25LP032D_REG_ER_WRITE_CMD
#define MEMORY_WRITE_ER_ACCESS_MODE                M2_IS25LP032D_REG_ER_WRITE_ACCESS_MODE
/* JEDEC ID and SFDP content reading commands */
#define MEMORY_READ_JEDEC_ID_CMD                   M2_IS25LP032D_JEDEC_READ_CMD
#define MEMORY_READ_JEDEC_ID_ACCESS_MODE           M2_IS25LP032D_READ_JEDEC_ID_ACCESS_MODE
#define MEMORY_READ_JEDEC_ID_VALUE                 M2_IS25LP032D_JEDEC_ID_VALUE
#define MEMORY_READ_SFDP_CMD                       M2_IS25LP032D_READ_SFDP_CMD
#define MEMORY_READ_SFDP_ACCESS_MODE               M2_IS25LP032D_READ_SFDP_ACCESS_MODE
#define MEMORY_SFDP_DUMMY_CYCLES                   M2_IS25LP032D_SFDP_DUMMY_CYCLES
/* Read and Page Program commands */
#define MEMORY_READ_CMD                            M2_IS25LP032D_QUAD_READ_CMD
#define MEMORY_READ_ACCESS_MODE                    M2_IS25LP032D_QUAD_READ_ACCESS_MODE
#define MEMORY_READ_DUMMY_CYCLE                    M2_IS25LP032D_QUAD_READ_DUMMY_CYCLE
#define MEMORY_PAGE_PROG_CMD                       M2_IS25LP032D_QUAD_PAGE_PROG_CMD
#define MEMORY_PAGE_PROG_ACCESS_MODE               M2_IS25LP032D_QUAD_PAGE_PROG_ACCESS_MODE
/* Sector Erase command */
#define MEMORY_SECTOR_ERASE_CMD                    M2_IS25LP032D_QUAD_SECTOR_ERASE_CMD
#define MEMORY_SECTOR_ERASE_ACCESS_MODE            M2_IS25LP032D_QUAD_SECTOR_ERASE_ACCESS_MODE
#define MEMORY_SECTOR_ERASE_TIMEOUT                M2_IS25LP032D_QUAD_SECTOR_ERASE_TIMEOUT
/* Enter quad operation command */
#define MEMORY_ENTER_QUAD_CMD                      M2_IS25LP032D_ENTER_QUAD_CMD
#define MEMORY_ENTER_QUAD_ACCESS_MODE              M2_IS25LP032D_ENTER_QUAD_ACCESS_MODE
/* Page Size */
#define MEMORY_PAGE_SIZE                           M2_IS25LP032D_PAGE_SIZE
/* Macro for retrieving xSPI peripheral frequency */
#define MEMORY_PERIPH_CLOCK_VALUE                  M2_IS25LP032D_PERIPH_CLOCK_VALUE


#elif defined(M2_W25Q16JV)

/* ###########################################################################
   Include file  : "m2_w25q16jv.h"
   Related board : MB1928-33LB from B-M2MEM-PACK1
   Memory Device Manufacturer : Winbond
   Memory Device Reference : W25Q16JVSNIQ
   Density : 16 Mbits
   Memory configuration steps : Drive Strength update in Status Register 3
                                Quad mode enabling in Status Register 2
   ###########################################################################*/

/* LDO Setup time (ms) */
#define MEMORY_M2_BOARD_LDO_SETUP_TIME             M2_W25Q16JV_LDO_SETUP_TIME

/* Write Enable command */
#define MEMORY_WRITE_ENABLE_CMD                    M2_W25Q16JV_WRITE_ENABLE_CMD
#define MEMORY_WRITE_DISABLE_CMD                   M2_W25Q16JV_WRITE_DISABLE_CMD
#define MEMORY_WRITE_ENABLE_ACCESS_MODE            M2_W25Q16JV_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_WRITE_DISABLE_ACCESS_MODE           M2_W25Q16JV_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_HIGH_PERF_WRITE_ENABLE_CMD          MEMORY_WRITE_ENABLE_CMD
#define MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE  MEMORY_WRITE_ENABLE_ACCESS_MODE
/* Autopolling for WEL and WIP bits */
#define MEMORY_WRITE_ENABLE_MATCH_VALUE            M2_W25Q16JV_WRITE_ENABLE_MATCH_VALUE
#define MEMORY_WRITE_ENABLE_MASK_VALUE             M2_W25Q16JV_WRITE_ENABLE_MASK_VALUE
#define MEMORY_WIP_STATUS_MATCH_VALUE              M2_W25Q16JV_WIP_STATUS_MATCH_VALUE
#define MEMORY_WIP_STATUS_MASK_VALUE               M2_W25Q16JV_WIP_STATUS_MASK_VALUE
/* Status Register access commands */
#define MEMORY_READ_SR_CMD                         M2_W25Q16JV_REG_SR_READ_CMD
#define MEMORY_READ_SR_ADDRESS                     0U /* No address used in Read Status Register on W25Q16JV */
#define MEMORY_READ_SR_ACCESS_MODE                 M2_W25Q16JV_REG_SR_READ_ACCESS_MODE
#define MEMORY_READ_SR_DUMMY_CYCLE                 M2_W25Q16JV_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_HIGH_PERF_READ_SR_CMD               MEMORY_READ_SR_CMD
#define MEMORY_HIGH_PERF_READ_SR_ADDRESS           0U /* No address used in Read Status Register on W25Q16JV */
#define MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE       MEMORY_READ_SR_ACCESS_MODE
#define MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE       MEMORY_READ_SR_DUMMY_CYCLE
#define MEMORY_WRITE_SR_CMD                        M2_W25Q16JV_REG_SR_WRITE_CMD
#define MEMORY_WRITE_SR_ACCESS_MODE                M2_W25Q16JV_REG_SR_WRITE_ACCESS_MODE
#define MEMORY_READ_SR2_CMD                        M2_W25Q16JV_REG_SR2_READ_CMD
#define MEMORY_READ_SR2_ACCESS_MODE                M2_W25Q16JV_REG_SR2_READ_ACCESS_MODE
#define MEMORY_READ_SR2_DUMMY_CYCLE                M2_W25Q16JV_REG_SR2_READ_DUMMY_CYCLE
#define MEMORY_WRITE_SR2_CMD                       M2_W25Q16JV_REG_SR2_WRITE_CMD
#define MEMORY_WRITE_SR2_ACCESS_MODE               M2_W25Q16JV_REG_SR2_WRITE_ACCESS_MODE
#define MEMORY_READ_SR3_CMD                        M2_W25Q16JV_REG_SR3_READ_CMD
#define MEMORY_READ_SR3_ACCESS_MODE                M2_W25Q16JV_REG_SR3_READ_ACCESS_MODE
#define MEMORY_READ_SR3_DUMMY_CYCLE                M2_W25Q16JV_REG_SR3_READ_DUMMY_CYCLE
#define MEMORY_WRITE_SR3_CMD                       M2_W25Q16JV_REG_SR3_WRITE_CMD
#define MEMORY_WRITE_SR3_ACCESS_MODE               M2_W25Q16JV_REG_SR3_WRITE_ACCESS_MODE
/* JEDEC ID and SFDP content reading commands */
#define MEMORY_READ_JEDEC_ID_CMD                   M2_W25Q16JV_JEDEC_READ_CMD
#define MEMORY_READ_JEDEC_ID_ACCESS_MODE           M2_W25Q16JV_READ_JEDEC_ID_ACCESS_MODE
#define MEMORY_READ_JEDEC_ID_VALUE                 M2_W25Q16JV_JEDEC_ID_VALUE
#define MEMORY_READ_SFDP_CMD                       M2_W25Q16JV_READ_SFDP_CMD
#define MEMORY_READ_SFDP_ACCESS_MODE               M2_W25Q16JV_READ_SFDP_ACCESS_MODE
#define MEMORY_SFDP_DUMMY_CYCLES                   M2_W25Q16JV_SFDP_DUMMY_CYCLES
/* Read and Page Program commands */
#define MEMORY_READ_CMD                            M2_W25Q16JV_QUAD_IO_READ_CMD
#define MEMORY_READ_ACCESS_MODE                    M2_W25Q16JV_QUAD_IO_READ_ACCESS_MODE
#define MEMORY_READ_DUMMY_CYCLE                    M2_W25Q16JV_QUAD_IO_READ_DUMMY_CYCLE
#define MEMORY_PAGE_PROG_CMD                       M2_W25Q16JV_QUAD_PAGE_PROG_CMD
#define MEMORY_PAGE_PROG_ACCESS_MODE               M2_W25Q16JV_QUAD_PAGE_PROG_ACCESS_MODE
/* Sector Erase command */
#define MEMORY_SECTOR_ERASE_CMD                    M2_W25Q16JV_SECTOR_ERASE_CMD
#define MEMORY_SECTOR_ERASE_ACCESS_MODE            M2_W25Q16JV_SECTOR_ERASE_ACCESS_MODE
#define MEMORY_SECTOR_ERASE_TIMEOUT                M2_W25Q16JV_SECTOR_ERASE_TIMEOUT
/* Page Size */
#define MEMORY_PAGE_SIZE                           M2_W25Q16JV_PAGE_SIZE
/* Macro for retrieving xSPI peripheral frequency */
#define MEMORY_PERIPH_CLOCK_VALUE                  M2_W25Q16JV_PERIPH_CLOCK_VALUE


#elif defined(M2_MX25UW25645G)

/* ###########################################################################
   Include file  : "m2_mx25uw25645g.h"
   Related board : MB1927-18BA from B-M2MEM-PACK1
   Memory Device Manufacturer : Macronix
   Memory Device Reference : MX25UW25645GXDI00
   Density : 32 Mbits
   Memory configuration steps : Drive Strength update in Configuration Register
                                Dummy Cycles update in Configuration Register 2
                                Switch in Octal mode (set DOPI bit) in Configuration Register 2
   ###########################################################################*/

/* LDO Setup time (ms) */
#define MEMORY_M2_BOARD_LDO_SETUP_TIME             M2_MX25UW25645G_LDO_SETUP_TIME

/* Write Enable command */
#define MEMORY_WRITE_ENABLE_CMD                    M2_MX25UW25645G_WRITE_ENABLE_CMD
#define MEMORY_WRITE_DISABLE_CMD                   M2_MX25UW25645G_WRITE_DISABLE_CMD
#define MEMORY_WRITE_ENABLE_ACCESS_MODE            M2_MX25UW25645G_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_WRITE_DISABLE_ACCESS_MODE           M2_MX25UW25645G_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_HIGH_PERF_WRITE_ENABLE_CMD          M2_MX25UW25645G_HIGH_PERF_WRITE_ENABLE_CMD
#define MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE  M2_MX25UW25645G_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE
/* Autopolling for WEL and WIP bits */
#define MEMORY_WRITE_ENABLE_MATCH_VALUE            M2_MX25UW25645G_WRITE_ENABLE_MATCH_VALUE
#define MEMORY_WRITE_ENABLE_MASK_VALUE             M2_MX25UW25645G_WRITE_ENABLE_MASK_VALUE
#define MEMORY_WIP_STATUS_MATCH_VALUE              M2_MX25UW25645G_WIP_STATUS_MATCH_VALUE
#define MEMORY_WIP_STATUS_MASK_VALUE               M2_MX25UW25645G_WIP_STATUS_MASK_VALUE
/* Status Register access commands */
#define MEMORY_READ_SR_CMD                         M2_MX25UW25645G_REG_SR_READ_CMD
#define MEMORY_READ_SR_ADDRESS                     0U /* No address used in Read Status Register on MX25UW25645G */
#define MEMORY_READ_SR_ACCESS_MODE                 M2_MX25UW25645G_REG_SR_READ_ACCESS_MODE
#define MEMORY_READ_SR_DUMMY_CYCLE                 M2_MX25UW25645G_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_HIGH_PERF_READ_SR_CMD               M2_MX25UW25645G_HIGH_PERF_REG_SR_READ_CMD
#define MEMORY_HIGH_PERF_READ_SR_ADDRESS           0U /* No address used in Read Status Register on MX25UW25645G */
#define MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE       M2_MX25UW25645G_HIGH_PERF_REG_SR_READ_ACCESS_MODE
#define MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE       M2_MX25UW25645G_HIGH_PERF_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_SR_CMD                        M2_MX25UW25645G_REG_SR_WRITE_CMD
#define MEMORY_WRITE_SR_ACCESS_MODE                M2_MX25UW25645G_REG_SR_WRITE_ACCESS_MODE
/* Configuration Register access commands */
#define MEMORY_READ_CR_CMD                         M2_MX25UW25645G_REG_CR_READ_CMD
#define MEMORY_READ_CR_ACCESS_MODE                 M2_MX25UW25645G_REG_CR_READ_ACCESS_MODE
#define MEMORY_READ_CR_DUMMY_CYCLE                 M2_MX25UW25645G_REG_CR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_CR_CMD                        M2_MX25UW25645G_REG_CR_WRITE_CMD
#define MEMORY_WRITE_CR_ACCESS_MODE                M2_MX25UW25645G_REG_CR_WRITE_ACCESS_MODE
/* Configuration Register 2 access commands */
#define MEMORY_READ_CR2_CMD                        M2_MX25UW25645G_REG_CR2_READ_CMD
#define MEMORY_READ_CR2_ACCESS_MODE                M2_MX25UW25645G_REG_CR2_READ_ACCESS_MODE
#define MEMORY_READ_CR2_DUMMY_CYCLE                M2_MX25UW25645G_REG_CR2_READ_DUMMY_CYCLE
#define MEMORY_WRITE_CR2_CMD                       M2_MX25UW25645G_REG_CR2_WRITE_CMD
#define MEMORY_WRITE_CR2_ACCESS_MODE               M2_MX25UW25645G_REG_CR2_WRITE_ACCESS_MODE
/* JEDEC ID and SFDP content reading commands */
#define MEMORY_READ_JEDEC_ID_CMD                   M2_MX25UW25645G_JEDEC_READ_CMD
#define MEMORY_READ_JEDEC_ID_ACCESS_MODE           M2_MX25UW25645G_READ_JEDEC_ID_ACCESS_MODE
#define MEMORY_READ_JEDEC_ID_VALUE                 M2_MX25UW25645G_JEDEC_ID_VALUE
#define MEMORY_READ_SFDP_CMD                       M2_MX25UW25645G_READ_SFDP_CMD
#define MEMORY_READ_SFDP_ACCESS_MODE               M2_MX25UW25645G_READ_SFDP_ACCESS_MODE
#define MEMORY_SFDP_DUMMY_CYCLES                   M2_MX25UW25645G_SFDP_DUMMY_CYCLES
/* Read and Page Program commands */
#define MEMORY_READ_CMD                            M2_MX25UW25645G_OCTAL_DTR_READ_CMD
#define MEMORY_READ_ACCESS_MODE                    M2_MX25UW25645G_OCTAL_DTR_READ_ACCESS_MODE
#define MEMORY_READ_DUMMY_CYCLE                    M2_MX25UW25645G_OCTAL_DTR_READ_DUMMY_CYCLE
#define MEMORY_PAGE_PROG_CMD                       M2_MX25UW25645G_OCTAL_DTR_PAGE_PROG_CMD
#define MEMORY_PAGE_PROG_ACCESS_MODE               M2_MX25UW25645G_OCTAL_DTR_PAGE_PROG_ACCESS_MODE
/* Sector Erase command */
#define MEMORY_SECTOR_ERASE_CMD                    M2_MX25UW25645G_OCTAL_DTR_SECTOR_ERASE_CMD
#define MEMORY_SECTOR_ERASE_ACCESS_MODE            M2_MX25UW25645G_OCTAL_DTR_SECTOR_ERASE_ACCESS_MODE
#define MEMORY_SECTOR_ERASE_TIMEOUT                M2_MX25UW25645G_OCTAL_DTR_SECTOR_ERASE_TIMEOUT
/* Page Size */
#define MEMORY_PAGE_SIZE                           M2_MX25UW25645G_PAGE_SIZE
/* Macro for retrieving xSPI peripheral frequency */
#define MEMORY_PERIPH_CLOCK_VALUE                  M2_MX25UW25645G_PERIPH_CLOCK_VALUE


#elif defined(M2_MX25LM51245G)

/* ###########################################################################
   Include file  : "m2_mx25lm51245g.h"
   Related board : MB1927-33BA from B-M2MEM-PACK1
   Memory Device Manufacturer : Macronix
   Memory Device Reference : MX25LM51245GXDI00
   Density : 32 Mbits
   Memory configuration steps : Drive Strength update in Configuration Register
                                Dummy Cycles update in Configuration Register 2
                                Switch in Octal mode (set DOPI bit) in Configuration Register 2
   ###########################################################################*/

/* LDO Setup time (ms) */
#define MEMORY_M2_BOARD_LDO_SETUP_TIME             M2_MX25LM51245G_LDO_SETUP_TIME

/* Write Enable command */
#define MEMORY_WRITE_ENABLE_CMD                    M2_MX25LM51245G_WRITE_ENABLE_CMD
#define MEMORY_WRITE_DISABLE_CMD                   M2_MX25LM51245G_WRITE_DISABLE_CMD
#define MEMORY_WRITE_ENABLE_ACCESS_MODE            M2_MX25LM51245G_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_WRITE_DISABLE_ACCESS_MODE           M2_MX25LM51245G_WRITE_ENABLE_DISABLE_ACCESS_MODE
#define MEMORY_HIGH_PERF_WRITE_ENABLE_CMD          M2_MX25LM51245G_HIGH_PERF_WRITE_ENABLE_CMD
#define MEMORY_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE  M2_MX25LM51245G_HIGH_PERF_WRITE_ENABLE_ACCESS_MODE
/* Autopolling for WEL and WIP bits */
#define MEMORY_WRITE_ENABLE_MATCH_VALUE            M2_MX25LM51245G_WRITE_ENABLE_MATCH_VALUE
#define MEMORY_WRITE_ENABLE_MASK_VALUE             M2_MX25LM51245G_WRITE_ENABLE_MASK_VALUE
#define MEMORY_WIP_STATUS_MATCH_VALUE              M2_MX25LM51245G_WIP_STATUS_MATCH_VALUE
#define MEMORY_WIP_STATUS_MASK_VALUE               M2_MX25LM51245G_WIP_STATUS_MASK_VALUE
/* Status Register access commands */
#define MEMORY_READ_SR_CMD                         M2_MX25LM51245G_REG_SR_READ_CMD
#define MEMORY_READ_SR_ADDRESS                     0U /* No address used in Read Status Register on MX25LM51245G */
#define MEMORY_READ_SR_ACCESS_MODE                 M2_MX25LM51245G_REG_SR_READ_ACCESS_MODE
#define MEMORY_READ_SR_DUMMY_CYCLE                 M2_MX25LM51245G_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_HIGH_PERF_READ_SR_CMD               M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_CMD
#define MEMORY_HIGH_PERF_READ_SR_ADDRESS           0U /* No address used in Read Status Register on MX25LM51245G */
#define MEMORY_HIGH_PERF_READ_SR_ACCESS_MODE       M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_ACCESS_MODE
#define MEMORY_HIGH_PERF_READ_SR_DUMMY_CYCLE       M2_MX25LM51245G_HIGH_PERF_REG_SR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_SR_CMD                        M2_MX25LM51245G_REG_SR_WRITE_CMD
#define MEMORY_WRITE_SR_ACCESS_MODE                M2_MX25LM51245G_REG_SR_WRITE_ACCESS_MODE
/* Configuration Register access commands */
#define MEMORY_READ_CR_CMD                         M2_MX25LM51245G_REG_CR_READ_CMD
#define MEMORY_READ_CR_ACCESS_MODE                 M2_MX25LM51245G_REG_CR_READ_ACCESS_MODE
#define MEMORY_READ_CR_DUMMY_CYCLE                 M2_MX25LM51245G_REG_CR_READ_DUMMY_CYCLE
#define MEMORY_WRITE_CR_CMD                        M2_MX25LM51245G_REG_CR_WRITE_CMD
#define MEMORY_WRITE_CR_ACCESS_MODE                M2_MX25LM51245G_REG_CR_WRITE_ACCESS_MODE
/* Configuration Register 2 access commands */
#define MEMORY_READ_CR2_CMD                        M2_MX25LM51245G_REG_CR2_READ_CMD
#define MEMORY_READ_CR2_ACCESS_MODE                M2_MX25LM51245G_REG_CR2_READ_ACCESS_MODE
#define MEMORY_READ_CR2_DUMMY_CYCLE                M2_MX25LM51245G_REG_CR2_READ_DUMMY_CYCLE
#define MEMORY_WRITE_CR2_CMD                       M2_MX25LM51245G_REG_CR2_WRITE_CMD
#define MEMORY_WRITE_CR2_ACCESS_MODE               M2_MX25LM51245G_REG_CR2_WRITE_ACCESS_MODE
/* JEDEC ID and SFDP content reading commands */
#define MEMORY_READ_JEDEC_ID_CMD                   M2_MX25LM51245G_JEDEC_READ_CMD
#define MEMORY_READ_JEDEC_ID_ACCESS_MODE           M2_MX25LM51245G_READ_JEDEC_ID_ACCESS_MODE
#define MEMORY_READ_JEDEC_ID_VALUE                 M2_MX25LM51245G_JEDEC_ID_VALUE
#define MEMORY_READ_SFDP_CMD                       M2_MX25LM51245G_READ_SFDP_CMD
#define MEMORY_READ_SFDP_ACCESS_MODE               M2_MX25LM51245G_READ_SFDP_ACCESS_MODE
#define MEMORY_SFDP_DUMMY_CYCLES                   M2_MX25LM51245G_SFDP_DUMMY_CYCLES
/* Read and Page Program commands */
#define MEMORY_READ_CMD                            M2_MX25LM51245G_OCTAL_DTR_READ_CMD
#define MEMORY_READ_ACCESS_MODE                    M2_MX25LM51245G_OCTAL_DTR_READ_ACCESS_MODE
#define MEMORY_READ_DUMMY_CYCLE                    M2_MX25LM51245G_OCTAL_DTR_READ_DUMMY_CYCLE
#define MEMORY_PAGE_PROG_CMD                       M2_MX25LM51245G_OCTAL_DTR_PAGE_PROG_CMD
#define MEMORY_PAGE_PROG_ACCESS_MODE               M2_MX25LM51245G_OCTAL_DTR_PAGE_PROG_ACCESS_MODE
/* Sector Erase command */
#define MEMORY_SECTOR_ERASE_CMD                    M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_CMD
#define MEMORY_SECTOR_ERASE_ACCESS_MODE            M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_ACCESS_MODE
#define MEMORY_SECTOR_ERASE_TIMEOUT                M2_MX25LM51245G_OCTAL_DTR_SECTOR_ERASE_TIMEOUT
/* Page Size */
#define MEMORY_PAGE_SIZE                           M2_MX25LM51245G_PAGE_SIZE
/* Macro for retrieving xSPI peripheral frequency */
#define MEMORY_PERIPH_CLOCK_VALUE                  M2_MX25LM51245G_PERIPH_CLOCK_VALUE
#endif /* M2_M95P32 */

/* Defines related to memory LDO voltage low time for power off */
#define MEMORY_M2_BOARD_LDO_RESET_TIME             100U

/* Auto-polling values */
#define MEMORY_AUTO_POLLING_INTERVAL               10U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
Memory_Toolbox_Status_t Memory_Toolbox_WriteEnable(XSPI_HandleTypeDef *hxspi, Memory_Toolbox_PerfMode_t mode);
Memory_Toolbox_Status_t Memory_Toolbox_AutoPollingMemReady(XSPI_HandleTypeDef *hxspi, Memory_Toolbox_PerfMode_t mode,
                                                           uint32_t timeout);
Memory_Toolbox_Status_t Memory_Toolbox_Read_JedecID(XSPI_HandleTypeDef *hxspi, uint8_t *JedecID);
Memory_Toolbox_Status_t Memory_Toolbox_Read_SFDP_Data(XSPI_HandleTypeDef *hxspi, uint8_t *SFDP_Table,
                                                      uint32_t SFDP_Table_Addr, uint32_t SFDP_Table_Size);
Memory_Toolbox_Status_t Memory_Toolbox_Read_All_SFDP_Table(XSPI_HandleTypeDef *hxspi);
Memory_Toolbox_Status_t Memory_Toolbox_Check_Write_Enabling(XSPI_HandleTypeDef *hxspi);
Memory_Toolbox_Status_t Memory_Toolbox_Erase_Sector(XSPI_HandleTypeDef *hxspi, uint32_t Sector_Addr);
Memory_Toolbox_Status_t Memory_Toolbox_Program_Page(XSPI_HandleTypeDef *hxspi, uint8_t *aPageBuffer,
                                                    uint32_t Page_Addr);
Memory_Toolbox_Status_t Memory_Toolbox_Read_MemData(XSPI_HandleTypeDef *hxspi, uint8_t *Read_Data_Buffer,
                                                    uint32_t Data_Addr, uint32_t Data_Size);
Memory_Toolbox_Status_t Memory_Toolbox_Read_MemData_MemoryMapped_Mode(uint8_t *Read_Data_Buffer,
                                                                      uint8_t *Data_Addr, uint32_t Data_Size);
Memory_Toolbox_Status_t Memory_Toolbox_Enable_MemoryMapped_Mode(XSPI_HandleTypeDef *hxspi);
Memory_Toolbox_Status_t Memory_Toolbox_Disable_MemoryMapped_Mode(XSPI_HandleTypeDef *hxspi);
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister(XSPI_HandleTypeDef *hxspi,
                                                           uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                          uint8_t RegisterBufferSize, Memory_Toolbox_PerfMode_t mode);
#if defined(M2_MX25UW25645G) || defined(M2_MX25LM51245G) || defined(M2_M95P32)
Memory_Toolbox_Status_t Memory_Toolbox_WriteConfigRegister(XSPI_HandleTypeDef *hxspi,
                                                           uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadConfigRegister(XSPI_HandleTypeDef *hxspi,
                                                          uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
#endif /* M2_MX25UW25645G || M2_MX25LM51245G || M2_M95P32 */
#if defined(M2_MX25UW25645G) || defined(M2_MX25LM51245G)
Memory_Toolbox_Status_t Memory_Toolbox_WriteConfigRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                            uint8_t RegisterBufferSize, uint32_t Reg_Addr);
Memory_Toolbox_Status_t Memory_Toolbox_ReadConfigRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize, uint32_t Reg_Addr);
#endif /* M2_MX25UW25645G || M2_MX25LM51245G */
#if defined(M2_W25Q16JV)
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister2(XSPI_HandleTypeDef *hxspi,
                                                            uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister2(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_WriteStatusRegister3(XSPI_HandleTypeDef *hxspi,
                                                            uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadStatusRegister3(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                           uint8_t RegisterBufferSize);
#endif /* M2_W25Q16JV */
#if defined(M2_IS25LP032D)
Memory_Toolbox_Status_t Memory_Toolbox_WriteReadRegister(XSPI_HandleTypeDef *hxspi,
                                                         uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadReadRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                        uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_WriteExtendedRegister(XSPI_HandleTypeDef *hxspi,
                                                             uint8_t *aRegisterBuffer, uint8_t RegisterBufferSize);
Memory_Toolbox_Status_t Memory_Toolbox_ReadExtendedRegister(XSPI_HandleTypeDef *hxspi, uint8_t *aRegisterBuffer,
                                                            uint8_t RegisterBufferSize);
#endif /* M2_IS25LP032D */
#if defined(M2_IS25LP032D)
Memory_Toolbox_Status_t Memory_Toolbox_EnterQuad(XSPI_HandleTypeDef *hxspi);
#endif /* M2_IS25LP032D */
Memory_Toolbox_Status_t Memory_Toolbox_MemCmp(uint8_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize);
void Memory_Toolbox_Update_Register_Value(Memory_Toolbox_RegisterId_t RegId, uint8_t Value);

/* Exported functions for Trace system ---------------------------------------*/
#if defined(EXAMPLE_TRACE_ENABLE)
void Memory_Toolbox_Trace(Memory_Toolbox_TraceLevel_t TraceLevel, const uint8_t *Msg);
void Memory_Toolbox_TraceBuffer(Memory_Toolbox_TraceLevel_t TraceLevel, const uint8_t *Buffer, uint16_t Size);
void Memory_Toolbox_RegisterTraceCallback(Memory_Toolbox_TraceCallback_t TraceCallback);
void Memory_Toolbox_SetTraceLevel(Memory_Toolbox_TraceLevel_t TraceLevel);
void Memory_Toolbox_LogStatus(Memory_Toolbox_Status_t Status);
void Memory_Toolbox_Dump_Registers(void);
void Memory_Toolbox_LogFrequency(XSPI_HandleTypeDef *hxspi);
#endif /* EXAMPLE_TRACE_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* __MEMORY_TOOLBOX_H */
