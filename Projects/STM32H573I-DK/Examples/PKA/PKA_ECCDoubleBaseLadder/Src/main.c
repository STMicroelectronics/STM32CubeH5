/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

PKA_HandleTypeDef hpka;

RNG_HandleTypeDef hrng;

/* USER CODE BEGIN PV */
/* vector 1 inputs and outputs*/
uint32_t input1_modulusSize                                      = 24;
uint32_t input1_orderSize                                        = 24;
uint32_t input1_coefSign                                         = 1;
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_MODULUS[24]          = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_A_COEFF[24]          = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_SCALAR_K[24]         = {0x25, 0x29, 0x7D, 0xD0, 0xD3, 0x4D, 0x26, 0xAB, 0x41, 0xC7, 0x18, 0xC5, 0x96, 0x4B, 0x41, 0xA7, 0xAD, 0x8C, 0xB6, 0x59, 0xA2, 0x84, 0xD5, 0x17};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_SCALAR_M[24]         = {0xDC, 0xA9, 0x91, 0x69, 0x87, 0x7B, 0x16, 0xAF, 0xD5, 0xC9, 0x53, 0xBE, 0x11, 0xA5, 0x13, 0xC3, 0x14, 0x0F, 0xDC, 0xBC, 0x96, 0x40, 0x1A, 0xD0};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_X[24] = {0xE7, 0xE5, 0x90, 0xDA, 0xC1, 0x2D, 0x4A, 0x2F, 0x21, 0x5E, 0x6E, 0x16, 0x3B, 0x52, 0x27, 0xFB, 0x48, 0x59, 0x81, 0xDF, 0x68, 0x02, 0xAA, 0xF7};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_Y[24] = {0x65, 0x18, 0xE1, 0xF1, 0x67, 0xB1, 0x7A, 0xA8, 0x90, 0x87, 0xD1, 0x53, 0x3B, 0xD1, 0x26, 0x76, 0xC8, 0x12, 0x4A, 0xE8, 0x91, 0x9C, 0x71, 0x45};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_Z[24] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_X[24] = {0xC8, 0xC1, 0x4C, 0x6E, 0x83, 0xDA, 0x2D, 0xAB, 0x99, 0xC4, 0xDB, 0xE7, 0xDB, 0x39, 0x5E, 0x54, 0x28, 0x55, 0x89, 0x45, 0x88, 0xBC, 0x74, 0x06};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_Y[24] = {0x2B, 0x35, 0x8F, 0x4B, 0xE4, 0xA1, 0xA9, 0xEB, 0x7F, 0x46, 0x21, 0xEE, 0x9F, 0xF9, 0xD7, 0xCB, 0xE1, 0xEB, 0x42, 0xCE, 0x87, 0xC6, 0xD9, 0xE0};
uint8_t input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_Z[24] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t output1_X[24]                                            = {0x19, 0x51, 0xE5, 0x16, 0x72, 0x1D, 0x98, 0xB6, 0x9E, 0xEC, 0x34, 0x0A, 0x2B, 0xA1, 0xC2, 0x08, 0x03, 0x1C, 0xB6, 0xC7, 0xD3, 0x81, 0xCD, 0xEA};
uint8_t output1_Y[24]                                            = {0x57, 0x1F, 0xA8, 0xFE, 0xCB, 0xA8, 0x5A, 0xD9, 0xBA, 0xFD, 0x47, 0xA6, 0x60, 0xA9, 0x5F, 0xF5, 0xE3, 0xCE, 0x22, 0x50, 0x42, 0xA9, 0x4D, 0x31};


uint8_t buffer_x[24];
uint8_t buffer_y[24];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_RNG_Init(void);
static void MX_PKA_Init(void);
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
  /* STM32H5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED2, LED1 */
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED1);

  /* Turn LED2, LED1 off */
  BSP_LED_Off(LED2);
  BSP_LED_Off(LED1);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_RNG_Init();
  MX_PKA_Init();
  /* USER CODE BEGIN 2 */

  PKA_ECCDoubleBaseLadderInTypeDef in;
  PKA_ECCDoubleBaseLadderOutTypeDef out;

 /* Set input parameters */
  in.modulusSize = input1_modulusSize;
  in.primeOrderSize = input1_orderSize;
  in.modulus = input1_1PKA_ECC_DoubleBaseLadder_IN_MODULUS;
  in.coefSign = input1_coefSign;
  in.coefA = input1_1PKA_ECC_DoubleBaseLadder_IN_A_COEFF;
  in.integerK = input1_1PKA_ECC_DoubleBaseLadder_IN_SCALAR_K;
  in.integerM = input1_1PKA_ECC_DoubleBaseLadder_IN_SCALAR_M;
  in.basePointX1 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_X;
  in.basePointY1 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_Y;
  in.basePointZ1 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT1_Z;
  in.basePointX2 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_X;
  in.basePointY2 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_Y;
  in.basePointZ2 = input1_1PKA_ECC_DoubleBaseLadder_IN_INITIAL_POINT2_Z;

  /* Set output parameters */
  out.ptX = &buffer_x[0];
  out.ptY = &buffer_y[0];

  /* Start PKA ECC Double Base Ladder operation */
  if (HAL_PKA_ECCDoubleBaseLadder(&hpka, &in, 5000) != HAL_OK)
  {
    /* HAL PKA Operation error */
    Error_Handler();
  }
  /* Retrieve computation result */
  HAL_PKA_ECCDoubleBaseLadder_GetResult(&hpka, &out);

  /* Check retrieved result with expected result */
  if ((memcmp((uint8_t*)out.ptX, (uint8_t*)output1_X, 24) != 0) ||
      (memcmp((uint8_t*)out.ptY, (uint8_t*)output1_Y, 24) != 0))
  {
    /* HAL PKA Operation error */
    Error_Handler();
  }

    /* PKA DeInit */
  HAL_PKA_DeInit(&hpka);

  /* RNG DeInit */
  HAL_RNG_DeInit(&hrng);

  /* Turn LED1 on */
    BSP_LED_On(LED1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
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
  * @brief PKA Initialization Function
  * @param None
  * @retval None
  */
static void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  hpka.Instance = PKA;
  if (HAL_PKA_Init(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PKA_Init 2 */

  /* USER CODE END PKA_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

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
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
