/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HASH/HASH_SHA512/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use the STM32H5xx HASH HAL API
  *          to get a HASH 512 digest value of a given buffer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
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

HASH_HandleTypeDef hhash;

/* USER CODE BEGIN PV */

__ALIGN_BEGIN const uint8_t aInput[INPUT_TAB_SIZE] __ALIGN_END = 
								{0x4f, 0x05, 0x60, 0x09, 0x50, 0x66, 0x4d, 0x51, 0x90, 0xa2,
                                 0xeb, 0xc2, 0x9c, 0x9e, 0xdb, 0x89, 0xc2, 0x00, 0x79, 0xa4,
                                 0xd3, 0xe6, 0xbc, 0x3b, 0x27, 0xd7, 0x5e, 0x34, 0xe2, 0xfa,
                                 0x3d, 0x02, 0x76, 0x85, 0x02, 0xbd, 0x69, 0x79, 0x00, 0x78,
                                 0x59, 0x8d, 0x5f, 0xcf, 0x3d, 0x67, 0x79, 0xbf, 0xed, 0x12,
                                 0x84, 0xbb, 0xe5, 0xad, 0x72, 0xfb, 0x45, 0x60, 0x15, 0x18,
                                 0x1d, 0x95, 0x87, 0xd6, 0xe8, 0x64, 0xc9, 0x40, 0x56, 0x4e,
                                 0xaa, 0xfb, 0x4f, 0x2f, 0xea, 0xd4, 0x34, 0x6e, 0xa0, 0x9b,
                                 0x68, 0x77, 0xd9, 0x34, 0x0f, 0x6b, 0x82, 0xeb, 0x15, 0x15,
                                 0x88, 0x08, 0x72, 0x21, 0x3d, 0xa3, 0xad, 0x88, 0xfe, 0xba,
                                 0x9f, 0x4f, 0x13, 0x81, 0x7a, 0x71, 0xd6, 0xf9, 0x0a, 0x1a,
                                 0x17, 0xc4, 0x3a, 0x15, 0xc0, 0x38, 0xd9, 0x88, 0xb5, 0xb2,
                                 0x9e, 0xdf, 0xfe, 0x2d, 0x6a, 0x06, 0x28, 0x13, 0xce, 0xdb,
                                 0xe8, 0x52, 0xcd, 0xe3, 0x02, 0xb3, 0xe3, 0x3b, 0x69, 0x68,
                                 0x46, 0xd2, 0xa8, 0xe3, 0x6b, 0xd6, 0x80, 0xef, 0xcc, 0x6c,
                                 0xd3, 0xf9, 0xe9, 0xa4, 0xc1, 0xae, 0x8c, 0xac, 0x10, 0xcc,
                                 0x52, 0x44, 0xd1, 0x31, 0x67, 0x71, 0x40, 0x39, 0x91, 0x76,
                                 0xed, 0x46, 0x70, 0x00, 0x19, 0xa0, 0x04, 0xa1, 0x63, 0x80,
                                 0x6f, 0x7f, 0xa4, 0x67, 0xfc, 0x4e, 0x17, 0xb4, 0x61, 0x7b,
                                 0xbd, 0x76, 0x41, 0xaa, 0xff, 0x7f, 0xf5, 0x63, 0x96, 0xba,
                                 0x8c, 0x08, 0xa8, 0xbe, 0x10, 0x0b, 0x33, 0xa2, 0x0b, 0x5d,
                                 0xaf, 0x13, 0x4a, 0x2a, 0xef, 0xa5, 0xe1, 0xc3, 0x49, 0x67,
                                 0x70, 0xdc, 0xf6, 0xba, 0xa4, 0xf7, 0xbb};
 
__ALIGN_BEGIN static uint8_t aSHA512Digest[64] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectSHA512Digest[64] __ALIGN_END = 
								{0xa9, 0xdb, 0x49, 0x0c, 0x70, 0x8c, 0xc7, 0x25, 0x48, 0xd7,
                                 0x86, 0x35, 0xaa, 0x7d, 0xa7, 0x9b, 0xb2, 0x53, 0xf9, 0x45,
                                 0xd7, 0x10, 0xe5, 0xcb, 0x67, 0x7a, 0x47, 0x4e, 0xfc, 0x7c,
                                 0x65, 0xa2, 0xaa, 0xb4, 0x5b, 0xc7, 0xca, 0x11, 0x13, 0xc8,
                                 0xce, 0x0f, 0x3c, 0x32, 0xe1, 0x39, 0x9d, 0xe9, 0xc4, 0x59,
                                 0x53, 0x5e, 0x88, 0x16, 0x52, 0x1a, 0xb7, 0x14, 0xb2, 0xa6,
                                 0xcd, 0x20, 0x05, 0x25};;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_HASH_Init(void);
static void MX_ICACHE_Init(void);
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_HASH_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED1, LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /* Compute SHA512 */
  if (HAL_HASH_Start(&hhash, (uint8_t *)aInput, INPUT_TAB_SIZE, aSHA512Digest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }
  /* Compare computed digest with expected one */
  if (memcmp(aSHA512Digest, aExpectSHA512Digest, sizeof(aExpectSHA512Digest) / sizeof(aExpectSHA512Digest[0])) != 0)
  {
    Error_Handler();
  }
  else
  {
    BSP_LED_On(LED1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
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
}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{

  /* USER CODE BEGIN HASH_Init 0 */

  /* USER CODE END HASH_Init 0 */

  /* USER CODE BEGIN HASH_Init 1 */

  /* USER CODE END HASH_Init 1 */
  hhash.Instance = HASH;
  hhash.Init.DataType = HASH_BYTE_SWAP;
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA512;
  if (HAL_HASH_Init(&hhash) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HASH_Init 2 */

  /* USER CODE END HASH_Init 2 */

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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
