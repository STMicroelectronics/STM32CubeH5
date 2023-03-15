/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Cortex/CORTEXM_ModePrivilege/Src/main.c
  * @author  MCD Application Team
  * @brief   Description of the Cortex-M33 Mode Privilege example.
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
#define SP_PROCESS_SIZE             0x200  /* Process stack size */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint8_t PSPMemAlloc[SP_PROCESS_SIZE];
__IO uint32_t Index = 0, PSPValue = 0, CurrentStack = 0, ThreadMode = 0, OperationComplete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static __INLINE  void __SVC(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if defined ( __CC_ARM   )
__ASM void __SVC(void)
{
  SVC 0x01
  BX R14
}
#elif defined ( __ICCARM__ )
static __INLINE  void __SVC()
{
  __ASM("svc 0x01");
}
#elif defined   (  __GNUC__  )
static __INLINE void __SVC()
{
  __ASM volatile("svc 0x01");
}
#endif
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED1);

  /* Switch Thread mode Stack from Main to Process -----------------------------*/
  /* Initialize memory reserved for Process Stack */
  for(Index = 0; Index < SP_PROCESS_SIZE; Index++)
  {
    PSPMemAlloc[Index] = 0x00;
  }

  /* Set Process stack value */
  __set_PSP((uint32_t)PSPMemAlloc + SP_PROCESS_SIZE);

  /* Select Process Stack as Thread mode Stack */
  __set_CONTROL(SP_PROCESS);

  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Get the Thread mode stack used */
  if((__get_CONTROL() & 0x02) == SP_MAIN)
  {
    /* Main stack is used as the current stack */
    CurrentStack = SP_MAIN;
  }
  else
  {
    /* Process stack is used as the current stack */
    CurrentStack = SP_PROCESS;

    /* Get process stack pointer value */
    PSPValue = __get_PSP();
  }

  /* Switch Thread mode from privileged to unprivileged ------------------------*/
  /* Thread mode has unprivileged access */
  __set_CONTROL(THREAD_MODE_UNPRIVILEGED | SP_PROCESS);
  
  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Unprivileged access mainly affect ability to:
      - Use or not use certain instructions such as MSR fields
      - Access System Control Space (SCS) registers such as NVIC and SysTick */

  /* Check Thread mode privilege status */
  if((__get_CONTROL() & 0x01) == THREAD_MODE_PRIVILEGED)
  {
    /* Thread mode has privileged access  */
    ThreadMode = THREAD_MODE_PRIVILEGED;
  }
  else
  {
    /* Thread mode has unprivileged access*/
    ThreadMode = THREAD_MODE_UNPRIVILEGED;
  }

  /* Switch back Thread mode from unprivileged to privileged -------------------*/
  /* Try to switch back Thread mode to privileged (Not possible, this can be
     done only in Handler mode) */
  __set_CONTROL(THREAD_MODE_PRIVILEGED | SP_PROCESS);

  /* Execute ISB instruction to flush pipeline as recommended by Arm */
  __ISB(); 

  /* Generate a system call exception, and in the ISR switch back Thread mode
    to privileged */
  __SVC();

  /* Check Thread mode privilege status */
  if((__get_CONTROL() & 0x01) == THREAD_MODE_PRIVILEGED)
  {
    /* Thread mode has privileged access  */
    ThreadMode = THREAD_MODE_PRIVILEGED;
  }
  else
  {
    /* Thread mode has unprivileged access*/
    ThreadMode = THREAD_MODE_UNPRIVILEGED;
  }
  OperationComplete = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Turn ON LED once test finished */
    BSP_LED_On(LED1);

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
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  OperationComplete = 2;
  while(1) 
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
