/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_OBK_EraseProgram/Src/main.c
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t Address = 0, status = 0, SectorError = 0;
__IO uint32_t EndofProgram;
uint32_t FlashWord[4] =
                      { 0x55555555, 0x66666666,
                        0x77777777, 0x88888888,
                      };

uint32_t FlashWord_FF[4] =
                      { 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF,
                      };

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
uint32_t Program_Data_OBK(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord);
uint32_t Program_Data_OBK_IT(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord);
uint32_t Program_Data_OBK_ALT(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord);
static uint32_t Check_Flash_Content(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data);
static void MPU_Config(void);
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
     practical (HDPL OBK area), MPU has to be used to disable local cacheability.
  */
  MPU_Config();

  /* Initialize LED1, LED2 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Unlock the Flash option bytes Keys to enable the flash OBK register access */
  HAL_FLASHEx_OBK_Unlock();

  /* Enable NVIC for FLASH Interuption */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(FLASH_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(FLASH_IRQn);

  /* Erase the OBK alternate area */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_OBK_ALT;
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Infinite loop */
    Error_Handler();
  }

  /* Swap OBK, HDPL0 data will be copied from current to alternate OBK sector */
  if(HAL_FLASHEx_OBK_Swap(FLASH_OBK_SWAP_OFFSET_HDPL0) !=  HAL_OK)
  {
    Error_Handler();
  }

  /********************************************** Writing HDPL1 OB-Keys ******************************************/

  /* Check the Flash OBK area is full 0xFF */
  if(Check_Flash_Content(HDPL1_START_ADRESS, HDPL1_END_ADRESS, FlashWord_FF) != 0)
  {
   Error_Handler();
  }

  /* Program Flash OBK alternate area in polling mode */
  if(Program_Data_OBK_ALT(HDPL1_START_ADRESS, HDPL1_END_ADRESS, FlashWord) != 0)
  {
    Error_Handler();
  }

  /* Swap OBK, HDPL0 data will be copied from current to alternate OBK sector */
  if(HAL_FLASHEx_OBK_Swap(FLASH_OBK_SWAP_OFFSET_HDPL0) !=  HAL_OK)
  {
    Error_Handler();
  }

  /* Check the Flash OBK area */
  if(Check_Flash_Content(HDPL1_START_ADRESS, HDPL1_END_ADRESS, FlashWord) != 0)
  {
   Error_Handler();
  }

  /********************************************** Writing HDPL2 OB-Keys ******************************************/

  /* Increment OBK-HDPL value */
  HAL_SBS_SetOBKHDPL(SBS_OBKHDPL_INCR_1);

  /* Program Flash OBK current area in interrupt mode */
  if(Program_Data_OBK_IT(HDPL2_START_ADRESS, HDPL2_END_ADRESS, FlashWord) != 0)
  {
    Error_Handler();
  }

  /* Check the Flash OBK area */
  if(Check_Flash_Content(HDPL2_START_ADRESS, HDPL2_END_ADRESS, FlashWord) != 0)
  {
    Error_Handler();
  }

  /********************************************** Writing HDPL3 OB-Keys ******************************************/

  /* Increment OBK-HDPL value */
  HAL_SBS_SetOBKHDPL(SBS_OBKHDPL_INCR_2);

  /* Check the Flash OBK area */
  if(Check_Flash_Content(HDPL3_START_ADRESS, HDPL3_END_ADRESS, FlashWord_FF) != 0)
  {
   Error_Handler();
  }

  /* Program Flash OBK current area in polling mode */
  if(Program_Data_OBK(HDPL3_START_ADRESS, HDPL3_END_ADRESS, FlashWord) != 0)
  {
    Error_Handler();
  }

  /* Check the Flash OBK area */
  if(Check_Flash_Content(HDPL3_START_ADRESS, HDPL3_END_ADRESS, FlashWord) != 0)
  {
    Error_Handler();
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation)
  */
  HAL_FLASH_Lock();

  /* Lock the Flash control option bytes keys to restrict register access */
  HAL_FLASHEx_OBK_Lock();

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
  * @brief  program Flash OBK current area in polling mode.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data data to be programmed
  * @retval Status
  */
uint32_t Program_Data_OBK(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord)
{
  uint32_t Address;
  uint32_t status;
  uint32_t offset = 16;

  Address = StartAddress;

  while(Address < EndAddress)
  {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD_OBK, Address, (uint32_t)FlashWord);
    if(status == HAL_OK)
    {
      Address = Address + offset; /* increment for the next Flash word*/
    }
    else
    {
      /* Error occurred while programming */
      Error_Handler();
    }
  }
  return 0;
}

/**
  * @brief  program Flash OBK current area in interrupt mode.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data data to be programmed
  * @retval Status
  */
uint32_t Program_Data_OBK_IT(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord)
{
  uint32_t Address;
  uint32_t status;
  uint32_t offset = 16;

  Address = StartAddress;

  while(Address < EndAddress)
  {
    EndofProgram = 0;
    status = HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_QUADWORD_OBK, Address, (uint32_t)FlashWord);
    while(!EndofProgram) {};
    if(status == HAL_OK)
    {
      Address = Address + offset; /* increment for the next Flash word*/
    }
    else
    {
      /* Error occurred while programming */
      Error_Handler();
    }
  }
  return 0;
}

/**
  * @brief  program Flash OBK Alternate area in polling mode.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data data to be programmed
  * @retval Status
  */
uint32_t Program_Data_OBK_ALT(uint32_t StartAddress, uint32_t EndAddress, uint32_t *FlashWord)
{
  uint32_t Address;
  uint32_t status;
  uint32_t offset = 16;

  Address = StartAddress;

  while(Address < EndAddress)
  {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD_OBK_ALT, Address, (uint32_t)FlashWord);

    if(status == HAL_OK)
    {
      Address = Address + offset; /* increment for the next Flash word*/
    }
    else
    {
      /* Error occurred while programming */
      Error_Handler();
    }
  }
  return 0;
}

/**
  * @brief  FLASH interrupt user callback
  * @retval None
  */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* This variable count the number of Program/Erase Operation */
  EndofProgram ++;
}

/**
  * @brief  Check program operation.
  * param StartAddress Area start address
  * param EndAddress Area end address
  * param Data Expected data
  * @retval FailCounter
  */
static uint32_t Check_Flash_Content(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data)
{
  uint32_t Address;
  uint32_t index, FailCounter = 0;
  uint32_t data32;

  Address = StartAddress;

  while(Address < EndAddress)
  {
    for(index = 0; index < 4 ; index++)
    {
      data32 = *(uint32_t*)Address;
      if(data32 != Data[index])
      {
        FailCounter++;
      }
      Address += 4;
    }
  }
  return FailCounter;
}

/**
  * @brief  Configure the MPU attributes as non-cacheable for OBK area
  * @note   The Base Address is Flash OBK area
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
  region.BaseAddress      = HDPL1_START_ADRESS;
  region.LimitAddress     = HDPL3_END_ADRESS;
  region.AccessPermission = MPU_REGION_ALL_RW;
  region.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  region.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  HAL_MPU_ConfigRegion(&region);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
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
