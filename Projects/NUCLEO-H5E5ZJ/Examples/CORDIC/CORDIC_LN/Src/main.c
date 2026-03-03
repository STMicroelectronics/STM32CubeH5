/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CORDIC/CORDIC_LN/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use GPIOs through
  *          the STM32H5xx HAL API.
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
#include <arm_math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Pass/Fail Status */
#define PASS              0
#define FAIL              1

/* Size of data array */
#define ARRAY_SIZE        46U

/* Scale factor */
#define SCALE_FACTOR      1U

/* Reference values in float format */
#define DELTA_FLOAT  (float)(1.0f / pow(2.0f, 18))  /* Max residual error for Natural Logarithm, 
                                                       with 6 cycles precision: 2^-18 max residual error */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CORDIC_HandleTypeDef hcordic;

/* USER CODE BEGIN PV */

/* CORDIC configuration structure */
CORDIC_ConfigTypeDef sCordicConfig;

/* Float values from range [0.107, 1.0[ for Scale factor 1 */
static const float aRefValues[ARRAY_SIZE] =
{
  0.107f, 0.12f, 0.14f, 0.16f, 0.18f,
  0.20f, 0.22f, 0.24f, 0.26f, 0.28f,
  0.30f, 0.32f, 0.34f, 0.36f, 0.38f,
  0.40f, 0.42f, 0.44f, 0.46f, 0.48f,
  0.50f, 0.52f, 0.54f, 0.56f, 0.58f,
  0.60f, 0.62f, 0.64f, 0.66f, 0.68f,
  0.70f, 0.72f, 0.74f, 0.76f, 0.78f,
  0.80f, 0.82f, 0.84f, 0.86f, 0.88f,
  0.90f, 0.92f, 0.94f, 0.96f, 0.98f, 0.999f
};
static float aRefValuesScaled[ARRAY_SIZE];

/* Reference float scaled values converted in q1.31 format */
static q31_t aRefValues_q1_31[ARRAY_SIZE]  __attribute__((section("noncacheable_buffer")));

/* Natural Logarithm values calculated by CORDIC in q1.31 format */
static q31_t aCalculatedNaturalLog_q1_31[ARRAY_SIZE]  __attribute__((section("noncacheable_buffer")));

/* Array of calculated Natural Logarithm in float format */
static float aCalculatedNaturalLog_float[ARRAY_SIZE];

/* State of CORDIC calculations: 0 - Not done, 1 - Done */
__IO uint32_t calculate_complete;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_ICACHE_Init(void);
static void MX_CORDIC_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
uint32_t Check_Residual_Error(float VarA, float VarB, float MaxError);
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
  MX_ICACHE_Init();
  MX_CORDIC_Init();
  /* USER CODE BEGIN 2 */
  /* -1- Initialize LEDs mounted on NUCLEO-H5E5ZJ board */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);


  /* Convert 'float' referential values to q1.31 format */
  /* First the values shall be multiple by 2^(- scale_factor) */
  /* Here, the scale factor is 1, then each user value is multiple by 2^-1 */
  /* Next, the values are converted in q1.31 format */
  for (uint32_t i = 0; i < ARRAY_SIZE; i++)
  {
    aRefValuesScaled[i] = aRefValues[i] / pow(2.0f, SCALE_FACTOR);
  }
  arm_float_to_q31(aRefValuesScaled, aRefValues_q1_31, ARRAY_SIZE);

  /*## Configure the CORDIC peripheral ####################################*/
  sCordicConfig.Function         = CORDIC_FUNCTION_NATURALLOG;  /* Hyperbolic cosine function */
  sCordicConfig.Precision        = CORDIC_PRECISION_6CYCLES;    /* Max precision for q1.31 sine */
  sCordicConfig.Scale            = CORDIC_SCALE_1;              /* Scale factor 1 */
  sCordicConfig.NbWrite          = CORDIC_NBWRITE_1;            /* One input data: angle */
  sCordicConfig.NbRead           = CORDIC_NBREAD_1;             /* One output data: natural logarithm */
  sCordicConfig.InSize           = CORDIC_INSIZE_32BITS;        /* q1.31 format for input data */
  sCordicConfig.OutSize          = CORDIC_OUTSIZE_32BITS;       /* q1.31 format for output data */

  if (HAL_CORDIC_Configure(&hcordic, &sCordicConfig) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler();
  }

  /*## Start calculation of cosh in DMA mode #############################*/
  calculate_complete = 0;
  if (HAL_CORDIC_Calculate_IT(&hcordic, aRefValues_q1_31, aCalculatedNaturalLog_q1_31,
                               ARRAY_SIZE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }

  /* Wait the end of CORDIC process */
  while (calculate_complete == 0)
  {
  }

  /* Convert Natural Logarithm q1_31 values to float */
  arm_q31_to_float(aCalculatedNaturalLog_q1_31, aCalculatedNaturalLog_float, ARRAY_SIZE);

  /*## Compare CORDIC results to the reference values #####################*/
  for (uint32_t i = 0; i < ARRAY_SIZE; i++)
  {
    /* Compute the Referential Natural Logarithm value with "logf" 'C' function */
    /* Instead of multiple CORDIC result by 2^(factor + 1), the 'logf' result is divided to simplify
       the check with residual error */
    float ref_log = logf(aRefValues[i]) / pow(2.0f, SCALE_FACTOR + 1);

    if (Check_Residual_Error(ref_log, aCalculatedNaturalLog_float[i], DELTA_FLOAT) == FAIL)
    {
      Error_Handler();
    }
  }

  /* Correct CORDIC output values: Turn on LED1 */
  BSP_LED_On(LED_GREEN);
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
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
  * @brief CORDIC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CORDIC_Init(void)
{

  /* USER CODE BEGIN CORDIC_Init 0 */

  /* USER CODE END CORDIC_Init 0 */

  /* USER CODE BEGIN CORDIC_Init 1 */

  /* USER CODE END CORDIC_Init 1 */
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CORDIC_Init 2 */

  /* USER CODE END CORDIC_Init 2 */

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
/**
  * @brief  CORDIC calculate complete callback.
  * @param  hcordic pointer to a CORDIC_HandleTypeDef structure that contains
  *         the configuration information for CORDIC module
  * @retval None
  */
void HAL_CORDIC_CalculateCpltCallback(CORDIC_HandleTypeDef *hcordic)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcordic);

  calculate_complete = 1;
}

/**
  * @brief  Check delta between two values is below threshold
  * @param  VarA First input variable
  * @param  VarB Second input variable
  * @param  MaxError Maximum delta allowed between VarA and VarB
  * @retval Status
  *           PASS: Delta is below threshold
  *           FAIL: Delta is above threshold
  */
uint32_t Check_Residual_Error(float VarA, float VarB, float MaxError)
{
  uint32_t status = PASS;

  if ((VarA - VarB) >= 0)
  {
    if ((VarA - VarB) > MaxError)
    {
      status = FAIL;
    }
  }
  else
  {
    if ((VarB - VarA) > MaxError)
    {
      status = FAIL;
    }
  }

  return status;
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
  while (1)
  {
    /* Wrong CORDIC output values: Toggle LED2 */
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
