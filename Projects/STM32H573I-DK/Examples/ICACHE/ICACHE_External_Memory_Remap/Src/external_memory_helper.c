/**
  ******************************************************************************
* @file    ICACHE/ICACHE_External_Memory_Remap/Src/external_memory_helper.c
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
#include "stm32h573i_discovery_ospi.h"

#if defined (HAL_XSPI_MODULE_ENABLED)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the external memory in indirect-mode and transmit
  *         an amount of Bytes at address DstAddress and size DataSize.
  * @param DstAddress : pointer to the address of the buffer.
  * @param DataSize   : size of the transferred Data.
  * @retval None
  */
void extMemory_SendDataToExtMem(uint8_t *DstAddress,uint32_t DataSize)
{
  BSP_OSPI_NOR_Init_t NOR_init;
  Ospi_Nor_Ctx[0].IsInitialized = OSPI_ACCESS_NONE;
  NOR_init.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
  NOR_init.TransferRate  = BSP_OSPI_NOR_STR_TRANSFER;
  BSP_OSPI_NOR_Init(0, &NOR_init);

  /* Erase external memory Block (64KByte)*/
  BSP_OSPI_NOR_Erase_Block(0, 0, BSP_OSPI_NOR_ERASE_64K);

  BSP_OSPI_NOR_Write(0, (uint8_t *)DstAddress, 0, DataSize);

}
/**
* @brief  Configure the external memory in memory-mapped mode.
* @retval None
*/
void extMemory_MemoryMappedConfiguration(void)
{
  BSP_OSPI_NOR_EnableMemoryMappedMode(0);
}

#endif /* HAL_XSPI_MODULE_ENABLED */

/**
* @}
*/

/**
* @}
*/
