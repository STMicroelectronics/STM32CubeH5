/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Firmware Update, protections
  *          and crypto testing functionalities.
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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "com.h"
#include "common.h"
#include "secure_nsc.h"
#include "main.h"
#include "low_level_flash.h"

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif /* defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

/** @addtogroup USER_APP User App Example
  * @{
  */


/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

extern ARM_DRIVER_FLASH FLASH_PRIMARY_DATA_NONSECURE_DEV_NAME;
extern ARM_DRIVER_FLASH FLASH_PRIMARY_NONSECURE_DEV_NAME;

#define BOOTLOADER_BASE_NS              0x0BF97000

#ifdef NS_DATA_IMAGE_EN
#define NS_DATA_PRIMARY_OFFSET          (FLASH_BASE_NS + FLASH_AREA_5_OFFSET)
#define NS_DATA_IMAGE_DATA1_SIZE        32U
#define BL2_DATA_HEADER_SIZE            0x20
#endif
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';

/* Private function prototypes -----------------------------------------------*/
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_SecureAppImage(void);
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */
#endif /* (MCUBOOT_APP_IMAGE_NUMBER == 2) */
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_AppImage(void);
#endif /* defined(MCUBOOT_OVERWRITE_ONLY) */
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_SecureDataImage(void);
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */
#endif /* (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
#if !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_NonSecureDataImage(void);
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */
#endif /* (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */

const uint32_t MagicTrailerValue[] =
{
  0xf395c277,
  0x7fefd260,
  0x0f505235,
  0x8079b62c,
};

static void SystemClock_Config(void);
void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);
void LOADER_Run(void);

/* Callbacks prototypes */
void SecureFault_Callback(void);
void SecureError_Callback(void);
void Error_Handler(void);
void NS_DATA_Display(void);

#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
int io_putchar(int ch);
#define PUTCHAR_PROTOTYPE int io_putchar(int ch)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int32_t uart_putc(int32_t ch)
#endif /* __ICCARM__ */

PUTCHAR_PROTOTYPE
{
  COM_Transmit((uint8_t*)&ch, 1, TX_TIMEOUT);
  return ch;
}

/* Redirects printf to TFM_DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
FILE __stdout;

/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
  /* Send byte to USART */
  io_putchar(ch);

  /* Return character written */
  return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to TFM_DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
  int i;

  for (i = 0; i < len; i++)
  {
    /* Send byte to USART */
    uart_putc(str[i]);
  }

  /* Return the number of characters written */
  return len;
}
#elif defined(__ICCARM__)
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
#endif /*  __GNUC__ */



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(int argc, char **argv)
/*int main(void) */
{
  /*  set example to const : this const changes in binary without rebuild */
  pUserAppId = (uint8_t *)&UserAppId;

  /* STM32H5xx HAL library initialization:
  - Systick timer is configured by default as source of time base, but user
  can eventually implement his proper time base source (a general purpose
  timer for example or other time source), keeping in mind that Time base
  duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
  handled in milliseconds basis.
  - Set NVIC Group Priority to 3
  - Low Level Initialization
  */
  HAL_Init();

  /* DeInitialize RCC to allow PLL reconfiguration when configuring system clock */
  HAL_RCC_DeInit();

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure Communication module */
  COM_Init();
  /* register the Secure Fault Call Back */
  /* Register SecureFault callback defined in non-secure and to be called by secure handler */
  SECURE_RegisterCallback(SECURE_FAULT_CB_ID, (void *)SecureFault_Callback);

  /* Register SecureError callback defined in non-secure and to be called by secure handler */
  SECURE_RegisterCallback(GTZC_ERROR_CB_ID, (void *)SecureError_Callback);

  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2023 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                          User App #%c                               =", *pUserAppId);
  printf("\r\n======================================================================");
  printf("\r\n\r\n");

  /* User App firmware runs*/
  FW_APP_Run();

  while (1U)
  {}

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (HSE BYPASS)
  *            SYSCLK(Hz)                     = 250000000  (CPU Clock)
  *            HCLK(Hz)                       = 250000000  (Bus matrix and AHBs Clock)
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1 (APB1 Clock  250MHz)
  *            APB2 Prescaler                 = 1 (APB2 Clock  250MHz)
  *            APB3 Prescaler                 = 1 (APB3 Clock  250MHz)
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 4
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

  /* Enable HSE Bypass and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 250;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
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
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n=================== Main Menu ============================\r\n\n");
  printf("  Start BootLoader -------------------------------------- 1\r\n\n");
#ifdef NS_DATA_IMAGE_EN
  printf("  Display Non secure Data  ------------------------------ 2\r\n\n");
#endif
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2)
  printf("  Validate Secure App Image ----------------------------- 3\r\n\n");
  printf("  Validate NonSecure App Image -------------------------- 4\r\n\n");
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 1)
  printf("  Validate App Image ------------------------------------ 3\r\n\n");
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 1) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
  printf("  Validate Secure Data Image ---------------------------- 5\r\n\n");
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
  printf("  Validate NonSecure Data Image ------------------------- 6\r\n\n");
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_Run(void)
{
  uint8_t key = 0U;

  /*##1- Print Main Menu message*/
  FW_APP_PrintMainMenu();

  while (1U)
  {
    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          LOADER_Run();
          break;
#ifdef NS_DATA_IMAGE_EN
        case '2' :
          NS_DATA_Display();
          break;
#endif
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2)
        case '3':
          FW_Valid_SecureAppImage();
          break;
        case '4':
          FW_Valid_AppImage();
          break;
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 2) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_APP_IMAGE_NUMBER == 1)
        case '3':
          FW_Valid_AppImage();
          break;
#endif /* defined(MCUBOOT_OVERWRITE_ONLY) && defined(MCUBOOT_APP_IMAGE_NUMBER == 1) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
        case '5':
          FW_Valid_SecureDataImage();
          break;
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_S_DATA_IMAGE_NUMBER == 1) */
#if !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
        case '6':
          FW_Valid_NonSecureDataImage();
          break;
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) && (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1) */
        default:
          printf("Invalid Number !\r");
          break;
      }

      /* Print Main Menu message */
      FW_APP_PrintMainMenu();
    }
  }
}


/**
  * @brief  Perform Jump to the BootLoader
  * @retval None.
  */
void LOADER_Run(void)
{
  printf("\r\n  Start config before jumping to the bootloader");
  SECURE_loader_cfg();

  for (int i = 0; i < 16; i++)
  {
  /*SRAM1 -> MPCBB1*/
  GTZC_MPCBB1_NS->SECCFGR[i] = 0;
  }
  uint32_t boot_address = *(uint32_t *)(BOOTLOADER_BASE_NS + 4U);

  /*Increment HDPL to HDPL=3*/
  SET_BIT(SBS->HDPLCR,  SBS_HDPLCR_INCR_HDPL);

  /*  change stack limit  */
  __set_MSPLIM(0);

  __set_MSP((*(uint32_t *)BOOTLOADER_BASE_NS));

  SCB->VTOR = BOOTLOADER_BASE_NS;

  printf("\r\n  Standard Bootloader started");
  printf("\r\n  If you want to connect through USART interface, disconnect your TeraTerm");
  printf("\r\n  Start download with STM32CubeProgrammer through supported interfaces (USART/SPI/I2C/USB)\r\n");
  printf("\r\n");

  __asm volatile("movs r0, %0\n"
               "bx r0\n" :: "r"(boot_address)); /*jump to non-secure address*/
}

#if  !defined(MCUBOOT_OVERWRITE_ONLY)
static void FW_Valid_SecureAppImage(void)
{
    SECURE_ConfirmSecureAppImage();
    printf("  -- Secure App Firmware Confirm Done\r\n\n");
}

static void FW_Valid_SecureDataImage(void)
{
    SECURE_ConfirmSecureDataImage();
    printf("  -- Secure Data Firmware Confirm Done\r\n\n");
}


/**
  * @brief  Write Confirm Flag for  :
  * @brief  - NonSecure Data image
  * @param  None
  * @retval None
  */
static void FW_Valid_NonSecureDataImage(void)
{
  const uint8_t FlagPattern[]={0x1 ,0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff };
  const uint32_t ConfirmAddress = FLASH_AREA_5_OFFSET  + FLASH_AREA_5_SIZE - (sizeof(MagicTrailerValue) + sizeof(FlagPattern));
  if (FLASH_PRIMARY_DATA_NONSECURE_DEV_NAME.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
#if defined(__ARMCC_VERSION)
    printf("  --  Confirm Flag  correctly written %x %x \r\n\n",ConfirmAddress ,FlagPattern[0] );
#else
    printf("  --  Confirm Flag  correctly written %lx %x \r\n\n",ConfirmAddress , FlagPattern[0] );
#endif
  }
  else
  {
    printf("  -- Confirm Flag Not Correctlty Written \r\n\n");
  }
}

static void FW_Valid_AppImage(void)
{
  const uint8_t FlagPattern[]={0x1 ,0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff , 0xff, 0xff, 0xff };
#if (MCUBOOT_APP_IMAGE_NUMBER == 1)
 const uint32_t ConfirmAddress = FLASH_AREA_0_OFFSET  + FLASH_PARTITION_SIZE - (sizeof(MagicTrailerValue) + sizeof(FlagPattern));
#else
  const uint32_t ConfirmAddress = FLASH_AREA_1_OFFSET + FLASH_NS_PARTITION_SIZE - (sizeof(MagicTrailerValue) + sizeof(FlagPattern));
#endif
  if (FLASH_PRIMARY_NONSECURE_DEV_NAME.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
#if defined(__ARMCC_VERSION)
    printf("  --  Confirm Flag  correctly written %x %x \r\n\n",ConfirmAddress ,FlagPattern[0] );
#else
    printf("  --  Confirm Flag  correctly written %lx %x \r\n\n",ConfirmAddress , FlagPattern[0] );
#endif
  }
  else
  {
    printf("  -- Confirm Flag Not Correctlty Written \r\n\n");
  }
}
#endif /* !defined(MCUBOOT_OVERWRITE_ONLY) */


/**
  * @brief  Display the Data on HyperTerminal
  * @param  None.
  * @retval None.
  */
#ifdef NS_DATA_IMAGE_EN
void NS_DATA_Display(void)
{
  uint8_t *data1;
  data1 = (uint8_t*)(NS_DATA_PRIMARY_OFFSET+BL2_DATA_HEADER_SIZE);

  printf("  -- NS Data: %08x%08x..%08x%08x\r\n\n",
               *((int *)(&data1[0])),
               *((int *)(&data1[4])),
               *((int *)(&data1[NS_DATA_IMAGE_DATA1_SIZE - 8])),
               *((int *)(&data1[NS_DATA_IMAGE_DATA1_SIZE - 4]))
              );
}
#endif
/**
  * @brief  Callback called by secure code following a secure fault interrupt
  * @note   This callback is called by secure code thanks to the registration
  *         done by the non-secure application with non-secure callable API
  *         SECURE_RegisterCallback(SECURE_FAULT_CB_ID, (void *)SecureFault_Callback);
  * @retval None
  */
void SecureFault_Callback(void)
{
  /* Go to error infinite loop when Secure fault generated by IDAU/SAU check */
  /* because of illegal access */
  Error_Handler();
}


/**
  * @brief  Callback called by secure code following a GTZC TZIC secure interrupt (GTZC_IRQn)
  * @note   This callback is called by secure code thanks to the registration
  *         done by the non-secure application with non-secure callable API
  *         SECURE_RegisterCallback(GTZC_ERROR_CB_ID, (void *)SecureError_Callback);
  * @retval None
  */
void SecureError_Callback(void)
{
  /* Go to error infinite loop when Secure error generated by GTZC check */
  /* because of illegal access */
  Error_Handler();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  NVIC_SystemReset();
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
  /* User can add his own implementation to report the file name and line number,
   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1U)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */
