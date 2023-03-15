/* USER CODE BEGIN Header */
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
/* DMA flags declaration */
__IO uint32_t TransferCompleteDetected = 0U, TransferErrorDetected = 0U;
uint32_t DMA_Flags;

/* Source data buffer */
static uint32_t aSRC_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DMA_HandleTypeDef handle_GPDMA1_Channel7;

RAMCFG_HandleTypeDef hramcfg_SRAM2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPDMA1_Init(void);
static void MX_GPIO_Init(void);
static void MX_RAMCFG_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
void Fill_SRAM2Page_DMA(uint32_t PageIdx);

static uint32_t Check_SRAM2Page_Content(uint32_t PageIdx, uint32_t WriteProtectionState);

static void TransferComplete(DMA_HandleTypeDef *hdma);
static void TransferError(DMA_HandleTypeDef *hdma);
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
  /* Initialize LED1 and LED3 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPDMA1_Init();
  MX_GPIO_Init();
  MX_RAMCFG_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Register MSP Init callback for RAMCFG peripheral */
  if (HAL_RAMCFG_RegisterCallback(&hramcfg_SRAM2, HAL_RAMCFG_MSPINIT_CB_ID, HAL_RAMCFG_MspInit) != HAL_OK)
  {
    Error_Handler();
  }

  /* Register MSP Init callback for RAMCFG peripheral */
  if (HAL_RAMCFG_RegisterCallback(&hramcfg_SRAM2, HAL_RAMCFG_MSPDEINIT_CB_ID, HAL_RAMCFG_MspDeInit) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initialize the RAMCFG peripheral */
  if (HAL_RAMCFG_Init(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Erase the the whole SRAM2 */
  if (HAL_RAMCFG_Erase(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }


  /*##-2- Fill SRAM2 pages when all pages are unprotected ####################*/
  /* Repeat for all SRAM2 pages */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    Fill_SRAM2Page_DMA(pageIdx);

    /* Wait for end of transmission or an error occurred */
    while ((TransferCompleteDetected != (pageIdx + 1U)) && (TransferErrorDetected == 0U));

    /* Check DMA error */
    if (TransferErrorDetected == 1U)
    {
      Error_Handler();
    }
  }

  /* Check all SRAM2 pages content */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    if (Check_SRAM2Page_Content(pageIdx, UNPROTECTED) != 0U)
    {
      Error_Handler();
    }
  }


  /*##-3- Fill SRAM2 pages when half pages are unprotected ####################*/
  /* Reset DMA flags */
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;

  /* Erase the the hole SRAM2 */
  if (HAL_RAMCFG_Erase(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Repeat for all DMA pages */
  /* Enable write protection for odd index pages */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    if ((pageIdx % 2U) != 0U)
    {
      if (HAL_RAMCFG_EnableWriteProtection(&hramcfg_SRAM2, pageIdx, 1U) != HAL_OK)
      {
        Error_Handler();
      }
    }
  }

  /* Repeat for all SRAM2 pages */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    Fill_SRAM2Page_DMA(pageIdx);

    /* Wait for end of transmission or an error occurred */
    if ((pageIdx % 2U) != 0U)
    {
      while (TransferErrorDetected != ((pageIdx / 2U) + 1U));
    }
    else
    {
      while (TransferCompleteDetected != (pageIdx / 2U) + 1U);
    }
  }

  /* DMA transactions */
  if ((TransferCompleteDetected != (SRAM2_PAGE_NUM / 2U)) || (TransferErrorDetected != (SRAM2_PAGE_NUM / 2U)))
  {
    Error_Handler();
  }

  /* Check unprotected SRAM2 pages content */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    if ((pageIdx % 2U) != 0U)
    {
      if (Check_SRAM2Page_Content(pageIdx, PROTECTED) != 0U)
      {
        Error_Handler();
      }
    }
    else
    {
      if (Check_SRAM2Page_Content(pageIdx, UNPROTECTED) != 0U)
      {
        Error_Handler();
      }
    }
  }


  /*##-4- Fill SRAM2 pages when no pages are unprotected ####################*/
  /* Reset DMA flags */
  TransferCompleteDetected = 0U;
  TransferErrorDetected    = 0U;

  /* Erase the the hole SRAM2 */
  if (HAL_RAMCFG_Erase(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }

  /* Repeat for all DMA pages */
  /* Enable write protection for odd index pages */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    if ((pageIdx % 2U) == 0U)
    {
      if (HAL_RAMCFG_EnableWriteProtection(&hramcfg_SRAM2, pageIdx, 1U) != HAL_OK)
      {
        Error_Handler();
      }
    }
  }

  /* Repeat for all SRAM2 pages */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    Fill_SRAM2Page_DMA(pageIdx);

    /* Wait for end of transmission or an error occurred */
    while ((TransferErrorDetected != (pageIdx + 1U)) && (TransferCompleteDetected == 0U));

    /* Check DMA error */
    if (TransferCompleteDetected != 0U)
    {
      Error_Handler();
    }
  }

  /* Check unprotected SRAM2 pages content */
  for (uint32_t pageIdx = 0U; pageIdx < SRAM2_PAGE_NUM; pageIdx++)
  {
    if (Check_SRAM2Page_Content(pageIdx, PROTECTED) != 0U)
    {
      Error_Handler();
    }
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_Delay(250);
    BSP_LED_Toggle(LED1);
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
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  DMA_RepeatBlockConfTypeDef RepeatBlockConfig = {0};

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel7_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  handle_GPDMA1_Channel7.Instance = GPDMA1_Channel7;
  handle_GPDMA1_Channel7.Init.Request = DMA_REQUEST_SW;
  handle_GPDMA1_Channel7.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
  handle_GPDMA1_Channel7.Init.Direction = DMA_MEMORY_TO_MEMORY;
  handle_GPDMA1_Channel7.Init.SrcInc = DMA_SINC_INCREMENTED;
  handle_GPDMA1_Channel7.Init.DestInc = DMA_DINC_INCREMENTED;
  handle_GPDMA1_Channel7.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
  handle_GPDMA1_Channel7.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
  handle_GPDMA1_Channel7.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
  handle_GPDMA1_Channel7.Init.SrcBurstLength = 1;
  handle_GPDMA1_Channel7.Init.DestBurstLength = 1;
  handle_GPDMA1_Channel7.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT1|DMA_DEST_ALLOCATED_PORT1;
  handle_GPDMA1_Channel7.Init.TransferEventMode = DMA_TCEM_REPEATED_BLOCK_TRANSFER;
  handle_GPDMA1_Channel7.Init.Mode = DMA_NORMAL;
  if (HAL_DMA_Init(&handle_GPDMA1_Channel7) != HAL_OK)
  {
    Error_Handler();
  }
  RepeatBlockConfig.RepeatCount = 8;
  RepeatBlockConfig.SrcAddrOffset = 0;
  RepeatBlockConfig.DestAddrOffset = 0;
  RepeatBlockConfig.BlkSrcAddrOffset = -128;
  RepeatBlockConfig.BlkDestAddrOffset = 0;
  if (HAL_DMAEx_ConfigRepeatBlock(&handle_GPDMA1_Channel7, &RepeatBlockConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel7, DMA_CHANNEL_NPRIV) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

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
  * @brief RAMCFG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RAMCFG_Init(void)
{

  /* USER CODE BEGIN RAMCFG_Init 0 */

  /* USER CODE END RAMCFG_Init 0 */

  /* USER CODE BEGIN RAMCFG_Init 1 */

  /* USER CODE END RAMCFG_Init 1 */

  /** Initialize RAMCFG SRAM2
  */
  hramcfg_SRAM2.Instance = RAMCFG_SRAM2;
  if (HAL_RAMCFG_Init(&hramcfg_SRAM2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RAMCFG_Init 2 */

  /* USER CODE END RAMCFG_Init 2 */

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
/**
  * @brief  Fill SRAM2 pages with DMA
  * @param  None
  * @retval None
  */
void Fill_SRAM2Page_DMA(uint32_t PageIdx)
{
  /* DMA Initialization Function */
  MX_GPDMA1_Init();

  /* Select Callbacks functions called after Half Transfer Complete, Transfer Complete and Transfer Error */
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel7, HAL_DMA_XFER_CPLT_CB_ID, TransferComplete);
  HAL_DMA_RegisterCallback(&handle_GPDMA1_Channel7, HAL_DMA_XFER_ERROR_CB_ID, TransferError);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&handle_GPDMA1_Channel7, (uint32_t)&aSRC_Buffer, (SRAM2_BASE + (PageIdx * SRAM2_PAGE_SIZE)), (BUFFER_SIZE * 4U)) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Check SRAM2 pages content
  * @param  None
  * @retval None
  */
static uint32_t Check_SRAM2Page_Content(uint32_t PageIdx, uint32_t WriteProtectionState)
{
  for (uint32_t buffIdx = 0U; buffIdx < 8U; buffIdx++)
  {
    for (uint32_t idx = 0U; idx < BUFFER_SIZE; idx++)
    {
      if (WriteProtectionState == UNPROTECTED)
      {
        if (aSRC_Buffer[idx] != (*(uint32_t *)(SRAM2_BASE + (PageIdx * SRAM2_PAGE_SIZE) + \
          (idx * 4U)  + (buffIdx * BUFFER_SIZE * 4U))))
        {
          return 1U;
        }
      }
      else
      {
        if (0U != (*(uint32_t *)(SRAM2_BASE + (PageIdx * SRAM2_PAGE_SIZE) + \
          (idx * 4U)  + (buffIdx * BUFFER_SIZE * 4U))))
        {
          return 1U;
        }
      }
    }
  }

  return 0U;
}

/**
  * @brief  DMA transfer complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void TransferComplete(DMA_HandleTypeDef *hdma)
{
  TransferCompleteDetected++;
}

/**
  * @brief  DMA transfer error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void TransferError(DMA_HandleTypeDef *hdma)
{
  TransferErrorDetected++;
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
  /* Turn LED3 on */
  BSP_LED_On(LED3);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
