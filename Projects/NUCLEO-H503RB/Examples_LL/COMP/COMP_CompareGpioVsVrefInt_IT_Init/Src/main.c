/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/COMP/COMP_CompareGpioVsVrefInt_IT_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   How to use a comparator peripheral to compare a voltage level 
  *          on a GPIO pin versus the internal voltage reference (VrefInt),
  *          in interrupt mode.
  *          This example is based on the STM32H5xx COMP LL API;
  *          Peripheral initialization done using LL unitary services functions.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t comparator_trig_event = 0U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_COMP1_Init(void);
/* USER CODE BEGIN PFP */

void Activate_COMP(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);

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

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_COMP1_Init();
  /* USER CODE BEGIN 2 */

  /* Activate comparator */
  Activate_COMP();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Note: LED state indicates comparator output state and is set into      */
    /*       comparator IRQ handler, refer to function                        */
    /*       "ComparatorTrigger_Callback()".                                  */

    if (comparator_trig_event != 0U)
    {
      comparator_trig_event = 0U;

      /* Rearm comparator interruption */
      Activate_COMP();
    }
      
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
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL1_SetSource(LL_RCC_PLL1SOURCE_HSE);
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_8_16);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(3);
  LL_RCC_PLL1_SetN(62);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1FRACN_Enable();
  LL_RCC_PLL1_SetFRACN(4096);
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
  * @brief COMP1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_COMP1_Init(void)
{

  /* USER CODE BEGIN COMP1_Init 0 */

  /* USER CODE END COMP1_Init 0 */

  LL_COMP_InitTypeDef COMP_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_COMP);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**COMP1 GPIO Configuration
  PA0   ------> COMP1_INP
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN COMP1_Init 1 */
  /* COMP1 interrupt Init */
  NVIC_SetPriority(COMP1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(COMP1_IRQn);
  /* USER CODE END COMP1_Init 1 */
  COMP_InitStruct.PowerMode = LL_COMP_POWERMODE_HIGHSPEED;
  COMP_InitStruct.InputPlus = LL_COMP_INPUT_PLUS_IO2;
  COMP_InitStruct.InputMinus = LL_COMP_INPUT_MINUS_VREFINT;
  COMP_InitStruct.InputHysteresis = LL_COMP_HYSTERESIS_MEDIUM;
  COMP_InitStruct.OutputPolarity = LL_COMP_OUTPUTPOL_NONINVERTED;
  COMP_InitStruct.OutputBlankingSource = LL_COMP_BLANKINGSRC_NONE;
  LL_COMP_Init(COMP1, &COMP_InitStruct);

  /* Wait loop initialization and execution */
  /* Note: Variable divided by 2 to compensate partially CPU processing cycles */
  __IO uint32_t wait_loop_index = 0;
  wait_loop_index = (LL_COMP_DELAY_VOLTAGE_SCALER_STAB_US * (SystemCoreClock / (1000000 * 2)));
  while(wait_loop_index != 0)
  {
    wait_loop_index--;
  }
  LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_29);
  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_29);
  /* USER CODE BEGIN COMP1_Init 2 */

  /* USER CODE END COMP1_Init 2 */

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
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Perform comparator activation procedure
  *         (COMP instance: COMP1).
  * @param  None
  * @retval None
  */
void Activate_COMP(void)
{
  __IO uint32_t wait_loop_index = 0U;
  uint32_t polarity_toggle = 0U;

  if(LL_COMP_IsEnabled(COMP1) == 0U)
  {
    /* Enable comparator */
    LL_COMP_Enable(COMP1);

    /* Delay for comparator startup time.                                       */
    /* Compute number of CPU cycles to wait for, from delay in us.              */
    /* Note: Variable divided by 2 to compensate partially                      */
    /*       CPU processing cycles (depends on compilation optimization).       */
    /* Note: If system clock frequency is below 200kHz, wait time will          */
    /*       correspond to the few CPU processing cycles to process this        */
    /*       calculation.                                                       */
    wait_loop_index = ((LL_COMP_DELAY_STARTUP_US * (SystemCoreClock / (100000U * 2U))) / 10U);
    while(wait_loop_index != 0U)
    {
      wait_loop_index--;
    }
  }

  /* Set comparator output initial state to low level */
  /* Note: Specific to comparator of this STM32 series: comparator output triggers interruption on high level.
           This implies that comparator output initial state must at low level. Then, comparator can trig signal 
           on rising edge.
           Trigger a signal on falling edge is possible by inverting comparator polarity. */
  if (LL_COMP_ReadOutputLevel(COMP1) != LL_COMP_OUTPUT_LEVEL_LOW)
  {
    polarity_toggle = 1U;
  }

  if (polarity_toggle == 1U)
  {
    /* Toggle poarity */
    if (LL_COMP_GetOutputPolarity(COMP1) == LL_COMP_OUTPUTPOL_INVERTED)
    {
      LL_COMP_SetOutputPolarity(COMP1, LL_COMP_OUTPUTPOL_NONINVERTED);
    }
    else
    {
      LL_COMP_SetOutputPolarity(COMP1, LL_COMP_OUTPUTPOL_INVERTED);
    }
  }

  /* Clear comparator interruption flag */
  LL_COMP_ClearFlag_OutputTrig(COMP1);
  NVIC_ClearPendingIRQ(COMP1_IRQn);

  /* Enable comparator interruption */
  LL_COMP_EnableIT_OutputTrig(COMP1);
}

/**
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED on */
  LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Turn-off LED2.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Toggle LED2.
  * @param  None
  * @retval None
  */
void LED_Toggle(void)
{
  /* Turn LED off */
  LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}

/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

/**
  * @brief  Comparator interruption callback
  * @note   This function is called when comparator threshold is triggered.
  * @retval None
  */
void ComparatorTrigger_Callback()
{
  uint32_t output_polarity = LL_COMP_GetOutputPolarity(COMP1);

  /* Set LED state in function of comparator output level */
  if ((LL_COMP_ReadOutputLevel(COMP1) == LL_COMP_OUTPUT_LEVEL_HIGH)
      ^ (output_polarity != LL_COMP_OUTPUTPOL_NONINVERTED))
  {
    LED_On();
  }
  else
  {
    LED_Off();
  }

  comparator_trig_event = 1U;
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

  /* Turn on LED and remain in infinite loop */
  while (1)
  {
    LED_On();
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
