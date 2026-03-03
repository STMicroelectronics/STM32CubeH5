/**
  ******************************************************************************
  * @file    otp_interface.c
  * @author  MCD Application Team
  * @brief   Contains One-time programmable access functions
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
#include "flash_interface.h"
#include "otp_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OTP_PROG_STEP_SIZE                ((uint8_t)0x02)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OPENBL_OTP_Program(uint32_t otp_address, uint32_t data_address);

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
  * @param  address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_OTP_Read(uint32_t address)
{
  uint32_t tmp_address = address;
  uint32_t index       = 0U;
  uint16_t read_data;

  if ((tmp_address % 2U) != 0U)
  {
    index        = (tmp_address % 2U);
    tmp_address -= index;
  }

  /* Read a half word to be able to extract from it the needed byte */
  read_data = (*(uint16_t *)tmp_address);

  /* Return only the needed byte */
  return (*(((uint8_t *)&read_data) + index));
}

/**
  * @brief  This function is used to write data in OTP.
  * @param  address The address where that data will be written.
  * @param  p_data The data to be written.
  * @param  data_length The length of the data to be written.
  * @retval None.
  */
void OPENBL_OTP_Write(uint32_t address, uint8_t *p_data, uint32_t data_length)
{
  uint32_t index;
  __ALIGNED(4) uint8_t data[OTP_PROG_STEP_SIZE] = {0x0U};
  uint8_t remaining;

  if ((p_data != NULL) && (data_length != 0U))
  {
    /* Unlock the flash memory for write operation */
    OPENBL_FLASH_Unlock();

    /* Clear error programming flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    /* Program half-word by half-word (2 bytes) */
    while (data_length > OTP_PROG_STEP_SIZE)
    {
      for (index = 0U; index < OTP_PROG_STEP_SIZE; index++)
      {
        data[index] = *(p_data + index);
      }

      OPENBL_OTP_Program(address, (uint32_t)data);

      address     += OTP_PROG_STEP_SIZE;
      p_data      += OTP_PROG_STEP_SIZE;
      data_length -= OTP_PROG_STEP_SIZE;
    }

    /* If remaining count, go back to fill the rest with 0xFF */
    if (data_length > 0U)
    {
      remaining = OTP_PROG_STEP_SIZE - data_length;

      /* Copy the remaining bytes */
      for (index = 0U; index < data_length; index++)
      {
        data[index] = *(p_data + index);
      }

      /* Fill the upper bytes with 0xFF */
      for (index = 0U; index < remaining; index++)
      {
        data[index + data_length] = 0xFFU;
      }

      /* FLASH word program */
      OPENBL_OTP_Program(address, (uint32_t)data);
    }

    /* Lock the Flash to disable the flash control register access */
    OPENBL_FLASH_Lock();
  }
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  otp_address specifies the address to be programmed.
  * @param  data_address specifies the address of the data to be programmed.
  * @retval None.
  */
static void OPENBL_OTP_Program(uint32_t otp_address, uint32_t data_address)
{
  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD_OTP, otp_address, data_address);
}
