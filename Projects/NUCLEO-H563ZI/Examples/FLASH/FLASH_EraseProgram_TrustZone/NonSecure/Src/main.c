/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_EraseProgram_TrustZone/NonSecure/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body (non-secure)
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
#include "stm32h5xx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define FLASH_USER_NS_START_ADDR    ((FLASH_BASE_NS + FLASH_BANK_SIZE) + (FLASH_SECTOR_SIZE * 6)) /* Start @ of user Flash area in Bank2 */
#define FLASH_USER_NS_END_ADDR      (FLASH_USER_NS_START_ADDR + FLASH_SECTOR_SIZE - 1)            /* End @ of user Flash area in Bank2   */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Variable used for Erase procedure */
static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t FirstSector = 0, NbOfSectors = 0, BankNumber = 0;
uint32_t Address = 0, SectorError = 0;
__IO uint32_t MemoryProgramStatus = 0;
uint32_t QuadWord[4] = { 0x12345678,
                         0x87654321,
                         0x12344321,
                         0x56788765
                       };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void SecureFault_Callback(void);
static void SecureError_Callback(void);
static uint32_t GetSector(uint32_t Address);
static uint32_t GetBank(uint32_t Addr);
static uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data);
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
  /* Register SecureFault callback defined in non-secure and to be called by secure handler */
  SECURE_RegisterCallback(SECURE_FAULT_CB_ID, (void *)SecureFault_Callback);

  /* Register SecureError callback defined in non-secure and to be called by secure handler */
  SECURE_RegisterCallback(GTZC_ERROR_CB_ID, (void *)SecureError_Callback);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_NS_START_ADDR and FLASH_USER_NS_END_ADDR) ***********/

  /* Get the 1st sector to erase */
  FirstSector = GetSector(FLASH_USER_NS_START_ADDR);

  /* Get the number of sectors to erase from 1st sector */
  NbOfSectors = GetSector(FLASH_USER_NS_END_ADDR) - FirstSector + 1;

  /* Get the bank */
  BankNumber = GetBank(FLASH_USER_NS_START_ADDR);

  /* Fill EraseInit structure */
  /* Bank 1 configures as secure and Bank 2 as non-secure, so non-secure erase is done in Bank 2 */
  /* Only non-secure area can be erased of programmed by non-secure code */
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.Banks       = BankNumber;
  EraseInitStruct.Sector      = FirstSector;
  EraseInitStruct.NbSectors   = NbOfSectors;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /*
      Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
    */
     /* Make LED_RED ON to indicate error in Erase operation */
    Error_Handler();

  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_NS_START_ADDR and FLASH_USER_NS_END_ADDR) ***********/

  Address = FLASH_USER_NS_START_ADDR;

  while (Address < FLASH_USER_NS_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, ((uint32_t)QuadWord)) == HAL_OK)
    {
      Address = Address + 16;  /* increment to the next Flash word */
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      /* Make LED_RED ON to indicate error in Write operation */
      Error_Handler();
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  MemoryProgramStatus = Check_Program(FLASH_USER_NS_START_ADDR, FLASH_USER_NS_END_ADDR, QuadWord);


  /* Check if there is an issue to program data */
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED_GREEN */
    BSP_LED_On(LED_GREEN);
  }
  else
  {
    /* Error detected. LED_RED will be ON */
    Error_Handler();
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

/* USER CODE BEGIN 4 */

/**
  * @brief  Callback called by secure code following a secure fault interrupt
  * @note   This callback is called by secure code thanks to the registration
  *         done by the non-secure application with non-secure callable API
  *         SECURE_RegisterCallback(SECURE_FAULT_CB_ID, (void *)SecureFault_Callback);
  * @retval None
  */
static void SecureFault_Callback(void)
{
  /* Go to infinite loop when Secure fault generated by IDAU/SAU check */
  /* because of illegal access */
  while (1)
  {
  }
}

/**
  * @brief  Callback called by secure code following a GTZC TZIC secure interrupt (GTZC_IRQn)
  * @note   This callback is called by secure code thanks to the registration
  *         done by the non-secure application with non-secure callable API
  *         SECURE_RegisterCallback(GTZC_ERROR_CB_ID, (void *)SecureError_Callback);
  * @retval None
  */
static void SecureError_Callback(void)
{
  /* Go to infinite loop when Secure error generated by GTZC check */
  /* because of illegal access */
  while (1)
  {
  }
}

/**
  * @brief  Gets the sector of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Addr)
{
  uint32_t sector = 0;

  if (Addr < (FLASH_BASE_NS + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    sector = (Addr - FLASH_BASE_NS) / FLASH_SECTOR_SIZE;
  }
  else
  {
    /* Bank 2 */
    sector = (Addr - (FLASH_BASE_NS + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE;
  }

  return sector;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  if((Addr >= FLASH_BASE_NS) && (Addr < FLASH_BASE_NS + FLASH_BANK_SIZE))
  {
    bank = FLASH_BANK_1;
  }
  else if ((Addr >= FLASH_BASE_NS + FLASH_BANK_SIZE) && (Addr < FLASH_BASE_NS + FLASH_SIZE))
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
  * @brief  Check program operation.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data Expected data
  * @retval FailCounter
  */
static uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data)
{
  uint32_t Address;
  uint32_t index, FailCounter = 0;
  uint32_t data32;

  /* check the user Flash area word by word */
  Address = StartAddress;

  while(Address < EndAddress)
  {
    for(index = 0; index < 4; index++)
    {
      data32 = *(uint32_t*)Address;
      if(data32 != Data[index])
      {
        FailCounter++;
        return FailCounter;
      }
      Address += 4;
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
