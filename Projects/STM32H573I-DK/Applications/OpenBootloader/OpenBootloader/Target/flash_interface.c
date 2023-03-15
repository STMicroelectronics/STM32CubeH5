/**
  ******************************************************************************
  * @file    flash_interface.c
  * @author  MCD Application Team
  * @brief   Contains FLASH access functions
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
#include "i2c_interface.h"
#include "optionbytes_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_SECTOR_NUMBER              ((uint16_t)256U)

#define OPENBL_FLASH_TIMEOUT_VALUE       0x00000FFFU

#define FLASH_PROG_STEP_SIZE             ((uint8_t)16U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t Flash_BusyState          = FLASH_BUSY_STATE_DISABLED;
static FLASH_ProcessTypeDef FlashProcess = {.Lock = HAL_UNLOCKED, \
                                            .ErrorCode = HAL_FLASH_ERROR_NONE, \
                                            .ProcedureOnGoing = 0U, \
                                            .Address = 0U, \
                                            .Bank = FLASH_BANK_1, \
                                            .Sector = 0U, \
                                            .NbSectorsToErase = 0U
                                           };

/* Private function prototypes -----------------------------------------------*/
static void OPENBL_FLASH_Program(uint32_t FlashAddress, uint32_t DataAddress);
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfSectors, uint32_t Length);
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void);

/* Exported variables --------------------------------------------------------*/
OPENBL_MemoryTypeDef FLASH_Descriptor =
{
  FLASH_START_ADDRESS,
  FLASH_END_ADDRESS,
  FLASH_MEM_SIZE,
  FLASH_AREA,
  OPENBL_FLASH_Read,
  OPENBL_FLASH_Write,
  NULL,
  OPENBL_FLASH_SetWriteProtection,
  OPENBL_FLASH_JumpToAddress,
  NULL,
  OPENBL_FLASH_Erase
};

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Unlock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Unlock(void)
{
  HAL_FLASH_Unlock();
}

/**
  * @brief  Lock the FLASH control register access.
  * @retval None.
  */
void OPENBL_FLASH_Lock(void)
{
  HAL_FLASH_Lock();
}

/**
  * @brief  Unlock the FLASH Option Bytes Registers access.
  * @retval None.
  */
void OPENBL_FLASH_OB_Unlock(void)
{
  HAL_FLASH_Unlock();

  HAL_FLASH_OB_Unlock();
}

/**
  * @brief  This function is used to read data from a given address.
  * @param  Address The address to be read.
  * @retval Returns the read value.
  */
uint8_t OPENBL_FLASH_Read(uint32_t Address)
{
  return (*(uint8_t *)(Address));
}

/**
  * @brief  This function is used to write data in FLASH memory.
  * @param  Address The address where that data will be written.
  * @param  pData The data to be written.
  * @param  DataLength The length of the data to be written.
  * @retval None.
  */
void OPENBL_FLASH_Write(uint32_t Address, uint8_t *pData, uint32_t DataLength)
{
  uint32_t index;
  __ALIGNED(4) uint8_t data[FLASH_PROG_STEP_SIZE] = {0x0U};
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
      for (index = 0U; index < FLASH_PROG_STEP_SIZE; index++)
      {
        data[index] = *(pData + index);
      }

      OPENBL_FLASH_Program(Address, (uint32_t)data);

      Address    += FLASH_PROG_STEP_SIZE;
      pData      += FLASH_PROG_STEP_SIZE;
      DataLength -= FLASH_PROG_STEP_SIZE;
    }

    /* If remaining count, go back to fill the rest with 0xFF */
    if (DataLength > 0U)
    {
      remaining = FLASH_PROG_STEP_SIZE - DataLength;

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
      OPENBL_FLASH_Program(Address, (uint32_t)data);
    }

    /* Lock the Flash to disable the flash control register access */
    OPENBL_FLASH_Lock();
  }
}

/**
  * @brief  This function is used to jump to a given address.
  * @param  Address The address where the function will jump.
  * @retval None.
  */
void OPENBL_FLASH_JumpToAddress(uint32_t Address)
{
  Function_Pointer jump_to_address;

  /* De-initialize all HW resources used by the Open Bootloader to their reset values */
  OPENBL_DeInit();

  /* Enable IRQ */
  Common_EnableIrq();

  jump_to_address = (Function_Pointer)(*(__IO uint32_t *)(Address + 4U));

  /* Initialize user application's stack pointer */
  Common_SetMsp(*(__IO uint32_t *) Address);

  jump_to_address();
}

/**
  * @brief  Get the the product state.
  * @retval ProductState returns the product state.
  *         This returned value can be one of @ref FLASH_OB_Product_State
  */
uint32_t OPENBL_FLASH_GetProductState(void)
{
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Get the Option bytes configuration */
  HAL_FLASHEx_OBGetConfig(&flash_ob);

  return flash_ob.ProductState;
}

/**
  * @brief  This function is used to enable or disable write protection of the specified FLASH areas.
  * @param  State Can be one of these values:
  *         @arg DISABLE: Disable FLASH write protection
  *         @arg ENABLE: Enable FLASH write protection
  * @param  pListOfSectors Contains the list of sectors to be protected.
  * @param  Length The length of the list of sectors to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
ErrorStatus OPENBL_FLASH_SetWriteProtection(FunctionalState State, uint8_t *pListOfSectors, uint32_t Length)
{
  ErrorStatus status = SUCCESS;

  if (State == ENABLE)
  {
    OPENBL_FLASH_EnableWriteProtection(pListOfSectors, Length);

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else if (State == DISABLE)
  {
    OPENBL_FLASH_DisableWriteProtection();

    /* Register system reset callback */
    Common_SetPostProcessingCallback(OPENBL_OB_Launch);
  }
  else
  {
    status = ERROR;
  }

  return status;
}

/**
  * @brief  This function is used to start FLASH mass erase operation.
  * @param  *pData Pointer to the buffer that contains mass erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Mass erase operation done
  *          - ERROR:   Mass erase operation failed or the value of one parameter is not OK
  */
ErrorStatus OPENBL_FLASH_MassErase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t sector_error;
  uint16_t bank_option;
  ErrorStatus status = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  erase_init_struct.TypeErase = FLASH_TYPEERASE_MASSERASE;

  if (DataLength >= 2U)
  {
    bank_option = *(uint16_t *)(pData);

    if (bank_option == FLASH_MASS_ERASE)
    {
      erase_init_struct.Banks = 0U;
    }
    else if (bank_option == FLASH_BANK1_ERASE)
    {
      erase_init_struct.Banks = FLASH_BANK_1;
    }
    else if (bank_option == FLASH_BANK2_ERASE)
    {
      erase_init_struct.Banks = FLASH_BANK_2;
    }
    else
    {
      status = ERROR;
    }

    if (status == SUCCESS)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &sector_error) != HAL_OK)
      {
        status = ERROR;
      }
      else
      {
        status = SUCCESS;
      }
    }
  }
  else
  {
    status = ERROR;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  return status;
}

/**
  * @brief  This function is used to erase the specified FLASH sectors.
  * @param  *pData Pointer to the buffer that contains erase operation options.
  * @param  DataLength Size of the Data buffer.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Erase operation done
  *          - ERROR:   Erase operation failed or the value of one parameter is not OK
  */
ErrorStatus OPENBL_FLASH_Erase(uint8_t *pData, uint32_t DataLength)
{
  uint32_t counter;
  uint32_t sectors_number;
  uint32_t sector_error = 0U;
  uint32_t errors       = 0U;
  ErrorStatus status    = SUCCESS;
  FLASH_EraseInitTypeDef erase_init_struct;

  /* Unlock the flash memory for erase operation */
  OPENBL_FLASH_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  sectors_number = (uint32_t)(*(uint16_t *)(pData));

  /* The sector number size is 2 bytes */
  pData += 2U;

  erase_init_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
  erase_init_struct.NbSectors = 1U;

  for (counter = 0U; ((counter < sectors_number) && (counter < (DataLength / 2U))); counter++)
  {
    erase_init_struct.Sector = ((uint32_t)(*(uint16_t *)(pData)));

    if (erase_init_struct.Sector <= ((FLASH_SECTOR_NUMBER / 2U) - 1U))
    {
      erase_init_struct.Banks = FLASH_BANK_1;
    }
    else if (erase_init_struct.Sector <= (FLASH_SECTOR_NUMBER - 1U))
    {
      erase_init_struct.Banks = FLASH_BANK_2;
    }
    else
    {
      status = ERROR;
    }

    if (status != ERROR)
    {
      if (OPENBL_FLASH_ExtendedErase(&erase_init_struct, &sector_error) != HAL_OK)
      {
        errors++;
      }
    }
    else
    {
      /* Reset the status for next erase operation */
      status = SUCCESS;
    }

    /* The sector number size is 2 bytes */
    pData += 2U;
  }

  /* Lock the Flash to disable the flash control register access */
  OPENBL_FLASH_Lock();

  if (errors > 0U)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/**
  * @brief  This function is used to Set Flash busy state variable to activate busy state sending
  *         during flash operations
  * @retval None.
  */
void OPENBL_Enable_BusyState_Flag(void)
{
  /* Enable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_ENABLED;
}

/**
  * @brief  This function is used to disable the send of busy state in I2C non stretch mode.
  * @retval None.
  */
void OPENBL_Disable_BusyState_Flag(void)
{
  /* Disable Flash busy state sending */
  Flash_BusyState = FLASH_BUSY_STATE_DISABLED;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Program double word at a specified FLASH address.
  * @param  FlashAddress specifies the address to be programmed.
  * @param  DataAddress specifies the address of data to be programmed.
  * @retval None.
  */
static void OPENBL_FLASH_Program(uint32_t FlashAddress, uint32_t DataAddress)
{
  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, FlashAddress, DataAddress);
}

/**
  * @brief  This function is used to enable write protection of the specified FLASH areas.
  * @param  pListOfSectors Contains the list of sectors to be protected.
  * @param  Length The length of the list of sectors to be protected.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_EnableWriteProtection(uint8_t *pListOfSectors, uint32_t Length)
{
  uint8_t index;
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear error programming flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  /* Enable FLASH_WRP_SECTORS write protection */
  flash_ob.OptionType = OPTIONBYTE_WRP;
  flash_ob.WRPState   = OB_WRPSTATE_ENABLE;
  flash_ob.WRPSector  = 0U;

  for (index = 0U; index < Length; index++)
  {
    if (*(pListOfSectors + index) < FLASH_SECTOR_127)
    {
      flash_ob.WRPSector |= (uint32_t)1U << (*(pListOfSectors + index));
      flash_ob.Banks      = FLASH_BANK_1;
    }
  }

  if (flash_ob.WRPSector != 0U)
  {
    status = (HAL_FLASHEx_OBProgram(&flash_ob) != HAL_OK) ? ERROR : SUCCESS;
  }

  flash_ob.WRPSector = 0U;

  for (index = 0U; index < Length; index++)
  {
    if (*(pListOfSectors + index) >= FLASH_SECTOR_127)
    {
      flash_ob.WRPSector |= (uint32_t)1U << ((*(pListOfSectors + index)) - (FLASH_SECTOR_127 + 1U));
      flash_ob.Banks      = FLASH_BANK_2;
    }
  }

  if (flash_ob.WRPSector != 0U)
  {
    status = (HAL_FLASHEx_OBProgram(&flash_ob) != HAL_OK) ? ERROR : SUCCESS;
  }

  return status;
}

/**
  * @brief  This function is used to disable write protection.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Enable or disable of the write protection is done
  *          - ERROR:   Enable or disable of the write protection is not done
  */
static ErrorStatus OPENBL_FLASH_DisableWriteProtection(void)
{
  ErrorStatus status = SUCCESS;
  FLASH_OBProgramInitTypeDef flash_ob;

  /* Unlock the FLASH registers & Option Bytes registers access */
  OPENBL_FLASH_OB_Unlock();

  /* Clear all FLASH errors flags before starting write operation */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

  flash_ob.OptionType = OPTIONBYTE_WRP;

  /* Disable write protection of bank 1 area */
  flash_ob.WRPState  = OB_WRPSTATE_DISABLE;
  flash_ob.Banks     = FLASH_BANK_1;
  flash_ob.WRPSector = OB_WRP_SECTOR_ALL;

  HAL_FLASHEx_OBProgram(&flash_ob);

  /* Disable write protection of bank 2 area */
  flash_ob.WRPState  = OB_WRPSTATE_DISABLE;
  flash_ob.Banks     = FLASH_BANK_2;
  flash_ob.WRPSector = OB_WRP_SECTOR_ALL;

  HAL_FLASHEx_OBProgram(&flash_ob);

  return status;
}

/**
  * @brief  Wait for a FLASH operation to complete.
  * @param  Timeout maximum flash operation timeout.
  * @retval HAL_Status
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_WaitForLastOperation(uint32_t Timeout)
#endif /* (__ICCARM__) */
{
  uint32_t error;
  __IO uint32_t *reg_sr;
  uint32_t counter         = 0U;
  HAL_StatusTypeDef status = HAL_OK;

  /* While the FLASH is in busy state, send busy byte to the host */
  while (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
  {
    /* Check if we need to send a busy byte
       NOTE: this can be removed if I2C protocol is not used */
    if (Flash_BusyState == FLASH_BUSY_STATE_ENABLED)
    {
      OPENBL_I2C_SendBusyByte();
    }

    if ((counter++) > Timeout)
    {
      status = HAL_TIMEOUT;
      break;
    }
  }

  /* Access to SECSR or NSSR registers depends on operation type */
  reg_sr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECSR) : &(FLASH_NS->NSSR);

  /* Check FLASH operation error flags */
  error = ((*reg_sr) & FLASH_FLAG_SR_ERRORS);

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  error |= (FLASH->NSSR & FLASH_FLAG_OPTWERR);
#endif /* __ARM_FEATURE_CMSE */

  if (error != 0U)
  {
    /* Save the error code */
    FlashProcess.ErrorCode |= error;

    /* Clear error programming flags */
    (*reg_sr) = error;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    if ((error & FLASH_FLAG_OPTWERR) != 0U)
    {
      FLASH->NSSR = FLASH_FLAG_OPTWERR;
    }
#endif /* __ARM_FEATURE_CMSE */

    status = HAL_ERROR;
  }

  /* Check FLASH End of Operation flag */
  if (((*reg_sr) & FLASH_FLAG_EOP) != 0U)
  {
    /* Clear FLASH End of Operation pending bit */
    (*reg_sr) = FLASH_FLAG_EOP;
  }

  return status;
}

/**
  * @brief  Perform a mass erase or erase the specified FLASH memory sectors.
  * @param[in]  pEraseInit pointer to an FLASH_EraseInitTypeDef structure that
  *         contains the configuration information for the erasing.
  * @param[out]  SectorError pointer to variable that contains the configuration
  *         information on faulty sector in case of error (0xFFFFFFFF means that all
  *         the sectors have been correctly erased).
  * @retval HAL_Status
  */
#if defined (__ICCARM__)
__ramfunc HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
#else
__attribute__((section(".ramfunc"))) HAL_StatusTypeDef OPENBL_FLASH_ExtendedErase(
  FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError)
#endif /* (__ICCARM__) */
{
  uint32_t errors = 0U;
  __IO uint32_t *reg_cr;
  HAL_StatusTypeDef status;

  /* Process Locked */
  __HAL_LOCK(&FlashProcess);

  /* Reset error code */
  FlashProcess.ErrorCode = HAL_FLASH_ERROR_NONE;

  /* Wait for last operation to be completed on Bank1 */
  status = OPENBL_FLASH_WaitForLastOperation((uint32_t)OPENBL_FLASH_TIMEOUT_VALUE);

  if (status == HAL_OK)
  {
    /* Initialization of SectorError variable */
    *SectorError = 0xFFFFFFFFU;

    /* Access to SECCR or NSCR registers depends on operation type */
#if defined (FLASH_OPTSR2_TZEN)
    reg_cr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECCR) : &(FLASH_NS->NSCR);
#else
    reg_cr = &(FLASH_NS->NSCR);
#endif /* FLASH_OPTSR2_TZEN */

    if (((pEraseInit->Banks) & FLASH_BANK_1) == FLASH_BANK_1)
    {
      /* Reset Sector Number for Bank1 */
      CLEAR_BIT((*reg_cr), (FLASH_CR_BKSEL | FLASH_CR_SNB));
    }

    if (((pEraseInit->Banks) & FLASH_BANK_2) == FLASH_BANK_2)
    {
      /* Reset Sector Number for Bank2 */
      CLEAR_BIT((*reg_cr), FLASH_CR_SNB);
      SET_BIT((*reg_cr), FLASH_CR_BKSEL);

      pEraseInit->Sector = pEraseInit->Sector - (FLASH_SECTOR_127 + 1U);
    }

    /* Proceed to erase the sector */
    SET_BIT((*reg_cr), (FLASH_CR_SER | (pEraseInit->Sector << FLASH_CR_SNB_Pos) | FLASH_CR_START));

    /* Wait for last operation to be completed */
    if (OPENBL_FLASH_WaitForLastOperation(PROGRAM_TIMEOUT) != HAL_OK)
    {
      errors++;
    }

    /* If the erase operation is completed, disable the SER Bit */
    CLEAR_BIT(FLASH->NSCR, (FLASH_CR_SER));

    if (status != HAL_OK)
    {
      /* In case of error, stop erase procedure and return the faulty sector */
      *SectorError = pEraseInit->Sector;
    }
  }

  /* Process Unlocked */
  __HAL_UNLOCK(&FlashProcess);

  if (errors > 0U)
  {
    status = HAL_ERROR;
  }
  else
  {
    status = HAL_OK;
  }

  return status;
}
