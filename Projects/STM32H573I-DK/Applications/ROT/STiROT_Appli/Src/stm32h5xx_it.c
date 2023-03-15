/**
  ******************************************************************************
  * @file    stm32h5xx_it.c
  * @author  MCD Application Team
  * @brief   Secure Interrupt Service Routines.
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


/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "stm32h5xx_it.h"
#include "main.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers             */
/******************************************************************************/

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  printf("\r\nHardFault");

  HAL_NVIC_SystemReset();
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  printf("\r\nMemManageFault");

  HAL_NVIC_SystemReset();
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  while (1)
  {
  }

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  while (1)
  {
  }

}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{

  HAL_IncTick();
}
