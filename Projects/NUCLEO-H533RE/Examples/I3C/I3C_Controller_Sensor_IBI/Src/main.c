/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Controller_Sensor_IBI/Src/main.c
  * @author  MCD Application Team
  * @brief   This sample code shows how to use STM32H5xx I3C HAL API as Controller
  *          to transmit and receive a data buffer
  *          with a communication process based on Interrupt transfer.
  *          The communication is done using STM32H5xx Nucleo and
  *          X-NUCLEO-IKS4A1A.
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

/* Multiple transfer Complete flag */
__IO uint32_t uwIBI_ProcessCplt = 0;

/* Buffer used for transmission */
uint8_t aTxBuffer[16];

/* Buffer used for reception */
uint8_t aRxBuffer[6];

/* Buffer used by HAL to compute control data for the Private Communication */
uint32_t aControlBuffer[15];

/* Variable to read data from FIFO */
int16_t Gyroscope_X = 0;
int16_t Gyroscope_Y = 0;
int16_t Gyroscope_Z = 0;
int16_t Accelerometer_X = 0;
int16_t Accelerometer_Y = 0;
int16_t Accelerometer_Z = 0;
int16_t Temperature = 0;
uint32_t count_Gyroscope = 0U;
uint32_t count_Accelerometer = 0U;
uint32_t count_Temperature = 0U;
uint32_t count_NotDefined = 0U;

/* Variable to display reading data */
uint32_t uwDisplayDelay   = 0U;

/* Array contain targets descriptor */
TargetDesc_TypeDef *aTargetDesc[1] = \
                          {
                            &TargetDesc1       /* DEVICE_ID1 */,
                          };

/* Array contain DISEC CCC associated data */
uint8_t aDISEC_data[1]   = {0x08};

/* Array contain SETDASA CCC associated data */
uint8_t aSETDASA_LSM6DSV16X_data[1]   = {(TARGET1_DYN_ADDR << 1)};

/* Array contain ENEC CCC associated data : Enable IBI */
uint8_t aENEC_data[1]   = {0x01};

/* Buffer used for LSM6DSV16X configuration */

uint8_t aLSM6DSV16X_Config_CTRL3[]          = {LSM6DSV16X_CTRL3_ADDRESS, LSM6DSV16X_CTRL3_VALUE};
uint8_t aLSM6DSV16X_Config_Accelerometer[]  = {LSM6DSV16X_CTRL1_ADDRESS, LSM6DSV16X_CTRL1_VALUE};
uint8_t aLSM6DSV16X_Config_Gyroscope[]      = {LSM6DSV16X_CTRL2_ADDRESS, LSM6DSV16X_CTRL2_VALUE};
uint8_t aLSM6DSV16X_Config_FIFO_WTR[]       = {LSM6DSV16X_FIFO_CTRL1_ADDRESS, LSM6DSV16X_FIFO_CTRL1_VALUE};
uint8_t aLSM6DSV16X_Config_FIFO_STP_WTR[]   = {LSM6DSV16X_FIFO_CTRL2_ADDRESS, LSM6DSV16X_FIFO_CTRL2_VALUE};
uint8_t aLSM6DSV16X_Config_FIFO_BDR[]       = {LSM6DSV16X_FIFO_CTRL3_ADDRESS, LSM6DSV16X_FIFO_CTRL3_VALUE};
uint8_t aLSM6DSV16X_Config_FIFO_IT[]        = {LSM6DSV16X_INT1_CTRL_ADDRESS, LSM6DSV16X_INT1_CTRL_VALUE};
uint8_t aLSM6DSV16X_Config_FIFO_MODE[]      = {LSM6DSV16X_FIFO_CTRL4_ADDRESS, LSM6DSV16X_FIFO_CTRL4_VALUE};

/* Buffer used for LSM6DSV16X read registers */
/* FIFO data output registers + 6 Read continuous register access */
uint8_t aFIFO_DATA[]      = {LSM6DSV16X_FIFO_DATA_OUT_X_L_ADDRESS};
/* FIFO tag register */
uint8_t aFIFO_GET_TAG[]   = {LSM6DSV16X_FIFO_DATA_OUT_TAG_ADDRESS};
/* FIFO status register 1*/
uint8_t aFIFO_STATUS_1[]  = {LSM6DSV16X_FIFO_STATUS1_ADDRESS};
/* FIFO status register 2 */
uint8_t aFIFO_STATUS_2[]  = {LSM6DSV16X_FIFO_STATUS2_ADDRESS};

/* Variable to get IBI information */
I3C_CCCInfoTypeDef CCCInfo;
uint32_t IBITgtNbPayload = 0U;
uint32_t IBICRTgtAddr = 0U;
uint32_t payload = 0U;

/*** Descriptor for broadcast CCC *************************************************************************************/
/* Descriptor for DISEC and RSTDAA CCC */
I3C_CCCTypeDef aBroadcast_DISEC_RSTDAA_CCC[] =
{
  /* Target Addr           CCC Value           CCC data + defbyte pointer  CCC size + defbyte      Direction    */
    {0,                 Broadcast_DISEC,                {aDISEC_data,               1},        HAL_I3C_DIRECTION_WRITE},
    {0,                 Broadcast_RSTDAA,               {NULL,                      0},        HAL_I3C_DIRECTION_WRITE},
};

/*** Descriptor for direct CCC ****************************************************************************************/
/* Descriptor for SETDASA CCC */
I3C_CCCTypeDef aDirect_SETDASA_CCC[] =
{
  /* Target Addr           CCC Value           CCC data + defbyte pointer  CCC size + defbyte         Direction    */
    {LSM6DSV16X_STA,      Direct_SETDASA,            {aSETDASA_LSM6DSV16X_data,     1},        HAL_I3C_DIRECTION_WRITE},
};

/* Descriptor for ENEC and GETBCR CCC */
I3C_CCCTypeDef aDirect_ENEC_GETBCR_CCC[] =
{
  /* Target Addr           CCC Value           CCC data + defbyte pointer  CCC size + defbyte         Direction    */
  {TARGET1_DYN_ADDR,      Direct_ENEC,                 {aENEC_data,                1},         HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,      Direct_GETBCR,               {aRxBuffer,                 1},         HAL_I3C_DIRECTION_READ },
};

/*** Descriptor for private data transfer *****************************************************************************/
/* Descriptor for private write sensor configuration */
I3C_PrivateTypeDef aPrivateDescriptorConfig[] =
{
  /* Target Addr      TxBuffer pointer         Txbuffer size  RxBuffer pointer Rxbuffer size   Direction              */
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_CTRL3         , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_Accelerometer , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_Gyroscope     , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_FIFO_BDR      , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_FIFO_WTR      , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_FIFO_STP_WTR  , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_FIFO_IT       , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {aLSM6DSV16X_Config_FIFO_MODE     , 2}, {NULL,                  0},      HAL_I3C_DIRECTION_WRITE},
};

/* Descriptor for private read FIFO status 1 register*/
I3C_PrivateTypeDef aPrivateDescriptor_FIFO_STATUS1[2] =
{
  /* Target Addr      TxBuffer pointer         Txbuffer size  RxBuffer pointer Rxbuffer size   Direction              */
  {TARGET1_DYN_ADDR,  {aFIFO_STATUS_1,                   1}, {NULL,                  0},       HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {NULL,                             0}, {aRxBuffer,             1},       HAL_I3C_DIRECTION_READ }
};

/* Descriptor for private read FIFO status 2 register*/
I3C_PrivateTypeDef aPrivateDescriptor_FIFO_STATUS2[2] =
{
  /* Target Addr      TxBuffer pointer         Txbuffer size  RxBuffer pointer Rxbuffer size   Direction              */
  {TARGET1_DYN_ADDR,  {aFIFO_STATUS_2,                   1}, {NULL,                   0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {NULL,                             0}, {aRxBuffer,              1},      HAL_I3C_DIRECTION_READ }
};

/* Descriptor for private read FIFO Tag register */
I3C_PrivateTypeDef aPrivateDescriptor_FIFO_Get_TAG[2] =
{
  /* Target Addr      TxBuffer pointer         Txbuffer size  RxBuffer pointer Rxbuffer size   Direction              */
  {TARGET1_DYN_ADDR,  {aFIFO_GET_TAG,                    1}, {NULL,                   0},      HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {NULL,                             0}, {aRxBuffer,              1},      HAL_I3C_DIRECTION_READ }
};

/* Descriptor for private FIFO data registers */
I3C_PrivateTypeDef aPrivateDescriptor_FIFO_DATA[2] =
{
  /* Target Addr      TxBuffer pointer         Txbuffer size  RxBuffer pointer Rxbuffer size   Direction              */
  {TARGET1_DYN_ADDR,  {aFIFO_DATA,                       1}, {NULL,                  0},       HAL_I3C_DIRECTION_WRITE},
  {TARGET1_DYN_ADDR,  {NULL,                             0}, {aRxBuffer,             6},       HAL_I3C_DIRECTION_READ }
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I3C1_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
static void DisplaySensorData(void);
static void ReadFIFO_DataOutput(void);
static uint8_t ReadByte(I3C_PrivateTypeDef* PrivateDescriptor);
static void FlushAllFIFO(void);
#if (defined (__GNUC__) && !defined(__ARMCC_VERSION))
extern void initialise_monitor_handles(void);
#endif
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
  I3C_CtrlTimingTypeDef CtrlTiming = {0};
  LL_I3C_CtrlBusConfTypeDef CtrlBusConf = {0};

#if (defined (__GNUC__) && !defined(__ARMCC_VERSION))
  initialise_monitor_handles();
  printf("Semihosting Test...\n\r");
#endif
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
  /* Configure LED2 */
  BSP_LED_Init(LED2);

  /* Configure USER push-button */
  BSP_PB_Init(BUTTON_USER,BUTTON_MODE_GPIO);

  /*#-STEP1-#- Disable IBI interrupt, Reset a previous Dynamic address ###############*/
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = COUNTOF(aControlBuffer);
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 1;

  /*##- Add context buffer Set CCC frame in Frame context ############################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             aBroadcast_DISEC_RSTDAA_CCC,
                             NULL,
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             COUNTOF(aBroadcast_DISEC_RSTDAA_CCC),
                             I3C_BROADCAST_WITHOUT_DEFBYTE_RESTART) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Transmit private data processus */
  if (HAL_I3C_Ctrl_TransmitCCC_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /*##- Wait for the end of the transfer ############################################*/
  /*  Before starting a new communication transfer, you need to check the current
  state of the peripheral; if it's busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing. */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
  {
  }

  /* Associate a new Dynamic address through Static address */
  aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_2].CtrlBuf.Size    = COUNTOF(aControlBuffer);
  aContextBuffers[I3C_IDX_FRAME_2].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_2].TxBuf.Size      = 1;

  /*#-STEP2-#- Set Dynamic address using Static address #############################*/
  /*##- Add context buffer Set CCC frame in Frame context ###########################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             aDirect_SETDASA_CCC,
                             NULL,
                             &aContextBuffers[I3C_IDX_FRAME_2],
                             COUNTOF(aDirect_SETDASA_CCC),
                             I3C_DIRECT_WITHOUT_DEFBYTE_STOP) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Transmit private data processus */
  if (HAL_I3C_Ctrl_TransmitCCC_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_2]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /*##- Wait for the end of the transfer ############################################*/
  /*  Before starting a new communication transfer, you need to check the current
  state of the peripheral; if it's busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing. */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
  {
  }

  /*#-STEP3-#- Enable IBI interrupt and get BCR              ########################*/
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = COUNTOF(aControlBuffer);
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 1;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.pBuffer   = aRxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.Size      = 1;

  /*##- Add context buffer Set CCC frame in Frame context ###########################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             aDirect_ENEC_GETBCR_CCC,
                             NULL,
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             COUNTOF(aDirect_ENEC_GETBCR_CCC),
                             I3C_DIRECT_WITHOUT_DEFBYTE_STOP) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Multiple transfer private data processus */
  if (HAL_I3C_Ctrl_MultipleTransfer_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_READY)
  {
  }

  /* Now modify a DEVR1, to autorized IBI acknowledgement */
  I3C_DeviceConfTypeDef DeviceConf[1] = {0};

  DeviceConf[0].DeviceIndex        = DEVICE_ID1;
  DeviceConf[0].TargetDynamicAddr  = TARGET1_DYN_ADDR;
  DeviceConf[0].IBIAck             = __HAL_I3C_GET_IBI_CAPABLE(aRxBuffer[0]);
  DeviceConf[0].IBIPayload         = __HAL_I3C_GET_IBI_PAYLOAD(aRxBuffer[0]);
  DeviceConf[0].CtrlRoleReqAck     = __HAL_I3C_GET_CR_CAPABLE(aRxBuffer[0]);
  DeviceConf[0].CtrlStopTransfer   = DISABLE;

  if (HAL_I3C_Ctrl_ConfigBusDevices(&hi3c1, &DeviceConf[0], 1U) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Activate notification for IBI event */
  if (HAL_I3C_ActivateNotification(&hi3c1, NULL, HAL_I3C_IT_IBIIE) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Set bus speed to 12.5 MHz as on LSM6DV16X is the maximum allowed speed */
  /* Calculate the new timing for Controller */
  CtrlTiming.clockSrcFreq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_I3C1);
  CtrlTiming.i3cPPFreq = 12500000;
  CtrlTiming.i2cODFreq = 0;
  CtrlTiming.dutyCycle = 50;
  CtrlTiming.busType = I3C_PURE_I3C_BUS;

  /* Calculate the new timing for Controller */
  I3C_CtrlTimingComputation(&CtrlTiming, &CtrlBusConf);

  /* Update Controller Bus characteristic */
  HAL_I3C_Ctrl_BusCharacteristicConfig(&hi3c1, &CtrlBusConf);

  /*#-STEP4-#- Configure the sensor #################################################*/
  /* Enable auto incremented and enable BDU */
  /* Enable and configure Accelerometer */
  /* Enable and configure Gyroscope */
  /* Configure FIFO */
  /* Enable FIFO Full interrupt */
  /*##- Prepare context buffers process #############################################*/
  /* Prepare Transmit context buffer with the different parameters */
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = COUNTOF(aPrivateDescriptorConfig);
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 16;

  /*##- Add context buffer transmit in Frame context ################################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             NULL,
                             &aPrivateDescriptorConfig[I3C_IDX_FRAME_1],
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             COUNTOF(aPrivateDescriptorConfig),
                             I3C_PRIVATE_WITH_ARB_STOP) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Wait for USER push-button press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_RESET)
  {
  }

  /* Wait for USER push-button release before starting the Communication */
  while (BSP_PB_GetState(BUTTON_USER) != GPIO_PIN_SET)
  {
  }

  /*##- Start the multiple transfer process #########################################*/
  if (HAL_I3C_Ctrl_MultipleTransfer_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /*##- Wait for the end of the transfer ############################################*/
  /*  Before starting a new communication transfer, you need to check the current
  state of the peripheral; if it's busy you need to wait for the end of current
  transfer before starting a new one.
  For simplicity reasons, this example is just waiting till the end of the
  transfer, but application may perform other tasks while transfer operation
  is ongoing.
  In this case, the default state is LISTEN, as an IBI notification is enabled */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_LISTEN)
  {
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* Check if we have an IBI received */
    if (uwIBI_ProcessCplt == 1)
    {
      uwIBI_ProcessCplt = 0;

      /* Get IBI information */
      if (HAL_I3C_GetCCCInfo(&hi3c1, EVENT_ID_IBI, &CCCInfo) == HAL_OK)
      {
        /* Number of data bytes received after an IBI */
        IBITgtNbPayload = CCCInfo.IBITgtNbPayload;

        /* Target address received during accepted IBI */
        IBICRTgtAddr = CCCInfo.IBICRTgtAddr;

        /* Received data payload */
        payload = CCCInfo.IBITgtPayload;
#if ! defined(DISABLE_PRINTF)
        printf("\n###Receive an IBI from (address target: 0x%02x )###\n\r", IBICRTgtAddr);
        printf("Flush the FIFO...\n\r");
#endif /* DISABLE_PRINTF */
        /* Flush the content of the FIFO and update sensor Measurement */
        FlushAllFIFO();
      }
    }

    /*##- Monitor the different value retrieve during the flush of the FIFO */
    /* For simplicity reasons, this example is just display the last data in FIFO */
    /* Display through external Terminal IO the value received */
    DisplaySensorData();
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
  hi3c1.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x95;
  hi3c1.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x63;
  hi3c1.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x95;
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
  * @brief  Controller multiple Direct CCC Command, I3C private or I2C transfer Complete callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_CtrlMultipleXferCpltCallback(I3C_HandleTypeDef *hi3c)
{
  /* Toggle LED2: Transfer in transmission process is correct */
  BSP_LED_Toggle(LED2);
}

/**
  * @brief I3C notify callback after receiving a notification.
  *        The main objective of this user function is to check on the notification ID and assign 1 to the global
  *        variable used to indicate that the event is well finished.
  * @par Called functions
  * - HAL_I3C_NotifyCallback()
  * @retval None
  */
void HAL_I3C_NotifyCallback(I3C_HandleTypeDef *hi3c, uint32_t notifyId)
{
  if ((notifyId & EVENT_ID_IBI) == EVENT_ID_IBI)
  {
    uwIBI_ProcessCplt++;
  }
}
/**
  * @brief  Error callback.
  * @param  hi3c : [IN] Pointer to an I3C_HandleTypeDef structure that contains the configuration information
  *                     for the specified I3C.
  * @retval None
  */
void HAL_I3C_ErrorCallback(I3C_HandleTypeDef *hi3c)
{
  /* Error_Handler() function is called when error occurs. */
  Error_Handler();
}

/**
  * @brief  Read register from sensor.
  * @param  PrivateDescriptor  Pointer to a private read register Descriptor.
  * @retval Register contents.
  */
static uint8_t ReadByte(I3C_PrivateTypeDef* PrivateDescriptor)
{
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = 2;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 1;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.pBuffer   = aRxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.Size      = 1;

  /*##- Add context buffer transmit in Frame context #####################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             NULL,
                             PrivateDescriptor,
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size,
                             I3C_PRIVATE_WITH_ARB_RESTART) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Multiple transfer private data processus */
  if (HAL_I3C_Ctrl_MultipleTransfer_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Wait for the end of the transfer
     Before starting a new communication transfer, you need to check the current
     state of the peripheral; if it's busy you need to wait for the end of current
     transfer before starting a new one.
     In this case, the default state is LISTEN, as we have activated the IBI notification */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_LISTEN)
  {
  }

  return aRxBuffer[0];
}

/**
  * @brief  Read FIFO data output registers.
  * @note   6 Read continuous register access.
  * @param  None.
  * @retval None.
  */
static void ReadFIFO_DataOutput(void)
{
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.pBuffer = aControlBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size    = 2;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.pBuffer   = aTxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].TxBuf.Size      = 1;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.pBuffer   = aRxBuffer;
  aContextBuffers[I3C_IDX_FRAME_1].RxBuf.Size      = 6;

  /*##- Add context buffer transmit in Frame context #####################*/
  if (HAL_I3C_AddDescToFrame(&hi3c1,
                             NULL,
                             aPrivateDescriptor_FIFO_DATA,
                             &aContextBuffers[I3C_IDX_FRAME_1],
                             aContextBuffers[I3C_IDX_FRAME_1].CtrlBuf.Size,
                             I3C_PRIVATE_WITH_ARB_RESTART) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Multiple transfer private data processus */
  if (HAL_I3C_Ctrl_MultipleTransfer_IT(&hi3c1, &aContextBuffers[I3C_IDX_FRAME_1]) != HAL_OK)
  {
    /* Error_Handler() function is called when error occurs. */
    Error_Handler();
  }

  /* Wait for the end of the transfer
     Before starting a new communication transfer, you need to check the current
     state of the peripheral; if it's busy you need to wait for the end of current
     transfer before starting a new one.
     In this case, the default state is LISTEN, as an IBI notification is enabled */
  while (HAL_I3C_GetState(&hi3c1) != HAL_I3C_STATE_LISTEN)
  {
  }
}

/**
  * @brief  Flush the content of FIFO.
  * @param  None.
  * @retval None.
  */
static void FlushAllFIFO(void)
{
/* Temperature offset is 25 degree celsius, mean Temperature value 0 correspond to 25 Celsius degree */
#define TEMP_OFFSET 25

  uint32_t uwUnreadSensorData = 0U;
  uint8_t FIFO_TagRegister = 0U;
  uint8_t FIFO_TagSensor = 0U;
  __IO uint32_t indexFIFO = 0U;

  /* Check the number of unread sensor data stored in FIFO */
  uwUnreadSensorData = ReadByte(aPrivateDescriptor_FIFO_STATUS1);

  for (indexFIFO = 0; indexFIFO < uwUnreadSensorData; indexFIFO++)
  {
    /* Read the FIFO_DATA_OUT_TAG register */
    FIFO_TagRegister = ReadByte(aPrivateDescriptor_FIFO_Get_TAG);

    /* Get tag sensor */
    /* FIFO_DATA_OUT_TAG[7:3], see the description of FIFO_DATA_OUT_TAG register in LSMDSV16X datasheet */
    FIFO_TagSensor = (FIFO_TagRegister & 0xF8) >> 3;

    switch (FIFO_TagSensor)
    {
      /* Gyroscope NC */
      case 0x01:
      {
        ReadFIFO_DataOutput();

        Gyroscope_X = (((int16_t)aRxBuffer[1]) * 256) + (int16_t)aRxBuffer[0];
        Gyroscope_Y = (((int16_t)aRxBuffer[3]) * 256) + (int16_t)aRxBuffer[2];
        Gyroscope_Z = (((int16_t)aRxBuffer[5]) * 256) + (int16_t)aRxBuffer[4];

        /* Increment the number of gyroscope samples */
        count_Gyroscope++;

        break;
      }
      /* Accelerometer NC */
      case 0x02:
      {
        ReadFIFO_DataOutput();

        Accelerometer_X = ((int16_t)aRxBuffer[1])*256 + (int16_t)aRxBuffer[0];
        Accelerometer_Y = ((int16_t)aRxBuffer[3])*256 + (int16_t)aRxBuffer[2];
        Accelerometer_Z = ((int16_t)aRxBuffer[5])*256 + (int16_t)aRxBuffer[4];

        /* Increment the number of accelerometer samples */
        count_Accelerometer++;

        break;
      }
      /* Temperature */
      case 0x03:
      {
        ReadFIFO_DataOutput();

        Temperature = TEMP_OFFSET + \
                      (((int16_t)(((int16_t)aRxBuffer[1] * 256) + \
                       (int16_t)aRxBuffer[0])) / 100);

        /* Increment the number of temperature samples */
        count_Temperature++;

        break;
      }

      default:
      {
        /* Increment the number of not defined samples */
        count_NotDefined++;
        break;
      }
    }
  }

  /* For simplicity reasons, this example is just reset the number of samples, but the application can be used
     for other purposes */
  count_Gyroscope = 0;
  count_Accelerometer = 0;
  count_Temperature = 0;
  count_NotDefined = 0;
}

/**
  * @brief  Display the different value retrieve during the flush of the FIFO.
  * @param  None.
  * @retval None.
  */
static void DisplaySensorData(void)
{
  /* Display the value each DISPLAY_REFRESH_DELAY */
  if (HAL_GetTick() >= (uwDisplayDelay + DISPLAY_REFRESH_DELAY))
  {
#if ! defined(DISABLE_PRINTF)
    printf("Gyroscope X: %d \n\r"      , Gyroscope_X);
    printf("Gyroscope Y: %d \n\r"      , Gyroscope_Y);
    printf("Gyroscope Z: %d \n\r"      , Gyroscope_Z);
    printf("Accelerometer X: %d \n\r"  , Accelerometer_X);
    printf("Accelerometer Y: %d \n\r"  , Accelerometer_Y);
    printf("Accelerometer Z: %d \n\r"  , Accelerometer_Z);
    printf("Temperature: %d \n\r"      , Temperature);
    printf("______________________________________________\n\r");
#endif /* DISABLE_PRINTF */
    uwDisplayDelay+=DISPLAY_REFRESH_DELAY;
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
  /* Error if LED2 is slowly blinking (1 sec. period) */
  while (1)
  {
    BSP_LED_Toggle(LED2);
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
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
