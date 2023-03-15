/**
  ******************************************************************************
  * @file    Cortex/CORTEXM_MPU/Inc/stm32_mpu.h
  * @author  MCD Application Team
  * @brief   Header for stm32_mpu.c module
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
#ifndef __STM32_MPU_H
#define __STM32_MPU_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define EXAMPLE_RAM_ADDRESS_START                (0x20000000UL)
#define EXAMPLE_RAM_SIZE                         SRAM1_SIZE
#define EXAMPLE_PERIPH_ADDRESS_START             (0x40000000)
#define EXAMPLE_PERIPH_SIZE                      (0x1F400000U)
#define EXAMPLE_FLASH_ADDRESS_START              (0x08000000)
#define EXAMPLE_FLASH_SIZE                       (0x200000U)
#define EXAMPLE_RAM_REGION_NUMBER                MPU_REGION_NUMBER0
#define EXAMPLE_FLASH_REGION_NUMBER              MPU_REGION_NUMBER1
#define EXAMPLE_PERIPH_REGION_NUMBER             MPU_REGION_NUMBER2
#define portMPU_REGION_READ_WRITE                MPU_REGION_ALL_RW
#define EXAMPLE_RAM_ATTRIBUTE_NUMBER             MPU_ATTRIBUTES_NUMBER0
#define EXAMPLE_FLASH_ATTRIBUTE_NUMBER           MPU_ATTRIBUTES_NUMBER1
#define EXAMPLE_PERIPH_ATTRIBUTE_NUMBER          MPU_ATTRIBUTES_NUMBER2
#define portMPU_REGION_PRIVILEGED_READ_ONLY      MPU_REGION_PRIV_RO
#define portMPU_REGION_READ_ONLY                 MPU_REGION_PRIV_RO_URO
#define portMPU_REGION_PRIVILEGED_READ_WRITE     MPU_REGION_PRIV_RW

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void MPU_Config(void);
void MPU_AccessPermConfig(void);

#endif /* __STM32_MPU_H */
