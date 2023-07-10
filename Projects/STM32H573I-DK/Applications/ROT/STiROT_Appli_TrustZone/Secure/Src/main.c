/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Secure Services
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

/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Non-secure Vector table to jump to (internal Flash Bank2 here)             */
/* Caution: address must correspond to non-secure internal Flash where is     */
/*          mapped in the non-secure vector table
                             */
#define VTOR_TABLE_NS_START_ADDR  (FLASH_BASE_NS + S_CODE_OFFSET + \
                                   S_CODE_SIZE + IMAGE_HEADER_SIZE)
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void  unsecure_sram3(void);
static void NonSecure_Init(void);
static void MX_GTZC_Init(void);
CMSE_NS_ENTRY void SECURE_loader_cfg(void);


/* Private user code ---------------------------------------------------------*/


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* SAU/IDAU, FPU and interrupts secure/non-secure allocation setup done */
  /* in SystemInit() based on partition_stm32h573xx.h file's definitions. */


  /* Enable SecureFault handler (HardFault is default) */
  SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk;

  /* STM32H5xx **SECURE** HAL library initialization:
       - Secure Systick timer is configured by default as source of time base,
         but user can eventually implement his proper time base source (a general
         purpose timer for example or other time source), keeping in mind that
         Time base duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined
         and handled in milliseconds basis.
       - Low Level Initialization
     */


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* GTZC initialisation */
  MX_GTZC_Init();

  /* Function where we deactivate MPU for both Secure and Non Secure application */
  HAL_MPU_Disable();

  /* All IOs are by default allocated to secure */
  /* Release them all to non-secure except PC.07 (LED1) kept as secure */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  HAL_GPIO_ConfigPinAttributes(GPIOA, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOB, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOC, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOD, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOE, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOF, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOG, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  HAL_GPIO_ConfigPinAttributes(GPIOH, GPIO_PIN_ALL, GPIO_PIN_NSEC);
  /*************** Setup and jump to non-secure *******************************/

  NonSecure_Init();

  /* Non-secure software does not return, this code is not executed */

  /* Infinite loop */

  while (1)
  {

  }
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
  * @brief  Unsecure Area of SRAM3
  * @retval None
  */
static void  unsecure_sram3()
{
  MPCBB_ConfigTypeDef MPCBB_desc;
  if (HAL_GTZC_MPCBB_GetConfigMem(SRAM3_BASE, &MPCBB_desc) != HAL_OK)
  {
    /* FIX ME */
    Error_Handler();
  }
  for(int i=0; i<32 ; i++)
  {
    MPCBB_desc.AttributeConfig.MPCBB_SecConfig_array[i] = 0;
    MPCBB_desc.AttributeConfig.MPCBB_PrivConfig_array[i] = 0;
  }
  if (HAL_GTZC_MPCBB_ConfigMem(SRAM3_BASE, &MPCBB_desc) != HAL_OK)
    /* FIX ME */
  {
    Error_Handler();
  }
}
/**
  * @brief GTZC Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_Init(void)
{

  if (HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_ICACHE_REG,
                                           GTZC_TZSC_PERIPH_SEC | GTZC_TZSC_PERIPH_NPRIV) != HAL_OK)
  {
    Error_Handler();
  }

  unsecure_sram3();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* Infinite loop */
  while (1)
  {

  }

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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }

}
#endif /* USE_FULL_ASSERT */
