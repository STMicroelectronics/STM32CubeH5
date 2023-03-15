/**
  ******************************************************************************
  * @file    otp_interface.c
  * @author  MCD Application Team
  * @brief   Contains One-time programmable access functions
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

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "common_interface.h"

#include "openbl_mem.h"

#include "app_openbootloader.h"
#include "flash_interface.h"
#include "otp_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OTP_PROG_STEP_SIZE                ((uint8_t)0x16U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OPENBL_OTP_Program(uint32_t OtpAddress, uint32_t DataAddress);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef OTP_Descriptor =
{
  OTP_START_ADDRESS,
  OTP_END_ADDRESS,
  OTP_SIZE,
  OTP_AREA,
  OPENBL_OTP_Read,
  OPENBL_OTP_Write,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OTP_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in OTP.
  * @param  Address The address where that data will be written.
  * @param  pData The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_OTP_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  uint32_t index;
  __ALIGNED(4) uint8_t data[OTP_PROG_STEP_SIZE] = {0x0U};
  uint8_t remaining;

  if ((pData != NULL) && (DataLength != 0U))
  {
    /* Unlock the flash memory for write operation */
    OPENBL_FLASH_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Program double-word by double-word (16 bytes) */
    while ((DataLength >> 4U) > 0U)
    {
      for (index = 0U; index < OTP_PROG_STEP_SIZE; index++)
      {
        data[index] = *(pData + index);
      }

      OPENBL_OTP_Program(Address, (uint32_t)data);

      Address    += OTP_PROG_STEP_SIZE;
      pData      += OTP_PROG_STEP_SIZE;
      DataLength -= OTP_PROG_STEP_SIZE;
    }

    /* If remaining count, go back to fill the rest with 0xFF */
    if (DataLength > 0U)
    {
      remaining = OTP_PROG_STEP_SIZE - DataLength;

      /* Copy the remaining bytes */
      for (index = 0U; index < DataLength; index++)
      {
        data[index] = *(pData + index);
      }

      /* Fill the upper bytes with 0xFF */
      for (index = 0U; index < remaining; index++)
      {
        data[index + DataLength] = 0xFFU;
      }

      /* FLASH word program */
      OPENBL_OTP_Program(Address, (uint32_t)data);
    }

    /* Lock the Flash to disable the flash control register access */
    OPENBL_FLASH_Lock();
  }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  OtpAddress specifies the address to be programmed.
  * @param  DataAddress specifies the address of the data to be programmed.
  * @retval None.
  */
static void OPENBL_OTP_Program(uint32_t OtpAddress, uint32_t DataAddress)
{
  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, OtpAddress, DataAddress);
}
