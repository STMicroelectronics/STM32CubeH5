/**
  ******************************************************************************
  * @file    optionbytes_interface.c
  * @author  MCD Application Team
  * @brief   Contains Option Bytes access functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "common_interface.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OB_Descriptor =
{
  OB_START_ADDRESS,
  OB_END_ADDRESS,
  OB_SIZE,
  OB_AREA,
  OPENBL_OB_Read,
  OPENBL_OB_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Launch the option byte loading.
  * @retval None.
  */
void OPENBL_OB_Launch(void)
{
  HAL_FLASH_OB_Launch();

  /* Set the option lock bit and Lock the flash */
  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OB_Read(uint32_t address)
{
  return (*(uint8_t *)(address));
}

/**
  * @brief  This function is used to write data in Option bytes.
  * @param  address The address where that data will be written.
  * @param  p_data The data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_OB_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  /* Unlock the FLASH & Option Bytes Registers access */
  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  if (address == OB1_START_ADDRESS)
  {
    /* Write in the FLASH option status register */
    if (data_length > 0x04U)
    {
      FLASH->OPTSR_PRG = *(uint32_t *)(uint32_t)&p_data[0x04U];
    }
    /* Write in the FLASH option status register 2 */
    if (data_length > 0x24U)
    {
      FLASH->OPTSR2_PRG = *(uint32_t *)(uint32_t)&p_data[0x24U];
    }
    /* Write in FLASH non-secure unique boot entry register */
    if (data_length > 0x34U)
    {
      FLASH->NSBOOTR_PRG = *(uint32_t *)(uint32_t)&p_data[0x34U];
    }

    /* Write the Non secure OTP block lock */
    if (data_length > 0x44U)
    {
      FLASH->OTPBLR_PRG = *(uint32_t *)(uint32_t)&p_data[0x44U];
    }

    /* Write in the FLASH write protection sector group 1 register for bank2 */
    if (data_length > 0x4CU)
    {
      FLASH->WRP12R_PRG = *(uint32_t *)(uint32_t)&p_data[0x4CU];
    }

    /* Write in the FLASH write protection sector group 1 register for bank1 */
    if (data_length > 0x9CU)
    {
      FLASH->WRP11R_PRG = *(uint32_t *)(uint32_t)&p_data[0x9CU];
    }

    /* Write in the FLASH data sectors configuration register for bank1 */
    if (data_length > 0xA4U)
    {
      FLASH->EDATA1R_PRG = *(uint32_t *)(uint32_t)&p_data[0xA4U];
    }

    /* Write in the FLASH HDP configuration register for bank1 */
    if (data_length > 0xACU)
    {
      FLASH->HDP1R_PRG = *(uint32_t *)(uint32_t)&p_data[0xACU];
    }
  }
  else if (address == OB2_START_ADDRESS)
  {
    /* Write in FLASH write protection sector group 2 register for bank2 */
    if (data_length > 0x0CU)
    {
      FLASH->WRP22R_PRG = *(uint32_t *)(uint32_t)&p_data[0x0CU];
    }

    /* Write in FLASH write protection sector group 2 register for bank2 */
    if (data_length > 0x5CU)
    {
      FLASH->WRP21R_PRG = *(uint32_t *)(uint32_t)&p_data[0x5CU];
    }

    /* Write in the FLASH data sectors configuration register for bank2 */
    if (data_length > 0x64U)
    {
      FLASH->EDATA2R_PRG = *(uint32_t *)(uint32_t)&p_data[0x64U];
    }

    /* Write in FLASH HDP configuration register for bank2 */
    if (data_length > 0x6CU)
    {
      FLASH->HDP2R_PRG = *(uint32_t *)(uint32_t)&p_data[0x6CU];
    }
  }
  else
  {
    /* Nothing to do since the start address must be the start address of OB1 or OB2 */
  }

  /* Register system reset callback */
  Common_SetPostProcessingCallback(OPENBL_OB_Launch);
}
