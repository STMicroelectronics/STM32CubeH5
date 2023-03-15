/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    RTC/RTC_ActiveTamper/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H5xx RTC HAL API to write/read
  *          data to/from RTC Backup data registers and demonstrates the Active Tamper
  *          detection feature.
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

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
__IO FlagStatus TamperStatus;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TAMP_RTC_Init(void);
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
  uint32_t Seed[4] = {0xFEFEFEFEu, 0xAAAAAAAAu, 0x0u, 0xFFFF0000u};
  uint32_t i;

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
  /* Reset the RTC peripheral and the RTC clock source selection */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();
  /* Configure LED1 */
  BSP_LED_Init(LED1);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_TAMP_RTC_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* Check if Tamper is not detected */
  for (i = 0; i < RTC_BKP_NUMBER; i++)
  {
    if (HAL_RTCEx_BKUPRead(&hrtc, i) == 0x0u)
    {
      /* Please verify that All Tampers In and the Tamper Out are linked together */
      Error_Handler();
    }
  }

  /* Update the seed with "random" values */
  for (i = 0; i < 5; i++)
  {
    Seed[0] -= 0xFEFEFEu * i;
    Seed[1] += 0xAAAu;
    Seed[2] += Seed[0] - Seed[1];
    Seed[3] += 4 * Seed[0] + 0x123456u;
  
    if (HAL_RTCEx_SetActiveSeed(&hrtc, Seed) != HAL_OK)
    {
      Error_Handler();
    }
  
    HAL_Delay(100);
  }

  /* Check if Tamper is not detected */
  for (i = 0; i < RTC_BKP_NUMBER; i++)
  {
    if (HAL_RTCEx_BKUPRead(&hrtc, i) == 0)
    {
       Error_Handler();
    }
  }
  /* Clear tamper interrupt flag */
  TamperStatus = RESET;

  /* Wait for tamper detection. User must disconnect the wire */
  while (TamperStatus == RESET);

  /* Disable Active Tampers */
  if (HAL_RTCEx_DeactivateActiveTampers(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* Check that Backup registers have been cleared by the tamper detection */
  for (i = 0; i < RTC_BKP_NUMBER; i++)
  {
    if (HAL_RTCEx_BKUPRead(&hrtc, i) != 0)
    {
      Error_Handler();
    }
  }

  /* Test is OK Turn on LED1 */
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* Use __HAL_DBGMCU_FREEZE_RTC(); to freeze the RTC during debug */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* Calendar ultra-low power mode */
  if (HAL_RTCEx_SetLowPowerCalib(&hrtc, RTC_LPCAL_SET) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set all backup registers to 0xFFFFFFFF.
     Backup registers will be cleared by a tamper detection. */
  for (uint32_t i = 0; i < RTC_BKP_NUMBER; i++)
  {
    HAL_RTCEx_BKUPWrite(&hrtc, i, 0xFFFFFFFF);
  }
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TAMP Initialization Function
  * @param None
  * @retval None
  */
static void MX_TAMP_RTC_Init(void)
{

  /* USER CODE BEGIN TAMP_Init 0 */

  /* USER CODE END TAMP_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};
  RTC_ActiveTampersTypeDef sAllTamper = {0};

  /* USER CODE BEGIN TAMP_Init 1 */

  /* USER CODE END TAMP_Init 1 */

  /** Enable the RTC Active Tamper
  */
  privilegeState.tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }
  sAllTamper.ActiveFilter = RTC_ATAMP_FILTER_ENABLE;
  sAllTamper.ActiveAsyncPrescaler = RTC_ATAMP_ASYNCPRES_RTCCLK;
  sAllTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
  sAllTamper.ActiveOutputChangePeriod = 0;
  sAllTamper.Seed[0] = 0x00000000;
  sAllTamper.Seed[1] = 0x00000000;
  sAllTamper.Seed[2] = 0x00000000;
  sAllTamper.Seed[3] = 0x00000000;
  sAllTamper.TampInput[RTC_ATAMP_1].Enable = RTC_ATAMP_ENABLE;
  sAllTamper.TampInput[RTC_ATAMP_1].Interrupt = RTC_ATAMP_INTERRUPT_ENABLE;
  sAllTamper.TampInput[RTC_ATAMP_1].Output = RTC_ATAMP_1;
  sAllTamper.TampInput[RTC_ATAMP_1].NoErase = RTC_TAMPER_ERASE_BACKUP_ENABLE;
  sAllTamper.TampInput[RTC_ATAMP_1].MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
  if (HAL_RTCEx_SetActiveTampers(&hrtc, &sAllTamper) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TAMP_Init 2 */

  /* USER CODE END TAMP_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Tamper event callback function
  * @param  RTC handle
  * @retval None
  */
void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc)
{
  TamperStatus = SET;
}

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
    BSP_LED_Toggle(LED1);
    HAL_Delay(100);
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
