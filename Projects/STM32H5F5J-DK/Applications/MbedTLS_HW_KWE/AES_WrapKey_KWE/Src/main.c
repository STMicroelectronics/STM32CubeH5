/**
  ******************************************************************************
  * @file    MbedTLS_HW_KWE/AES_WrapKey_KWE/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the
  *          Mbed TLS PSA opaque driver with STM32 Key Wrap Engine to wrap
  *          AES-GCM, AES-ECB, and AES-CBC keys.
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
#include <string.h>
#include "main.h"
#include "crypto.h"
#include "kwe_psa_driver_interface.h"

/* Global variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  FAILED = 0,
  PASSED = 1
} TestStatus;
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* User key ID should be between PSA_KEY_ID_USER_MIN and PSA_KWE_KEY_ID_RSSE_MIN
 * Key ID from PSA_KWE_KEY_ID_RSSE_MIN to PSA_KEY_ID_USER_MAX are reserved for
 * RSSE keys.
 */
#define PSA_AES_GCM_KEY_ID_USER  ((psa_key_id_t)0x1fff0001)
#define PSA_AES_ECB_KEY_ID_USER  ((psa_key_id_t)0x1fff0002)
#define PSA_AES_CBC_KEY_ID_USER  ((psa_key_id_t)0x1fff0003)

/* Private variables ---------------------------------------------------------*/
/* AES GCM */
/** Extract from NIST Special Publication 800-38D
  * gcmEncryptExtIV256.rsp
  * [Keylen = 128]
  * [IVlen = 96]
  * [PTlen = 408]
  * [AADlen = 384]
  * [Taglen = 128]
  *
  * Count = 2
  * aes_key = 463b412911767d57a0b33969e674ffe7845d313b88c6fe312f3d724be68e1fca
  * IV = 611ce6f9a6880750de7da6cb
  * PT = e7d1dcf668e2876861940e012fe52a98dacbd78ab63c08842cc9801ea581682ad54af0c34d0d7f6f59e8ee0bf4900e0fd85042
  * AAD = 0a682fbc6192e1b47a5e0868787ffdafe5a50cead3575849990cdd2ea9b3597749403efb4a56684f0c6bde352d4aeec5
  * CT = 8886e196010cb3849d9c1a182abe1eeab0a5f3ca423c3669a4a8703c0f146e8e956fb122e0d721b869d2b6fcd4216d7d4d3758
  * Tag = 2469cecd70fd98fec9264f71df1aee9a
  */
const uint8_t Aes_GCM_Key[] =
{
  0x46, 0x3b, 0x41, 0x29, 0x11, 0x76, 0x7d, 0x57, 0xa0, 0xb3, 0x39, 0x69, 0xe6, 0x74, 0xff, 0xe7,
  0x84, 0x5d, 0x31, 0x3b, 0x88, 0xc6, 0xfe, 0x31, 0x2f, 0x3d, 0x72, 0x4b, 0xe6, 0x8e, 0x1f, 0xca
};

/* AES CBC */
/** Extract from NIST Special Publication 800-38A
  * F.2.1 CBC-AES128.Encrypt
  * Key 2b7e151628aed2a6abf7158809cf4f3c
  * IV 000102030405060708090a0b0c0d0e0f
  * Block #1
  * Plaintext 6bc1bee22e409f96e93d7e117393172a
  * Input Block 6bc0bce12a459991e134741a7f9e1925
  * Output Block 7649abac8119b246cee98e9b12e9197d
  * Ciphertext 7649abac8119b246cee98e9b12e9197d
  * Block #2
  * Plaintext ae2d8a571e03ac9c9eb76fac45af8e51
  * Input Block d86421fb9f1a1eda505ee1375746972c
  * Output Block 5086cb9b507219ee95db113a917678b2
  * Ciphertext 5086cb9b507219ee95db113a917678b2
  * Block #3
  * Plaintext 30c81c46a35ce411e5fbc1191a0a52ef
  * Input Block 604ed7ddf32efdff7020d0238b7c2a5d
  * Output Block 73bed6b8e3c1743b7116e69e22229516
  * Ciphertext 73bed6b8e3c1743b7116e69e22229516
  * Block #4
  * Plaintext f69f2445df4f9b17ad2b417be66c3710
  * Input Block 8521f2fd3c8eef2cdc3da7e5c44ea206
  * Output Block 3ff1caa1681fac09120eca307586e1a7
  * Ciphertext 3ff1caa1681fac09120eca307586e1a7
  */
const uint8_t Aes_CBC_Key[] =
{
  0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

/* AES ECB */
/** Extract from NIST Special Publication 800-38A
  * F.1.5 ECB-AES256.Encrypt
  * Key 603deb1015ca71be2b73aef0857d7781
  *     1f352c073b6108d72d9810a30914dff4
  * Block #1
  * Plaintext 6bc1bee22e409f96e93d7e117393172a
  * Input Block 6bc1bee22e409f96e93d7e117393172a
  * Output Block f3eed1bdb5d2a03c064b5a7e3db181f8
  * Ciphertext f3eed1bdb5d2a03c064b5a7e3db181f8
  * Block #2
  * Plaintext ae2d8a571e03ac9c9eb76fac45af8e51
  * Input Block ae2d8a571e03ac9c9eb76fac45af8e51
  * Output Block 591ccb10d410ed26dc5ba74a31362870
  * Ciphertext 591ccb10d410ed26dc5ba74a31362870
  * Block #3
  * Plaintext 30c81c46a35ce411e5fbc1191a0a52ef
  * Input Block 30c81c46a35ce411e5fbc1191a0a52ef
  * Output Block b6ed21b99ca6f4f9f153e7b1beafed1d
  * Ciphertext b6ed21b99ca6f4f9f153e7b1beafed1d
  * Block #4
  * Plaintext f69f2445df4f9b17ad2b417be66c3710
  * Input Block f69f2445df4f9b17ad2b417be66c3710
  * Output Block 23304b7a39f9f3ff067d8d8f9e24ecc7
  * Ciphertext 23304b7a39f9f3ff067d8d8f9e24ecc7
  */
uint8_t Aes_ECB_Key[] =
{
  0x60, 0x3D, 0xEB, 0x10 ,0x15, 0xCA, 0x71, 0xBE ,0x2B, 0x73, 0xAE, 0xF0 ,0x85, 0x7D, 0x77, 0x81,
  0x1F, 0x35, 0x2C, 0x07 ,0x3B, 0x61, 0x08, 0xD7 ,0x2D, 0x98, 0x10, 0xA3 ,0x09, 0x14, 0xDF, 0xF4
};

__IO TestStatus glob_status = FAILED;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Error_Handler(void);
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  psa_status_t retval;

  /* Define key attributes */
  psa_key_attributes_t key_attributes;
  psa_key_handle_t key_handle_aes_gcm; /* AES CBC Key */
  psa_key_handle_t key_handle_aes_ecb; /* AES ECB Key */
  psa_key_handle_t key_handle_aes_cbc; /* AES CBC Key */

  /* STM32H5xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the System clock */
  SystemClock_Config();

  /* Configure LED3 */
  BSP_LED_Init(LED3);

  /* --------------------------------------------------------------------------
   *                    PSA Crypto library Initialization
   * --------------------------------------------------------------------------
   */
  retval = psa_crypto_init();
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   *                   STM32 Key Wrap Engine
   * --------------------------------------------------------------------------
   */

  /* --------------------------------------------------------------------------
   *                    AES GCM Key Import
   *     PSA Persistent Private Key Wrap using STM32 Key Wrap Engine (KWE)
   *        (Persistent Wrapped key is stored using PSA ITS)
   * --------------------------------------------------------------------------
   */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_GCM);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Aes_GCM_Key));
  /* Set up the key location using PSA_CRYPTO_KWE_DRIVER_LOCATION to wrap the private key using STM32 Key Wrap Engine (KWE) */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_CRYPTO_KWE_DRIVER_LOCATION));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, PSA_AES_GCM_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Aes_GCM_Key, sizeof(Aes_GCM_Key), &key_handle_aes_gcm);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* --------------------------------------------------------------------------
   *                    AES CBC Key Import
   *     PSA Persistent Private Key Wrap using STM32 Key Wrap Engine (KWE)
   *        (Persistent Wrapped key is stored using PSA ITS)
   * --------------------------------------------------------------------------
   */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CBC_NO_PADDING/*PSA_ALG_CBC_NO_PADDINGPSA_ALG_GCM*/);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Aes_CBC_Key));
  /* Set up the key location using PSA_CRYPTO_KWE_DRIVER_LOCATION to wrap the private key using STM32 Key Wrap Engine (KWE) */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_CRYPTO_KWE_DRIVER_LOCATION));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, PSA_AES_CBC_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Aes_CBC_Key, sizeof(Aes_CBC_Key), &key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* --------------------------------------------------------------------------
   *                    AES ECB Key Import
   *     PSA Persistent Private Key Wrap using STM32 Key Wrap Engine (KWE)
   *        (Persistent Wrapped key is stored using PSA ITS)
   * --------------------------------------------------------------------------
   */

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECB_NO_PADDING);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Aes_ECB_Key));
  /* Set up the key location using PSA_CRYPTO_KWE_DRIVER_LOCATION to wrap the private key using STM32 Key Wrap Engine (KWE) */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_CRYPTO_KWE_DRIVER_LOCATION));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, PSA_AES_ECB_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Aes_ECB_Key, sizeof(Aes_ECB_Key), &key_handle_aes_ecb);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Reset the key attribute */
  psa_reset_key_attributes(&key_attributes);

  /* --------------------------------------------------------------------------
   * Destroy the PSA key and clear all data
   * --------------------------------------------------------------------------
  */

  /* Destroy the private keys */
  retval = psa_destroy_key(key_handle_aes_gcm);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  retval = psa_destroy_key(key_handle_aes_ecb);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  retval = psa_destroy_key(key_handle_aes_cbc);
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Clear all data associated with the PSA layer */
  mbedtls_psa_crypto_free();

  /* Turn on LED3 in an infinite loop in case of Key Wrap operations are successful */
  BSP_LED_On(LED3);
  glob_status = PASSED;
  while (1)
  {}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL
  *            SYSCLK(Hz)                     = 250000000  (CPU Clock)
  *            HCLK(Hz)                       = 250000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  250MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  250MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  250MHz)
  *            HSE Frequency(Hz)              = 48000000
  *            PLL_M                          = 24
  *            PLL_N                          = 250
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* The voltage scaling allows optimizing the power consumption when the device is
  clocked below the maximum system frequency, to update the voltage scaling value
  regarding system frequency refer to product datasheet.
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Use HSE in bypass mode and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 24;
  RCC_OscInitStruct.PLL.PLLN = 250;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /* Select PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK3
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2| RCC_CLOCKTYPE_PCLK3);
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
  * @brief  This function is executed in case of error occurrence
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  /* Toggle LED3 @2Hz to notify error condition */
  while (1)
  {
    BSP_LED_Toggle(LED3);
    HAL_Delay(250);
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif /* USE_FULL_ASSERT */
