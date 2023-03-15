/* USER CODE BEGIN Header */
/**
  ******************************************************************************
* @file    I3C/I3C_Controller_HotJoin_IT/Src/main.c
* @author  MCD Application Team
* @brief   This sample code shows how to use STM32H5xx I3C LL API to
*          manage a HotJoin procedure between a Controller and Targets with a 
*          communication process based on Interrupt transfer.
*          The communication is using 2 or 3 Boards.
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
#include "desc_target2.h"
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

/* USER CODE BEGIN PV */
/* Number of Targets detected during DAA procedure */
__IO uint32_t uwTargetCount = 0;

/* Variable to catch HotJoin event */
__IO uint8_t ubHotJoinRequested = 0;

/* Array contain targets descriptor */
TargetDesc_TypeDef *aTargetDesc[2] = \
  {
    &TargetDesc1,       /* DEVICE_ID1 */
    &TargetDesc2        /* DEVICE_ID2 */
  };

/* Buffer that contain payload data, mean PID, BCR, DCR */
uint8_t aPayloadBuffer[64*COUNTOF(aTargetDesc)];

/* Function pointer on function TX buffer treatment */
void (*ptrTXFunc)(void);

/* Completion status */
__IO uint8_t ubFrameComplete = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I3C1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void LED_Blinking(uint32_t Period);
void Handle_ENTDAA_Controller(void);
static void I3C_DynamicAddressTreatment(void);
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

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

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

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /*##- Waiting Hot Join request from the Target #######################*/
    while(ubHotJoinRequested == 0U)
    {
    }

    /*##- Send a Dynamic Address Assignment procedure ####################*/

    /* Handle Dynamic Address Assignment */
    Handle_ENTDAA_Controller();

    /* Wait end of Dynamic Address Assignment procedure */
    while(ubFrameComplete == 0U);

    /* Reset Complete variable */
    ubFrameComplete = 0;

    /* Reset of the variable to catch other HotJoin event. */
    ubHotJoinRequested = 0;
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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL1_SetSource(LL_RCC_PLL1SOURCE_HSE);
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(12);
  LL_RCC_PLL1_SetN(250);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  LL_Init1msTick(250000000);

  LL_SetSystemCoreClock(250000000);
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

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetI3CClockSource(LL_RCC_I3C1_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I3C1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**I3C1 GPIO Configuration
  PB6   ------> I3C1_SCL
  PB7   ------> I3C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_3;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* I3C1 interrupt Init */
  NVIC_SetPriority(I3C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(I3C1_EV_IRQn);
  NVIC_SetPriority(I3C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(I3C1_ER_IRQn);

  /* USER CODE BEGIN I3C1_Init 1 */

  /* USER CODE END I3C1_Init 1 */

  /** I3C Initialization
  */
  LL_I3C_SetMode(I3C1, LL_I3C_MODE_CONTROLLER);
  LL_I3C_SetDataHoldTime(I3C1, LL_I3C_SDA_HOLD_TIME_1_5);
  LL_I3C_SetControllerActivityState(I3C1, LL_I3C_OWN_ACTIVITY_STATE_0);
  LL_I3C_ConfigClockWaveForm(I3C1, 0x00590909);
  LL_I3C_SetCtrlBusCharacteristic(I3C1, 0x103200f8);
  LL_I3C_EnableHJAck(I3C1);

  /** Configure FIFO
  */
  LL_I3C_SetRxFIFOThreshold(I3C1, LL_I3C_RXFIFO_THRESHOLD_1_4);
  LL_I3C_SetTxFIFOThreshold(I3C1, LL_I3C_TXFIFO_THRESHOLD_1_4);
  LL_I3C_DisableControlFIFO(I3C1);
  LL_I3C_DisableStatusFIFO(I3C1);

  /** Configure Controller
  */
  LL_I3C_SetOwnDynamicAddress(I3C1, 0);
  LL_I3C_EnableOwnDynAddress(I3C1);
  LL_I3C_SetStallTime(I3C1, 0x00);
  LL_I3C_DisableStallACK(I3C1);
  LL_I3C_DisableStallParityCCC(I3C1);
  LL_I3C_DisableStallParityData(I3C1);
  LL_I3C_DisableStallTbit(I3C1);
  LL_I3C_DisableHighKeeperSDA(I3C1);

  /** Enable the selected I3C peripheral
  */
  LL_I3C_Enable(I3C1);
  /* USER CODE BEGIN I3C1_Init 2 */
  LL_mDelay(1);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_3;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  LL_I3C_EnableIT_FC(I3C1);
  LL_I3C_EnableIT_HJ(I3C1);
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
  LL_ICACHE_Enable();
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
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Turn-off LED2.
  * @param  None
  * @retval None
  */
void LED_Off(void)
{
  /* Turn LED2 off */
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Toggle LED2.
  * @param  None
  * @retval None
  */
void LED_Toggle(void)
{
  /* Toggle LED2 on */
  LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Turn LED2 on */
  LED_On();

  /* Toggle IO in an infinite loop */
  while (1)
  {
    LED_Toggle();
    LL_mDelay(Period);
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
    /* For loop to get target payload */
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
  * @brief  Target Request Dynamic Address callback.
  * @param  targetPayload Parameter indicates the target payload.
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
  * @brief  Controller Notification callback.
  * @retval None
  */
void Controller_Notification_Callback(void)
{
  /* Update Hot Join request status */
  ubHotJoinRequested = 1U;
}

/**
  * @brief  Controller Complete callback.
  * @retval None
  */
void Controller_Complete_Callback(void)
{
  /* Update Completion status */
  ubFrameComplete = 1;

  /* Toggle LED2: Transfer in Transmission process is correct */
  LED_Toggle();
}

/**
  * @brief  Controller Transmit callback.
  * @retval None
  */
void Controller_Transmit_Callback(void)
{
  ptrTXFunc();
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
    /* Unexpected event : Set LED2 to Blinking mode to indicate error occurs */
    LED_Blinking(LED_BLINK_ERROR);
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
