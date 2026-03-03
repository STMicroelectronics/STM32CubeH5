/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "main.h"

#include "app_threadx.h"
#include "app_openbootloader.h"
#include "interfaces_conf.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HSE_READY_TIMEOUT         0x00098000U   /* (200ms) */
#define VOSRDY_TIMEOUT            0x1000000U    /* Timeout for Voltage Scaling Ready Flag */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* STM32H5xx HAL library initialization:
       - Configure the Flash prefetch
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 3
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock to have a frequency of 160 MHz */
  SystemClock_Config();

  /* Initialize the OpenBootloader */
  OpenBootloader_Init();

  /* Initialize the Threads and start the kernel */
  MX_ThreadX_Init();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 160000000 (CPU Clock)
  *            HCLK(Hz)                       = 160000000 (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock 160MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock 160MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock 160MHz)
  *            HSI Frequency(Hz)              = 64000000
  *            PLL_M                          = 8
  *            PLL_N                          = 40
  *            PLL_P                          = 2
  *            PLL_Q                          = 16
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef rcc_osc_init_struct = {0U};
  RCC_ClkInitTypeDef rcc_clk_init_struct = {0U};
  uint32_t time_out;

  /* Configure the main internal regulator output voltage */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  time_out = VOSRDY_TIMEOUT;

  while ((__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY) == 0U) && (time_out != 0U))
  {
    time_out--;
  }

  if (time_out == 0U)
  {
    Error_Handler();
  }

  /* Initialize the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure */
  rcc_osc_init_struct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  rcc_osc_init_struct.HSIState            = RCC_HSI_ON;
  rcc_osc_init_struct.HSIDiv              = RCC_HSI_DIV1;
  rcc_osc_init_struct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  rcc_osc_init_struct.PLL.PLLState        = RCC_PLL_ON;
  rcc_osc_init_struct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  rcc_osc_init_struct.PLL.PLLM            = 8U;
  rcc_osc_init_struct.PLL.PLLN            = 40U;
  rcc_osc_init_struct.PLL.PLLP            = 2U;
  rcc_osc_init_struct.PLL.PLLQ            = 16U;
  rcc_osc_init_struct.PLL.PLLR            = 2U;
  rcc_osc_init_struct.PLL.PLLRGE          = RCC_PLL1_VCIRANGE_2;
  rcc_osc_init_struct.PLL.PLLVCOSEL       = RCC_PLL1VCOWIDE;
  rcc_osc_init_struct.PLL.PLLFRACN        = 0U;

  if (HAL_RCC_OscConfig(&rcc_osc_init_struct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB buses clocks */
  rcc_clk_init_struct.ClockType      = RCC_CLOCKTYPE_HCLK
                                     | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1
                                     | RCC_CLOCKTYPE_PCLK2
                                     | RCC_CLOCKTYPE_PCLK3;
  rcc_clk_init_struct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clk_init_struct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  rcc_clk_init_struct.APB3CLKDivider = RCC_HCLK_DIV1;
  rcc_clk_init_struct.APB1CLKDivider = RCC_HCLK_DIV1;
  rcc_clk_init_struct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&rcc_clk_init_struct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable the HSE */
  LL_RCC_HSE_Enable();

  /* Check if the HSE is ready */
  time_out = HSE_READY_TIMEOUT;

  while ((LL_RCC_HSE_IsReady() == 0U) && (time_out != 0U))
  {
    time_out--;
  }

  if (time_out == 0U)
  {
    Error_Handler();
  }

  /* Enable USB HS Clock */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_OTGHS);

  /* set OTGHSx kernel clock source HSE */
  __HAL_RCC_OTGPHY_CLK_DISABLE();
  LL_RCC_SetOTGHSClockSource(LL_RCC_OTGHS_CLKSOURCE_HSE_DIV2);
  __HAL_RCC_OTGPHY_CLK_ENABLE();

  /* Configure the programming delay */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

/**
  * @brief This function is used to de-initializes the system peripherals and disables interrupts.
  * @retval None.
  */
void System_DeInit(void)
{
  HAL_RCC_DeInit();

  /* Disable timer */
  HAL_TIM_Base_DeInit(&htim6);
  HAL_TIM_Base_Stop_IT(&htim6);
  HAL_NVIC_DisableIRQ(TIM6_IRQn);

  __HAL_RCC_TIM6_CLK_DISABLE();

   __HAL_RCC_USB_OTG_HS_CLK_DISABLE();
   __HAL_RCC_USB_OTG_HS_FORCE_RESET();
   __HAL_RCC_USB_OTG_HS_RELEASE_RESET();

  HAL_PWREx_DisableUSBVoltageDetector();
  HAL_PWREx_DisableVddUSB();
  HAL_PWREx_DisableUSBOTGHSPhy();

  HAL_NVIC_DisableIRQ(OTG_HS_IRQn);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  while (true)
  {
  }
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
