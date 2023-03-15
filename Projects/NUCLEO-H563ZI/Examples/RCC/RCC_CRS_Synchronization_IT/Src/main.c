/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RCC/RCC_CRS_Synchronization_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to use the RCC HAL API to configure the
  *          Clock Recovery System (CRS) in Interrupt mode.
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
#define CRS_STATUS_INIT       0
#define CRS_STATUS_SYNCOK     1
#define CRS_STATUS_SYNCWARN   2
#define CRS_STATUS_SYNCMISS   3
#define CRS_STATUS_SYNCERR    4

#define CRS_MAX_FELIM_VALUE   0xFFU
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RCC_CRSInitTypeDef CRSInitStruct = {0};
/* Variable to save SYNC status*/
__IO uint32_t CRSSyncStatus = CRS_STATUS_INIT;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void CRS_Init(void);
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
  RCC_CRSSynchroInfoTypeDef syncinfo = {0};
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
  /* USER CODE BEGIN 2 */
  /* Configure LED1,  LED2 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* CRS initialization (enable HSI48 and LSE oscillators and then enable CRS clock */
  /* Enable CRS interrupt */
  CRS_Init();

  /* HSI48 Synchronization without synchronization frequency (source set to GPIO */
  /* which is not configured)*/
  CRSInitStruct.Prescaler             = RCC_CRS_SYNC_DIV1;
  CRSInitStruct.Source                = RCC_CRS_SYNC_SOURCE_GPIO;
  CRSInitStruct.Polarity              = RCC_CRS_SYNC_POLARITY_FALLING;
  /* F(LSE)=32.768kHz then Reload=(f(Target)/f(LSE))-1= 0x5B7*/
  CRSInitStruct.ReloadValue           = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(HSI48_VALUE, LSE_VALUE);
  /* Felim value calculated like this FELIM = (fTARGET / fSYNC) * STEP[%] / 100% / 2 with STEP=0.14% then FELIM = 2 */
  CRSInitStruct.ErrorLimitValue       = 2;
  /* Change the HSI trimming value to see the automatic calibration performed by CRS */
  CRSInitStruct.HSI48CalibrationValue = 0x00;

  /* Start automatic synchronization*/
  HAL_RCCEx_CRSConfig(&CRSInitStruct);

  /* Enable CRS interrupts */
  __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);

  /* Simulate a SYNC EVENT which will raise a SYNC_MISS EVENT */
  HAL_RCCEx_CRSSoftwareSynchronizationGenerate();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    switch (CRSSyncStatus)
    {
      case CRS_STATUS_SYNCOK:
        /* Power on LED1 */
        BSP_LED_On(LED1);
        /* Power off LED2*/
        BSP_LED_Off(LED2);
        /* Power off LED3*/
        BSP_LED_Off(LED3);
        /* For visual comfort on LED1 apply delay */
        HAL_Delay(1000);
        /* Enable CRS SYNCWARN interrupt (in case it goes out of sync) */
        __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCWARN);
        break;

      case CRS_STATUS_SYNCMISS:
        /* Power off LED1 */
        BSP_LED_Off(LED1);
        /* Power off LED2*/
        BSP_LED_Off(LED2);
        /* Power on LED3*/
        BSP_LED_On(LED3);
        /* Perform a new configuration to LSE when SYNC MISS is detected. */
        /* Delay to see the LED power on switch from LED3 to LED1 */
        HAL_Delay(1000);

        /* Retrieve synchronization information */
        HAL_RCCEx_CRSGetSynchronizationInfo(&syncinfo);

        /* Change source to LSE and restart synchronization if necessary */
        if (CRSInitStruct.Source != RCC_CRS_SYNC_SOURCE_LSE)
        {
          CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_LSE;
          HAL_RCCEx_CRSConfig(&CRSInitStruct);

          /* Enable CRS interrupts after new CRS configuration */
          __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);
        }
        else
        {
          /* Enable CRS SYNCOK interrupt */
          __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK);
        }
        break;

      case CRS_STATUS_SYNCWARN:
        /* Power off LED1 */
        BSP_LED_Off(LED1);
        /* Power on LED2*/
        BSP_LED_On(LED2);
        /* Power off LED3*/
        BSP_LED_Off(LED3);
        HAL_Delay(100);
        /* Increase tolerance (FELIM value) until no more warning */
        CRSInitStruct.ErrorLimitValue += 1;
        /* Max value for FELIM has been reached? */
        if (CRSInitStruct.ErrorLimitValue > CRS_MAX_FELIM_VALUE)
        {
          Error_Handler();
        }
        HAL_RCCEx_CRSConfig(&CRSInitStruct);
        /* Enable CRS interrupts after new CRS configuration */
        __HAL_RCC_CRS_ENABLE_IT(RCC_CRS_IT_SYNCOK | RCC_CRS_IT_SYNCWARN | RCC_CRS_IT_ERR | RCC_CRS_IT_ESYNC);
        break;

      case CRS_STATUS_SYNCERR:
        /* Power off LED1 */
        BSP_LED_Off(LED1);
        /* Power off LED2*/
        BSP_LED_Off(LED2);
        /* Power on LED3*/
        BSP_LED_On(LED3);
        break;

      default:
        break;
    }
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  This function initializes the clock configuration for CRS.
  * @param  None
  * @retval None
  */
static void CRS_Init(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);

  if (RCC_OscInitStruct.HSI48State != RCC_HSI48_ON)
  {
    /* Enable HSI48 and LSE Oscillator*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  }

  /* Enable CRS clock*/
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Enable and set CRS interrupt*/
  HAL_NVIC_SetPriority(CRS_IRQn, 0x03, 0x00);
  HAL_NVIC_EnableIRQ(CRS_IRQn);

  /* Output HSI48 MCO pin(PA8) */
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, RCC_MCODIV_1);
}


/**
  * @brief  RCCEx Clock Recovery System SYNCOK interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_SyncOkCallback(void)
{
  CRSSyncStatus = CRS_STATUS_SYNCOK;

  /* Disable CRS SYNCOK interrupt since synchronization succeeded */
  __HAL_RCC_CRS_DISABLE_IT(RCC_CRS_IT_SYNCOK);
}

/**
  * @brief  RCCEx Clock Recovery System SYNCWARN interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_SyncWarnCallback(void)
{
  CRSSyncStatus = CRS_STATUS_SYNCWARN;
  /* Disable temporary CRS SYNCWARN interrupt to let the main loop to be execute */
  __HAL_RCC_CRS_DISABLE_IT(RCC_CRS_IT_SYNCWARN);
}

/**
  * @brief  RCCEx Clock Recovery System Expected SYNC interrupt callback.
  * @retval none
  */
void HAL_RCCEx_CRS_ExpectedSyncCallback(void)
{
}

/**
  * @brief  RCCEx Clock Recovery System Error interrupt callback.
  * @param  Combination of Error status.
  *         This parameter can be a combination of the following values:
  *           @arg @ref RCC_CRS_SYNCERR
  *           @arg @ref RCC_CRS_SYNCMISS
  *           @arg @ref RCC_CRS_TRIMOVF
  * @retval none
  */
void HAL_RCCEx_CRS_ErrorCallback(uint32_t Error)
{
  if ((Error & RCC_CRS_SYNCMISS) != RESET)
  {
    CRSSyncStatus = CRS_STATUS_SYNCMISS;
  }
  else if ((Error & RCC_CRS_SYNCERR) != RESET)
  {
    CRSSyncStatus = CRS_STATUS_SYNCERR;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* LED3 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(1000);
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
