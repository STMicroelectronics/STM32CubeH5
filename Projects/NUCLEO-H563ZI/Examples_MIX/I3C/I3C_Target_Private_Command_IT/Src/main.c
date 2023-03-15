/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Target_Private_Command_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H5xx I3C HAL API as Target
  *          to receive and transmit a data buffer
  *          with a communication process based on Interrupt transfer.
  *          The communication is done using 2 Boards.
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

I3C_HandleTypeDef hi3c1;

/* USER CODE BEGIN PV */
/* Context buffer related to Frame context, contain different buffer value for a communication */
I3C_XferTypeDef aContextBuffers;

/* Variable used to catch completion of Dynamic address assignment procedure */
__IO uint8_t ubDynamicAddressCplt = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Pointer used for reception */
uint32_t *pBuffRXWordPtr = NULL;

/* Pointer used for transmission */
uint32_t *pBuffTXWordPtr = NULL;

/* Completion status */
__IO uint8_t ubFrameComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I3C1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void FlushBuffer8(uint8_t* pBuffer1, uint16_t BufferSize);
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
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
  /* Initialize transfer pointer */
  pBuffRXWordPtr    = ((uint32_t *)&aRxBuffer[0]);
  pBuffTXWordPtr    = ((uint32_t *)&aTxBuffer[0]);

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
  MX_I3C1_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED1 */
  BSP_LED_Init(LED1);

  /* Set Preload information */
  LL_I3C_ConfigTxPreload(I3C1, TXBUFFERSIZE);

  /* Wait end of Private transfer procedure */
  while(ubFrameComplete != 2);

  /*##- Compare the sent and received buffers ############################*/
  if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, RXBUFFERSIZE))
  {
    /* Processing Error */
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

/**
  * @brief I3C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I3C1_Init(void)
{

  /* USER CODE BEGIN I3C1_Init 0 */

  /* USER CODE END I3C1_Init 0 */

  I3C_FifoConfTypeDef sFifoConfig = {0};
  I3C_TgtConfTypeDef sTgtConfig = {0};

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */
  hi3c1.Instance = I3C1;
  hi3c1.Mode = HAL_I3C_MODE_TARGET;
  hi3c1.Init.TgtBusCharacteristic.BusAvailableDuration = 0xf8;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure FIFO
  */
  sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_4_4;
  sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_4_4;
  sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
  sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;

  if (HAL_I3C_SetConfigFifo(&hi3c1, &sFifoConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Target
  */
  sTgtConfig.Identifier = 0xC6;
  sTgtConfig.MIPIIdentifier = DEVICE_ID;
  sTgtConfig.CtrlRoleRequest = DISABLE;
  sTgtConfig.HotJoinRequest = DISABLE;
  sTgtConfig.IBIRequest = DISABLE;
  sTgtConfig.IBIPayload = DISABLE;
  sTgtConfig.IBIPayloadSize = HAL_I3C_PAYLOAD_EMPTY;
  sTgtConfig.MaxReadDataSize = 0xFF;
  sTgtConfig.MaxWriteDataSize = 0xFF;
  sTgtConfig.CtrlCapability = DISABLE;
  sTgtConfig.GroupAddrCapability = DISABLE;
  sTgtConfig.DataTurnAroundDuration = HAL_I3C_TURNAROUND_TIME_TSCO_LESS_12NS;
  sTgtConfig.MaxReadTurnAround = 0;
  sTgtConfig.MaxDataSpeed = HAL_I3C_GETMXDS_FORMAT_1;
  sTgtConfig.MaxSpeedLimitation = DISABLE;
  sTgtConfig.HandOffActivityState = HAL_I3C_HANDOFF_ACTIVITY_STATE_0;
  sTgtConfig.HandOffDelay = DISABLE;
  sTgtConfig.PendingReadMDB = DISABLE;
  if (HAL_I3C_Tgt_Config(&hi3c1, &sTgtConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I3C1_Init 2 */
  LL_I3C_EnableIT_DAUPD(I3C1);
  LL_I3C_EnableIT_FC(I3C1);
  LL_I3C_EnableIT_RXFNE(I3C1);
  LL_I3C_EnableIT_TXFNF(I3C1);
  LL_I3C_EnableIT_ERR(I3C1);
  /* USER CODE END I3C1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Flush 8-bit buffer.
  * @param  pBuffer1: pointer to the buffer to be flushed.
  * @param  BufferSize: Size of Buffer.
  * @retval None
  */
void FlushBuffer8(uint8_t* pBuffer1, uint16_t BufferSize)
{
  uint8_t Index = 0;

  for (Index = 0; Index < BufferSize; Index++)
  {
    pBuffer1[Index] = 0;
  }
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval 0  : pBuffer1 identical to pBuffer2
  *         >0 : pBuffer1 differs from pBuffer2
  */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if ((*pBuffer1) != *pBuffer2)
    {
      return BufferLength;
    }
    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
/**
  * @brief  Target Event treatment callback.
  * @param  targetPayload Parameter indicates the target payload.
  * @retval None
  */
void Target_Event_Treatment_Callback(void)
{
  /* Check Dynamic Address update flag in EVR register */
  if (LL_I3C_IsActiveFlag_DAUPD(I3C1))
  {
    /* Clear Dynamic Address update flag */
    LL_I3C_ClearFlag_DAUPD(I3C1);

    BSP_LED_Toggle(LED1);
  }
}

/**
  * @brief  Target Complete callback.
  * @param  None
  * @retval None
  */
void Target_Complete_Callback(void)
{
  /* Update Completion status */
  ubFrameComplete++;

  /* Toggle LED1: Transfer in Transmission process is correct */
  BSP_LED_Toggle(LED1);
}

/**
  * @brief  Target Receive callback.
  * @param  None
  * @retval None
  */
void Target_Reception_Callback(void)
{
  do{
    *pBuffRXWordPtr = LL_I3C_ReceiveData32(I3C1);
    pBuffRXWordPtr++;
  }while (LL_I3C_IsActiveFlag_RXFNE(I3C1));
}

/**
  * @brief  Target Transmit callback.
  * @param  None
  * @retval None
  */
void Target_Transmit_Callback(void)
{
  do{
    /* Write Tx buffer data to transmit register */
    LL_I3C_TransmitData32(I3C1, *pBuffTXWordPtr);
    pBuffTXWordPtr++;
  }while (LL_I3C_IsActiveFlag_TXFNF(I3C1));
}

/**
  * @brief  Function called in case of error detected
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  /* Unexpected event : call error handler */
  Error_Handler();
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* Error if LED1 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LED1);
    HAL_Delay(1000);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
