/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    CEC/CEC_DataExchange_Device_1/Src/main.c 
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use the CEC through 
  *          the STM32H5xx HAL API.
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

CEC_HandleTypeDef hcec;

   uint8_t Tab_Rx[16];
/* USER CODE BEGIN PV */
TS_Init_t hTouchScreen;
uint32_t x_size, y_size;
uint8_t  Tab_Tx[CEC_MAX_PAYLOAD-1]; /* Transmitted data buffer.
                                     * header is not included in Tab_Tx.
                                     *  Max size = 15 bytes.
                                     *  one opcode followed by up to 14 operands.
                                     *  When payload size = 0, only the header is sent
                                     *  (ping operation) */
__IO uint8_t ReceivedFrame       = 0x0;  /* Set when a reception occurs */
__IO uint16_t NbOfReceivedBytes  = 0x0;  /* Number of received bytes in addition to the header.
                                     * when a ping message has been received (header
                                     * only), NbOfReceivedBytes = 0 */
__IO uint8_t StartSending        = 0x0;  /* Set when a transmission is triggered by the user */
uint32_t TxSize                  = 0x0;  /* Number of bytes to transmit in addition to the header.
                                     * In case of ping operation (only the header sent),
                                     * TxSize = 0 */
uint8_t DestinationAddress = 0;     /* Destination logical address */
uint8_t InitiatorAddress = 0;       /* Initiator logical address */
__IO uint8_t TxStatus = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_HDMI_CEC_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void CEC_FlushRxBuffer(void);
static void CEC_SetHint(void);
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
  MX_HDMI_CEC_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */

  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  
  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Display test description on screen */
  CEC_SetHint();

  /* -2- Configure touch screen */
  hTouchScreen.Width = 240;
  hTouchScreen.Height = 240;
  hTouchScreen.Orientation = TS_ORIENTATION_LANDSCAPE;
  hTouchScreen.Accuracy = 5;

  /* Touchscreen initialization */
  BSP_TS_Init(0, &hTouchScreen);
  BSP_TS_EnableIT(0);  /* Touch screen interrupt configuration and enable */

  /* CEC device initialization */
  
  DestinationAddress = 0x3; /* follower address */
  InitiatorAddress = 0x1;

  /* -3- CEC configuration (transfer will take place in Interrupt mode) */

  /* -4- CEC transfer general variables initialization */
  ReceivedFrame = 0;
  StartSending = 0;
  NbOfReceivedBytes = 0;
  CEC_FlushRxBuffer();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* if no reception has occurred and no error has been detected,
     * transmit a message if the user has pushed a button */
    while( (StartSending == 1) && (ReceivedFrame == 0))
    {
      HAL_CEC_Transmit_IT(&hcec, InitiatorAddress, DestinationAddress, (uint8_t *)&Tab_Tx, TxSize);
      /* loop until TX ends or TX error reported */
        while (TxStatus != 1);
      StartSending = 0;
      TxStatus = 0;
    }  
    
    /* if a frame has been received */
    if (ReceivedFrame == 1)
    { 
      if (Tab_Rx[1] == 0x44) /* Test on the opcode value */
      {
        /* Receive command is equal to Command 1 */
        if (Tab_Rx[2] == 0x41) /* Test on the operand value */
        {
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
          UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
          UTIL_LCD_DisplayStringAt(0, (y_size/2)+30, (uint8_t *)"  Received opcode 44, operand 41  ", CENTER_MODE);
        }
        else if (Tab_Rx[2] == 0x42) /* Receive command is equal to Command 2 */
        {
          UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
          UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_GREEN);
          UTIL_LCD_DisplayStringAt(0, (y_size/2)+30, (uint8_t *)"  Received opcode 44, operand 42  ", CENTER_MODE);
        }
      }
      else if (Tab_Rx[1] == 0x46) /* Test on the opcode value */
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ORANGE);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+30, (uint8_t *)"       Received opcode 46         ", CENTER_MODE);
      }
      else if (Tab_Rx[1] == 0x9F) /* Test on the opcode value */
      {
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_DARKMAGENTA);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+30, (uint8_t *)"       Received opcode 9F         ", CENTER_MODE);
      }
      ReceivedFrame = 0;
    }
    else if (ReceivedFrame == 2) /* means CEC error detected */
    { 
      UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
      UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_RED);
      UTIL_LCD_DisplayStringAt(0, (y_size/2)+45, (uint8_t *)"            CEC Error             ", CENTER_MODE);
      ReceivedFrame = 0;
    }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
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
  * @brief HDMI_CEC Initialization Function
  * @param None
  * @retval None
  */
static void MX_HDMI_CEC_Init(void)
{

  /* USER CODE BEGIN HDMI_CEC_Init 0 */

  /* USER CODE END HDMI_CEC_Init 0 */

  /* USER CODE BEGIN HDMI_CEC_Init 1 */

  /* USER CODE END HDMI_CEC_Init 1 */
  hcec.Instance = CEC;
  hcec.Init.SignalFreeTime = CEC_DEFAULT_SFT;
  hcec.Init.Tolerance = CEC_STANDARD_TOLERANCE;
  hcec.Init.BRERxStop = CEC_NO_RX_STOP_ON_BRE;
  hcec.Init.BREErrorBitGen = CEC_BRE_ERRORBIT_NO_GENERATION;
  hcec.Init.LBPEErrorBitGen = CEC_LBPE_ERRORBIT_NO_GENERATION;
  hcec.Init.BroadcastMsgNoErrorBitGen = CEC_BROADCASTERROR_NO_ERRORBIT_GENERATION;
  hcec.Init.SignalFreeTimeOption = CEC_SFT_START_ON_TXSOM;
  hcec.Init.ListenMode = CEC_FULL_LISTENING_MODE;
  hcec.Init.OwnAddress = CEC_OWN_ADDRESS_1;
  hcec.Init.RxBuffer = Tab_Rx;
  if (HAL_CEC_Init(&hcec) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN HDMI_CEC_Init 2 */

  /* USER CODE END HDMI_CEC_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Display CEC test Hint
  * @param  None
  * @retval None
  */
static void CEC_SetHint(void)
{
  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set Touchscreen Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 120, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"HDMI CEC", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"Please use the Touchscreen to send", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 45, (uint8_t *)"CEC commands                      ", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 60, (uint8_t *)"Top:    send opcode 44, operand 41", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 75, (uint8_t *)"Bottom: send opcode 44, operand 42", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 90, (uint8_t *)"Left:   send opcode 46            ", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 105, (uint8_t *)"Right:  send opcode 9F            ", CENTER_MODE);

  /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 130, x_size - 20, y_size - 140, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 131, x_size - 22, y_size - 142, UTIL_LCD_COLOR_BLUE);
}

/**
  * @brief Tx Transfer completed callback
  * @param hcec: CEC handle
  * @retval None
  */
void HAL_CEC_TxCpltCallback(CEC_HandleTypeDef *hcec)
{
  /* End of transmission */
  TxStatus =1;
}


/**
  * @brief Rx Transfer completed callback
  * @param hcec: CEC handle
  * @retval None
  */
void HAL_CEC_RxCpltCallback(CEC_HandleTypeDef *hcec, uint32_t RxFrameSize)
{
  ReceivedFrame = 1;
}

/**
  * @brief CEC error callbacks
  * @param hcec: CEC handle
  * @retval None
  */
void HAL_CEC_ErrorCallback(CEC_HandleTypeDef *hcec)
{
  ReceivedFrame = 2;
}

/**
  * @brief  Reset CEC reception buffer
  * @param  None
  * @retval None
  */
static void CEC_FlushRxBuffer(void)
{
  uint32_t cpt;
  
  for (cpt = CEC_MAX_PAYLOAD; cpt > 0; cpt--)
  {
    Tab_Rx[cpt-1] = 0;
  }
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void BSP_TS_Callback(uint32_t Instance)
{
  TS_State_t  TS_State;
  
  if(Instance == 0)
  {
    BSP_TS_GetState(0, &TS_State);
    if(TS_State.TouchDetected)
    {
      if (TS_State.TouchY < 30)
      {
        /* Top of the screen touch detected */
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+45, (uint8_t *)" Send opcode 44, operand 41 ", CENTER_MODE);
        Tab_Tx[0] = 0x44;
        Tab_Tx[1] = 0x41;
        TxSize    = 0x02;
        StartSending = 1;
      }
      else if (TS_State.TouchY > (y_size - 30))
      {
        /* Bottom of the screen touch detected */
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_GREEN);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+45, (uint8_t *)" Send opcode 44, operand 42 ", CENTER_MODE);
        Tab_Tx[0] = 0x44;
        Tab_Tx[1] = 0x42;
        TxSize    = 0x02;
        StartSending = 1;
      }
      else if (TS_State.TouchX < 30)
      {
        /* Left of the screen touch detected */
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ORANGE);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+45, (uint8_t *)"       Send opcode 46       ", CENTER_MODE);
        Tab_Tx[0] = 0x46;
        TxSize    = 0x01;
        StartSending = 1;
      }
      else if (TS_State.TouchX > (x_size - 30))
      {
        /* Right of the screen touch detected */
        UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
        UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_DARKMAGENTA);
        UTIL_LCD_DisplayStringAt(0, (y_size/2)+45, (uint8_t *)"       Send opcode 9F       ", CENTER_MODE);
        Tab_Tx[0] = 0x9F;
        TxSize    = 0x01;
        StartSending = 1;
      }
      /* Wait for touch screen no touch detected */
      do
      {
        BSP_TS_GetState(0, &TS_State);
      }while(TS_State.TouchDetected > 0);
    }
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
