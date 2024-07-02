/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   UART/UART_AutoBaudrate_Detection/Src/main.c
  * @brief  This sample code shows how to use STM32H5xx UART HAL API to use
  *         automatic baud rate detection feature on a UART Rx line.
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
#include "stm32h5xx_ll_usart.h"

#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
  * @brief  Structure containing Auto baud rate detection method description
  */
typedef struct
{
  uint32_t      AutoBaudRateMode;         /*!< Specifies Auto baud rate method value */
  uint8_t const *DetectionInfoText;       /*!< Indicates user prompt for execution   */
  uint8_t       DetectionInfoTextSize;    /*!< Indicates size of user prompt         */
} AutoBaudRate_Detection_Method_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Default baud rate used on serial port at startup */
#define DEFAULT_BAUDRATE        115200U

/* DEL character ascii code */
#define DEL_ASCII_CODE          0x7FU

/* Timeout value for sending information on UART Tx */
#define PRINT_TIMEOUT                  1000U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/**
  * @brief Text strings printed on PC Com port for user information
  */
static const uint8_t aDelString[] = "DEL";
static const uint8_t aTextInfoStart[] = "\r\nUART AutoBaudRate Detection Example (default baud rate after UART initialisation is 115200).\r\n";
static const uint8_t aTextErrorDetected[] = "\r\n***** Error during Auto baud rate detection process *****\r\n";
static const uint8_t aTextAskForNewBaudRate[] = "Please configure your serial port on PC to a different baud rate, and enter requested character.\r\n";
static const uint8_t aTextDetectionOnStartBit[] = "1 - Baud rate detection on Start bit (received character should start with a 1 in b0) => Enter \"A\"\n\r";
static const uint8_t aTextDetectionOnFallingEdge[] = "2 - Baud rate detection on 0x7F frame (received character should start with a 10xx bit pattern) => Enter \"1\"\n\r";
static const uint8_t aTextDetectionOn7F[] = "3 - Baud rate detection on 0x7F frame (received character should be 0x7F) => Press DEL\n\r";
static const uint8_t aTextDetectionOn55[] = "4 - Baud rate detection on 0x55 frame (received character should be 0x55) =>  Enter \"U\"\n\r";
static const uint8_t aTextDetectionResult[] = " has been properly received.\n\rBaud rate value has been adjusted in BRR to (bps) : ";

/**
  * @brief reception buffer
  */
uint8_t aRXBufferUser[RX_BUFFER_SIZE];

/**
  * @brief Array containing information for each auto baud rate detection method
  */
AutoBaudRate_Detection_Method_t aDetectionMethods[NB_AUTO_BAUD_RATE_MODES] =
{
  { UART_ADVFEATURE_AUTOBAUDRATE_ONSTARTBIT,    aTextDetectionOnStartBit,    COUNTOF(aTextDetectionOnStartBit)    },
  { UART_ADVFEATURE_AUTOBAUDRATE_ONFALLINGEDGE, aTextDetectionOnFallingEdge, COUNTOF(aTextDetectionOnFallingEdge) },
  { UART_ADVFEATURE_AUTOBAUDRATE_ON0X7FFRAME,   aTextDetectionOn7F,          COUNTOF(aTextDetectionOn7F)          },
  { UART_ADVFEATURE_AUTOBAUDRATE_ON0X55FRAME,   aTextDetectionOn55,          COUNTOF(aTextDetectionOn55)          }
};

/**
  * @brief Status indicating if reception is complete or not
  */
__IO ITStatus ReceptionReady = RESET;
__IO ITStatus ErrorDetected  = RESET;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void PrintInfo(UART_HandleTypeDef *huart, uint8_t const *pString, uint16_t Size);

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
  uint32_t baudrate;
  uint32_t periphclk;
  uint8_t  baudrate_string[RX_BUFFER_SIZE];
  uint8_t  autobaudrate_mode;

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

  /* USART2 is clocked on PCLK. Store PCLK frequency value into local variable */
  periphclk = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_USART3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  /* At startup, UART has been first configured with 115200 bps */
  baudrate = DEFAULT_BAUDRATE;

  /* Configure LED2 */
  BSP_LED_Init(LED2);

  PrintInfo(&huart2, aTextInfoStart, COUNTOF(aTextInfoStart));

  /* For all supported Auto baud rate detection methods */
  for (autobaudrate_mode = 0; autobaudrate_mode < NB_AUTO_BAUD_RATE_MODES; autobaudrate_mode++)
  {
    /* Configure UART with Auto baud rate method and current baud rate */
    huart2.Init.BaudRate = baudrate;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_AUTOBAUDRATE_INIT;
    huart2.AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_ENABLE;
    huart2.AdvancedInit.AutoBaudRateMode = aDetectionMethods[autobaudrate_mode].AutoBaudRateMode;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
      Error_Handler();
    }

    /* Send msg using previously set baud rate */
    PrintInfo(&huart2, aTextAskForNewBaudRate, COUNTOF(aTextAskForNewBaudRate));
    PrintInfo(&huart2, aDetectionMethods[autobaudrate_mode].DetectionInfoText,
              aDetectionMethods[autobaudrate_mode].DetectionInfoTextSize);

    /* Reset transmission flag */
    ReceptionReady = RESET;

    /* Wait for receiving 1 char */
    HAL_UART_Receive_IT(&huart2, aRXBufferUser, 1);

    /* While waiting for character from PC side, LED2 is
       blinking according to the following pattern: toggle every 100ms */
    while (ReceptionReady != SET)
    {
      /* Toggle LED2 every 100ms */
      BSP_LED_Toggle(LED2);
      HAL_Delay(100);
    }

    /* Check if Auto baud rate detection has been successfully executed.
       If not, Auto baud Rate Detection error (ABRE) is reported */
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ABRE))
    {
      ErrorDetected = SET;
      PrintInfo(&huart2, aTextErrorDetected, COUNTOF(aTextErrorDetected));
    }
    else
    {
      /* Wait for completion of auto baud rate detection : ABRF flag in ISR */
      while (!__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ABRF));

      /* Clear ABRF flag */
      __HAL_UART_SEND_REQ(&huart2, UART_AUTOBAUD_REQUEST);

      /* Retrieve baud rate */
      baudrate = LL_USART_GetBaudRate(huart2.Instance, periphclk,
                                      huart2.Init.ClockPrescaler, huart2.Init.OverSampling);

      /* Print result to Terminal */
      if (aRXBufferUser[0] == DEL_ASCII_CODE)
      {
        /* Specific case of DEL character entered : "DEL" string to print printed on Terminal */
        PrintInfo(&huart2, aDelString, COUNTOF(aDelString));
      }
      else
      {
        PrintInfo(&huart2, aRXBufferUser, 1);
      }
      PrintInfo(&huart2, aTextDetectionResult, COUNTOF(aTextDetectionResult));
      sprintf((char *) baudrate_string, "%u bps \n\n\r", (unsigned int) baudrate);
      PrintInfo(&huart2, baudrate_string, strlen((char *) baudrate_string));
    }
  }

  /* End of example program :
     - If no error has been detected, set LED2 On
     - Otherwise, toggle LED2
  */
  if (ErrorDetected == RESET)
  {
    BSP_LED_On(LED2);
  }
  else
  {
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
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
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

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_AUTOBAUDRATE_INIT;
  huart2.AdvancedInit.AutoBaudRateEnable = UART_ADVFEATURE_AUTOBAUDRATE_ENABLE;
  huart2.AdvancedInit.AutoBaudRateMode = UART_ADVFEATURE_AUTOBAUDRATE_ONSTARTBIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Send text information message on UART Tx line (to PC Com port).
  * @param  huart UART handle.
  * @param  pString String to be sent to user display
  * @param  Size   Size of string
  * @retval None
  */
void PrintInfo(UART_HandleTypeDef *huart, uint8_t const *pString, uint16_t Size)
{
  if (HAL_OK != HAL_UART_Transmit(huart, pString, Size, PRINT_TIMEOUT))
  {
    Error_Handler();
  }
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of Rx transfer
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set reception flag: transfer complete */
  ReceptionReady = SET;
}

/**
  * @brief  UART Error callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report an UART error
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Error has been detected */
  ErrorDetected = SET;
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
  while (1)
  {
    /* Toggle LED2 for error */
    BSP_LED_Toggle(LED2);
    HAL_Delay(500);
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

  Error_Handler();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
