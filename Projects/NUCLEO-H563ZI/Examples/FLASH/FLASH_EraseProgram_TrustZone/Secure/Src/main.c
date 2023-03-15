/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    FLASH/FLASH_EraseProgram_TrustZone/Secure/Src/main.c
  * @author  MCD Application Team
  * @brief   Main secure program body
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
/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  0x08100000UL

#define FLASH_USER_SEC_START_ADDR   (FLASH_BASE + (FLASH_SECTOR_SIZE * 48))                       /* Start @ of user Flash area in Bank1 */
#define FLASH_USER_SEC_END_ADDR     (FLASH_BASE + FLASH_BANK_SIZE - 1)                            /* End @ of user Flash area (before NSC area) */

#define FLASH_USER_NS_START_ADDR    ((FLASH_BASE_NS + FLASH_BANK_SIZE) + (FLASH_SECTOR_SIZE * 6)) /* Start @ of user Flash area in Bank2 */
#define FLASH_USER_NS_END_ADDR      (FLASH_USER_NS_START_ADDR + FLASH_SECTOR_SIZE - 1)            /* End @ of user Flash area in Bank2   */

#define FLASH_READ_ONLY_START_ADDR  (uint32_t)0x08FFF800
#define FLASH_READ_ONLY_END_ADDR    (uint32_t)0x08FFFFFF
/* USER CODE END PD */

/* USER CODE BEGIN VTOR_TABLE */

/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */

#define VTOR_TABLE_NS_START_ADDR  0x08100000UL

/* USER CODE END VTOR_TABLE*/

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern funcptr_NS pSecureErrorCallback;
/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t FirstSector = 0, NbOfSectors = 0, BankNumber = 0;
uint32_t Address = 0, SectorError = 0;
__IO uint32_t MemoryProgramStatus = 0;
uint32_t Index= 0;
__IO uint32_t data64 = 0;
uint32_t QuadWord[4] = { 0x12345678,
                         0x87654321,
                         0x12344321,
                         0x56788765
                       };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void MX_GPIO_Init(void);
static void MX_GTZC_S_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t GetSector(uint32_t Address);
static uint32_t GetSector_NS(uint32_t Addr);
static uint32_t GetBank(uint32_t Address);
static uint32_t GetBank_NS(uint32_t Addr);
static uint32_t Check_Program(uint32_t StartAddress, uint32_t EndAddress, uint32_t *Data);
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

  /* Initialize LED1 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /*IO attributes management functions for LED1 and LED3 */
  HAL_GPIO_ConfigPinAttributes(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_NSEC);


  /* USER CODE END Init */

  /* GTZC initialisation */
  MX_GTZC_S_Init();

  /* USER CODE BEGIN SysInit */
 /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* MPU Configuration--------------------------------------------------------*/
  /* By default, all the AHB memory range is cacheable. For regions where caching is not
     practical (Read only area), MPU has to be used to disable local cacheability.
  */
  MPU_Config();

  /* Disable instruction cache prior to internal cacheable memory update   */  
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    Error_Handler();
  }

 /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_SEC_START_ADDR and FLASH_USER_SEC_END_ADDR) ***********/

  /* Get the 1st sector to erase */
  FirstSector = GetSector(FLASH_USER_SEC_START_ADDR);

  /* Get the number of sectors to erase from 1st sector */
  NbOfSectors = GetSector(FLASH_USER_SEC_END_ADDR) - FirstSector + 1;

  /* Get the bank */
  BankNumber = GetBank(FLASH_USER_SEC_START_ADDR);

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.Banks         = BankNumber;
  EraseInitStruct.Sector        = FirstSector;
  EraseInitStruct.NbSectors     = NbOfSectors;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /*
      Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
    */
    Error_Handler();
  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_SEC_START_ADDR and FLASH_USER_SEC_END_ADDR) ***********/

  Address = FLASH_USER_SEC_START_ADDR;

  while (Address < FLASH_USER_SEC_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, Address, ((uint32_t)QuadWord)) == HAL_OK)
    {
      Address = Address + 16; /* increment to next quad word*/
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      Error_Handler();
    }
  }

  /* Enable instruction cache with no security attribute at GTZC level  */
  /* to let the instruction cache being updated by the non-secure       */
  /* application to insure memory programming into cacheable memory     */
  /* with cache disabled                                                */
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of quadwords not programmed correctly ******/
  MemoryProgramStatus = Check_Program(FLASH_USER_SEC_START_ADDR, FLASH_USER_SEC_END_ADDR, QuadWord);
  if( MemoryProgramStatus != 0)
  {
    Error_Handler();
  }

  /* In this part of example, flash Bank2 non-secure will be erased        */
  /* from Bank1 secure. Once this operation is finished, sector quad-word  */
  /* programming operation will be performed in the non-secure part of     */
  /* the Flash memory. The written data is then read back and checked      */
  
  /* Disable instruction cache prior to internal cacheable memory update   */  
  if (HAL_ICACHE_Disable() != HAL_OK)
  {
    Error_Handler();
  }

   /* Get the 1st sector to erase */
  FirstSector   = GetSector_NS(FLASH_USER_NS_START_ADDR);

   /* Get the number of sectors to erase from 1st sector */
  NbOfSectors = GetSector_NS(FLASH_USER_NS_END_ADDR) - FirstSector + 1;

  /* Get the bank */
  BankNumber = GetBank_NS(FLASH_USER_NS_START_ADDR);

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS_NS;
  EraseInitStruct.Banks     = BankNumber;
  EraseInitStruct.Sector    = FirstSector;
  EraseInitStruct.NbSectors   = NbOfSectors;
  
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Error occurred while sector erase */
    Error_Handler();
  }
  
  Address = FLASH_USER_NS_START_ADDR;
     
  while (Address < FLASH_USER_NS_END_ADDR)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD_NS, Address, ((uint32_t)QuadWord)) == HAL_OK)
    {
      Address = Address + 16;  /* increment to the next Flash word */
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      Error_Handler();
    }
  }
    
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Enable instruction cache with no security attribute at GTZC level  */
  /* to let the instruction cache being updated by the non-secure       */
  /* application to insure memory programming into cacheable memory     */
  /* with cache disabled                                                */
  MX_ICACHE_Init();

  /* Check if the programmed data is OK
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  MemoryProgramStatus = Check_Program(FLASH_USER_NS_START_ADDR, FLASH_USER_NS_END_ADDR, QuadWord);
  /* Check if there is an issue to program data */
  if (MemoryProgramStatus != 0)
  {
    Error_Handler();
  }

  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /* USER CODE END 2 */

  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */
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
  * @brief  Non-secure call function
  *         This function is responsible for Non-secure initialization and switch
  *         to non-secure state
  * @retval None
  */
static void NonSecure_Init(void)
{
  funcptr_NS NonSecure_ResetHandler;

  SCB_NS->VTOR = VTOR_TABLE_NS_START_ADDR;

  /* Set non-secure main stack (MSP_NS) */
  __TZ_set_MSP_NS((*(uint32_t *)VTOR_TABLE_NS_START_ADDR));

  /* Get non-secure reset handler */
  NonSecure_ResetHandler = (funcptr_NS)(*((uint32_t *)((VTOR_TABLE_NS_START_ADDR) + 4U)));

  /* Start non-secure state software application */
  NonSecure_ResetHandler();
}

/**
  * @brief GTZC_S Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_S_Init(void)
{

  /* USER CODE BEGIN GTZC_S_Init 0 */

  /* USER CODE END GTZC_S_Init 0 */

  MPCBB_ConfigTypeDef MPCBB_Area_Desc = {0};

  /* USER CODE BEGIN GTZC_S_Init 1 */

  /* USER CODE END GTZC_S_Init 1 */
  MPCBB_Area_Desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_Area_Desc.InvertSecureState = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[0] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[1] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[2] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[3] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[4] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[5] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[6] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[7] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[8] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[9] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[10] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[11] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[12] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[13] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[14] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[15] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[16] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[17] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[18] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_SecConfig_array[19] =   0x00000000;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[0] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[1] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[2] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[3] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[4] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[5] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[6] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[7] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[8] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[9] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[10] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[11] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[12] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[13] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[14] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[15] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[16] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[17] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[18] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_PrivConfig_array[19] =   0xFFFFFFFF;
  MPCBB_Area_Desc.AttributeConfig.MPCBB_LockConfig_array[0] =   0x00000000;
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM3_BASE, &MPCBB_Area_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* Clear all illegal access flags in GTZC TZIC */
  if(HAL_GTZC_TZIC_ClearFlag(GTZC_PERIPH_ALL) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Enable all illegal access interrupts in GTZC TZIC */
  if(HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_ALL) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  /* USER CODE END GTZC_S_Init 2 */

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
  * @brief  GTZC TZIC interrupt callback.
  * @param  PeriphId Peripheral identifier triggering the illegal access.
  *         This parameter can be a value of @ref GTZC_TZSC_TZIC_PeriphId
  * @retval None.
  */
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
{
  funcptr_NS callback_NS; /* non-secure callback function pointer */

  /* Prevent unused argument(s) compilation warning */
  UNUSED(PeriphId);

  if(pSecureErrorCallback != (funcptr_NS)NULL)
  {
   /* return function pointer with cleared LSB */
   callback_NS = (funcptr_NS)cmse_nsfptr_create(pSecureErrorCallback);

   callback_NS();
  }
  else
  {
    /* Something went wrong in test case */
    while(1);
  }
}

/**
  * @brief  Gets the sector of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address >= FLASH_BASE) && (Address < FLASH_BASE + FLASH_BANK_SIZE))
  {
    sector = (Address & ~FLASH_BASE) / FLASH_SECTOR_SIZE;
  }
  else if ((Address >= FLASH_BASE + FLASH_BANK_SIZE) && (Address < FLASH_BASE + FLASH_SIZE))
  {
    sector = ((Address & ~FLASH_BASE) - FLASH_BANK_SIZE) / FLASH_SECTOR_SIZE;
  }
  else
  {
    sector = 0xFFFFFFFF; /* Address out of range */
  }

  return sector;
}

/**
  * @brief  Gets the sector of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The sector of a given address
  */
static uint32_t GetSector_NS(uint32_t Addr)
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

  if((Addr >= FLASH_BASE) && (Addr < FLASH_BASE + FLASH_BANK_SIZE))
  {
    bank = FLASH_BANK_1;
  }
  else if ((Addr >= FLASH_BASE + FLASH_BANK_SIZE) && (Addr < FLASH_BASE + FLASH_SIZE))
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
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank_NS(uint32_t Addr)
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

/**
  * @brief  Configure the MPU attributes as non-cacheable for Read only area
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
  region.BaseAddress      = FLASH_READ_ONLY_START_ADDR;
  region.LimitAddress     = FLASH_READ_ONLY_END_ADDR;
  region.AccessPermission = MPU_REGION_ALL_RW;
  region.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  region.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  HAL_MPU_ConfigRegion(&region);
  HAL_MPU_ConfigRegion_NS(&region);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  HAL_MPU_Enable_NS(MPU_PRIVILEGED_DEFAULT);
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
