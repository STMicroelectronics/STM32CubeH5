/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include <stdbool.h>
#include "main.h"

#include "app_threadx.h"
#include "app_openbootloader.h"
#include "interfaces_conf.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
  *            SYSCLK(Hz)                     = 160000000  (CPU Clock)
  *            HCLK(Hz)                       = 160000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  160MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  160MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  160MHz)
  *            HSI Frequency(Hz)              = 64000000
  *            PLL_M                          = 8
  *            PLL_N                          = 40
  *            PLL_P                          = 2
  *            PLL_Q                          = 16
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0U};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0U};

  /* Configure the main internal regulator output voltage */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Initialize the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State          = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM            = 8U;
  RCC_OscInitStruct.PLL.PLLN            = 40U;
  RCC_OscInitStruct.PLL.PLLP            = 2U;
  RCC_OscInitStruct.PLL.PLLQ            = 16U;
  RCC_OscInitStruct.PLL.PLLR            = 2U;
  RCC_OscInitStruct.PLL.PLLRGE          = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL       = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN        = 0U;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK
                                     | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1
                                     | RCC_CLOCKTYPE_PCLK2
                                     | RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL1Q as source of FDCAN clock */
  LL_RCC_PLL1Q_Enable();
  LL_RCC_SetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE_PLL1Q);
}

void System_DeInit(void)
{
  HAL_RCC_DeInit();

  /* Disable timer */
  HAL_TIM_Base_DeInit(&htim6);
  HAL_TIM_Base_Stop_IT(&htim6);
  __HAL_RCC_TIM6_CLK_DISABLE();
  HAL_NVIC_DisableIRQ(TIM6_IRQn);

  /* Disable interfaces */
  USARTx_DEINIT();
  I2Cx_DEINIT();
  SPIx_DEINIT();
  I3Cx_DEINIT();
  FDCANx_CLK_DISABLE();
  FDCANx_FORCE_RESET();
  FDCANx_RELEASE_RESET();
  HAL_NVIC_DisableIRQ(USB_DRD_FS_IRQn);
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
}
#endif /* USE_FULL_ASSERT */
