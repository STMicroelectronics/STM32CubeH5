/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
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

TIM_HandleTypeDef htim16;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* USER CODE BEGIN PV */
uint32_t aDST_Buffer[BUFFER_SIZE] __attribute__((section("noncacheable_buffer")));

/* DMA transfer result flags */
__IO uint32_t TransferCompleteDetected = 0U;
__IO uint32_t TransferErrorDetected = 0U;

uint32_t uwMinFrequency = 0;
uint32_t uwMaxFrequency = 0;
uint32_t uwAvgFrequency = 0;
uint32_t TimerClock = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
void ClockConfig(void);
uint32_t GetTimerCLKFreq(void);
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
  uint32_t cnt_diff = 0;
  uint32_t cnt_min = 0;
  uint32_t cnt_max = 0;
  uint32_t cnt_avg = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* LSI clock and MCO2 configuration */
  ClockConfig();

  /* Initialize LD1 and LD2 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */
  /* Clear the DMA buffer */
  memset(aDST_Buffer, 0, sizeof(aDST_Buffer));

  /* Start the DMA transfer and the Timer 15 */
  HAL_TIM_IC_Start_DMA(&htim16, TIM_CHANNEL_1, aDST_Buffer, BUFFER_SIZE*4);

  /* Wait for end of transmission or an error occurred */
  while ((TransferCompleteDetected == 0) && (TransferErrorDetected == 0U));

  if (TransferCompleteDetected == 1)
  {
    /* Get the TIM16 clock frequency */
    TimerClock = GetTimerCLKFreq();

    /* Calculation of max, min & avg period values of the input signal with the buffer */
    for (uint8_t i = 0 ; i<BUFFER_SIZE-1 ; i++)
    {
      /* In case of the next value is smaller than the current one,
         it means that the counter restarted after reaching the Autoreload value.
         So the difference is made with the ARR value */
      cnt_diff = aDST_Buffer[i+1] > aDST_Buffer[i] ? \
                 aDST_Buffer[i+1] - aDST_Buffer[i] : \
                 ((uint32_t)__HAL_TIM_GET_AUTORELOAD(&htim16) - aDST_Buffer[i]) + aDST_Buffer[i+1] + 1;

      /* Load the first value as it is the maximum, otherwise take the value if it is higher */
      cnt_max = cnt_diff > cnt_max || i == 0 ? cnt_diff : cnt_max;

      /* Load the first value as it is the minimum, otherwise take the value if it is lower */
      cnt_min = cnt_diff < cnt_min || i == 0 ? cnt_diff : cnt_min;

      /* Add the differences for average calculation */
      cnt_avg += cnt_diff;
    }
    /* Average calculation */
    cnt_avg = cnt_avg / (BUFFER_SIZE-1);

    /* Reshaping the values in frequency from Timer clock */
    uwMaxFrequency = TimerClock / cnt_min;
    uwMinFrequency = TimerClock / cnt_max;
    uwAvgFrequency = TimerClock / cnt_avg;

    /* Check if the average value is close to the LSI theoretical value */
    if ((uwAvgFrequency > (LSI_VALUE * MIN_TOLERANCE)) && (uwAvgFrequency < (LSI_VALUE * MAX_TOLERANCE)))
    {
      /* Green Led, right frequency result value */
      BSP_LED_On(LED1);
    }
    else
    {
      /* Red Led, wrong frequency result value */
      BSP_LED_On(LED2);
    }
    TransferCompleteDetected = 0U;
  }
  else
  {
    /* Red Led, transfer error */
    BSP_LED_On(LED2);

    TransferErrorDetected = 0U;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 24;
  RCC_OscInitStruct.PLL.PLLN = 250;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

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
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim16, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIMEx_TISelection(&htim16, TIM_TIM16_TI1_LSI, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief Configure the LSI clock to redirect to MCO2
  * @param None
  * @retval None
  */
void ClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Activate the LSI */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* LSI clock redirected to MCO2 */
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_LSI, RCC_MCODIV_1);
}

/**
  * @brief Compute the TIM16 clock frequency.
  * @param None
  * @retval Timer clock frequency
  */
uint32_t GetTimerCLKFreq(void)
{
  uint32_t timer_clock_freq = 0;                      /* Timer clock frequency result */
  RCC_ClkInitTypeDef clock_config = {0};              /* Temporary variable to retrieve RCC clock configuration */
  uint32_t latency;                                   /* Temporary variable to retrieve Flash Latency */
  RCC_PeriphCLKInitTypeDef periph_clock_config = {0}; /* Temporary variable to retrieve RCC configuration information for Extended peripherals clocks */

  /* Retrieve RCC clock configuration */
  HAL_RCC_GetClockConfig(&clock_config, &latency);
  /* Retrieve RCC configuration information */
  HAL_RCCEx_GetPeriphCLKConfig(&periph_clock_config);
  UNUSED(latency); /* To avoid compilation warning */

  if (clock_config.APB2CLKDivider < RCC_HCLK_DIV2)
  {
    /* If APBx prescaler (PPREx) is lower than RCC_HCLK_DIV2, then TIMclock is equal to APBx clock */
    timer_clock_freq = HAL_RCC_GetPCLK2Freq();
  }
  else if ((clock_config.APB2CLKDivider >= RCC_HCLK_DIV2))
  {
    /* If APBx prescaler (PPREx) is higher or equal than RCC_HCLK_DIV2 and Timers clocks selection (TIMCS)
       is equal to 0, then TIMclock is twice APBx clock */
    timer_clock_freq = HAL_RCC_GetPCLK2Freq() * 2;
  }
  else
  {
    /* If APBx prescaler (PPREx) is higher or equal than RCC_HCLK_DIV2 and Timers clocks selection (TIMCS) 
       is equal to 1, then TIMclock is 4 times the APBx clock */
    timer_clock_freq = HAL_RCC_GetPCLK2Freq() * 4;
  }

  return timer_clock_freq;
}

/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  TransferCompleteDetected = 1U;
}

/**
  * @brief  Timer error callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim)
{
  TransferErrorDetected = 1U;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  /* Error if LD2 is slowly blinking (1 sec. period) */
  while(1)
  {
    BSP_LED_Toggle(LED2);
    HAL_Delay(1000);
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
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
