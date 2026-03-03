/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CCB/CCB_Protected_ECCScalarMul_BlobUse/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use GPIOs through
  *          the STM32H5xx HAL API.
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

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CCB_HandleTypeDef hccb;

/* USER CODE BEGIN PV */
CCB_ECCMulCurveParamTypeDef ECCparam;
CCB_ECCMulKeyBlobTypeDef ECCKeyBlob;
CCB_ECCMulPointTypeDef Point_In;
CCB_ECCMulPointTypeDef Point_Out;
CCB_WrappingKeyTypeDef wrappedKeyConf;

/* Input buffers */
uint8_t eccMul192_Sclr_k_P[24]        = {0xD5, 0x6E, 0x0A, 0x82, 0xAF, 0x91, 0xEE, 0xDD, 0x62, 0x82, 0x00, 0x3B, 0x02, 0x55, 0x38, 0x35, 0x1F, 0x9B, 0x90, 0xB4, 0x00, 0x81, 0x2A, 0x43};

const uint32_t eccMul192_n_length     = 24;

const uint32_t eccMul192_p_length     = 24;

const uint32_t eccMul192_a_sign       = 0x00000001;

const uint8_t eccMul192_a_abs[24]     = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

const uint8_t eccMul192_b[24]         = {0x64, 0x21, 0x05, 0x19, 0xE5, 0x9C, 0x80, 0xE7, 0x0F, 0xA7, 0xE9, 0xAB, 0x72, 0x24, 0x30, 0x49, 0xFE, 0xB8, 0xDE, 0xEC, 0xC1, 0x46, 0xB9, 0xB1};

const uint8_t eccMul192_p[24]         = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const uint8_t eccMul192_n[24]         = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x99, 0xDE, 0xF8, 0x36, 0x14, 0x6B, 0xC9, 0xB1, 0xB4, 0xD2, 0x28, 0x31};

uint8_t eccMul192_Custumized_x[24]    = {0xEA, 0x31, 0x34, 0xD4, 0xCC, 0x80, 0xF0, 0xEA, 0xAB, 0xEB, 0x1B, 0x08, 0x70, 0x12, 0x52, 0x26, 0x36, 0x0F, 0x08, 0xD7, 0xA9, 0x4B, 0xF6, 0x19};

uint8_t eccMul192_Custumized_y[24]    = {0x49, 0x7B, 0x27, 0x0A, 0x26, 0xEE, 0xC8, 0xBB, 0x6E, 0x58, 0x40, 0x9D, 0x8E, 0x8A, 0xDD, 0x09, 0x03, 0xD3, 0x30, 0x1D, 0x04, 0x0D, 0x3A, 0x03};

const uint8_t expected_x[24]          = {0x64, 0x7F, 0x33, 0x4F, 0x37, 0x76, 0xBB, 0xF0, 0x06, 0x7D, 0x39, 0x68, 0x83, 0xFE, 0x75, 0xB4, 0xDE, 0x85, 0xC3, 0xD6, 0xC5, 0x51, 0x28, 0xB3};

const uint8_t expected_y[24]          = {0xFC, 0x24, 0xA0, 0x4C, 0x95, 0x93, 0x72, 0x71, 0xC8, 0xEA, 0x1E, 0x11, 0x68, 0x48, 0xE0, 0xCA, 0x5B, 0xDC, 0xF3, 0x98, 0x85, 0x15, 0xBE, 0x94};


/* Output buffers */
uint32_t SclrK_encrypted_192[8]       = {0};
uint8_t ECCXout_192[24]               = {0};
uint8_t ECCYout_192[24]               = {0};
uint32_t Wrapped_key_Buffer[8]        = {0};
uint32_t IV_Buffer[4]                 = {0};
uint32_t TAG_Buffer[4]                = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_CCB_Init(void);
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
  MX_CCB_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* -1- Initialize LEDs mounted on STM32H5F5J-DK board */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  /* Configure Wrapped Key */
  wrappedKeyConf.WrappingKeyType       = HAL_CCB_USER_KEY_HW;

  /* Fill ECCMul In parameters */
  ECCparam.primeOrderSizeByte          = eccMul192_n_length;
  ECCparam.modulusSizeByte             = eccMul192_p_length;
  ECCparam.coefSignA                   = eccMul192_a_sign;
  ECCparam.pAbsCoefA                   = eccMul192_a_abs;
  ECCparam.pCoefB                      = eccMul192_b;
  ECCparam.pModulus                    = eccMul192_p;
  ECCparam.pPrimeOrder                 = eccMul192_n;
  ECCparam.pPointX                     = eccMul192_Custumized_x;
  ECCparam.pPointY                     = eccMul192_Custumized_y;

  /* Prepare ECC Out parameters  */
  ECCKeyBlob.pIV                       = IV_Buffer;
  ECCKeyBlob.pTag                      = TAG_Buffer;
  ECCKeyBlob.pWrappedKey               = SclrK_encrypted_192;

  if ((HAL_CCB_ECC_WrapPrivateKey(&hccb, &ECCparam, eccMul192_Sclr_k_P, &wrappedKeyConf, &ECCKeyBlob) != HAL_OK) || (hccb.State != HAL_CCB_STATE_READY))
  {
    Error_Handler();
  }

  /* Fill ECCMul customized Point In */
  Point_In.pPointX                      = eccMul192_Custumized_x;
  Point_In.pPointY                      = eccMul192_Custumized_y;

  /* Prepare ECCMul Point Out */
  Point_Out.pPointX                     = ECCXout_192;
  Point_Out.pPointY                     = ECCYout_192;

  if ( HAL_CCB_ECC_ComputeScalarMul(&hccb, &ECCparam, &wrappedKeyConf, &ECCKeyBlob, &Point_In, &Point_Out) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CCB_DeInit(&hccb) != HAL_OK)
  {
    Error_Handler();
  }

  /* Check out point */
  for (int i = 0; i < 24; i++)
  {
    if(Point_Out.pPointX[i] != expected_x[i] )
    {
      while (1)
      {
        /* Toggle red led for error */
        BSP_LED_Toggle(LED_RED);
        HAL_Delay(500);
      }
    }
  }

  for (int i = 0; i < 24; i++)
  {
    if(Point_Out.pPointY[i] != expected_y[i] )
    {
      while (1)
      {
        /* Toggle red led for error */
        BSP_LED_Toggle(LED_RED);
        HAL_Delay(500);
      }
    }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    BSP_LED_On(LED_GREEN);
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
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
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

  /** Enable the CRS APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_GPIO;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief CCB Initialization Function
  * @param None
  * @retval None
  */
static void MX_CCB_Init(void)
{

  /* USER CODE BEGIN CCB_Init 0 */

  /* USER CODE END CCB_Init 0 */

  /* USER CODE BEGIN CCB_Init 1 */

  /* USER CODE END CCB_Init 1 */
  hccb.Instance = CCB;
  if (HAL_CCB_Init(&hccb) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CCB_Init 2 */

  /* USER CODE END CCB_Init 2 */

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
  while(1)
  {
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(500);
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
