/**
  ******************************************************************************
  * @file    loader.c
  * @author  MCD Application Team
  * @brief   Test Protections module.
  *          This file provides set of firmware functions to manage Test Protections
  *          functionalities.
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
#include "main.h"
#include "string.h"
#include "stm32h5xx_hal.h"
#include "low_level_security.h"
#include "stdio.h"

#define BOOTLOADER_BASE_NS              (0x0BF97000)
#define CODE_SECONDARY_OFFSET           (0x001E0000)
#define CODE_SECONDARY_SIZE             (0x00020000)
#ifdef DATA_IMAGE_EN
#define DATA_SECONDARY_OFFSET           (0x001DE000)
#define DATA_SECONDARY_SIZE             (0x00002000)
#endif

extern uint32_t __ICFEDIT_region_RAM_start__;
extern uint32_t __ICFEDIT_region_RAM_end__;
void LOADER_Run(void);
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  Testnumber giving the test that generate a reset
  * @retval None.
  */
void LOADER_Run(void)
{
  printf("\r\n  Standard Bootloader started");
  printf("\r\n  If you want to connect through USART interface, disconnect your TeraTerm");
  printf("\r\n  Start download with STM32CubeProgrammer through supported interfaces (USART/SPI/I2C/USB)\r\n");

  printf("\r\n");

  /* configure GTZC to allow non secure / privileged loader execution */
  gtzc_loader_cfg();

  /* configure GPIO to allow non secure / privileged loader execution */
  gpio_loader_cfg();

  /* reconfigure MPU to allow non secure / privileged loader execution */
  mpu_loader_cfg();

  /* configure SAU to allow non secure / privileged loader execution */
  sau_loader_cfg();

  /* Configure NVIC */
  nvic_loader_cfg();

  /* enable FPU */
  fpu_enable_cfg();

  uint32_t boot_address = cmse_nsfptr_create(*(uint32_t *)(BOOTLOADER_BASE_NS + 4U));

  /*Increment HDPL to HDPL3*/
  SET_BIT(SBS->HDPLCR,  SBS_HDPLCR_INCR_HDPL);

  __TZ_set_MSP_NS((*(uint32_t *)BOOTLOADER_BASE_NS));
  SCB_NS->VTOR = BOOTLOADER_BASE_NS;


  __asm volatile("movs r0, %0\n"
               "movs r1, #0\n" /*clear registers before jumping to non-secure*/
               "movs r2, #0\n"
               "movs r3, #0\n"
               "movs r4, #0\n"
               "movs r5, #0\n"
               "movs r6, #0\n"
               "movs r7, #0\n"
               "mov r8, r5\n"
               "mov r9, r5\n"
               "mov r10, r5\n"
               "mov r11, r5\n"
               "mov r12, r5\n"
               "MSR APSR_nzcvq,r1\n" /*clear APSR*/
               "bxns r0\n"::"r"(boot_address)); /*jump to non-secure address*/
  /*BXNS, no return here possible*/
}
