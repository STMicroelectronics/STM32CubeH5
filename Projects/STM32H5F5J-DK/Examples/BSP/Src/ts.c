/**
  ******************************************************************************
  * @file    Examples/BSP/Src/ts.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the TS feature in the
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t x_new_pos = 0, x_previous_pos = 0;
uint16_t y_new_pos = 0, y_previous_pos = 40;
extern uint32_t  colors[12][4];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Test TS features
  * @param  None
  * @retval None
  */
void Test_Ts(void)
{
  uint32_t  i, k;
  
  StepBack = 0;
  
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);  
  UTIL_LCD_FillRect(0, 0, 480, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test TS  ", CENTER_MODE);
  BSP_LCD_FillRGBRect(0, 400, 0, (uint8_t *)GoBack, 40, 40);
  
  for(k = 0; k < 12; k++)
  {
    for(i = 0; i < 4; i++)
    {
      UTIL_LCD_DrawRect(40*k, 40 + 50*i,40,50, UTIL_LCD_COLOR_BLACK);
    }
  }  
  
  while (StepBack != 1)
  { 
    if(TouchPressed == 1)
    {
      TouchPressed = 0;
      /* Get X and Y position of the first touch post calibrated */
      x_new_pos = x_disp;
      y_new_pos = y_disp;
      
      for(k = 0; k < 12; k++)
      {
        for(i = 0; i < 4; i++)
        {
          if(((x_new_pos > 40*k) && (x_new_pos < 40*(k+1))) && ((y_new_pos > 40 + 50*i) && (y_new_pos < 40 + 50*(i+1))))
          {
            UTIL_LCD_FillRect(x_previous_pos, y_previous_pos,40,50, UTIL_LCD_COLOR_WHITE);
            UTIL_LCD_DrawRect(x_previous_pos, y_previous_pos,40,50, UTIL_LCD_COLOR_BLACK);
            
            UTIL_LCD_FillRect(40*k, 40 + 50*i,40,50, colors[k][i]);
            
            x_previous_pos = 40*k;
            y_previous_pos = 40 + 50*i;
            break;
          }
        }
      }      
      /* Quit the Test */
      if ((x_disp > 400) && (x_disp < 440)&&(y_disp > 0) && (y_disp < 40))
      {
        StepBack = 1;
        Menu_Display();
      }
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */
