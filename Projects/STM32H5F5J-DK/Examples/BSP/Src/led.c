/**
  ******************************************************************************
  * @file    Examples/BSP/Src/led.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the led feature in the 
  *          STM32H5F5J-DK BSP driver
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
__IO uint32_t                Led1_State = 0;
__IO uint32_t                Led2_State = 0;
__IO uint32_t                Led3_State = 0;
__IO uint32_t                Led4_State = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Test Led
  * @param  None
  * @retval None
  */
void Test_Led(void)
{
  StepBack = 0;
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);  
  UTIL_LCD_FillRect(0, 0, 480, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);;
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)"  Test LED  ", CENTER_MODE);
  BSP_LCD_FillRGBRect(0, 400, 0, (uint8_t *)GoBack, 40, 40);
  
  /* Fill a magenta circle */
  UTIL_LCD_FillCircle(142, 120, 20, UTIL_LCD_COLOR_GRAY);
  UTIL_LCD_FillCircle(198, 120, 20, UTIL_LCD_COLOR_GRAY);
  UTIL_LCD_FillCircle(254, 120, 20, UTIL_LCD_COLOR_GRAY);
  UTIL_LCD_FillCircle(310, 120, 20, UTIL_LCD_COLOR_GRAY);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_ST_BLUE_DARK);   
  UTIL_LCD_DisplayStringAt(0, 200, (uint8_t *)"Touch to toggle LEDs", CENTER_MODE);
  
  /* Initialize LED1, LED2, LED3, LED4 */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_ORANGE);
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_BLUE);
  
  while (StepBack != 1)
  {
    if(TouchPressed > 0)
    {
      HAL_Delay(100);
      TouchPressed = 0;
      
      /* Switch LED1 */
      if ((x_disp > 142) && (x_disp < 162)&&(y_disp > 120) && (y_disp < 140))
      {
        if(Led1_State == 0)
        {
          BSP_LED_On(LED1);
          UTIL_LCD_FillCircle(142, 120, 20, UTIL_LCD_COLOR_RED);
          Led1_State = 1;
        }
        else
        {
          BSP_LED_Off(LED1);
          UTIL_LCD_FillCircle(142, 120, 20, UTIL_LCD_COLOR_GRAY);
          Led1_State = 0;
        }
      }
      /* Switch LED2 */
      if ((x_disp > 198) && (x_disp < 218)&&(y_disp > 120) && (y_disp < 140))
      {
        if(Led2_State == 0)
        {
          BSP_LED_On(LED2);
          UTIL_LCD_FillCircle(198, 120, 20, UTIL_LCD_COLOR_ORANGE);
          Led2_State = 1;
        }
        else
        {
          BSP_LED_Off(LED2);
          UTIL_LCD_FillCircle(198, 120, 20, UTIL_LCD_COLOR_GRAY);
          Led2_State = 0;
        }
      }
      /* Switch LED3 */
      if ((x_disp > 254) && (x_disp < 274)&&(y_disp > 120) && (y_disp < 140))
      {
        if(Led3_State == 0)
        {
          BSP_LED_On(LED3);
          UTIL_LCD_FillCircle(254, 120, 20, UTIL_LCD_COLOR_GREEN);
          Led3_State = 1;
        }
        else
        {
          BSP_LED_Off(LED3);
          UTIL_LCD_FillCircle(254, 120, 20, UTIL_LCD_COLOR_GRAY);
          Led3_State = 0;
        }
      }
      /* Switch LED4 */
      if ((x_disp > 310) && (x_disp < 330)&&(y_disp > 120) && (y_disp < 140))
      {
        if(Led4_State == 0)
        {
          BSP_LED_On(LED4);
          UTIL_LCD_FillCircle(310, 120, 20, UTIL_LCD_COLOR_BLUE);
          Led4_State = 1;
        }
        else
        {
          BSP_LED_Off(LED4);
          UTIL_LCD_FillCircle(310, 120, 20, UTIL_LCD_COLOR_GRAY);
          Led4_State = 0;
        }
      }
      /* Quit the Test */
      if ((x_disp > 400) && (x_disp < 440)&&(y_disp > 0) && (y_disp < 40))
      {
        BSP_LED_Off(LED1);
        BSP_LED_Off(LED2);
        BSP_LED_Off(LED3);
        BSP_LED_Off(LED4);
        
        Led1_State = 0;
        Led2_State = 0;
        Led3_State = 0;
        Led4_State = 0;
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
