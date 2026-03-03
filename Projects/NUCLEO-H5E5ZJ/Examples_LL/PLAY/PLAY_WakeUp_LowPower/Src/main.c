/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  LED_BLUE,
  LED_RED,
  LED_NBR
} Led_TypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static GPIO_TypeDef *LED_PORT[LED_NBR] =
{
  LED_BLUE_GPIO_Port,
  LED_RED_GPIO_Port
};

static const uint16_t LED_PIN[LED_NBR] =
{
  LED_BLUE_Pin,
  LED_RED_Pin
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_PLAY1_Init(void);
/* USER CODE BEGIN PFP */
void LED_On(Led_TypeDef led);
void LED_Off(Led_TypeDef led);
void LED_Toggle(Led_TypeDef led);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_PLAY1_Init();
  /* USER CODE BEGIN 2 */

  /* Start PLAY module */
  LL_PLAY_Lock(PLAY1);

  LL_PWR_SetStopModeRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SVOS_SCALE3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* The system enters STOP mode to demonstrate that the alarm signal is fully handled by the PLAY hardware */
    
    /* Disable SysTick Interrupt */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;

    /** Enter STOP Mode with WaitForevent parameter, a PLAY EXTI event would wake the system up.
      * During STOP mode, the PLAY LUT output flags are not functional.
      * To wakeup the system, this example configures the PLAY EXTI output so that the falling edge of LUT 15
      * will trigger the EXTI event, waking up the system.
      */
    LL_PWR_SetPowerMode(LL_PWR_STOP_MODE);

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    LL_LPM_EnableDeepSleep();

    /* Wait For Event Request */
    __WFE();

    /**
      * The user can change the PLAY1_IN12 (PE9) input signal to trigger the alarm signal.
      * As the PLAY peripheral handles the alarm signal, the application does not need to do anything in this step.
      * When the user changes the input signal a second time, the PLAY peripheral will stop the alarm signal.
      * The alarm stop will trigger an EXTI event, which will wake the system.
      * The application will then turn on the LED to indicate that the alarm has been stopped, wait for one second,
      * and go back again in STOP mode.
      */

    /* Reach this point when the MCU wakes up */
    
    /* Enable SysTick Interrupt */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

    /* The PLAY event wakes the device up */
    SystemClock_Config();

    /* The alarm was stopped: rising edge on LUT 15 registered output. Turn on the LED */
    LED_On(LED_BLUE);

    /* Wait for one second so the LED status can be noticed.*/
    LL_mDelay(1000);
    LED_Off(LED_BLUE);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_HSI_SetDivider(LL_RCC_HSI_DIV_2);
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }

  LL_RCC_PLL1_SetSource(LL_RCC_PLL1SOURCE_HSI);
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(16);
  LL_RCC_PLL1_SetN(250);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  LL_Init1msTick(250000000);

  LL_SetSystemCoreClock(250000000);
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache (default 2-ways set associative cache)
  */
  LL_ICACHE_Enable();
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief PLAY1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_PLAY1_Init(void)
{

  /* USER CODE BEGIN PLAY1_Init 0 */
  LL_APB3_GRP1_EnableClockSleep(LL_APB3_GRP1_PERIPH_PLAY1);

  /* USER CODE END PLAY1_Init 0 */

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetPLAY1ClockSource(LL_RCC_PLAY1_CLKSOURCE_LSI);
  LL_RCC_SetPLAY1ClockDivider(LL_RCC_PLAY1_CLK_DIV_1);

  /* Peripheral clock enable */
  LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_PLAY1);
  LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_PLAY1APB);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
  /**PLAY1 GPIO Configuration
  PE9   ------> PLAY1_IN12
  PE14   ------> PLAY1_OUT14
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_14;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_14;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* PLAY1 interrupt Init */
  NVIC_SetPriority(PLAY1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(PLAY1_IRQn);

  /* USER CODE BEGIN PLAY1_Init 1 */

  /* USER CODE END PLAY1_Init 1 */

  /** Configure Input 12
  */
  LL_PLAY_INPUT_Config(PLAY1, LL_PLAY_IN12, 0, LL_PLAY_EDGE_DETECTION_MODE_BYPASSED, 100);

  /** LUT0 configuration
  */
  LL_PLAY_LUT_SetTruthTable(PLAY1, LL_PLAY_LUT0, 0xAAAA);
  LL_PLAY_LUT_ConfigInputs(PLAY1, LL_PLAY_LUT0, LL_PLAY_LUT_SOURCE_FILTER12, LL_PLAY_LUT_SOURCE_SWTRIG15, LL_PLAY_LUT_SOURCE_SWTRIG15, LL_PLAY_LUT_SOURCE_SWTRIG15, LL_PLAY_LUT_CLK_GATE_OFF);

  /** Configure Output 14
  */
  LL_PLAY_OUTPUT_SetSource(PLAY1, LL_PLAY_OUT14, LL_PLAY_LUT0_OUT_DIRECT);
  /* USER CODE BEGIN PLAY1_Init 2 */
  LL_EXTI_InitTypeDef exti_config;

  /* EXTI line 64 is connected to PLAY1_OUT14 */
  exti_config.Line_0_31 = 0;
  exti_config.Line_32_63 = 0;
  exti_config.Line_64_95 = LL_EXTI_LINE_64;
  exti_config.LineCommand = ENABLE;
  exti_config.Mode = LL_EXTI_MODE_EVENT;
  exti_config.Trigger = LL_EXTI_TRIGGER_FALLING;
  if (LL_EXTI_Init(&exti_config) != SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END PLAY1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);

  /**/
  LL_GPIO_SetOutputPin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);

  /**/
  LL_GPIO_SetOutputPin(LED_RED_GPIO_Port, LED_RED_Pin);

  /**/
  GPIO_InitStruct.Pin = LED_BLUE_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_BLUE_GPIO_Port, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Turn-on LED.
  * @param  None
  * @retval None
  */
void LED_On(Led_TypeDef led)
{
  /* Turn LED on */
  LL_GPIO_ResetOutputPin(LED_PORT[led], LED_PIN[led]);
}

/**
  * @brief  Turn-off LED.
  * @param  None
  * @retval None
  */
void LED_Off(Led_TypeDef led)
{
  /* Turn LED1 off */
  LL_GPIO_SetOutputPin(LED_PORT[led], LED_PIN[led]);
}

/**
  * @brief  Toggle LED.
  * @param  None
  * @retval None
  */
void LED_Toggle(Led_TypeDef led)
{
  /* Toggle LED on */
  LL_GPIO_TogglePin(LED_PORT[led], LED_PIN[led]);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    LED_Toggle(LED_RED);
    LL_mDelay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
