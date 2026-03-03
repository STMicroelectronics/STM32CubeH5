/**
  ******************************************************************************
  * @file    MbedTLS_SW/Cipher_AES_GCM_Encrypt_Decrypt_MBED/Src/stm32h5xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file contains HAL MSP module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Initialize the Global MSP.
  * @retval None
  */
void HAL_MspInit(void)
{
  __HAL_RCC_RNG_CLK_ENABLE(); /* Enable RNG clock */

  /** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
  */
  HAL_PWREx_DisableBatteryCharging();

}

/**
  * @brief RNG MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hrng: RNG handle pointer
  * @retval None
  */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{
  if (hrng->Instance == RNG)
  {
    /** Initializes the peripherals clock
      */
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      while(1);
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RNG;
    PeriphClkInit.RngClockSelection = RCC_RNGCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      while (1);
    }
    /* Peripheral clock enable */
    __HAL_RCC_RNG_CLK_ENABLE();
  }
}

/**
  * @brief RNG MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hrng: RNG handle pointer
  * @retval None
  */
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{
  if (hrng->Instance == RNG)
  {
    /* Peripheral clock disable */
    __HAL_RCC_RNG_CLK_DISABLE();
    /* Enable RNG reset state */
    __HAL_RCC_RNG_FORCE_RESET();
    /* Release RNG from reset state */
    __HAL_RCC_RNG_RELEASE_RESET();
  }
}


#if defined(HAL_CRYP_MODULE_ENABLED)

/**
  * @brief  Initializes the CRYP MSP.
  * @param  hcryp pointer to a CRYP_HandleTypeDef structure that contains
  *         the configuration information for CRYP module
  * @retval None
  */
void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp)
{
  if(hcryp->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspInit 0 */

  /* USER CODE END AES_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_AES_CLK_ENABLE();
    /* AES interrupt Init */
    HAL_NVIC_SetPriority(AES_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(AES_IRQn);
  /* USER CODE BEGIN AES_MspInit 1 */

  /* USER CODE END AES_MspInit 1 */
  }
  else if (hcryp->Instance==SAES)
  {
    __HAL_RCC_SAES_CLK_ENABLE();
    /* AES interrupt Init */
    HAL_NVIC_SetPriority(SAES_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SAES_IRQn);
  }
}

/**
* @brief CRYP MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcryp: CRYP handle pointer
* @retval None
*/
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef* hcryp)
{
  if(hcryp->Instance==AES)
  {
  /* USER CODE BEGIN AES_MspDeInit 0 */

  /* USER CODE END AES_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_AES_CLK_DISABLE();

    /* AES interrupt DeInit */
    HAL_NVIC_DisableIRQ(AES_IRQn);
  /* USER CODE BEGIN AES_MspDeInit 1 */

  /* USER CODE END AES_MspDeInit 1 */
  }
  else if (hcryp->Instance==SAES)
  {
  /* USER CODE BEGIN AES_MspDeInit 0 */

  /* USER CODE END AES_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SAES_CLK_DISABLE();

    /* AES interrupt DeInit */
    HAL_NVIC_DisableIRQ(SAES_IRQn);
  /* USER CODE BEGIN AES_MspDeInit 1 */

  /* USER CODE END AES_MspDeInit 1 */
  }
}
#endif /* HAL_CRYP_MODULE_ENABLED */

#if defined(HAL_HASH_MODULE_ENABLED)
/**
  * @brief  Initialize the HASH MSP.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module
  * @retval None
  */
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash)
{
  /* USER CODE BEGIN HASH_MspInit 0 */

  /* USER CODE END HASH_MspInit 0 */
  /* Peripheral clock enable */
  __HAL_RCC_HASH_CLK_ENABLE();
  /* HASH interrupt Init */
  HAL_NVIC_SetPriority(HASH_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(HASH_IRQn);
  /* USER CODE BEGIN HASH_MspInit 1 */

  /* USER CODE END HASH_MspInit 1 */
}

/**
  * @brief  DeInitialize the HASH MSP.
  * @param  hhash pointer to a HASH_HandleTypeDef structure that contains
  *         the configuration information for HASH module
  * @retval None
  */
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash)
{
  /* USER CODE BEGIN HASH_MspInit 0 */

  /* USER CODE END HASH_MspInit 0 */
  /* Peripheral clock disable */
  __HAL_RCC_HASH_CLK_DISABLE();

  /* HASH interrupt DeInit */
  HAL_NVIC_DisableIRQ(HASH_IRQn);
  /* USER CODE BEGIN HASH_MspInit 1 */

  /* USER CODE END HASH_MspInit 1 */
}
#endif /* HAL_HASH_MODULE_ENABLED */ 

#if defined(HAL_PKA_MODULE_ENABLED)
/**
* @brief  PKA MSP Initialization
* @param  hpka: PKA handle pointer
* @retval None
*/
void HAL_PKA_MspInit(PKA_HandleTypeDef* hpka)
{
  if(hpka->Instance==PKA)
  {
  /* USER CODE BEGIN PKA_MspInit 0 */

  /* USER CODE END PKA_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_PKA_CLK_ENABLE();
  /* USER CODE BEGIN PKA_MspInit 1 */

  /* USER CODE END PKA_MspInit 1 */
  }
}

/**
* @brief  PKA MSP De-Initialization
* @param  hpka: PKA handle pointer
* @retval None
*/
void HAL_PKA_MspDeInit(PKA_HandleTypeDef* hpka)
{
  if(hpka->Instance==PKA)
  {
  /* USER CODE BEGIN PKA_MspDeInit 0 */

  /* USER CODE END PKA_MspDeInit 0 */

  /* Reset PKA RAM */
    HAL_PKA_RAMReset(hpka);

  /* Peripheral clock disable */
    __HAL_RCC_PKA_CLK_DISABLE();

  /* USER CODE BEGIN PKA_MspDeInit 1 */

  /* USER CODE END PKA_MspDeInit 1 */
  }
}
#endif /* HAL_PKA_MODULE_ENABLED */

