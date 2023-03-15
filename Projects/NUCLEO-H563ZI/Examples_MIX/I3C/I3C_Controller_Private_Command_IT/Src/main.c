/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Controller_Private_Command_IT/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H5xx I3C HAL and LL API as Controller
  *          to transmit and receive a data buffer
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
#include "desc_target1.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define I3C_IDX_FRAME_1         0U  /* Index of Frame 1 */
#define I3C_IDX_FRAME_2         1U  /* Index of Frame 2 */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I3C_HandleTypeDef hi3c1;

/* USER CODE BEGIN PV */
/* Context buffer related to Frame context, contain different buffer value for a communication */
I3C_XferTypeDef aContextBuffers[2];

/* Number of Targets detected during DAA procedure */
__IO uint32_t uwTargetCount = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT****  ****I3C_TwoBoards communication based on IT**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Buffer used by HAL to compute control data for the Private Communication */
uint32_t aControlBuffer[0xF];

/* Array contain targets descriptor */
TargetDesc_TypeDef *aTargetDesc[1] = \
                          {
                            &TargetDesc1       /* DEVICE_ID1 */
                          };

/* Descriptor for private data transmit */
I3C_PrivateTypeDef aPrivateDescriptor[2] = \
                                          {
                                            {TARGET1_DYN_ADDR, {aTxBuffer, TXBUFFERSIZE}, {NULL, 0}, HAL_I3C_DIRECTION_WRITE},
                                            {TARGET1_DYN_ADDR, {NULL, 0}, {aRxBuffer, RXBUFFERSIZE}, HAL_I3C_DIRECTION_READ}
                                          };

/* CCC transfer variables */
uint8_t ubPrivateIdx = 0;
uint8_t ubFrameIdx = 0;
uint32_t uwPrivateMessage[0xFF] = {0};
__IO uint8_t ubNbPrivate = 0;

/* Buffer used for reception */
__IO uint8_t ubNbRxData = 0;

/* Buffer used for transmission */
__IO uint8_t ubNbTxDataToTransfer = 0;
__IO uint8_t ubNbTxData = 0;

/* Function pointer on function TX buffer treatment */
void (*ptrTXFunc)(void);

/* Completion status */
__IO uint8_t ubFrameComplete = 0;
__IO uint8_t ubReceptionComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I3C1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void FlushBuffer8(uint8_t* pBuffer1, uint16_t BufferSize);
void Handle_ENTDAA_Controller(void);
void Prepare_Private_Transfer(void);
void Handle_Private_Controller(void);
static void I3C_DynamicAddressTreatment(void);
static void I3C_TransmitByteTreatment(void);
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

  /* Configure USER push-button */
  BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);

  /* Wait for USER push-button press before starting the Communication   */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }

  /* Wait for USER push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /* Handle Dynamic Address Assignment */
  Handle_ENTDAA_Controller();

  /* Wait end of Dynamic Address Assignment procedure */
  while(ubFrameComplete == 0U);

  /* Reset Complete variable */
  ubFrameComplete = 0;

  /*##- Prepare Private message  ####################################################*/
  /* An auto stop is automatically generated between
     each element of aPrivateDescriptor */

  /* Wait for USER push-button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }

  /* Wait for USER push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /* Prepare Private transfer */
  Prepare_Private_Transfer();

  /*##- Start the Private communication #############################################*/
  /* An auto stop is automatically generated after
     first element of aPrivateDescriptor */

  /* Handle Private transfer */
  Handle_Private_Controller();

  /* Wait end of Private transfer procedure */
  while(ubFrameComplete == 0);

  ubFrameComplete = 0;

  /* Wait for USER push-button press before starting the Communication   */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }

  /* Wait for USER push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /*##- Start the Private communication #############################################*/
  /* An auto stop is automatically generated after
     second element of aPrivateDescriptor */

  /* Handle Private transfer */
  Handle_Private_Controller();

  /* Wait end of Private transfer procedure */
  while(ubFrameComplete == 0);

  ubFrameComplete = 0;

  /*##- Compare the sent and received buffers ############################*/
  if (Buffercmp((uint8_t *)aTxBuffer, (uint8_t *)aRxBuffer, RXBUFFERSIZE))
  {
    /* Processing Error */
    Error_Handler();
  }
  else
  {
    /* LED1 On */
    BSP_LED_On(LED1);
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
  I3C_CtrlConfTypeDef sCtrlConfig = {0};

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */
  hi3c1.Instance = I3C1;
  hi3c1.Mode = HAL_I3C_MODE_CONTROLLER;
  hi3c1.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_1_5;
  hi3c1.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
  hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x09;
  hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x09;
  hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x59;
  hi3c1.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x00;
  hi3c1.Init.CtrlBusCharacteristic.BusFreeDuration = 0x32;
  hi3c1.Init.CtrlBusCharacteristic.BusIdleDuration = 0xf8;
  if (HAL_I3C_Init(&hi3c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure FIFO
  */
  sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_1_4;
  sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_1_4;
  sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
  sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;
  if (HAL_I3C_SetConfigFifo(&hi3c1, &sFifoConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure controller
  */
  sCtrlConfig.DynamicAddr = 0;
  sCtrlConfig.StallTime = 0x00;
  sCtrlConfig.HotJoinAllowed = DISABLE;
  sCtrlConfig.ACKStallState = DISABLE;
  sCtrlConfig.CCCStallState = DISABLE;
  sCtrlConfig.TxStallState = DISABLE;
  sCtrlConfig.RxStallState = DISABLE;
  sCtrlConfig.HighKeeperSDA = DISABLE;
  if (HAL_I3C_Ctrl_Config(&hi3c1, &sCtrlConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I3C1_Init 2 */
  LL_I3C_EnableIT_FC(I3C1);
  LL_I3C_EnableIT_CFNF(I3C1);
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
  * @brief  This Function handle Controller events to perform an Assign dynamic address process
  * @note  This function is composed in one step :
  *        -1- Initiate the Dynamic address assignment procedure.
  * @param  None
  * @retval None
  */
void Handle_ENTDAA_Controller(void)
{
  /* Set Treatment function pointer */
  ptrTXFunc = &I3C_DynamicAddressTreatment;

  /* (1) Initiate a Dynamic Address Assignment to the Target connected on the bus ****/
  /* Controller Generate Start condition for a write request with a Broadcast ENTDAA:
  *  - to the Targets connected on the bus
  *  - with an auto stop condition generation when all Targets answer the ENTDAA sequence.
  */
  LL_I3C_ControllerHandleCCC(I3C1, Broadcast_ENTDAA, 0, LL_I3C_GENERATE_STOP);
}

/**
  * @brief  This Function handle a private transfer to a target
  * @note  This function is composed in one step :
  *        -1- Compute the different parameter of aPrivateDescriptor array into private message.
  *            The private message take the structure of hardware Control Register
  *            to safe process time during the sending of Control data into the hardware.
  *            A repeated start is inserted between each private command.
  *            A stop is inserted at the end of the private array.
  * @param  None
  * @retval None
  */
void Prepare_Private_Transfer(void)
{
  uint32_t ControlMask = (I3C_CR_ADD | I3C_CR_DCNT | I3C_CR_RNW | I3C_CR_MTYPE | I3C_CR_MEND);
  uint32_t XferSize = 0U;

  for(ubPrivateIdx = 0; ubPrivateIdx < COUNTOF(aPrivateDescriptor); ubPrivateIdx++)
  {
    /* (1) Prepare the private transfer to a Target connected on the bus ***********************/
    /* First part correspond to Direct Command Code value, size of the defining byte and
       repeated start for second part of private message.
    */
    /* Second part correspond to Target address, size of the private associated data, direction and
       repeated start between each private message or stop the last private message.
    */

    if (aPrivateDescriptor[ubPrivateIdx].Direction == LL_I3C_DIRECTION_WRITE)
    {
      XferSize =aPrivateDescriptor[ubPrivateIdx].TxBuf.Size;
      ubNbTxDataToTransfer = XferSize;
    }
    else
    {
      XferSize =aPrivateDescriptor[ubPrivateIdx].RxBuf.Size;
    }

    MODIFY_REG(uwPrivateMessage[ubFrameIdx],                                                                           \
               ControlMask,                                                                                            \
               ((aPrivateDescriptor[ubPrivateIdx].TargetAddr << I3C_CR_ADD_Pos)                                      | \
               XferSize                                                                                              | \
               aPrivateDescriptor[ubPrivateIdx].Direction                                                            | \
               LL_I3C_CONTROLLER_MTYPE_PRIVATE                                                                       | \
               LL_I3C_GENERATE_STOP));

    /* Increment Frame index */
    ubFrameIdx++;
  }
}

/**
  * @brief  This Function handle Private transfer to then from a target
  * @note  This function is composed in one step :
  *        -1- Initiate the start of the Private transfer.
  * @param  None
  * @retval None
  */
void Handle_Private_Controller(void)
{
  /* Flush Buffer before start a Private transfer */
  FlushBuffer8(aRxBuffer, COUNTOF(aRxBuffer));

  /* Reset Counter */
  ubNbRxData = 0;

  /* Set Treatment function pointer */
  ptrTXFunc = &I3C_TransmitByteTreatment;

  /* Start Transfer Private communication */
  LL_I3C_RequestTransfer(I3C1);
}

/******************************************************************************/
/*   IRQ HANDLER TREATMENT Functions                                          */
/******************************************************************************/
/**
  * @brief  I3C transmit Dynamic address to a Target.
  * @param  None
  * @retval None
  */
static void I3C_DynamicAddressTreatment(void)
{
  uint64_t target_payload = 0U;

  /* Check on the Rx FIFO threshold to know the Dynamic Address Assignment treatment process : byte or word */
  if (LL_I3C_GetRxFIFOThreshold(I3C1) == LL_I3C_RXFIFO_THRESHOLD_1_4)
  {
    /* Loop to get target payload */
    for (uint32_t index = 0U; index < 8U; index++)
    {
      /* Retrieve payload byte by byte */
      target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData8(I3C1) << (index * 8U));
    }
  }
  else
  {
    /* Retrieve first 32 bits payload */
    target_payload = (uint64_t)LL_I3C_ReceiveData32(I3C1);
    
    /* Retrieve second 32 bits payload */
    target_payload |= (uint64_t)((uint64_t)LL_I3C_ReceiveData32(I3C1) << 32U);
  }

  /* Call the corresponding callback */
  Target_Request_DynamicAddrCallback(target_payload);
}

/**
  * @brief  I3C transmit data by byte per byte.
  * @param  None
  * @retval None
  */
static void I3C_TransmitByteTreatment(void)
{
  /* Check counter of remaining bytes counter before send a new data */
  if (ubNbTxDataToTransfer > 0)
  {
    /* Write Tx buffer data to transmit register */
    LL_I3C_TransmitData8(I3C1, aTxBuffer[ubNbTxData++]);

    /* Decrement remaining bytes counter */
    ubNbTxDataToTransfer--;
  }
}

/**
  * @brief  Target Request Dynamic Address callback.
  * @param  targetPayload : Contain the target payload.
  * @retval None
  */
void Target_Request_DynamicAddrCallback(uint64_t targetPayload)
{
  /* Store Payload in aTargetDesc */
  aTargetDesc[uwTargetCount]->TARGET_BCR_DCR_PID = targetPayload;

  /* Send associated dynamic address */
  /* Write device address in the TDR register */
  /* Increment Target counter */
  LL_I3C_TransmitData8(I3C1, aTargetDesc[uwTargetCount++]->DYNAMIC_ADDR);
}

/**
  * @brief  Controller Complete callback.
  * @param  None
  * @retval None
  */
void Controller_Complete_Callback(void)
{
  /* Update Completion status */
  ubFrameComplete++;

  /* Toggle LED1: Transfer in transmission process is correct */
  BSP_LED_Toggle(LED1);
}

/**
  * @brief  Controller Receive callback.
  * @param  None
  * @retval None
  */
void Controller_Reception_Callback(void)
{
  aRxBuffer[ubNbRxData++] = LL_I3C_ReceiveData8(I3C1);
}

/**
  * @brief  Controller Transmit callback.
  * @param  None
  * @retval None
  */
void Controller_Transmit_Callback(void)
{
  ptrTXFunc();
}

/**
  * @brief  Controller Control callback.
  * @param  None
  * @retval None
  */
void Controller_Frame_Update_Callback(void)
{
  WRITE_REG(I3C1->CR, uwPrivateMessage[ubNbPrivate++]);
}

/**
  * @brief I3C target request a dynamic address callback.
  *        The main objective of this user function is to check if a target request a dynamic address.
  *        if the case we should assign a dynamic address to the target.
  * @par Called functions
  * - HAL_I3C_TgtReqDynamicAddrCallback()
  * - HAL_I3C_Ctrl_SetDynamicAddress()
  * @retval None
  */
void HAL_I3C_TgtReqDynamicAddrCallback(I3C_HandleTypeDef *hi3c, uint64_t targetPayload)
{
  /* Update Payload on aTargetDesc */
  aTargetDesc[uwTargetCount ]->TARGET_BCR_DCR_PID = targetPayload;

  /* Send associated dynamic address */
  HAL_I3C_Ctrl_SetDynAddr(hi3c, aTargetDesc[uwTargetCount++]->DYNAMIC_ADDR);
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
