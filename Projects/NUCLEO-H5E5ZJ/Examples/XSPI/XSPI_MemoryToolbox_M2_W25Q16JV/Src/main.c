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

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

XSPI_HandleTypeDef hxspi1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****Memory-mapped XSPI communication****  ****Memory-mapped XSPI communication****  ****Memory-mapped XSPI communication****  ****Memory-mapped XSPI communication****  ****Memory-mapped XSPI communication****  ****Memory-mapped XSPI communication**** ";

/* Buffer used to check erased pages */
uint8_t ErasedBuffer[MEMORY_PAGE_SIZE];
/* Buffer used to check read data */
uint8_t ReadData[MEMORY_PAGE_SIZE] = {0};

/* Glabal execution result */
Memory_Toolbox_Status_t exec_status = MEMORY_OK;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
static Memory_Toolbox_Status_t M2_W25Q16JV_NorFlash_Configuration(XSPI_HandleTypeDef *hxspi);

#if defined(EXAMPLE_TRACE_ENABLE)
static void TraceOutput(const uint8_t *Msg);
#if (defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW))
extern void initialise_monitor_handles(void);
#endif
#endif /* EXAMPLE_TRACE_ENABLE */

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
  uint8_t JedecID[3] = {0};
  uint32_t address = 0;
  uint8_t *mem_addr;
  memset(ErasedBuffer, 0xFF, sizeof(ErasedBuffer));
#if defined(EXAMPLE_TRACE_ENABLE)
#if (defined (__GNUC__) && !defined(__ARMCC_VERSION) && defined(EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW))
  initialise_monitor_handles();
  printf("Semihosting Test...\n\r");
#endif
#endif
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
  MX_OCTOSPI1_Init();
  MX_ICACHE_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

#if defined(EXAMPLE_TRACE_ENABLE)
  /* If Trace system is enabled, register Trace callback */
  Memory_Toolbox_RegisterTraceCallback(TraceOutput);
  /* Select Trace level :
    EXAMPLE_TRACE_LEVEL_NONE     : No information provided in logs
    EXAMPLE_TRACE_LEVEL_CMD_INFO : Step descriptions and commands are provided in logs
    EXAMPLE_TRACE_LEVEL_ALL      : Step descriptions, commands and buffer contents are provided in logs
  */
  Memory_Toolbox_SetTraceLevel(EXAMPLE_TRACE_LEVEL_ALL);
#endif /* EXAMPLE_TRACE_ENABLE */

  /* Initialize LED OK on board */
  BSP_LED_Init(LED_GREEN);

  /* Initialize LED Error on board */
  BSP_LED_Init(LED_RED);

  /* Program Step 1 : Read Jedec ID in 1 Line mode ------------------------------------------- */
  EXECUTE_AND_CHECK(Memory_Toolbox_Read_JedecID(&hxspi1, JedecID));

  /* Program Step 2 : Read SFDP table -------------------------------------------------------- */
  EXECUTE_AND_CHECK(Memory_Toolbox_Read_All_SFDP_Table(&hxspi1));

  /* Program Step 3 : Check Write Enable and Write Disable commands are properly understood by the memory
     and value of WEL bit in SR is updated accordingly
     (Write Enable command use is mandatory for further memory configuration steps ----------- */
  EXECUTE_AND_CHECK(Memory_Toolbox_Check_Write_Enabling(&hxspi1));

  /* Program Step 4 : Configure the Nor Flash memory (Drive strength, Quad mode) ------------- */
  EXECUTE_AND_CHECK(M2_W25Q16JV_NorFlash_Configuration(&hxspi1));

  /* Program Step 5 : Write and read data using indirect and memory mapped modes ------------- */
  /* Program Step 5.1: Enable write operation (WE bit) and Erase Sector related to address --- */
  EXECUTE_AND_CHECK(Memory_Toolbox_Erase_Sector(&hxspi1, address));

  /* Read a page from the sector that has been erased (Indirect mode) */
  EXECUTE_AND_CHECK(Memory_Toolbox_Read_MemData(&hxspi1, ReadData, address, MEMORY_PAGE_SIZE));

  /* Verify the page has been erased correctly */
  EXECUTE_AND_CHECK(Memory_Toolbox_MemCmp(ReadData, ErasedBuffer, BUFFERSIZE));

  /* Program Step 5.2: Enable write operation (WE bit) and write data present at address: aTxBuffer */
  EXECUTE_AND_CHECK(Memory_Toolbox_Program_Page(&hxspi1, aTxBuffer, address));

  /* Program Step 5.3: Read written area in indirect mode and store it in buffer ------------- */
  EXECUTE_AND_CHECK(Memory_Toolbox_Read_MemData(&hxspi1, ReadData, address, MEMORY_PAGE_SIZE));

  /* Compare read buffer with original one and check validity of data */
  EXECUTE_AND_CHECK(Memory_Toolbox_MemCmp(ReadData, aTxBuffer, BUFFERSIZE));

  /* Program Step 5.4: Switch memory in Memory Mapped Mode */
  EXECUTE_AND_CHECK(Memory_Toolbox_Enable_MemoryMapped_Mode(&hxspi1));

  /* Program Step 5.5: Read data from memory in memory mapped mode and compare --------------- */
  /* Reading address */
  mem_addr = (uint8_t *)(OCTOSPI1_BASE + address);

  /* Read written area in memory mapped mode and store it in buffer (in memory mapped mode, reading data
     from memory is equivalent to a memory to memory transfer, and could be achieved using a memcpy() service
     or a DMA Memory to Memory transfer for instance */
  EXECUTE_AND_CHECK(Memory_Toolbox_Read_MemData_MemoryMapped_Mode(ReadData, mem_addr, MEMORY_PAGE_SIZE));

  /* Compare read data with original one and check validity of data */
  EXECUTE_AND_CHECK(Memory_Toolbox_MemCmp(ReadData, aTxBuffer, BUFFERSIZE));

  /*  Program Step 5.6: Disable Memory Mapped mode */
  EXECUTE_AND_CHECK(Memory_Toolbox_Disable_MemoryMapped_Mode(&hxspi1));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* Set LED Green on : successful execution of program */
  BSP_LED_On(LED_GREEN);

  while (1)
  {
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
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  HAL_XSPI_DLYB_CfgTypeDef HAL_XSPI_DLYB_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hxspi1.Instance = OCTOSPI1;
  hxspi1.Init.FifoThresholdByte = 4;
  hxspi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hxspi1.Init.MemoryType = HAL_XSPI_MEMTYPE_MICRON;
  hxspi1.Init.MemorySize = HAL_XSPI_SIZE_16MB;
  hxspi1.Init.ChipSelectHighTimeCycle = 10;
  hxspi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hxspi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hxspi1.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hxspi1.Init.ClockPrescaler = 0;
  hxspi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hxspi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_DISABLE;
  hxspi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  hxspi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_ON;
  hxspi1.Init.MaxTran = 0;
  hxspi1.Init.Refresh = 0;
  hxspi1.Init.MemorySelect = HAL_XSPI_CSSEL_NCS1;
  if (HAL_XSPI_Init(&hxspi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_XSPI_DLYB_Cfg_Struct.Units = 0;
  HAL_XSPI_DLYB_Cfg_Struct.PhaseSel = 0;
  if (HAL_XSPI_DLYB_SetConfig(&hxspi1, &HAL_XSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */
  if (HAL_XSPI_DLYB_GetClockPeriod(&hxspi1, &HAL_XSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_XSPI_DLYB_Cfg_Struct.PhaseSel /= 4;

  if (HAL_XSPI_DLYB_SetConfig(&hxspi1, &HAL_XSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LDO_M2_Slot_GPIO_Port, LDO_M2_Slot_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LDO_M2_Slot_Pin */
  GPIO_InitStruct.Pin = LDO_M2_Slot_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LDO_M2_Slot_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* Configure GPIO pin Output Level : Disable LDO to ensure cold reset of memory */
  HAL_GPIO_WritePin(LDO_M2_Slot_GPIO_Port, LDO_M2_Slot_Pin, GPIO_PIN_RESET);
  HAL_Delay(MEMORY_M2_BOARD_LDO_RESET_TIME);

  /* Configure GPIO pin Output Level : enable LDO */
  HAL_GPIO_WritePin(LDO_M2_Slot_GPIO_Port, LDO_M2_Slot_Pin, GPIO_PIN_SET);

  /* Wait for LDO setup time */
  HAL_Delay(MEMORY_M2_BOARD_LDO_SETUP_TIME);

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Configures the memory Winbond W25Q16JV in Quad mode + Drive Strength update.
  * @param  hxspi: XSPI handle
  * @retval None
  */
static Memory_Toolbox_Status_t M2_W25Q16JV_NorFlash_Configuration(XSPI_HandleTypeDef *hxspi)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  HAL_XSPI_DLYB_CfgTypeDef DlybCfgStruct       = {0};
  uint8_t RegBufferWrite[3]                    = {0};
  uint8_t RegBufferRead[3]                     = {0};

#if defined(EXAMPLE_TRACE_ENABLE)
  /* Print current frequency of Clock signal provided to memory in log.
     Default value is expected to be 50Mhz for configuration step */
  Memory_Toolbox_LogFrequency(&hxspi1);
#endif /* EXAMPLE_TRACE_ENABLE */

  /* Read Status Register  */
  Memory_Toolbox_ReadStatusRegister(hxspi, RegBufferRead, 1U, MEMORY_STARTUP_MODE);

  /* Read Status Register 3 */
  Memory_Toolbox_ReadStatusRegister3(hxspi, RegBufferRead, 1U);
  RegBufferWrite[0] = (RegBufferRead[0] & ~M2_W25Q16JV_REG_SR3_DRIVE_STRENGH_MASK)
                      | M2_W25Q16JV_REG_SR3_DRIVE_STRENGH_VALUE;

  /* Write updated value in Status Register 3 (Drive Strength) */
  Memory_Toolbox_WriteStatusRegister3(hxspi, RegBufferWrite, 1U);

  /* Read Status Register 2 */
  Memory_Toolbox_ReadStatusRegister2(hxspi, RegBufferRead, 1U);
  RegBufferWrite[0] = (RegBufferRead[0] & ~M2_W25Q16JV_REG_SR2_QUAD_ENABLE_MASK)
                      | M2_W25Q16JV_REG_SR2_QUAD_ENABLE_VALUE;

  /* Write updated value in Status Register 2 (Quad-SPI Enable) */
  Memory_Toolbox_WriteStatusRegister2(hxspi, RegBufferWrite, 1U);

  /* Update clock Frequency to max supported frequency : 133 Mhz */
  /* Set PLL2N to 133 - PLL2R to 2 */
  HAL_RCCEx_GetPeriphCLKConfig(&PeriphClkInitStruct);
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
  PeriphClkInitStruct.PLL2.PLL2N = 133U;
  PeriphClkInitStruct.PLL2.PLL2R = 2U;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    return (MEMORY_RCC_CONFIG_KO);
  }

  /* Reinitialize DLYB to fit with this new frequency */
  if (HAL_XSPI_DLYB_GetClockPeriod(&hxspi1, &DlybCfgStruct) != HAL_OK)
  {
    return (MEMORY_DLYB_CONFIG_KO);
  }

  /* Update PhaseSel for DTR configuration */
  DlybCfgStruct.PhaseSel = 1U;

  /* Set new delay block configuration*/
  if (HAL_XSPI_DLYB_SetConfig(&hxspi1, &DlybCfgStruct) != HAL_OK)
  {
    return (MEMORY_DLYB_CONFIG_KO);
  }

#if defined(EXAMPLE_TRACE_ENABLE)
  /* Dump register content after configuration step (could be found in memory_toolbox_register_dump[] array) */
  Memory_Toolbox_Dump_Registers();

  /* Print updated frequency of Clock signal after configuration step */
  Memory_Toolbox_LogFrequency(&hxspi1);
#endif /* EXAMPLE_TRACE_ENABLE */

  /* Read status Register to validate configuration and clock change */
  return (Memory_Toolbox_ReadStatusRegister(hxspi, RegBufferRead, 1U, MEMORY_HIGH_PERFORMANCE));
}

#if defined(EXAMPLE_TRACE_ENABLE)
/**
  * @brief This function outputs trace data
  * @param Msg Trace data to be evacuated
  * @retval None
  **/
static void TraceOutput(const uint8_t *Msg)
{
#if defined(EXAMPLE_TRACE_OUTPUT_UART)
  HAL_UART_Transmit(&huart3, (const uint8_t *)Msg, strlen((const char *)Msg), 1000U);
#elif defined(EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW)
  printf("%s", Msg);
#endif /* EXAMPLE_TRACE_OUTPUT_UART */
}
#endif /* EXAMPLE_TRACE_ENABLE */

#if defined(__GNUC__)
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar(int ch)
{
  ITM_SendChar(ch);

  return ch;
}
#endif /* __GNUC__ */

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
  /* Switch LED RED on */
  BSP_LED_On(LED_RED);

#if defined(EXAMPLE_TRACE_ENABLE)
  /* Print error message in log */
  Memory_Toolbox_LogStatus(exec_status);
#endif /* EXAMPLE_TRACE_ENABLE */

  while (1)
  {
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
