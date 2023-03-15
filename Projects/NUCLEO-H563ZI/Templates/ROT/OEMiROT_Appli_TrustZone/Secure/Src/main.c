/**
  ******************************************************************************
  * @file    Templates/ROT/OEMiROT_Appli_TrustZone/Secure/Src/main.c
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32H5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */
extern funcptr_NS pSecureErrorCallback;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table                             */
#define VTOR_TABLE_NS_START_ADDR  (FLASH_BASE_NS + S_CODE_OFFSET + \
                                   S_CODE_SIZE + IMAGE_HEADER_SIZE)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void NonSecure_Init(void);
static void SystemIsolation_Config(void);
static void CACHE_Enable(void);
/* static void SystemClock_Config(void);  provided as example if secure sets clocks */

/**
  * @brief  Main program
  * @retval None
  */
int main(void)
{
  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32h563xx.h file's definitions. */

  /* STM32H5xx **SECURE** HAL library initialization:
       - Configure the Flash prefetch
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Function where we deactivate MPU for both Secure and Non Secure application */
  HAL_MPU_Disable();

  /* This project template calls CACHE_Enable() in order to enable the Instruction
     and Data Cache. This function is provided as template implementation that
     User may integrate in his application in order to enhance performance */

  /* Enable the Instruction Cache */
  CACHE_Enable();

  /* Secure/Non-secure Memory and Peripheral isolation configuration */
  SystemIsolation_Config();

  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* Secure application may configure the System clock here */
  /* SystemClock_Config(); */

  /* Add your secure application code here prior to non-secure initialization
     */


  /* Secure SysTick should rather be suspended before calling non-secure  */
  /* in order to avoid wake-up from sleep mode entered by non-secure      */
  /* The Secure SysTick shall be resumed on non-secure callable functions */
  HAL_SuspendTick();

  /*************** Setup and jump to non-secure *******************************/
  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */
  while (1)
  {
  }
}

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

/* Private functions ---------------------------------------------------------*/

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
  * @brief  System Isolation Configuration
  *         This function is responsible for Memory and Peripheral isolation
  *         for secure and non-secure application parts
  * @retval None
  */
static void SystemIsolation_Config(void)
{
  uint32_t index;
  MPCBB_ConfigTypeDef MPCBB_desc = {0};

  /* Enable GTZC peripheral clock */
  __HAL_RCC_GTZC1_CLK_ENABLE();

  /* -------------------------------------------------------------------------*/
  /*                   Memory isolation configuration                         */
  /* Initializes the memory that secure application books for non secure      */
  /* -------------------------------------------------------------------------*/

  /* -------------------------------------------------------------------------*/
  /* Internal RAM :                                                           */
  /* The booking is done through GTZC MPCBB.                                  */
  /* Internal SRAMs are secured by default and configured by block            */
  /* of 512 bytes.                                                            */
  /* Internal SRAM3 (starting from 0x20050000) will be configured as          */
  /* fully non-secure.                                                        */

  MPCBB_desc.SecureRWIllegalMode = GTZC_MPCBB_SRWILADIS_ENABLE;
  MPCBB_desc.InvertSecureState   = GTZC_MPCBB_INVSECSTATE_NOT_INVERTED;
  MPCBB_desc.AttributeConfig.MPCBB_LockConfig_array[0] = 0x00000000U;  /* Unlocked configuration */

  for (index=0; index<20; index++)
  {
    /* Non-secure blocks */
    MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[index] = 0x00000000U;

    /* Privilege blocks */
    MPCBB_desc.AttributeConfig.MPCBB_PrivConfig_array[index] = 0xFFFFFFFFU;
  }

  if (HAL_GTZC_MPCBB_ConfigMem(SRAM3_BASE, &MPCBB_desc) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* -------------------------------------------------------------------------*/
  /* Internal Flash */
  /* The booking is done in both IDAU/SAU and FLASH interface */

  /* Setup done based on Flash dual-bank mode described with 1 area per bank  */
  /* Non-secure Flash memory area starting from 0x08100000 (Bank2)            */
  /* Flash memory is secured by default and modified with Option Byte Loading */
  /* Insure SECWM2_PSTRT > SECWM2_PEND in order to have all Bank2 non-secure  */

  /* -------------------------------------------------------------------------*/
  /* External OctoSPI memories */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */

  /* -------------------------------------------------------------------------*/
  /* External NOR/FMC memory */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */

  /* -------------------------------------------------------------------------*/
  /* External NAND/FMC memory */
  /* The booking is done in both IDAU/SAU and GTZC MPCWM interface */

  /* Default secure configuration */
  /* Else need to use HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes() */

  /* -------------------------------------------------------------------------*/
  /*                   Peripheral isolation configuration                     */
  /* Initializes the peripherals and features that secure application books   */
  /* for secure (RCC, PWR, RTC, EXTI, DMA, OTFDEC, etc..) or leave them to    */
  /* non-secure (GPIO (secured by default))                                   */
  /* -------------------------------------------------------------------------*/

  /* All IOs are by default allocated to secure                           */
  /* Release PB.00 I/O for LED_GREEN control on non-secure                */
   __HAL_RCC_GPIOB_CLK_ENABLE();
   HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_0, GPIO_PIN_NSEC);
  /* Leave the GPIO clock enabled to let non-secure having I/O control    */

  /* -------------------------------------------------------------------------*/
  /*                   Activation of illegal access errors                    */
  /* -------------------------------------------------------------------------*/

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

  /* Enable GTZC secure interrupt */
  HAL_NVIC_SetPriority(GTZC_IRQn, 0, 0); /* Highest priority level */
  HAL_NVIC_EnableIRQ(GTZC_IRQn);
}

#if 0   /* provided as example if secure sets clocks */
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSE BYPASS)
  *            SYSCLK(Hz)                     = 250000000  (CPU Clock)
  *            HCLK(Hz)                       = 250000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  250MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  250MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  250MHz)
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 4
  *            PLL_N                          = 250
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* The voltage scaling allows optimizing the power consumption when the device is
  clocked below the maximum system frequency, to update the voltage scaling value
  regarding system frequency refer to product datasheet.
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Bypass and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 250;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK3
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2| RCC_CLOCKTYPE_PCLK3);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}
#endif /* 0 */

/**
  * @brief  Enable ICACHE with 2-ways set-associative configuration.
  * @param  None
  * @retval None
  */
static void CACHE_Enable(void)
{
  /* Enable instruction cache (default 2-ways set associative cache) */
  HAL_ICACHE_Enable();
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name
  * @param  line assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */
