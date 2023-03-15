/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    HASH/HASH_SHA384/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to use the HASH peripheral to 
  *          hash data using SHA_384 Algorithm with STM32H5xx.
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

HASH_HandleTypeDef hhash;

/* USER CODE BEGIN PV */

__ALIGN_BEGIN const uint8_t Input[INPUT_TAB_SIZE] __ALIGN_END = 
  {0x62, 0xc6, 0xa1, 0x69, 0xb9, 0xbe, 0x02, 0xb3, 0xd7, 0xb4, 0x71, 0xa9, 0x64, 0xfc, 0x0b, 0xcc, 0x72, 0xb4, 0x80,
   0xd2, 0x6a, 0xec, 0xb2, 0xed, 0x46, 0x0b, 0x7f, 0x50, 0x01, 0x6d, 0xda, 0xf0, 0x4c, 0x51, 0x21, 0x87, 0x83, 0xf3,
   0xaa, 0xdf, 0xdf, 0xf5, 0xa0, 0x4d, 0xed, 0x03, 0x0d, 0x7b, 0x3f, 0xb7, 0x37, 0x6b, 0x61, 0xba, 0x30, 0xb9, 0x0e,
   0x2d, 0xa9, 0x21, 0xa4, 0x47, 0x07, 0x40, 0xd6, 0x3f, 0xb9, 0x9f, 0xa1, 0x6c, 0xc8, 0xed, 0x81, 0xab, 0xaf, 0x8c,
   0xe4, 0x01, 0x6e, 0x50, 0xdf, 0x81, 0xda, 0x83, 0x20, 0x70, 0x37, 0x2c, 0x24, 0xa8, 0x08, 0x90, 0xaa, 0x3a, 0x26,
   0xfa, 0x67, 0x57, 0x10, 0xb8, 0xfb, 0x71, 0x82, 0x66, 0x24, 0x9d, 0x49, 0x6f, 0x31, 0x3c, 0x55, 0xd0, 0xba, 0xda,
   0x10, 0x1f, 0x8f, 0x56, 0xee, 0xcc, 0xee, 0x43, 0x45, 0xa8, 0xf9, 0x8f, 0x60, 0xa3, 0x66, 0x62, 0xcf, 0xda, 0x79,
   0x49, 0x00, 0xd1, 0x2f, 0x94, 0x14, 0xfc, 0xbd, 0xfd, 0xeb, 0x85, 0x38, 0x8a, 0x81, 0x49, 0x96, 0xb4, 0x7e, 0x24,
   0xd5, 0xc8, 0x08, 0x6e, 0x7a, 0x8e, 0xdc, 0xc5, 0x3d, 0x29, 0x9d, 0x0d, 0x03, 0x3e, 0x6b, 0xb6, 0x0c, 0x58, 0xb8,
   0x3d, 0x6e, 0x8b, 0x57, 0xf6, 0xc2, 0x58, 0xd6, 0x08, 0x1d, 0xd1, 0x0e, 0xb9, 0x42, 0xfd, 0xf8, 0xec, 0x15, 0x7e,
   0xc3, 0xe7, 0x53, 0x71, 0x23, 0x5a, 0x81, 0x96, 0xeb, 0x9d, 0x22, 0xb1, 0xde, 0x3a, 0x2d, 0x30, 0xc2, 0xab, 0xbe,
   0x0d, 0xb7, 0x65, 0x0c, 0xf6, 0xc7, 0x15, 0x9b, 0xac, 0xbe, 0x29, 0xb3, 0xa9, 0x3c, 0x92, 0x10, 0x05, 0x08};

__ALIGN_BEGIN static uint8_t aDigest[48] __ALIGN_END;
__ALIGN_BEGIN static uint8_t aExpectedDigest[48] __ALIGN_END = 
                                                         {0x07, 0x30, 0xe1, 0x84, 0xe7, 0x79, 0x55, 0x75, 0x56, 0x9f,
                                                          0x87, 0x03, 0x02, 0x60, 0xbb, 0x8e, 0x54, 0x49, 0x8e, 0x0e,
                                                          0x5d, 0x09, 0x6b, 0x18, 0x28, 0x5e, 0x98, 0x8d, 0x24, 0x5b,
                                                          0x6f, 0x34, 0x86, 0xd1, 0xf2, 0x44, 0x7d, 0x5f, 0x85, 0xbc,
                                                          0xbe, 0x59, 0xd5, 0x68, 0x9f, 0xc4, 0x94, 0x25};
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

  /* Compute SHA384 */
  if (HAL_HASH_Start(&hhash, (uint8_t *)Input, INPUT_TAB_SIZE, aDigest, 0xFF) != HAL_OK)
  {
    Error_Handler();
  }
   /* Check the output buffer containing the computing digest with the expected buffer */
  if(memcmp(aDigest, aExpectedDigest ,48) != 0)
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
  hhash.Init.Algorithm = HASH_ALGOSELECTION_SHA384;
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

  while (1)
  {
    /* Toggle LED3 with a period of 200 ms */ 
    BSP_LED_Toggle(LED3);
    HAL_Delay(200);
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
