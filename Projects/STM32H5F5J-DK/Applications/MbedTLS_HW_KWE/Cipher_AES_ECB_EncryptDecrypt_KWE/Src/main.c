/**
  ******************************************************************************
  * @file    MbedTLS_HW_KWE/Cipher_AES_ECB_EncryptDecrypt_KWE/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the
  *          Mbed TLS PSA opaque driver with STM32 Key Wrap Engine to wrap
  *          AES-ECB key and use the wrapped key for AES-ECB encryption
  *          decryption.
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
#define PSA_AES_ECB_KEY_ID_USER  ((psa_key_id_t)0x1fff0001)

/* Private variables ---------------------------------------------------------*/
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

uint8_t Plaintext_ECB[] =
{
  0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
  0xAE, 0x2D, 0x8A, 0x57 ,0x1E, 0x03, 0xAC, 0x9C ,0x9E, 0xB7, 0x6F, 0xAC ,0x45, 0xAF, 0x8E, 0x51,
  0x30, 0xC8, 0x1C, 0x46 ,0xA3, 0x5C, 0xE4, 0x11 ,0xE5, 0xFB, 0xC1, 0x19 ,0x1A, 0x0A, 0x52, 0xEF,
  0xF6, 0x9F, 0x24, 0x45 ,0xDF, 0x4F, 0x9B, 0x17 ,0xAD, 0x2B, 0x41, 0x7B ,0xE6, 0x6C, 0x37, 0x10
};

uint8_t Expected_Ciphertext_ECB[] =
{
  0xF3, 0xEE, 0xD1, 0xBD ,0xB5, 0xD2, 0xA0, 0x3C ,0x06, 0x4B, 0x5A, 0x7E ,0x3D, 0xB1, 0x81, 0xF8,
  0x59, 0x1C, 0xCB, 0x10 ,0xD4, 0x10, 0xED, 0x26 ,0xDC, 0x5B, 0xA7, 0x4A ,0x31, 0x36, 0x28, 0x70,
  0xB6, 0xED, 0x21, 0xB9 ,0x9C, 0xA6, 0xF4, 0xF9 ,0xF1, 0x53, 0xE7, 0xB1 ,0xBE, 0xAF, 0xED, 0x1D,
  0x23, 0x30, 0x4B, 0x7A ,0x39, 0xF9, 0xF3, 0xFF ,0x06, 0x7D, 0x8D, 0x8F ,0x9E, 0x24, 0xEC, 0xC7
};

uint8_t Computed_Ciphertext_ECB[sizeof(Expected_Ciphertext_ECB)];
uint8_t Computed_Plaintext_ECB[sizeof(Plaintext_ECB)];

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
  size_t computed_size;
  /* Define key attributes */
  psa_key_attributes_t key_attributes;
  psa_key_handle_t key_handle_aes_ecb;

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
   *                    AES ECB Encryption & Decryption
   *     PSA Persistent Private Key Wrap using STM32 Key Wrap Engine (KWE)
   *        (Persistent Wrapped key is stored using PSA ITS)
   * --------------------------------------------------------------------------
   */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

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
   * SINGLE CALL USAGE
   * --------------------------------------------------------------------------
  */

  /* Compute encrypt the plaintext passing all the needed parameters */
  retval = psa_cipher_encrypt(key_handle_aes_ecb,                    /* The key id */
                              PSA_ALG_ECB_NO_PADDING,                /* Algorithm type */
                              Plaintext_ECB, sizeof(Plaintext_ECB),  /* Plaintext to encrypt */
                              Computed_Ciphertext_ECB,               /* Data buffer to receive generated ciphertext */
                              sizeof(Computed_Ciphertext_ECB),       /* Size of buffer to receive ciphertext */
                              &computed_size);                       /* Size of computed ciphertext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Expected_Ciphertext_ECB))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Ciphertext_ECB, Computed_Ciphertext_ECB, computed_size) != 0)
  {
    Error_Handler();
  }

  /* Decrypt directly ciphertext passing all the needed parameters */
  retval= psa_cipher_decrypt(key_handle_aes_ecb,                   /* The key id */
                             PSA_ALG_ECB_NO_PADDING,               /* Algorithm type */
                             Expected_Ciphertext_ECB,              /*Ciphertext to decrypt */
                             sizeof(Expected_Ciphertext_ECB),      /* Size of IV + Ciphertext to decrypt */
                             Computed_Plaintext_ECB,               /* Data buffer to receive generated plaintext */
                             sizeof(Computed_Plaintext_ECB),       /* Size of buffer to receive plaintext */
                             &computed_size);                      /* Size of computed plaintext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Plaintext_ECB))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Plaintext_ECB, Computed_Plaintext_ECB, computed_size) != 0)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * Destroy the PSA key and clear all data
   * --------------------------------------------------------------------------
  */

  /* Destroy the private key */
  retval = psa_destroy_key(key_handle_aes_ecb);
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
