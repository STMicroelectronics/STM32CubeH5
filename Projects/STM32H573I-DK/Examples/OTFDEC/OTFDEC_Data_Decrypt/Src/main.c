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
#define START_ADRESS_OCOTOSPI1                   0x90000000
#define START_ADRESS_OTFDEC1_REGION1             0x90000000
#define END_ADRESS_OTFDEC1_REGION1               0x900003FF
#define VERSION_NUMBER                           4

#define PLAIN_SIZE                               0x100
#define START_ADRESS_WRITING_REGION              0
#define NONCE_NUMBER                             2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
uint32_t pKeyAES[4] = {0x00000000,0x00000000,0x00000000,0x00000000};
uint32_t pInitVectAES[4] = {0x00000000,0x00000000,0x00000000,0x00000000};

DCACHE_HandleTypeDef hdcache1;

XSPI_HandleTypeDef hospi1;

OTFDEC_HandleTypeDef hotfdec1;

/* USER CODE BEGIN PV */
OTFDEC_RegionConfigTypeDef Config;
XSPI_MemoryMappedTypeDef sMemMappedCfg;

uint32_t OSPIAddressMode;
uint32_t OSPIDataMode;

__ALIGN_BEGIN uint32_t Cipher[PLAIN_SIZE] __ALIGN_END ;
uint32_t Plain_AES_test[PLAIN_SIZE] = {0};
uint16_t FirmwareVersion[VERSION_NUMBER] = { 0xABBA, 0xA5A5, 0x5A5A,0xF119 };

uint32_t Nonce[NONCE_NUMBER][2]       = {    {0xA5A5A5A5, 0xC3C3C3C3},
{0x11111111, 0x55555555}
};

uint32_t Region1_Range[2] = {START_ADRESS_OTFDEC1_REGION1, END_ADRESS_OTFDEC1_REGION1};

uint32_t Key[4]  = { 0x71234567, 0x89ABCDEF, 0x71234567, 0x89ABCDEF };

uint32_t Plain[PLAIN_SIZE] ={ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_AES_Init(void);
static void MX_OTFDEC1_Init(void);
static void MX_DCACHE1_Init(void);
/* USER CODE BEGIN PFP */
static void CRYP_Cipher(uint32_t index,uint32_t* pRange, uint32_t StartAdressWritingRegion);
static uint32_t OSPI_Write(uint32_t * pBuffer, uint32_t FlashAddress, uint32_t Size);
static uint32_t OSPI_MemoryMap(void);
static void OSPI_ResetMemory(void);
static uint32_t OSPI_Erase(void);
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
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  uint32_t i,errorBuffer = 0;
  __IO uint32_t *mem_addr_32;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Fill Cipher Buffer*/
  for (i=0; i <PLAIN_SIZE; i++)
  {
    Cipher[i] = 0xFF ;
  }
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_OCTOSPI1_Init();
  MX_AES_Init();
  MX_OTFDEC1_Init();
  MX_DCACHE1_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);
  
  /* Reset the Flash Memory   */
  OSPI_ResetMemory();
  
  /* Erase the Flash Memory   */
  OSPI_Erase();

  /* Preload external HyperRAM with ciphred data issued from crypto into dedicated address*/
  CRYP_Cipher(OTFDEC_REGION1,(uint32_t*)Region1_Range,START_ADRESS_WRITING_REGION);
  OSPI_Write( Cipher,START_ADRESS_OTFDEC1_REGION1- START_ADRESS_OCOTOSPI1, PLAIN_SIZE);
  
  /* Activate memory mapping */
  OSPI_MemoryMap();
  
  __HAL_OTFDEC_ENABLE_IT(&hotfdec1, OTFDEC_ALL_INT);
  
  /* Set OTFDEC Mode */
  if (HAL_OTFDEC_RegionSetMode(&hotfdec1, OTFDEC_REGION1, OTFDEC_REG_MODE_INSTRUCTION_OR_DATA_ACCESSES) != HAL_OK)
  {
    Error_Handler();
  }
  /* Set OTFDEC Key */
  if (HAL_OTFDEC_RegionSetKey(&hotfdec1, OTFDEC_REGION1, Key)  != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure and lock region,enable OTFDEC decryption */
  Config.Nonce[0]     = Nonce[OTFDEC_REGION1/4][0];
  Config.Nonce[1]     = Nonce[OTFDEC_REGION1/4][1];
  Config.StartAddress = START_ADRESS_OTFDEC1_REGION1 ;
  Config.EndAddress   = END_ADRESS_OTFDEC1_REGION1;
  Config.Version      = FirmwareVersion[OTFDEC_REGION1];
  if (HAL_OTFDEC_RegionConfig(&hotfdec1, OTFDEC_REGION1, &Config, OTFDEC_REG_CONFIGR_LOCK_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  mem_addr_32= (uint32_t *)START_ADRESS_OTFDEC1_REGION1;
  
  /* OTFDEC activated, should read decrypted DATA */
  for (uint16_t index = 0; index < PLAIN_SIZE/4; index++)
  {
    if (*mem_addr_32 != Plain[index])
    { 
      BSP_LED_On(LED_RED);
      errorBuffer++;
    }
    mem_addr_32++;
  }
  /* DeInit OTFDEC */
  HAL_OTFDEC_DeInit(&hotfdec1);
  
  /* DeInit OCTOSPI */
  HAL_XSPI_DeInit(&hospi1);
  
  if (errorBuffer == 0)
  {
    /* Turn LED GREEN on */
    BSP_LED_On(LED_GREEN);
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
  * @brief AES Initialization Function
  * @param None
  * @retval None
  */
static void MX_AES_Init(void)
{

  /* USER CODE BEGIN AES_Init 0 */
  
  /* USER CODE END AES_Init 0 */

  /* USER CODE BEGIN AES_Init 1 */
  
  /* USER CODE END AES_Init 1 */
  hcryp.Instance = AES;
  hcryp.Init.DataType = CRYP_NO_SWAP;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.pInitVect = (uint32_t *)pInitVectAES;
  hcryp.Init.Algorithm = CRYP_AES_CTR;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN AES_Init 2 */
  
  /* USER CODE END AES_Init 2 */

}

/**
  * @brief DCACHE1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCACHE1_Init(void)
{

  /* USER CODE BEGIN DCACHE1_Init 0 */
  
  /* USER CODE END DCACHE1_Init 0 */

  /* USER CODE BEGIN DCACHE1_Init 1 */
  
  /* USER CODE END DCACHE1_Init 1 */
  hdcache1.Instance = DCACHE1;
  hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
  if (HAL_DCACHE_Init(&hdcache1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCACHE1_Init 2 */
  
  /* USER CODE END DCACHE1_Init 2 */

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
  hospi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_BYPASS;
  hospi1.Init.Refresh = 0;
  if (HAL_XSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */
  
  /* USER CODE END OCTOSPI1_Init 2 */

}

/**
  * @brief OTFDEC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OTFDEC1_Init(void)
{

  /* USER CODE BEGIN OTFDEC1_Init 0 */
  
  /* USER CODE END OTFDEC1_Init 0 */

  /* USER CODE BEGIN OTFDEC1_Init 1 */
  
  /* USER CODE END OTFDEC1_Init 1 */
  hotfdec1.Instance = OTFDEC1;
  if (HAL_OTFDEC_Init(&hotfdec1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OTFDEC1_Init 2 */
  
  /* USER CODE END OTFDEC1_Init 2 */

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
/******************************************************************************/
/*                           OCTOSPI  Helper                                  */
/******************************************************************************/

static uint32_t OSPI_Write(uint32_t * pBuffer, uint32_t FlashAddress, uint32_t Size)
{
  XSPI_RegularCmdTypeDef  sCommand ={0};
  XSPI_AutoPollingTypeDef sConfig ={0};
  
  if (Size == 0)
  {
    Error_Handler();
  }
  
  sConfig.MatchMode              = HAL_XSPI_MATCH_MODE_AND;
  sConfig.AutomaticStop          = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  sConfig.IntervalTime           = 0x10;
  
  /* Initialize the command */
  sCommand.OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction           = READ_ID_CMD;
  
  sCommand.InstructionMode       = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth      = HAL_XSPI_INSTRUCTION_8_BITS ;
  sCommand.InstructionDTRMode    = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode           = HAL_XSPI_ADDRESS_NONE;
  sCommand.AddressDTRMode        = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytes        = 0x00;
  sCommand.AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.AlternateBytesWidth    = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
  sCommand.DataDTRMode           = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles           = 0;
  sCommand.DQSMode               = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode              = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  /* 1- Enable write operations ----------------------------------------- */
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;
  
  if (HAL_XSPI_SetFifoThreshold(&hospi1, 4) != HAL_OK)
  {
    Error_Handler();
  }
  
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  sConfig.MatchValue           = 0x02;
  sConfig.MatchMask            = 0x02;
  
  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength     = 1;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(&hospi1, &sConfig,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Writing Sequence ----------------------------------------------- */
  if (HAL_XSPI_SetFifoThreshold(&hospi1, 4) != HAL_OK)
  {
    Error_Handler();
  }
  
  sCommand.Instruction = PAGE_PROG_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.Address     = FlashAddress;
  sCommand.DataMode    = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength  = Size;
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_Transmit(&hospi1,(uint8_t *) pBuffer,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for end of program ---- */
  sConfig.MatchValue           = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask            = MEMORY_READY_MASK_VALUE;
  
  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength     = 1;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(&hospi1, &sConfig,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  return 0;
}

/**
* @brief  This function set memory mapped mode.
* @param  None
* @retval None
*/
static uint32_t OSPI_MemoryMap(void)
{
  
  XSPI_RegularCmdTypeDef   sCommand;
  XSPI_MemoryMappedTypeDef sMemMappedCfg;
  
  /* Initialize the command */
  sCommand.OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction           = PAGE_PROG_CMD;
  
  sCommand.InstructionMode       = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth       = HAL_XSPI_INSTRUCTION_8_BITS ;
  sCommand.InstructionDTRMode    = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressDTRMode        = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytes        = 0x00;
  sCommand.AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.AlternateBytesWidth    = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
  sCommand.DataDTRMode           = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles           = 0;
  sCommand.DQSMode               = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode              = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  if (HAL_XSPI_SetFifoThreshold(&hospi1, 4) != HAL_OK)
  {
    Error_Handler();
  }
  
  sCommand.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction   = PAGE_PROG_CMD;
  sCommand.AddressMode   = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth   = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.DataMode      = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles   = 0;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sCommand.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
  sCommand.Instruction        = 0x03;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataMode           = HAL_XSPI_DATA_1_LINE;
  sCommand.DummyCycles        = 0;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  sMemMappedCfg.TimeOutActivation      = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeoutPeriodClock     = 0x40;
  
  if (HAL_XSPI_MemoryMapped(&hospi1, &sMemMappedCfg))
  {
    Error_Handler();
  }
  
  return 0;
}

/**
* @brief  This function reset the OSPI memory.
* @param  None
* @retval None
*/
static void OSPI_ResetMemory(void)
{
  XSPI_RegularCmdTypeDef  sCommand ={0};
  
  /* Initialize the reset enable command */
  sCommand.OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  
  sCommand.InstructionMode       = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth       = HAL_XSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDTRMode    = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode           = HAL_XSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.DataDTRMode           = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles           = 0;
  sCommand.DQSMode               = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode              = HAL_XSPI_SIOO_INST_EVERY_CMD;
  sCommand.DataMode              = HAL_XSPI_DATA_NONE;
  
  sCommand.Instruction = RESET_ENABLE_CMD;
  /* Send the command */
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  
  /* Send the reset memory command */
  sCommand.Instruction = RESET_MEMORY_CMD;
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_Delay(40);
}

static uint32_t OSPI_Erase(void)
{
  XSPI_RegularCmdTypeDef  sCommand ={0};
  XSPI_AutoPollingTypeDef sConfig ={0};
  
  sConfig.MatchMode              = HAL_XSPI_MATCH_MODE_AND;
  sConfig.AutomaticStop          = HAL_XSPI_AUTOMATIC_STOP_ENABLE;
  sConfig.IntervalTime           = 0x10;
  
  /* Initialize the command */
  sCommand.OperationType         = HAL_XSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction           = READ_ID_CMD;
  
  sCommand.InstructionMode       = HAL_XSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionWidth      = HAL_XSPI_INSTRUCTION_8_BITS ;
  sCommand.InstructionDTRMode    = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  sCommand.AddressDTRMode        = HAL_XSPI_ADDRESS_DTR_DISABLE;
  sCommand.AlternateBytes        = 0x00;
  sCommand.AlternateBytesMode    = HAL_XSPI_ALT_BYTES_NONE;
  sCommand.AlternateBytesWidth    = HAL_XSPI_ALT_BYTES_8_BITS;
  sCommand.AlternateBytesDTRMode = HAL_XSPI_ALT_BYTES_DTR_DISABLE;
  sCommand.DataDTRMode           = HAL_XSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles           = 0;
  sCommand.DQSMode               = HAL_XSPI_DQS_DISABLE;
  sCommand.SIOOMode              = HAL_XSPI_SIOO_INST_EVERY_CMD;
  
  
  /* 1- Enable write operations ----------------------------------------- */
  sCommand.Instruction = WRITE_ENABLE_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */
  sConfig.MatchValue           = 0x02;
  sConfig.MatchMask            = 0x02;
  
  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength     = 1;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(&hospi1, &sConfig,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Erasing Sequence ----------------------------------------------- */
  
  sCommand.Instruction = BLOCK_ERASE_CMD;
  sCommand.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
  sCommand.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
  sCommand.Address     = 0;
  sCommand.DataMode    = HAL_XSPI_DATA_NONE;
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for end of erase ------ */
  sConfig.MatchValue           = MEMORY_READY_MATCH_VALUE;
  sConfig.MatchMask            = MEMORY_READY_MASK_VALUE;
  
  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.AddressMode    = HAL_XSPI_ADDRESS_NONE;
  sCommand.DataMode       = HAL_XSPI_DATA_1_LINE;
  sCommand.DataLength     = 1;
  
  if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  if (HAL_XSPI_AutoPolling(&hospi1, &sConfig,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  return 0;
}

/****************************************************************************/
/*                           Cryp Helper                                    */
/****************************************************************************/

void CRYP_Cipher(uint32_t index,uint32_t* pRange, uint32_t StartAdressWritingRegion)
{
  uint32_t tmp;
  uint32_t AESIV[4] = {0};
  uint32_t  j = 0;
  uint32_t Key_tmp[4]={Key[0],Key[1],Key[2],Key[3]};
  CRYP_ConfigTypeDef AES_Conf = {0};
  
  tmp       = Key_tmp[0];
  Key_tmp[0] = Key_tmp[3];
  Key_tmp[3] = tmp;
  tmp       = Key_tmp[1];
  Key_tmp[1] = Key_tmp[2];
  Key_tmp[2] = tmp;
  
  AESIV [3] = ((index%4)<<28) | ((pRange[index*2]+StartAdressWritingRegion)>>4);
  AESIV [2] = FirmwareVersion[index];
  AESIV [1] = Nonce[index/4][0];
  AESIV [0] = Nonce[index/4][1];
  
  MX_AES_Init();
  
  HAL_CRYP_GetConfig(&hcryp, &AES_Conf);
  AES_Conf.pKey =  Key_tmp;
  AES_Conf.pInitVect = AESIV;
  HAL_CRYP_SetConfig(&hcryp, &AES_Conf);
  
  for (j=0; j<(PLAIN_SIZE)/4;j++)
  {
    Plain_AES_test[4*j] = Plain[4*j+3+(StartAdressWritingRegion/4)];
    Plain_AES_test[4*j+3] = Plain[4*j+(StartAdressWritingRegion/4)];
    Plain_AES_test[4*j+1] = Plain[4*j+2+(StartAdressWritingRegion/4)];
    Plain_AES_test[4*j+2] = Plain[4*j+1+(StartAdressWritingRegion/4)] ;
  }
  HAL_CRYP_Encrypt(&hcryp, Plain_AES_test, (PLAIN_SIZE), Cipher, 0x1000);
  
  for (j=0; j<(PLAIN_SIZE)/4;j++)
  {
    tmp = Cipher[4*j];
    Cipher[4*j] = Cipher[4*j+3];
    Cipher[4*j+3] = tmp;
    tmp = Cipher[4*j+1];
    Cipher[4*j+1] = Cipher[4*j+2];
    Cipher[4*j+2] = tmp;
  }
  HAL_CRYP_DeInit(&hcryp);
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
