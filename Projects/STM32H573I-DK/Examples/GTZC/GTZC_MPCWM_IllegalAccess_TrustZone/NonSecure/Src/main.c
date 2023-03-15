/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : GTZC/GTZC_MPCWM_IllegalAccess_TrustZone/NonSecure/Src/main.c
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
#define EXT_MEM_GRANULARITY 0x20000UL
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DCACHE_HandleTypeDef hdcache1;

/* USER CODE BEGIN PV */
__IO uint32_t UserButtonPressed = 0U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_DCACHE1_Init(void);
/* USER CODE BEGIN PFP */

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
  uint32_t i, temp_value;
  volatile uint32_t read_value;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DCACHE1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    while (UserButtonPressed == 0U)
    {

      /* read/write test in first half of memory (non-secure area), should be OK
      * Reminder: in secure sub-project
      * - OCTOSPI used memory size is 1MB = 8*128KB
      * - mapped nons-secure address range is 0x90000000..0x9007FFFF
      * - so half size is 4U*128KB
      * - first half is set as non-secure (4U*128KB) in initial config
      * - second half is set as secure (4U*128KB) in initial config
      */
      for (i = 0U; i < (4U * EXT_MEM_GRANULARITY); i+=EXT_MEM_GRANULARITY)
      {
        read_value = *(uint32_t *)(OCTOSPI1_BASE + i);
        temp_value = ~read_value;
        *(uint32_t *)(OCTOSPI1_BASE + i) = temp_value;
        if( *(uint32_t *)(OCTOSPI1_BASE + i) != temp_value)
        {
          SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure read/write access mismatch in non-secure SRAM");
          /* Infinite loop */
          while(1);
        }
      }

      /* Insert delay 1000 ms */
      HAL_Delay(1000U);

      /* on previous test success, blink green led */
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }

    if (UserButtonPressed == 1U)
    {
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure detection of BUTTON_USER");
      /* here we do a write operation from the non-secure on a secure area.
      * This is of course not allowed, and the result should be a Secure fault
      * event because it doesn't respect SAU rules (1st obstacle, before MPCWM)
      * This access is done at the beginning of the second half of memory, set
      * as secured in initial config.
      */
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure Read test on Secure area of OCTOSPI NOR");
      read_value = 0xABABABAB;
      *(uint32_t *)(OCTOSPI1_BASE + 8U * EXT_MEM_GRANULARITY) = ~read_value;

      /* In the secure fault interrupt handler (on secure sub-project), OCTOSPI NOR
      * memory security properties have been updated, in order to avoid an
      * infinite loop. So we need to restore initial properties, to continue
      * SW execution in good conditions.
      */

      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests to restore SAU/MPCWM initial configuration");
      SECURE_SAU_MPCWM_SetInitConfig();

      while(UserButtonPressed == 1U)
      {
        /* Toggle RED LED after a 1s delay */
        HAL_Delay(1000U);
        SECURE_ToggleRedLed();
      }

      /* invalidate the full cache content */
      HAL_DCACHE_Invalidate(&hdcache1);

    }
    else if (UserButtonPressed == 2U)
    {
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure detection of a second push on USER BUTTON");

      /* here we do a write operation from the non-secure world on an address in
      * a non-secure area from SAU point of view (so should be OK) but on a
      * secure area from MPCWM point of view. This should triggered GTZC_IRQn.
      * For this, we should misalign SAU and MPCWM configuration, do the access
      * and restore initial configurations.
      */
      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests misalignment between SAU and MPCWM configuration");

      /* Change SAU/MPCWM parameters to allow illegal access detection */
      SECURE_SAU_MPCWM_SetIllegalAccessConfig();

      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-Secure Write test on Secure area of OCTOSPI NOR");
      read_value = 0xBABABABA;
      *(uint32_t *)(OCTOSPI1_BASE) = ~read_value;

      SECURE_DisplayMessage((const uint8_t *)"\n\rNon-secure requests to restore SAU/MPCWM initial configuration");

      /* Return to initial SAU/MPCWM configuration */
      SECURE_SAU_MPCWM_SetInitConfig();

      UserButtonPressed = 0U;

      /* invalidate the full cache content */
      HAL_DCACHE_Invalidate(&hdcache1);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_USER_Pin */
  GPIO_InitStruct.Pin = BUTTON_USER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(BUTTON_USER_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI13_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI13_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
* @brief  EXTI line rising detection callback.
* @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
* @retval None
*/
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_USER_Pin)
  {
    UserButtonPressed++;
  }
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

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
