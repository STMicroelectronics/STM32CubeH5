/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CCB/CCB_Protected_ECDSA_BlobCreation/Src/main.c
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
CCB_WrappingKeyTypeDef wrappedKeyConf;
CCB_ECDSACurveParamTypeDef ECDSAparam;
CCB_ECDSAKeyBlobTypeDef ECDSAKeyBlob;
CCB_ECDSASignTypeDef Signature;
CCB_ECCMulPointTypeDef publickeyOut;

/* Input buffers */
uint32_t Symetric_AESKey256[8]         = {0xdecafe00, 0xdecafe01, 0xdecafe02, 0xdecafe03, 0xdecafe04, 0xdecafe05, 0xdecafe06, 0xdecafe07};

uint32_t AESIV[4]                      = {0x00010203 , 0x04050607 , 0x08090A0B , 0x0C0D0E0F};

uint32_t Wrapped_key_Buffer[8]         = {0x1ff4d7fb, 0xbb515f14, 0x676df6b4, 0xcb58c0c8, 0x2b48e65e, 0xf9bfcf32, 0xd9c0a66c, 0xeec362d7};

uint8_t ecdsa224_Prvt_keyP[28]         = {0xf3, 0x32, 0xb6, 0x8c, 0xa9, 0x5a, 0x3c, 0xfa, 0x4e, 0x99, 0x8e, 0x43, 0x4d, 0x31, 0xbc, 0x10,0x58, 0x9c, 0x17, 0x47, 0xb7, 0xff, 0x9d, 0xd4, 0x08, 0x9f, 0x79, 0x2b};

const uint32_t ecdsa224_n_length       = 28;

const uint32_t ecdsa224_p_length       = 28;

const uint32_t ecdsa224_a_sign         = 0x00000001;

const uint8_t ecdsa224_a_abs[28]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

const uint8_t ecdsa224_b[28]           = {0xB4, 0x05, 0x0A, 0x85, 0x0C, 0x04, 0xB3, 0xAB, 0xF5, 0x41, 0x32, 0x56, 0x50, 0x44, 0xB0, 0xB7, 0xD7, 0xBF, 0xD8, 0xBA, 0x27, 0x0B, 0x39, 0x43, 0x23, 0x55, 0xFF, 0xB4};

const uint8_t ecdsa224_p[28]           = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

const uint8_t ecdsa224_n[28]           = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x16, 0xA2, 0xE0, 0xB8, 0xF0, 0x3E, 0x13, 0xDD, 0x29, 0x45, 0x5C, 0x5C, 0x2A, 0x3D};

const uint8_t ecdsa224_Gx[28]          = {0xB7, 0x0E, 0x0C, 0xBD, 0x6B, 0xB4, 0xBF, 0x7F, 0x32, 0x13, 0x90, 0xB9, 0x4A, 0x03, 0xC1, 0xD3, 0x56, 0xC2, 0x11, 0x22, 0x34, 0x32, 0x80, 0xD6, 0x11, 0x5C, 0x1D, 0x21};

const uint8_t ecdsa224_Gy[28]          = {0xBD, 0x37, 0x63, 0x88, 0xB5, 0xF7, 0x23, 0xFB, 0x4C, 0x22, 0xDF, 0xE6, 0xCD, 0x43, 0x75, 0xA0, 0x5A, 0x07, 0x47, 0x64, 0x44, 0xD5, 0x81, 0x99, 0x85, 0x00, 0x7E, 0x34};

/* Output buffers */
uint32_t IV_Buffer[4]                  = {0};
uint32_t TAG_Buffer[4]                 = {0};
uint32_t PrivateKey_encrypted_224[8]   = {0};
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
  wrappedKeyConf.WrappingKeyType         = HAL_CCB_USER_KEY_WRAPPED;
  wrappedKeyConf.pInitVect               = AESIV;
  wrappedKeyConf.AES_Algorithm           = CCB_AES_CBC;
  wrappedKeyConf.pUserWrappedWrappingKey = Wrapped_key_Buffer;
  if (HAL_CCB_ECDSA_WrapSymmetricKey(&hccb, Symetric_AESKey256, &wrappedKeyConf) != HAL_OK)
  {
    Error_Handler();
  }

  /* Fill ECDSA parameters */
  ECDSAparam.primeOrderSizeByte           = ecdsa224_n_length;
  ECDSAparam.modulusSizeByte              = ecdsa224_p_length;
  ECDSAparam.coefSignA                    = ecdsa224_a_sign;
  ECDSAparam.pAbsCoefA                    = ecdsa224_a_abs;
  ECDSAparam.pCoefB                       = ecdsa224_b;
  ECDSAparam.pModulus                     = ecdsa224_p;
  ECDSAparam.pPrimeOrder                  = ecdsa224_n;
  ECDSAparam.pPointX                      = ecdsa224_Gx;
  ECDSAparam.pPointY                      = ecdsa224_Gy;

  /* Prepare ECDSA Out parameters  */
  ECDSAKeyBlob.pIV                        = IV_Buffer;
  ECDSAKeyBlob.pTag                       = TAG_Buffer;
  ECDSAKeyBlob.pWrappedKey                = PrivateKey_encrypted_224;

  if ((HAL_CCB_ECDSA_WrapPrivateKey(&hccb, &ECDSAparam, ecdsa224_Prvt_keyP, &wrappedKeyConf, &ECDSAKeyBlob) != HAL_OK) || (hccb.State != HAL_CCB_STATE_READY))
  {
    Error_Handler();
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
    BSP_LED_On(LED_RED);
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
