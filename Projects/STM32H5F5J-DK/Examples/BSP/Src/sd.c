/**
  ******************************************************************************
  * @file    Examples/BSP/Src/sd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the BSP SD Driver
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
#define BLOCK_START_ADDR         0     /* Block start address      */
#define NUM_OF_BLOCKS            5     /* Total number of blocks   */
#define BUFFER_WORDS_SIZE        ((BLOCKSIZE * NUM_OF_BLOCKS) >> 2) /* Total data size in bytes */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t aTxBuffer[BUFFER_WORDS_SIZE];
uint32_t aRxBuffer[BUFFER_WORDS_SIZE];
__IO uint32_t SDWriteStatus = 0, SDReadStatus = 0, SDDetectStatus = 0;

/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  SD Demo
  * @param  None
  * @retval None
  */
void Test_Sd(void)
{
  int32_t SD_state = BSP_ERROR_NONE;
  static uint8_t prev_status = 2;  /* Undefined state */
  
  StepBack = 0;
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);  
  UTIL_LCD_FillRect(0, 0, 480, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test SD  ", CENTER_MODE);
  BSP_LCD_FillRGBRect(0, 400, 0, (uint8_t *)GoBack, 40, 40);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  
  if(BSP_SD_Init(0) != BSP_ERROR_NONE)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"Is a SD card inserted ?", LEFT_MODE);
    UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Reset the board", LEFT_MODE);
    Error_Handler();
  }
  
  SD_state = BSP_SD_DetectITConfig(0);
  if(SD_state != BSP_ERROR_NONE) 
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED); 
    if (SD_state == BSP_ERROR_PERIPH_FAILURE)
    {     
      UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"SD shall be inserted", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"before running test", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"SD Test Aborted", LEFT_MODE);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"SD Initialization : FAIL", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"SD Test Aborted", LEFT_MODE);
    }
    Error_Handler();
  }
  else
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_DisplayStringAt(0, 40, (uint8_t *)"SD Initialization: OK", LEFT_MODE);
    
    UTIL_LCD_DisplayStringAt(0, 220, (uint8_t *)"     SD Connected", RIGHT_MODE);    
    SDDetectStatus = SD_PRESENT;
    prev_status = SD_PRESENT;
    
    SD_state = BSP_SD_Erase(0, BLOCK_START_ADDR, (BLOCKSIZE * NUM_OF_BLOCKS));
    
    /* Wait until SD card is ready to use for new operation */
    while (BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
    {
    }
    
    if(SD_state != BSP_ERROR_NONE)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
      UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"SD ERASE : FAIL", LEFT_MODE);
      UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"SD Test Aborted", CENTER_MODE);
      Error_Handler();
    }
    else
    {
      UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"SD ERASE : OK", LEFT_MODE);
      /* Fill the buffer to write */
      Fill_Buffer(aTxBuffer, BUFFER_WORDS_SIZE, 0x22FF);
      SD_state = BSP_SD_WriteBlocks_DMA(0, aTxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS);
      if(SD_state != BSP_ERROR_NONE) Error_Handler();
      
      /* Wait for the write process is completed */
      while((SDWriteStatus == 0))
      {
      }
      SDWriteStatus = 0;
      
      /* Wait until SD cards are ready to use for new operation */
      while((BSP_SD_GetCardState(0) != SD_TRANSFER_OK))
      {
      } 
      
      if (SD_state != BSP_ERROR_NONE)
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"SD WRITE : FAIL", LEFT_MODE);
        UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"SD Test Aborted", CENTER_MODE);
      }
      else
      {
        UTIL_LCD_DisplayStringAt(0, 80, (uint8_t *)"SD WRITE : OK", LEFT_MODE);
        SD_state = BSP_SD_ReadBlocks_DMA(0, aRxBuffer, BLOCK_START_ADDR, NUM_OF_BLOCKS);
        
        /* Wait for the read process is completed */
        while(SDReadStatus == 0)
        {
        }
        SDReadStatus = 0;
        
        /* Wait until SD card is ready to use for new operation */
        while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK)
        {
        }
        
        if (SD_state != BSP_ERROR_NONE)
        {
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
          UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"SD READ : FAIL", LEFT_MODE);
          UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"SD Test Aborted", CENTER_MODE);
        }
        else
        {
          UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"SD READ : OK", LEFT_MODE);
          if (Buffercmp(aTxBuffer, aRxBuffer, BUFFER_WORDS_SIZE) > 0)
          {
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
            UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"SD COMPARE : FAIL", LEFT_MODE);
            UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"SD Test Aborted", CENTER_MODE);
            Error_Handler();
          }
          else
          {
            UTIL_LCD_DisplayStringAt(0, 120, (uint8_t *)"SD COMPARE : OK", LEFT_MODE);
            UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ORANGE);
            UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)"SD can be removed", LEFT_MODE);
          }
        }
      }
    }
  }
  
  while (StepBack != 1)
  {   
    /* Check if the SD card is plugged in the slot */
    if (SDDetectStatus != SD_PRESENT)
    {
      if (prev_status != SD_NOT_PRESENT)
      {
        prev_status = SD_NOT_PRESENT;
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
        UTIL_LCD_DisplayStringAt(0, 220, (uint8_t *)"SD NOT Connected", RIGHT_MODE);
      }
    }
    else if (prev_status != SD_PRESENT)
    {
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
      UTIL_LCD_DisplayStringAt(0, 220, (uint8_t *)"     SD Connected", RIGHT_MODE);
      prev_status = SD_PRESENT;
    }
    
    if(TouchPressed > 0)
    {
      TouchPressed = 0;
      /* Quit the Test */
      if ((x_disp > 400) && (x_disp < 440)&&(y_disp > 0) && (y_disp < 40))
      {
        if(BSP_SD_DeInit(0) != BSP_ERROR_NONE) Error_Handler(); 
        StepBack = 1;
        Menu_Display();
      }
    }
  }
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
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
  * @retval 1: pBuffer identical to pBuffer1
  *         0: pBuffer differs from pBuffer1
  */
static uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief Tx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
void BSP_SD_WriteCpltCallback(uint32_t Instance)
{
  SDWriteStatus = 1;
}

/**
  * @brief Rx Transfer completed callback
  * @param  Instance     SD Instance
  * @retval None
  */
void BSP_SD_ReadCpltCallback(uint32_t Instance)
{
  SDReadStatus = 1;
}

/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None.
  */
void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status)
{
  SDDetectStatus = Status;
}

/**
  * @brief SDMMC error callback
  * @param None
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  Error_Handler();
}

/**
  * @}
  */

/**
  * @}
  */
