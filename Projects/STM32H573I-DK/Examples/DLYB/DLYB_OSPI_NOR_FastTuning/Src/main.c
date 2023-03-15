/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples/DLYB/DLYB_OSPI_NOR_FastTuning/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
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

XSPI_HandleTypeDef hospi1;

/* USER CODE BEGIN PV */
__IO uint8_t CmdCplt;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication**** ";
LL_DLYB_CfgTypeDef dlyb_cfg,dlyb_cfg_test;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi);
static void XSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi);
static void OSPI_OctalDTRModeCfg(XSPI_HandleTypeDef *hospi);
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
  XSPI_RegularCmdTypeDef sCommand = {0};
  XSPI_MemoryMappedTypeDef sMemMappedCfg = {0};
  uint32_t address = 0;
  uint16_t index;
  __IO uint8_t step = 0;
  __IO uint8_t *mem_addr;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_OCTOSPI1_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Delay block configuration ------------------------------------------------ */
  if (HAL_XSPI_DLYB_GetClockPeriod(&hospi1,&dlyb_cfg) != HAL_OK)
  {
    BSP_LED_On(LED_RED);
  }
  
  /*when DTR, PhaseSel is divided by 4 (emperic value)*/
  dlyb_cfg.PhaseSel /=4;
  
  /* save the present configuration for check*/
  dlyb_cfg_test = dlyb_cfg;
  
  /*set delay block configuration*/
  HAL_XSPI_DLYB_SetConfig(&hospi1,&dlyb_cfg);
  
  /*check the set value*/
  HAL_XSPI_DLYB_GetConfig(&hospi1,&dlyb_cfg);
  if ((dlyb_cfg.PhaseSel != dlyb_cfg_test.PhaseSel) || (dlyb_cfg.Units != dlyb_cfg_test.Units))
  {
    BSP_LED_On(LED_RED);
  }
  
  /* Configure the memory in octal mode ------------------------------------- */
  OSPI_OctalDTRModeCfg(&hospi1);
  
  sCommand.InstructionMode     = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth    = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode  = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.AddressWidth        = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode      = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode  = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataDTRMode         = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.SIOOMode            = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    switch(step)
    {
    case 0:
      
      CmdCplt = 0;
      
      /* Enable write operations ------------------------------------------ */
      OSPI_WriteEnable(&hospi1);
      
      /* Erasing Sequence ------------------------------------------------- */
      sCommand.OperationType = HAL_XSPI_OPTYPE_COMMON_CFG;
      sCommand.Instruction   = OCTAL_SECTOR_ERASE_CMD;
      sCommand.AddressMode   = HAL_XSPI_ADDRESS_8_LINES;
      sCommand.Address       = address;
      sCommand.DataMode      = HAL_XSPI_DATA_NONE;
      sCommand.DummyCycles   = 0;
      sCommand.DQSMode       = HAL_XSPI_DQS_DISABLE;
      
      if (HAL_XSPI_Command_IT(&hospi1, &sCommand) != HAL_OK)
      {
        Error_Handler();
      }
      
      step++;
      break;
    case 1:
      if(CmdCplt != 0)
      {
        CmdCplt = 0;
        
        /* Configure automatic polling mode to wait for end of erase ------ */
        XSPI_AutoPollingMemReady(&hospi1);
        
        /* Enable write operations ---------------------------------------- */
        OSPI_WriteEnable(&hospi1);
        
        /* Memory-mapped mode configuration ------------------------------- */
        sCommand.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
        sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
        sCommand.DataMode      = HAL_XSPI_DATA_8_LINES;
        sCommand.DataLength    = 1;
        sCommand.DQSMode       = HAL_XSPI_DQS_ENABLE;
        
        if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
          Error_Handler();
        }
        
        sCommand.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
        sCommand.Instruction   = OCTAL_IO_DTR_READ_CMD;
        sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;
        sCommand.DQSMode       = HAL_XSPI_DQS_ENABLE;
        
        if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
          Error_Handler();
        }
        
        sMemMappedCfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_ENABLE;
        sMemMappedCfg.TimeoutPeriodClock     = 0x40;
        if (HAL_XSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
        {
          Error_Handler();
        }
        
        HAL_Delay(1000);
        
        /* Writing Sequence ----------------------------------------------- */
        mem_addr = (uint8_t *)(OCTOSPI1_BASE + address);
        for (index = 0; index < BUFFERSIZE; index++)
        {
          *mem_addr = aTxBuffer[index];
          mem_addr++;
        }
        
        /* In memory-mapped mode, not possible to check if the memory is ready
        after the programming. So a delay corresponding to max page programming
        time is added */
        HAL_Delay(MEMORY_PAGE_PROG_DELAY);
        
        /* Reading Sequence ----------------------------------------------- */
        mem_addr = (uint8_t *)(OCTOSPI1_BASE + address);
        for (index = 0; index < BUFFERSIZE; index++)
        {
          if (*mem_addr != aTxBuffer[index])
          {
            BSP_LED_On(LED_RED);
     
          }
          else
          {
            BSP_LED_On(LED_GREEN);
          }
          mem_addr++;
        }
        
        address += OSPI_PAGE_SIZE;
        if(address >= OSPI_END_ADDR)
        {
          address = 0;
        }
        
        /* Abort OctoSPI driver to stop the memory-mapped mode ------------ */
        if (HAL_XSPI_Abort(&hospi1) != HAL_OK)
        {
          Error_Handler();
        }
        
        step = 2;
        break;
      default :
        break;
        
      }
      
    }
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
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */
  
  /* USER CODE END OCTOSPI1_Init 0 */

  HAL_XSPI_DLYB_CfgTypeDef HAL_OSPI_DLYB_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */
  
  /* USER CODE END OCTOSPI1_Init 1 */
  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThresholdByte = 4;
  hospi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hospi1.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
  hospi1.Init.MemorySize = HAL_XSPI_SIZE_8MB;
  hospi1.Init.ChipSelectHighTimeCycle = 1;
  hospi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 2;
  hospi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  hospi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_ON;
  hospi1.Init.Refresh = 0;
  if (HAL_XSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_OSPI_DLYB_Cfg_Struct.Units = 0;
  HAL_OSPI_DLYB_Cfg_Struct.PhaseSel = 0;
  if (HAL_XSPI_DLYB_SetConfig(&hospi1, &HAL_OSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */
  
  /* USER CODE END OCTOSPI1_Init 2 */

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
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
* @brief  Command completed callback.
* @param  hospi: OSPI handle
* @retval None
*/
void HAL_XSPI_CmdCpltCallback(XSPI_HandleTypeDef *hospi)
{
  CmdCplt++;
}

/**
* @brief  Transfer Error callback.
* @param  hospi: OSPI handle
* @retval None
*/
void HAL_XSPI_ErrorCallback(XSPI_HandleTypeDef *hospi)
{
  Error_Handler();
}

/**
* @brief  This function send a Write Enable and wait it is effective.
* @param  hospi: OSPI handle
* @retval None
*/
static void OSPI_WriteEnable(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand ={0};
  XSPI_AutoPollingTypeDef sConfig  ={0};
  
  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction        = OCTAL_WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction    = OCTAL_READ_STATUS_REG_CMD;
  sCommand.Address        = 0x0;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth   = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.DataMode       = HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode    = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DataLength         = 2;
  sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode        = HAL_XSPI_DQS_ENABLE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.MatchValue    = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask     = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode     = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime           = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  
  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  This function read the SR of the memory and wait the EOP.
* @param  hospi: OSPI handle
* @retval None
*/
static void XSPI_AutoPollingMemReady(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand = {0};
  XSPI_AutoPollingTypeDef sConfig = {0};
  
  
  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  
  sCommand.Instruction        = OCTAL_READ_STATUS_REG_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_16_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.Address            = 0x0;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
  sCommand.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_8_LINES;
  sCommand.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  sCommand.DataLength         = 2;
  sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
  sCommand.DQSMode            = HAL_XSPI_DQS_ENABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.MatchValue         = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask          = MEMORY_READY_MASK_VALUE;
  sConfig.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime       = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  
  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
* @brief  This function configure the memory in Octal DTR mode.
* @param  hospi: OSPI handle
* @retval None
*/
static void OSPI_OctalDTRModeCfg(XSPI_HandleTypeDef *hospi)
{
  XSPI_RegularCmdTypeDef  sCommand ={0};
  XSPI_AutoPollingTypeDef sConfig ={0};
  uint8_t reg;
  
  /* Enable write operations ---------------------------------------- */
  sCommand.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  
  sCommand.Instruction        = WRITE_ENABLE_CMD;
  sCommand.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
  sCommand.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DataLength  = 1;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.MatchValue         = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask          = WRITE_ENABLE_MASK_VALUE;
  sConfig.MatchMode          = HAL_XSPI_MATCH_MODE_AND;
  sConfig.IntervalTime       = AUTO_POLLING_INTERVAL;
  sConfig.AutomaticStop      = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  
  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Write Configuration register 2 (with new dummy cycles) --------- */
  sCommand.Instruction    = WRITE_CFG_REG_2_CMD;
  sCommand.Address        = CONFIG_REG2_ADDR3;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth    = HAL_XSPI_ADDRESS_32_BITS;
  sCommand.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  reg = CR2_DUMMY_CYCLES_66MHZ;
  
  if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait that the memory is ready ---------------------------------- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_NONE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.MatchValue = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask  = MEMORY_READY_MASK_VALUE;
  
  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Enable write operations ---------------------------------------- */
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction = READ_STATUS_REG_CMD;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.MatchValue = WRITE_ENABLE_MATCH_VALUE;
  sConfig.MatchMask  = WRITE_ENABLE_MASK_VALUE;
  
  if (HAL_XSPI_AutoPolling(hospi, &sConfig, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Write Configuration register 2 (with octal mode) --------------- */
  sCommand.Instruction = WRITE_CFG_REG_2_CMD;
  sCommand.Address     = CONFIG_REG2_ADDR1;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
  
  if (HAL_XSPI_Command(hospi, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  reg = CR2_DTR_OPI_ENABLE;
  
  if (HAL_XSPI_Transmit(hospi, &reg, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Wait that the configuration is effective and check that memory is ready */
  HAL_Delay(MEMORY_REG_WRITE_DELAY);
  
  /* Wait that the memory is ready ---------------------------------- */
  XSPI_AutoPollingMemReady(hospi);
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
  __disable_irq();
  while (1)
  {
    HAL_Delay(100);
    BSP_LED_Toggle(LED_RED);
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
