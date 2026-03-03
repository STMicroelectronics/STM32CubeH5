/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "RGB565_240x130_1.h"
#include "RGB565_240x130_2.h"
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

DMA2D_HandleTypeDef hdma2d;

/* USER CODE BEGIN PV */
/* Blended image Ready flag */
static __IO uint32_t   blended_image_ready = 0;
static uint32_t   offset_address_area_blended_image_in_lcd_buffer =  0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_DMA2D_Init(void);
/* USER CODE BEGIN PFP */
static uint8_t CopyImageToLcdFrameBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize);
static void TransferError(DMA2D_HandleTypeDef* Dma2dHandle);
static void TransferComplete(DMA2D_HandleTypeDef* Dma2dHandle);
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
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint8_t  lcd_status = BSP_ERROR_NONE;

  
  blended_image_ready = 0;
  offset_address_area_blended_image_in_lcd_buffer =  ((((WVGA_RES_Y - LAYER_SIZE_Y) / 2) * WVGA_RES_X)
                                                    +   ((WVGA_RES_X - LAYER_SIZE_X) / 2))
                                                    * ARGB8888_BYTE_PER_PIXEL;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Configure LED1, LED2 and LED3 */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_ORANGE);
  BSP_LED_Init(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  /* USER CODE BEGIN 2 */
  /* Proceed to LTDC and LCD screen initialization */
  lcd_status = BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  if(lcd_status != BSP_ERROR_NONE)
  {
     Error_Handler();
  }

  /* Prepare using DMA2D the 480*272 LCD frame buffer to display background color black */
  /* and title of the example                                                           */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font16);

  /* Print example description */
  UTIL_LCD_DisplayStringAt(0, 20, (uint8_t *)"DMA2D_MemToMemWithBlending example", CENTER_MODE);

  HAL_Delay(200);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /*##-1- Copy Foreground image in center of LCD frame buffer ################################################*/
    UTIL_LCD_DisplayStringAt(0, 50, (uint8_t *)"Display Foreground Image :", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 70, (uint8_t *)"     life.augmented     ", CENTER_MODE); 
    /* Blocking copy Foreground image buffer to LCD frame buffer center area */
    /* Using the DMA2D in polling mode                            */
    lcd_status = CopyImageToLcdFrameBuffer((void*)&(RGB565_240x130_1[0]),
                                                (void*)(LCD_FRAME_BUFFER + offset_address_area_blended_image_in_lcd_buffer),
                                                LAYER_SIZE_X,
                                                LAYER_SIZE_Y);
    if(lcd_status != BSP_ERROR_NONE)
    {
       Error_Handler();
    }
    HAL_Delay(2000);  


    /*##-2- Copy Background image in center of LCD frame buffer ################################################*/
    UTIL_LCD_DisplayStringAt(0, 50, (uint8_t *)"Display Background Image :", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 70, (uint8_t *)"        ST  Logo        ", CENTER_MODE);
    /* Blocking copy Background image buffer to LCD frame buffer center area */
    /* Using the DMA2D in polling mode  */    
    lcd_status = CopyImageToLcdFrameBuffer((void*)&(RGB565_240x130_2[0]),
                                                (void*)(LCD_FRAME_BUFFER + offset_address_area_blended_image_in_lcd_buffer),
                                                LAYER_SIZE_X,
                                                LAYER_SIZE_Y);
    if(lcd_status != BSP_ERROR_NONE)
    {
      Error_Handler();
    }    
   HAL_Delay(2000); 


    /*##-3- foreground and background Blending using the DMA2D ################################################*/
    UTIL_LCD_DisplayStringAt(0, 50, (uint8_t *)"Display Blended Image :   ", CENTER_MODE);
    UTIL_LCD_DisplayStringAt(0, 70, (uint8_t *)"ST Logo + life.augmented", CENTER_MODE);

    MX_DMA2D_Init();
    /*##-4- Start DMA2D transfer in interrupt mode ################################################*/
    /*## RGB565_240x130_1[] is the foreground layer and LCD_FRAME_BUFFER is the background layer */
    hal_status = HAL_DMA2D_BlendingStart_IT(&hdma2d,
                                            (uint32_t)&RGB565_240x130_1,   /* Foreground image */
                                            (uint32_t)&RGB565_240x130_2,   /* Background image */
                                            (uint32_t)(LCD_FRAME_BUFFER + offset_address_area_blended_image_in_lcd_buffer),   /* Destination address */
                                             LAYER_SIZE_X ,  /* width in pixels   */
               LAYER_SIZE_Y);  /* height in pixels   */
    if(hal_status != BSP_ERROR_NONE)
    {
      Error_Handler();
    }   
    /* Wait until blended image is ready to be displayed */
    while(blended_image_ready == 0) {;}
  
    HAL_Delay(2000); 
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_CSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 24;
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
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */
  /* DMA2D Callbacks Configuration */
  hdma2d.XferCpltCallback  = TransferComplete;
  hdma2d.XferErrorCallback = TransferError;
  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M_BLEND;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
  hdma2d.Init.OutputOffset = WVGA_RES_X - LAYER_SIZE_X;
  hdma2d.Init.BytesSwap = DMA2D_BYTES_REGULAR;
  hdma2d.Init.LineOffsetMode = DMA2D_LOM_PIXELS;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0x7F;
  hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;
  hdma2d.LayerCfg[1].ChromaSubSampling = DMA2D_NO_CSS;
  hdma2d.LayerCfg[0].InputOffset = 0;
  hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
  hdma2d.LayerCfg[0].InputAlpha = 0x7F;
  hdma2d.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;
  hdma2d.LayerCfg[0].RedBlueSwap = DMA2D_RB_REGULAR;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

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
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Copy and convert image (LAYER_SIZE_X, LAYER_SIZE_Y) of format RGB565
  * to LCD frame buffer area centered in WVGA resolution.
  * The area of copy is of size (LAYER_SIZE_X, LAYER_SIZE_Y) in ARGB8888.
  * @param  pSrc: Pointer to source buffer : source image buffer start here
  * @param  pDst: Pointer to destination buffer LCD frame buffer center area start here
  * @param  xSize: Buffer width (LAYER_SIZE_X here)
  * @param  ySize: Buffer height (LAYER_SIZE_Y here)
  * @retval LCD Status : BSP_ERROR_NONE or LCD_ERROR
  */
static uint8_t CopyImageToLcdFrameBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize)
{
  DMA2D_HandleTypeDef hdma2d_dk;
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint8_t lcd_status = BSP_ERROR_NONE;

  memset(&hdma2d_dk, 0, sizeof(hdma2d_dk));
  /* Configure the DMA2D Mode, Color Mode and output offset */
  hdma2d_dk.Init.Mode          = DMA2D_M2M_PFC;
  hdma2d_dk.Init.ColorMode     = DMA2D_OUTPUT_RGB888; /* Output color out of PFC */
  hdma2d_dk.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;  /* No Output Alpha Inversion*/  
  hdma2d_dk.Init.RedBlueSwap   = DMA2D_RB_REGULAR;     /* No Output Red & Blue swap */   

  /* Output offset in pixels == nb of pixels to be added at end of line to come to the  */
  /* first pixel of the next line : on the output side of the DMA2D computation         */
  hdma2d_dk.Init.OutputOffset = WVGA_RES_X - LAYER_SIZE_X;

  /* Foreground Configuration */
  hdma2d_dk.LayerCfg[1].AlphaMode      = DMA2D_NO_MODIF_ALPHA;
  hdma2d_dk.LayerCfg[1].InputAlpha     = 0x7F; /* fully opaque */
  hdma2d_dk.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d_dk.LayerCfg[1].InputOffset    = 0;
  hdma2d_dk.LayerCfg[1].RedBlueSwap    = DMA2D_RB_REGULAR; /* No ForeGround Red/Blue swap */
  hdma2d_dk.LayerCfg[1].AlphaInverted  = DMA2D_REGULAR_ALPHA; /* No ForeGround Alpha inversion */
  
  hdma2d_dk.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d_dk) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d_dk, 1) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hdma2d_dk, (uint32_t)pSrc, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* Polling For DMA transfer */
        hal_status = HAL_DMA2D_PollForTransfer(&hdma2d_dk, 10);
        if(hal_status == HAL_OK)
        {
          /* return good status on exit */
          lcd_status = BSP_ERROR_NONE;
        }
      }
    }
  }

  return(lcd_status);
}
/**
  * @brief  DMA2D Transfer completed callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
static void TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{
  /* Turn LED_GREEN On */
  BSP_LED_On(LED_GREEN);
  /* The Blended image is now ready for display */
  blended_image_ready = 1;
}

/**
  * @brief  DMA2D error callbacks
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
static void TransferError(DMA2D_HandleTypeDef *hdma2d)
{
  /* Turn LED_ORANGE On */
  BSP_LED_On(LED_ORANGE);
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
    BSP_LED_On(LED_RED);
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
