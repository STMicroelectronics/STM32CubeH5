/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_EDATA_EraseProgram/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a description of how to erase and program the
  *          STM32H5xx high-cycle FLASH area.
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

/* Start @ of user Flash eData area */
#define EDATA_USER_START_ADDR   ADDR_EDATA1_STRT_7
/* End @ of user Flash eData area */
/* (FLASH_EDATA_SIZE/16) is the sector size of high-cycle area (6KB) */
#define EDATA_USER_END_ADDR     (ADDR_EDATA1_STRT_7 + (8*(FLASH_EDATA_SIZE/16)) - 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t FirstSector = 0, NbOfSectors = 0, EndSector = 0;
uint32_t Address = 0, SectorError = 0;
uint32_t Index= 0;

uint32_t offset = 2;
uint16_t FlashHalfWord[1] = { 0xAA55 };
uint16_t FlashHalfWord_FF[1] = { 0xFFFF };

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/* Variable used for OB Program procedure */
FLASH_OBProgramInitTypeDef FLASH_OBInitStruct;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t GetSector_EDATA(uint32_t Address);
static uint32_t GetBank_EDATA(uint32_t Address);
static void MPU_Config(void);
static uint32_t Check_Flash_Content(uint32_t StartAddress, uint32_t EndAddress, uint16_t *Data);

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
  /* Configure the system clock to 250 MHz */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* MPU Configuration--------------------------------------------------------*/
  /* By default, all the AHB memory range is cacheable. For regions where caching is not
     practical (High-cycle data area), MPU has to be used to disable local cacheability.
  */
  MPU_Config();
  /* Initialize LED1, LED2 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Flash option bytes to enable the flash option control register access */
  HAL_FLASH_OB_Unlock();

  /* Erase the EDATA Flash area
    (area defined by EDATA_USER_START_ADDR and EDATA_USER_END_ADDR) ***********/

  /* Get the 1st sector of FLASH high-cycle data area */
  FirstSector = GetSector_EDATA(EDATA_USER_START_ADDR);

  /* Get the last sector of FLASH high-cycle data area */
  EndSector = GetSector_EDATA(EDATA_USER_END_ADDR);

  /* Get the number of sectors */
  NbOfSectors = EndSector - FirstSector + 1;

  /* Configure 8 sectors for FLASH high-cycle data */
  FLASH_OBInitStruct.OptionType = OPTIONBYTE_EDATA;
  FLASH_OBInitStruct.Banks = GetBank_EDATA(EDATA_USER_START_ADDR);
  FLASH_OBInitStruct.EDATASize = NbOfSectors;
  if(HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start option byte load operation after successful programming operation */
  HAL_FLASH_OB_Launch();

  /* Get the first sector of FLASH high-cycle data area */
  FirstSector = GetSector_EDATA(EDATA_USER_START_ADDR);

  /* Get the last sector of FLASH high-cycle data area */
  EndSector = GetSector_EDATA(EDATA_USER_END_ADDR);

  /* Get the number of sectors to erase */
  NbOfSectors = EndSector - FirstSector + 1;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.Banks = GetBank_EDATA(EDATA_USER_START_ADDR);
  EraseInitStruct.Sector = FirstSector;
  EraseInitStruct.NbSectors = NbOfSectors;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /*
      Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
    */
    /* Infinite loop */
    Error_Handler();
  }

  /* Program the FLASH high-cycle data area of BANK1 */
  Address = EDATA_USER_START_ADDR;

  while(Address < EDATA_USER_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD_EDATA, Address, (uint32_t)FlashHalfWord) == HAL_OK)
    {
      Address = Address + offset; /* increment for the next Flash word*/
    }
    else
    {
      /* Error occurred while half word Programming */
      Error_Handler();
    }
  }

  /* Check if the programmed data is OK */
  if (Check_Flash_Content(EDATA_USER_START_ADDR, EDATA_USER_END_ADDR, FlashHalfWord) != 0)
  {
    Error_Handler();
  }

  /* Disable the FLASH High-cycle data */
  FLASH_OBInitStruct.OptionType = OPTIONBYTE_EDATA;
  FLASH_OBInitStruct.Banks = GetBank_EDATA(EDATA_USER_START_ADDR);
  FLASH_OBInitStruct.EDATASize = 0;
  if (HAL_FLASHEx_OBProgram(&FLASH_OBInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start option byte load operation after successful programming operation */
  HAL_FLASH_OB_Launch();

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation)
  */
  HAL_FLASH_Lock();

  /* Lock the Flash control option to restrict register access */
  HAL_FLASH_OB_Lock();

  /* No error detected. Switch on LED1*/
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
/**
  * @brief  Gets the sector of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The sector of a given address
  */
static uint32_t GetSector_EDATA(uint32_t Address)
{
  uint32_t sector = 0;
  uint32_t edataSectorSize = 0;

  /*
    (FLASH_EDATA_SIZE/2) is the size of high-cycle area of flash BANK1.
    Flash high-cycle area have 8 sectors in each Bank.
  */
  edataSectorSize = (FLASH_EDATA_SIZE / 2) / 8;
  /* Check if the address is located in the FLASH high-cycle data area of BANK1 */
  if((Address >= FLASH_EDATA_BASE) && (Address < FLASH_EDATA_BASE + (FLASH_EDATA_SIZE / 2)))
  {
    sector = (Address & ~FLASH_EDATA_BASE) / edataSectorSize;
    sector += 120;
  }
  /* Check if the address is located in the FLASH high-cycle data area of BANK2 */
  else if ((Address >= FLASH_EDATA_BASE + (FLASH_EDATA_SIZE / 2)) && (Address < FLASH_EDATA_BASE + FLASH_EDATA_SIZE))
  {
    sector = ((Address & ~FLASH_EDATA_BASE) - (FLASH_EDATA_SIZE / 2)) / edataSectorSize;
    sector += 120;
  }
  else
  {
    sector = 0xFFFFFFFF; /* Address out of range */
  }

  return sector;
}

/**
  * @brief  Gets the bank of a given address in EDATA area
  * @param  Addr: Address of A given address in EDATA area
  * @retval The bank of a given address in EDATA area
  */
static uint32_t GetBank_EDATA(uint32_t Addr)
{
  uint32_t bank = 0;

  /* (FLASH_EDATA_SIZE/2) is the size of high-cycle area of flash BANK1 */
  if((Addr >= FLASH_EDATA_BASE) && (Addr < FLASH_EDATA_BASE + (FLASH_EDATA_SIZE/2)))
  {
    bank = FLASH_BANK_1;
  }
  else if ((Addr >= FLASH_EDATA_BASE + (FLASH_EDATA_SIZE/2)) && (Addr < FLASH_EDATA_BASE + FLASH_EDATA_SIZE))
  {
    bank = FLASH_BANK_2;
  }
  else
  {
    bank = 0xFFFFFFFF; /* Address out of range */
  }
  return bank;
}

/**
  * @brief  Configure the MPU attributes as non-cacheable for Flash high-cycle data area
  * @note   The Base Address is Flash high-cycle data area
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Attributes_InitTypeDef   attr;
  MPU_Region_InitTypeDef       region;

  /* Disable MPU before perloading and config update */
  HAL_MPU_Disable();

  /* Define cacheable memory via MPU */
  attr.Number             = MPU_ATTRIBUTES_NUMBER0;
  attr.Attributes         = 0 ;
  HAL_MPU_ConfigMemoryAttributes(&attr);

  /* BaseAddress-LimitAddress configuration */
  region.Enable           = MPU_REGION_ENABLE;
  region.Number           = MPU_REGION_NUMBER0;
  region.AttributesIndex  = MPU_ATTRIBUTES_NUMBER0;
  region.BaseAddress      = EDATA_USER_START_ADDR;
  region.LimitAddress     = EDATA_USER_END_ADDR;
  region.AccessPermission = MPU_REGION_ALL_RW;
  region.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  region.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  HAL_MPU_ConfigRegion(&region);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  Check program operation.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data Expected data
  * @retval FailCounter
  */
static uint32_t Check_Flash_Content(uint32_t StartAddress, uint32_t EndAddress, uint16_t *Data)
{
  uint32_t Address;
  uint32_t FailCounter = 0;
  uint16_t data16;

  Address = StartAddress;

  while(Address < EndAddress)
  {
    for(Index = 0; Index<4; Index++)
    {
      data16 = *(uint16_t*)Address;
      if(data16 != Data[0])
      {
        FailCounter++;
      }
      Address += offset;
    }
  }
  return FailCounter;
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
      /* Turn on LED3 */
      BSP_LED_On(LED3);
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
