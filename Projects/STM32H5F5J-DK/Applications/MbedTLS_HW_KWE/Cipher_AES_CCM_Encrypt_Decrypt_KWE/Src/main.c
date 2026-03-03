/**
  ******************************************************************************
  * @file    MbedTLS_HW_KWE/Cipher_AES_CCM_Encrypt_Decrypt_KWE/Src/main.c
  * @author  MCD Application Team
  * @brief   This example provides a short description of how to use the
  *          Mbed TLS PSA opaque driver with STM32 Key Wrap Engine to wrap
  *          AES-GCM key and use the wrapped key for AES-CCM encryption
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
#define PSA_AES_GCM_KEY_ID_USER  ((psa_key_id_t)0x1fff0001)

/* Private variables ---------------------------------------------------------*/
/** Extract from NIST Special Publication 800-38C
  * VTT256.rsp
[Keylen = 256]
[IVlen = 104]
[PTlen = 192]
[AADlen = 256]
[Taglen = 4]

Count = 0
Key = 9074b1ae4ca3342fe5bf6f14bcf2f27904f0b15179d95a654f61e699692e6f71
IV = 2e1e0132468500d4bd47862563
Adata = 3c5f5404370abdcb1edde99de60d0682c600b034e063b7d3237723da70ab7552
PT = 239029f150bccbd67edbb67f8ae456b4ea066a4beee065f9
CT = 9c8d5dd227fd9f81237601830afee4f0115636c8e5d5fd74
Tag = 3cb9afed

  */
const uint8_t Key[] =
{
  0x90, 0x74, 0xb1, 0xae, 0x4c, 0xa3, 0x34, 0x2f, 0xe5, 0xbf, 0x6f, 0x14, 0xbc, 0xf2, 0xf2, 0x79,
  0x04, 0xf0, 0xb1, 0x51, 0x79, 0xd9, 0x5a, 0x65, 0x4f, 0x61, 0xe6, 0x99, 0x69, 0x2e, 0x6f, 0x71
};
const uint8_t IV[] =
{
  0x2e, 0x1e, 0x01, 0x32, 0x46, 0x85, 0x00, 0xd4, 0xbd, 0x47, 0x86, 0x25, 0x63
};
const uint8_t Plaintext[] =
{
  0x23, 0x90, 0x29, 0xf1, 0x50, 0xbc, 0xcb, 0xd6, 0x7e, 0xdb, 0xb6, 0x7f, 0x8a, 0xe4, 0x56, 0xb4,
  0xea, 0x06, 0x6a, 0x4b, 0xee, 0xe0, 0x65, 0xf9
};
const uint8_t AddData[] =
{
  0x3c, 0x5f, 0x54, 0x04, 0x37, 0x0a, 0xbd, 0xcb, 0x1e, 0xdd, 0xe9, 0x9d, 0xe6, 0x0d, 0x06, 0x82,
  0xc6, 0x00, 0xb0, 0x34, 0xe0, 0x63, 0xb7, 0xd3, 0x23, 0x77, 0x23, 0xda, 0x70, 0xab, 0x75, 0x52
};
const uint8_t Expected_Ciphertext[] =
{
  /* Ciphertext */
  0x9c, 0x8d, 0x5d, 0xd2, 0x27, 0xfd, 0x9f, 0x81, 0x23, 0x76, 0x01, 0x83, 0x0a, 0xfe, 0xe4, 0xf0,
  0x11, 0x56, 0x36, 0xc8, 0xe5, 0xd5, 0xfd, 0x74
};
const uint8_t Expected_Tag[] =
{
  0x3c, 0xb9, 0xaf, 0xed
};

const uint8_t Expected_Ciphertext_and_Tag[] =
{
  0x9c, 0x8d, 0x5d, 0xd2, 0x27, 0xfd, 0x9f, 0x81, 0x23, 0x76, 0x01, 0x83, 0x0a, 0xfe, 0xe4, 0xf0,
  0x11, 0x56, 0x36, 0xc8, 0xe5, 0xd5, 0xfd, 0x74,
  0x3c, 0xb9, 0xaf, 0xed
};

/* Computed data buffer */
uint8_t Computed_Ciphertext[sizeof(Expected_Ciphertext) + sizeof(Expected_Tag)];
uint8_t Computed_Plaintext[sizeof(Plaintext)];

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
  psa_key_handle_t key_handle;

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
   *                    AES GCM Encryption & Decryption
   *     PSA Persistent Private Key Wrap using STM32 Key Wrap Engine (KWE)
   *        (Persistent Wrapped key is stored using PSA ITS)
   * --------------------------------------------------------------------------
   */

  /* Init the key attributes */
  key_attributes = psa_key_attributes_init();

  /* Setup the key policy for the private key */
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4));
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attributes, 8U*sizeof(Key));
  /* Set up the key location using PSA_CRYPTO_KWE_DRIVER_LOCATION to wrap the private key using STM32 Key Wrap Engine (KWE) */
  psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT, PSA_CRYPTO_KWE_DRIVER_LOCATION));
  /* Set up persistent Key ID */
  psa_set_key_id(&key_attributes, PSA_AES_GCM_KEY_ID_USER);
  /* Import the private key */
  retval = psa_import_key(&key_attributes, Key, sizeof(Key), &key_handle);
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

  /* Compute directly the ciphertext passing all the needed parameters */
  retval = psa_aead_encrypt(key_handle,                         /* The key id */
                            PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4), /* Algorithm type */
                            IV, sizeof(IV),                     /* Initialization vector */
                            AddData, sizeof(AddData),           /* Additional authenticated data */
                            Plaintext, sizeof(Plaintext),       /* Plaintext to encrypt and authenticate */
                            Computed_Ciphertext,                /* Data buffer to receive ciphertext and auth tag */
                            sizeof(Computed_Ciphertext),        /* Size of buffer to receive ciphertext and tag */
                            &computed_size);                    /* Size of computed ciphertext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != (sizeof(Expected_Ciphertext) + sizeof(Expected_Tag)))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Expected_Ciphertext, Computed_Ciphertext, sizeof(Expected_Ciphertext)) != 0)
  {
    Error_Handler();
  }

  /* Verify generated authentication tag is the expected one */
  if (memcmp(Expected_Tag, &Computed_Ciphertext[sizeof(Expected_Ciphertext)], sizeof(Expected_Tag)) != 0)
  {
    Error_Handler();
  }

  /* Decrypt and verify directly ciphertext and tag passing all the needed parameters */
  retval = psa_aead_decrypt(key_handle,                          /* The key id */
                            PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4), /* Algorithm type */
                            IV, sizeof(IV),                      /* Initialization vector */
                            AddData, sizeof(AddData),            /* Additional authenticated data */
                            Expected_Ciphertext_and_Tag,         /* Ciphertext + tag to decrypt and verify */
                            sizeof(Expected_Ciphertext_and_Tag), /* Sizeof Ciphertext + tag to decrypt and verify */
                            Computed_Plaintext,                  /* Data buffer to receive generated plaintext */
                            sizeof(Computed_Plaintext),          /* Size of data buff to receive generated plaintext */
                            &computed_size);                     /* Size of computed plaintext */

  /* Verify API returned value */
  if (retval != PSA_SUCCESS)
  {
    Error_Handler();
  }

  /* Verify generated data size is the expected one */
  if (computed_size != sizeof(Plaintext))
  {
    Error_Handler();
  }

  /* Verify generated data are the expected ones */
  if (memcmp(Plaintext, Computed_Plaintext,
             computed_size) != 0)
  {
    Error_Handler();
  }

  /* --------------------------------------------------------------------------
   * Destroy the PSA key and clear all data
   * --------------------------------------------------------------------------
  */
  /* Destroy the key */
  retval = psa_destroy_key(key_handle);
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
