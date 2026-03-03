/**
  ******************************************************************************
  * @file    stm32h5xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h5xx_it.h"

#include "openbl_core.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd;
extern TIM_HandleTypeDef htim6;

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* WORKAROUND: This is used to clear the NMI generated when reading an empty OTP */
  uint32_t flash_data_ecc_flag;
  uint32_t flash_data_eccd_flag;
  uint32_t flash_otp_ecc_flag;

  /* Read the ECCD flags from FLASH insterface to check if there is an error */
  flash_data_ecc_flag  = ((FLASH->ECCDETR & FLASH_ECCR_DATA_ECC) >> 21U);
  flash_otp_ecc_flag   = ((FLASH->ECCDETR & FLASH_ECCR_OTP_ECC) >> 24U);
  flash_data_eccd_flag = ((FLASH->ECCDETR & FLASH_ECCR_ECCD) >> 31U);

  if ((flash_data_ecc_flag == 0x1U) || (flash_otp_ecc_flag == 0x1U) || (flash_data_eccd_flag == 0x1U))
  {
    /* Clear the ECC flags and then return to finish executing the code */
    FLASH->ECCDETR &= ~(FLASH_ECCR_DATA_ECC | FLASH_ECCR_OTP_ECC);
    FLASH->ECCDETR |= FLASH_ECCR_ECCD;

    return;
  }
  else
  {
    while (1)
    {
    }
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {
  }
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
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/******************************************************************************/
/* STM32H5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

/**
  * @brief This function handles USB FS global interrupt.
  */
void OTG_HS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd);
}
