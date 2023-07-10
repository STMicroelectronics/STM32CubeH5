/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ThreadX/Tx_CMSIS_Wrapper/CM7/Src/app_tx_cmsisrtos.c
  * @author  MCD Application Team
  * @brief   ThreadX CMSIS RTOS Wrapper applicative file
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
#include "app_tx_cmsisrtos.h"

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
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart3;
__IO uint32_t OsStatus = 0;
osThreadId_t ThreadOne;
osThreadId_t ThreadTwo;
APP_SYNC_TYPE SyncObject;
static osThreadAttr_t attr = {
                        .priority = osPriorityNormal,
                        .stack_size = APP_THREAD_STACK_SIZE,
                      };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void ThreadOne_Entry(void *argument);
static void ThreadTwo_Entry(void *argument);
static void Led_Toggle(uint32_t delay);

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */
/* USER CODE END PFP */

/* Global user code ---------------------------------------------------------*/
/* USER CODE BEGIN Global user code */

/* USER CODE END Global user code */
/**
  * @brief  Application ThreadX with CMSIS RTOS Wrapper Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
uint32_t App_TX_CmsisRTOS_Init(void)
{
  osStatus_t ret = osOK;
  /* USER CODE BEGIN  App_TX_CmsisRTOS_Init */
  ret = osKernelInitialize();

  if (ret == osOK)
  {
    /* Create the thread(s) */
    /* definition and creation of ThreadOne */
    attr.name = "ThreadOne";
    ThreadOne = osThreadNew(ThreadOne_Entry, NULL, (const osThreadAttr_t *)&attr);
    if(ThreadOne == NULL)
    {
      return ((uint32_t)osError);
    }
    /* definition and creation of ThreadTwo */
    attr.name = "ThreadTwo";
    ThreadTwo = osThreadNew(ThreadTwo_Entry, NULL, (const osThreadAttr_t *)&attr);
    if(ThreadTwo == NULL)
    {
      return ((uint32_t)osError);
    }

    SyncObject = APP_SYNC_CREATE();
    /* Start scheduler */
    ret = osKernelStart();
  }
  /* USER CODE END  App_TX_CmsisRTOS_Init */
  return ((uint32_t)ret);
}

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN Private user code */

/* USER CODE END Private user code */

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing the ThreadOne thread.
  * @param  argument: Not used
  * @retval None
  */
static void ThreadOne_Entry(void *argument)
{
  (void) argument;
  uint32_t iteration = 0;

  /* Infinite loop */
  while(1)
  {
    /* try to acquire the sync object without waiting */
    if (APP_SYNC_GET(SyncObject, DEFAULT_WAIT_TIME) == osOK)
    {
      printf("** ThreadOne : SyncObject acquired ** \n");

      /*sync object acquired, toggle the LED_GREEN each 500ms for 5s */
      Led_Toggle(50);


      /*release the sync object */
      APP_SYNC_PUT(SyncObject);

      printf("** ThreadOne : SyncObject released ** \n");

      osDelay(1);
    }
    else
    {

      if ((iteration % 20) == 0)
      {
        printf("** ThreadOne : waiting for SyncObject !! **\n");
      }
    }
    iteration++;
  }
}

/**
* @brief Function implementing the ThreadTwo thread.
* @param argument: Not used
* @retval None
*/
static void ThreadTwo_Entry(void *argument)
{
  (void) argument;
  uint32_t iteration = 0;

  /* Infinite loop */
  while(1)
  {
    /* try to acquire the sync object without waiting */
    if (APP_SYNC_GET(SyncObject, DEFAULT_WAIT_TIME) == osOK)
    {
      printf("** ThreadTwo : SyncObject acquired ** \n");

      /*Sync object acquired toggle the LED_GREEN each 200ms for 2s*/
      Led_Toggle(20);

      /*release the sync object*/
      APP_SYNC_PUT(SyncObject);

      printf("** ThreadTwo : SyncObject released ** \n");

      osDelay(1);
    }
    else
    {
      if ((iteration % 20) == 0)
      {
        printf("** ThreadTwo : waiting for SyncObject !! **\n");
      }
    }
    iteration++;
  }
}

/**
  * @brief Critical section function that needs acquiring SyncObject.
  * @param  led: LED to toggle
  * @param  iter: Number of iterations
  * @retval None
  */
static void Led_Toggle(uint32_t delay)
{
  uint32_t i;

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

  for (i =0; i < 10; i++)
  {
    /* Toggle Led */
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    osDelay(delay);
  }
}
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of putchar here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

/* USER CODE END 1 */
