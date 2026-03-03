/**
  ******************************************************************************
  * @file    ram_interface.c
  * @author  MCD Application Team
  * @brief   Contains RAM access functions
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
#include "openbl_core.h"

#include "app_openbootloader.h"
#include "ram_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef RAM_Descriptor =
{
  RAM_START_ADDRESS + OPENBL_RAM_SIZE, /* OPENBL_RAM_SIZE is added to protect OpenBootloader RAM area */
  RAM_END_ADDRESS,
  RAM_SIZE,
  RAM_AREA,
  OPENBL_RAM_Read,
  OPENBL_RAM_Write,
  NULL,
  NULL,
  OPENBL_RAM_JumpToAddress,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to read data from a given address.
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_RAM_Read(uint32_t address)
{
  return (*(uint8_t *)(address));
}

/**
  * @brief  This function is used to write data in RAM memory.
  * @param  address The address where that data will be written.
  * @param  p_data The data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_RAM_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  uint32_t index;
  uint32_t aligned_length = data_length;

  if (aligned_length & 0x3U)
  {
    aligned_length = (aligned_length & 0xFFFFFFFCU) + 4U;
  }

  for (index = 0U; index < aligned_length; index += 4U)
  {
    *(__IO uint32_t *)(address + index) = *(__IO uint32_t *)(p_data + index);
  }
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  address The address where the function will jump.
  * @retval None.
  */
void OPENBL_RAM_JumpToAddress(uint32_t address)
{
  Function_Pointer jump_to_address;

  /* De-initialize all HW resources used by the Open Bootloader to their reset values */
  OPENBL_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) address);

  jump_to_address();
}
