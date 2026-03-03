/**
  ******************************************************************************
  * @file    Examples/BSP/Src/xspi.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the OSPI feature in the
  *          STM32H5F5J-DK driver
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32H5xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE         ((uint32_t) 0x1000)
#define OPI_START_ADDRESS   0*MX25LM51245G_SECTOR_64K
#define TEST_VALUE_START    0x43
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];
char str1[50];
static BSP_XSPI_NOR_Info_t pXSPI_Info;
BSP_XSPI_NOR_Init_t Flash;
uint8_t *XSPI_MMP_pointer;

BSP_XSPI_NOR_Interface_t InterfaceMode[2] =
{
  BSP_XSPI_NOR_SPI_MODE,
  BSP_XSPI_NOR_OPI_MODE
};

/* Private function prototypes -----------------------------------------------*/
static void XSPI_DTR_Mode(void);
static void XSPI_STR_Mode(void);
static void XSPI_STR_MM_Mode(void);
static void XSPI_DTR_MM_Mode(void);
static void    Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLength, uint32_t uwOffset);
static uint8_t RMABuffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength, uint32_t *ErrorAd);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Test XSPI interfaces and transfer modes
  * @param  None
  * @retval None
  */
void Test_Xspi(void)
{
  StepBack = 0;
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);  
  UTIL_LCD_FillRect(0, 0, 480, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test XSPI  ", CENTER_MODE);
  BSP_LCD_FillRGBRect(0, 400, 0, (uint8_t *)GoBack, 40, 40);
  
  
  /* Fill the buffer for write operation *************************************/
  Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, TEST_VALUE_START);
  
  /*##-2- Read & check the XSPI info #######################################*/
  /* Initialize the structure */
  pXSPI_Info.FlashSize          = (uint32_t)0x00;
  pXSPI_Info.EraseSectorSize    = (uint32_t)0x00;
  pXSPI_Info.EraseSectorsNumber = (uint32_t)0x00;
  pXSPI_Info.ProgPageSize       = (uint32_t)0x00;
  pXSPI_Info.ProgPagesNumber    = (uint32_t)0x00;
  
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

  
  /* Read the XSPI memory info */
  if(BSP_XSPI_NOR_GetInfo(0, &pXSPI_Info) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* Test the correctness */
  else if((pXSPI_Info.FlashSize != 0x4000000) || (pXSPI_Info.EraseSectorSize != 0x10000)  ||
          (pXSPI_Info.ProgPageSize != 0x100)  || (pXSPI_Info.EraseSectorsNumber != 0x400) ||
          (pXSPI_Info.ProgPagesNumber != 262144))
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
    UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"GET INFO : FAILED", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"Test Aborted", CENTER_MODE);
    Error_Handler();
  }
  else
  {
    XSPI_STR_Mode();
    XSPI_STR_MM_Mode();
    XSPI_DTR_Mode();
    XSPI_DTR_MM_Mode();
    while (StepBack != 1)
    {
      if(TouchPressed > 0)
      {
        TouchPressed = 0;
        /* Quit the Test */
      if ((x_disp > 400) && (x_disp < 440)&&(y_disp > 0) && (y_disp < 40))
        {
          StepBack = 1;
          Menu_Display();
        }
      }
    }
  }
}

/**
  * @brief  Test XSPI Read/Write in STR Mode.
  * @param  None
  * @retval None
  */
static void XSPI_STR_Mode(void)
{
  uint32_t Offset;
  uint32_t block_size = MX25LM51245G_SECTOR_64K;
  uint32_t j = 0, temp_value = 1*block_size;
  static uint32_t i = 0;
  uint32_t error_code = 0;
  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK); 
  UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"STR Mode", CENTER_MODE);
  
  if(BSP_XSPI_NOR_DeInit(0) != BSP_ERROR_NONE) Error_Handler();
  
  /* XSPI device configuration */
  XSPI_Nor_Ctx[0].IsInitialized = XSPI_ACCESS_NONE;
  Flash.InterfaceMode = InterfaceMode[j];
  Flash.TransferRate  = BSP_XSPI_NOR_STR_TRANSFER;
  if(BSP_XSPI_NOR_Init(0, &Flash) != BSP_ERROR_NONE) Error_Handler();
  
  /* Erase chip prior to program/read operations *************************************/ 
  if(BSP_XSPI_NOR_Erase_Block(0, OPI_START_ADDRESS, BSP_XSPI_NOR_ERASE_CHIP) != BSP_ERROR_NONE) Error_Handler();
  
  for(j = 0; j < 2; j++)
  {
    if(BSP_XSPI_NOR_ConfigFlash(0, InterfaceMode[j], Flash.TransferRate) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Config flash: FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    
    for(i = 0; i < temp_value/block_size; i++)
    {
      if(BSP_XSPI_NOR_Erase_Block(0, OPI_START_ADDRESS + i*block_size, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
        UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Erase flash: FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"Test Aborted", LEFT_MODE);
        Error_Handler();
      }
      while(BSP_XSPI_NOR_GetStatus(0) == BSP_ERROR_BUSY);
    }
    
    for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
    {
      if(BSP_XSPI_NOR_Write(0, qspi_aTxBuffer, OPI_START_ADDRESS + i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
        UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"WRITE : FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"Test Aborted", LEFT_MODE);
        Error_Handler();
      }
      
      if(BSP_XSPI_NOR_Read(0, qspi_aRxBuffer, OPI_START_ADDRESS + i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
        UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"READ : FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"Test Aborted", LEFT_MODE);
        Error_Handler();
      }
      
      if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
      {
        error_code++;
      }
    }
    if(error_code != 0)
    {
      break;
    }
    /* Clear the buffer for the next test */
    memset(qspi_aRxBuffer, 0, sizeof(qspi_aRxBuffer));
  }
  
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"STR Test: Success", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"STR Test: Failed", LEFT_MODE);
    error_code = 0;
    Error_Handler();
  }
}

/**
  * @brief  Test XSPI Read/Write in STR memory Mapped Mode.
  * @param  None
  * @retval None
  */
static void XSPI_STR_MM_Mode(void)
{
  uint32_t Offset;
  uint32_t block_size = MX25LM51245G_SECTOR_64K;
  uint32_t j = 0, temp_value = 4*block_size;
  static uint32_t i = 0;
  uint32_t error_code = 0;
  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"STR MM Mode", CENTER_MODE);  
  if(BSP_XSPI_NOR_DeInit(0) != BSP_ERROR_NONE) Error_Handler();
  
  /* XSPI device configuration */
  Flash.InterfaceMode = InterfaceMode[j];
  Flash.TransferRate  = BSP_XSPI_NOR_STR_TRANSFER;
  if(BSP_XSPI_NOR_Init(0, &Flash) != BSP_ERROR_NONE) Error_Handler();
  
  for(j = 0; j < 2; j++)
  {
    if(BSP_XSPI_NOR_ConfigFlash(0, InterfaceMode[j], Flash.TransferRate) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"Config flash: FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    
    if(BSP_XSPI_NOR_GetStatus(0) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"Get Status: FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    
    for(i = 0; i < temp_value/block_size; i++)
    {
      /* Erase sectors prior to program/read operations *************************************/
      if(BSP_XSPI_NOR_Erase_Block(0, OPI_START_ADDRESS + i*block_size, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
        UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"Erase flash: FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"Test Aborted", LEFT_MODE);
        Error_Handler();
      }
      while(BSP_XSPI_NOR_GetStatus(0) == BSP_ERROR_BUSY);
    }
    
    for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
    {
      if(BSP_XSPI_NOR_Write(0, qspi_aTxBuffer, OPI_START_ADDRESS + i*BUFFER_SIZE, BUFFER_SIZE) != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
        UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"WRITE : FAILED", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"Test Aborted", LEFT_MODE);
        Error_Handler();
        break;
      }
    }
    
    if(BSP_XSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE) Error_Handler();
    
    XSPI_MMP_pointer = (uint8_t*)(0x90000000 + OPI_START_ADDRESS);
    for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
    {
      for(int k = 0; k < BUFFER_SIZE; k++)
      {
        qspi_aRxBuffer[k] = *(XSPI_MMP_pointer++);
      }
      if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
      {
        error_code++;
      }
    }
    
    if(BSP_XSPI_NOR_DisableMemoryMappedMode(0) != BSP_ERROR_NONE) Error_Handler();
    
    if(error_code != 0)
    {
      break;
    }
    /* Clear the buffer for the next test */
    memset(qspi_aRxBuffer, 0, sizeof(qspi_aRxBuffer));
  }
  
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"STR MM Test: Success", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"STR MM Test: Failed", LEFT_MODE);
    error_code = 0;
    Error_Handler();
  }
}

/**
  * @brief  Test XSPI Read/Write in DTR Mode.
  * @param  None
  * @retval None
  */
static void XSPI_DTR_Mode(void)
{  
  uint32_t Offset;
  uint32_t block_size = 65536;
  uint32_t i = 0, temp_value = 4*block_size;
  uint32_t error_code = 0;
  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);  
  UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"DTR Mode", CENTER_MODE);  
  if(BSP_XSPI_NOR_DeInit(0) != BSP_ERROR_NONE) Error_Handler();
  
  /* XSPI device configuration */
  Flash.InterfaceMode = BSP_XSPI_NOR_OPI_MODE;
  Flash.TransferRate  = BSP_XSPI_NOR_DTR_TRANSFER;
  if(BSP_XSPI_NOR_Init(0, &Flash) != BSP_ERROR_NONE) Error_Handler();
  
  for(i = 0; i < temp_value/block_size; i++)
  {
    /* Erase sectors prior to program/read operations *************************************/ 
    if(BSP_XSPI_NOR_Erase_Block(0, i*block_size + OPI_START_ADDRESS, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"Erase flash: FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    while(BSP_XSPI_NOR_GetStatus(0) == BSP_ERROR_BUSY);
  }
  
  for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
  {
    if(BSP_XSPI_NOR_Write(0, qspi_aTxBuffer, i*BUFFER_SIZE + OPI_START_ADDRESS, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"WRITE : FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
      break;
    }
    if(BSP_XSPI_NOR_Read(0, qspi_aRxBuffer, i*BUFFER_SIZE + OPI_START_ADDRESS, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"READ : FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
    {
      error_code++;
    }
  }
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"DTR Test: Success", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"DTR Test: Failed", LEFT_MODE);
    error_code = 0;
    Error_Handler();
  }
  
  /* Clear the buffer for the next test */
  memset(qspi_aRxBuffer, 0, sizeof(qspi_aRxBuffer));
}

/**
  * @brief  Test XSPI Read/Write in DTR memory Mapped Mode.
  * @param  None
  * @retval None
  */
static void XSPI_DTR_MM_Mode(void)
{  
  uint32_t Offset;
  uint32_t block_size = 65536;
  uint32_t i = 0, temp_value = 4*block_size;
  uint32_t error_code = 0;
  
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);  
  UTIL_LCD_DisplayStringAt(0, 160, (uint8_t *)"DTR MM Mode", CENTER_MODE);  
  if(BSP_XSPI_NOR_DeInit(0) != BSP_ERROR_NONE) Error_Handler();
  
  /* XSPI device configuration */
  Flash.InterfaceMode = BSP_XSPI_NOR_OPI_MODE;
  Flash.TransferRate  = BSP_XSPI_NOR_DTR_TRANSFER;
  if(BSP_XSPI_NOR_Init(0, &Flash) != BSP_ERROR_NONE) Error_Handler();
 
  for(i = 0; i < temp_value/block_size; i++)
  {
    /* Erase sectors prior to program/read operations *************************************/
    if(BSP_XSPI_NOR_Erase_Block(0, i*block_size + OPI_START_ADDRESS, MX25LM51245G_ERASE_64K) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 180, (uint8_t *)"Erase flash: FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 200, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
    }
    while(BSP_XSPI_NOR_GetStatus(0) == BSP_ERROR_BUSY);
  }
  
  for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
  {
    if(BSP_XSPI_NOR_Write(0, qspi_aTxBuffer, i*BUFFER_SIZE + OPI_START_ADDRESS, BUFFER_SIZE) != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);  
      UTIL_LCD_DisplayStringAt(0, 180, (uint8_t *)"WRITE : FAILED", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 200, (uint8_t *)"Test Aborted", LEFT_MODE);
      Error_Handler();
      break;
    }
  }
  if(BSP_XSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE) Error_Handler();
  
  XSPI_MMP_pointer = (uint8_t*)(0x90000000 + OPI_START_ADDRESS);
  for(i = 0; i < (temp_value/BUFFER_SIZE); i++)
  {
    for(int k = 0; k < BUFFER_SIZE; k++)
    {
      qspi_aRxBuffer[k] = *(XSPI_MMP_pointer++);
    }
    if(RMABuffercmp(qspi_aRxBuffer, qspi_aTxBuffer, (uint32_t)BUFFER_SIZE, &Offset) != 0)
    {
      error_code++;
    }
  }
  
  if(BSP_XSPI_NOR_DisableMemoryMappedMode(0) != BSP_ERROR_NONE) Error_Handler();
  
  if(error_code == 0)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, 180, (uint8_t *)"DTR MM Test: Success", LEFT_MODE);
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 180, (uint8_t *)"DTR MM Test: Failed", LEFT_MODE);
    error_code = 0;
    Error_Handler();
  }
  
  /* Clear the buffer for the next test */
  memset(qspi_aRxBuffer, 0, sizeof(qspi_aRxBuffer));
}

/**
* @brief  Fills buffer with user predefined data.
* @param  pBuffer: pointer on the buffer to fill
* @param  uwBufferLenght: size of the buffer to fill
* @param  uwOffset: first value to fill on the buffer
* @retval None
*/
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
*         ErrorAd: Difference address
* @retval 0: pBuffer identical to pBuffer1
*         1: pBuffer differs from pBuffer1
*/
static uint8_t RMABuffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength, uint32_t *ErrorAd)
{
  (*ErrorAd) = 0;

  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    (*ErrorAd)++;
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
* @}
*/

/**
* @}
*/
