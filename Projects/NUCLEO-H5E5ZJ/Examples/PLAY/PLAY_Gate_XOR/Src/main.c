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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

#define CONFIGURED_INPUT_NBR   1U
#define CONFIGURED_LUT_NBR     1U
#define CONFIGURED_OUTPUT_NBR  1U

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

HAL_PLAY_HandleTypeDef hplay1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_PLAY1_Init(void);
/* USER CODE BEGIN PFP */

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
  HAL_PLAY_EdgeTriggerConfTypeDef edge_trigger_config;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

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
  BSP_LED_Init(LED_RED);

  /* In this example, edge triggers are not used */
  edge_trigger_config.lut_out_falling_mask = 0U;
  edge_trigger_config.lut_out_rising_mask = 0U;

  /* Start PLAY module */
  if (HAL_PLAY_Start(&hplay1, &edge_trigger_config) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Toggle the second input with a software trigger */
    if (HAL_PLAY_ToggleSWTrigger(&hplay1, HAL_PLAY_SWTRIG0, 100U) != HAL_OK)
    {
      Error_Handler();
    }

    HAL_Delay(1000);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  * @brief PLAY1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_PLAY1_Init(void)
{

  /* USER CODE BEGIN PLAY1_Init 0 */

  /* USER CODE END PLAY1_Init 0 */

  HAL_PLAY_IN_ConfTypeDef configINPUTs[CONFIGURED_INPUT_NBR];
  HAL_PLAY_LUT_ConfTypeDef configLUTs[CONFIGURED_LUT_NBR];
  HAL_PLAY_OUT_ConfTypeDef configOUTPUTs[CONFIGURED_OUTPUT_NBR];

  /* USER CODE BEGIN PLAY1_Init 1 */

  /* USER CODE END PLAY1_Init 1 */
  hplay1.instance = PLAY1;

  if (HAL_PLAY_Init(&hplay1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Input 12
  */
  configINPUTs[0].min_pulse_width = 100;
  configINPUTs[0].mode = HAL_PLAY_EDGE_DETECTION_BYPASSED;
  configINPUTs[0].source = HAL_PLAY1_IN_IN12_MUX12;
  if (HAL_PLAY_INPUT_SetConfig(&hplay1, configINPUTs, CONFIGURED_INPUT_NBR) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Logic Element 0
  */
  configLUTs[0].lut = HAL_PLAY_LUT0;
  configLUTs[0].truth_table = 0x6666;
  configLUTs[0].input_source[HAL_PLAY_LUT_INPUT0] = HAL_PLAY_LUT_INPUT_FILTER12;
  configLUTs[0].input_source[HAL_PLAY_LUT_INPUT1] = HAL_PLAY_LUT_INPUT_SWTRIG0;
  configLUTs[0].input_source[HAL_PLAY_LUT_INPUT2] = HAL_PLAY_LUT_INPUT_DEFAULT;
  configLUTs[0].input_source[HAL_PLAY_LUT_INPUT3] = HAL_PLAY_LUT_INPUT_DEFAULT;
  configLUTs[0].clk_gate_source = HAL_PLAY_LUT_CLK_GATE_OFF;
  if (HAL_PLAY_LUT_SetConfig(&hplay1, configLUTs, CONFIGURED_LUT_NBR) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Output 0
  */
  configOUTPUTs[0].output_mux = HAL_PLAY_OUT0;
  configOUTPUTs[0].lut_output = HAL_PLAY_LUT0_OUT_DIRECT;
  if (HAL_PLAY_OUTPUT_SetConfig(&hplay1, configOUTPUTs, CONFIGURED_OUTPUT_NBR) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PLAY1_Init 2 */

  /* USER CODE END PLAY1_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
    BSP_LED_Toggle(LED_RED);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
