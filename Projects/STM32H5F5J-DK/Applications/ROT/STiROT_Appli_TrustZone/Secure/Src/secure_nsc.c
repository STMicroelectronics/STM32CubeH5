/**
  ******************************************************************************
  * @file    secure_nsc.c
  * @author  MCD Application Team
  * @brief   This file contains the non-secure callable APIs (secure world)
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
#include "secure_nsc.h"
#include <stdio.h>
#include "appli_flash_layout.h"

#define BOOTLOADER_BASE_NS                  (0x0BFAC000)
#define BOOTLOADER_SIZE                     (0xC000)
/* Engi bits */
#define ENGI_START                          (0x08FFF800UL)
#define ENGI_SIZE                           (0x40UL)
#define ARRAY_SIZE(array)                   (sizeof(array) / sizeof((array)[0]))
/* SRAM1 configuration
   =================== */
/* SRAM1 NB super-block */
#define GTZC_MPCBB1_NB_VCTR (16U)
/* SRAM3 NB super-block */
#define GTZC_MPCBB3_NB_VCTR (20U)
/* MPCBB : All SRAM block non privileged + privileged */
#define GTZC_MPCBB_ALL_NPRIV (0x00000000UL)
/* MPCBB : All SRAM block non secure */
#define GTZC_MPCBB_ALL_NSEC (0x00000000UL)
#define TZSC_MASK_R1  (GTZC_CFGR1_USART2_Msk | GTZC_CFGR1_USART3_Msk | GTZC_CFGR1_I3C1_Msk | GTZC_CFGR1_IWDG_Msk )

#define TZSC_MASK_R2  (GTZC_CFGR2_USART1_Msk | GTZC_CFGR2_I2C3_Msk | GTZC_CFGR2_I2C4_Msk | GTZC_CFGR2_UCPD1_Msk | \
                       GTZC_CFGR2_TIM17_Msk )
#define TZSC_MASK_R3  (GTZC_CFGR3_OTGFS_Msk | GTZC_CFGR3_OTGHS_Msk | GTZC_CFGR3_ICACHE_REG_Msk | GTZC_CFGR3_CRC_Msk )

/* NVIC configuration
   ================== */
/** Interrupts 0 .. 31 */
/** in ITNS0 no bit is non secure */
#define RSS_NVIC_INIT_ITNS0_VAL      (0x00000000U)

/** Interrupts 32 .. 63 */
/** in ITNS1 no bit is non secure */
#define RSS_NVIC_INIT_ITNS1_VAL      (0x00000000U)

/** Interrupts 64 .. 95 */
/** in ITNS2 OTG_FS_IRQn (position 74) => bit 10 is non secure */
#define RSS_NVIC_INIT_ITNS2_VAL      (0x00000400U)

/** Interrupts 96 .. 127 */
/** in ITNS3 I3C1_EV_IRQn (position 123) => bit 27 is non secure */
#define RSS_NVIC_INIT_ITNS3_VAL      (0x08000000U)

/** Interrupts 128 .. 159 */
/** in ITNS4 no bit is non secure */
#define RSS_NVIC_INIT_ITNS4_VAL      (0x00000000U)

/** Interrupts 160 .. 191 */
/** in ITNS5 OTG_HS_IRQn (position 161) => bit 1 is non secure */
#define RSS_NVIC_INIT_ITNS5_VAL      (0x00000002U)

#define GPIOA_MASK_SECCFG    (GPIO_PIN_MASK & \
                              (GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12))

#define GPIOB_MASK_SECCFG    (GPIO_PIN_MASK & (GPIO_PIN_6  | GPIO_PIN_7 ))

#define GPIOC_MASK_SECCFG    (GPIO_PIN_MASK & (GPIO_PIN_9))

#define GPIOD_MASK_SECCFG    (GPIO_PIN_MASK & (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_13))

/* TAMP ALL register values */
#define TAMP_INIT_SECCFGR_VAL        (0x00000000U)

struct sau_cfg_t
{
  uint32_t RNR;
  uint32_t RBAR;
  uint32_t RLAR;
};
const struct sau_cfg_t sau_load_cfg[] =
{
  /* allow non secure access to SRAM3 */
  {
    0,
    (uint32_t)SRAM1_BASE_NS,
    ((uint32_t)SRAM3_BASE_NS + SRAM3_SIZE - 1U),
  },
  /* allow non secure access to periph */
  {
    1,
    (uint32_t)PERIPH_BASE_NS,
    ((uint32_t)PERIPH_BASE_S + 0xFFFFFFFUL),
  },
  /* allow non secure access to all user flash except secure part and area covered by HDP extension */
  {
    2,
    (uint32_t)FLASH_BASE_NS,
    (uint32_t)(FLASH_BASE_NS + FLASH_SIZE_DEFAULT - 1U),
  },
  /* allow non secure access to bootloader code */
  {
    3,
    (uint32_t)BOOTLOADER_BASE_NS,
    ((uint32_t)BOOTLOADER_BASE_NS + BOOTLOADER_SIZE - 1U),
  },
  /* allow non secure access to Engi bits */
  {
    4,
    (uint32_t)ENGI_START,
    ((uint32_t)ENGI_START + ENGI_SIZE - 1U),
  },
};
/** @addtogroup STM32H5xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Global variables ----------------------------------------------------------*/
void *pSecureFaultCallback = NULL;   /* Pointer to secure fault callback in Non-secure */
void *pSecureErrorCallback = NULL;   /* Pointer to secure error callback in Non-secure */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Secure registration of non-secure callback.
  * @param  CallbackId  callback identifier
  * @param  func        pointer to non-secure function
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func)
{
  if (func != NULL)
  {
    switch (CallbackId)
    {
      case SECURE_FAULT_CB_ID:           /* SecureFault Interrupt occurred */
        pSecureFaultCallback = func;
        break;
      case GTZC_ERROR_CB_ID:             /* GTZC Interrupt occurred */
        pSecureErrorCallback = func;
        break;
      default:
        /* unknown */
        break;
    }
  }
}

#define BANK_NUMBER  2

static void secure_internal_flash(uint32_t offset_start, uint32_t offset_end)
{
  volatile uint32_t *SecBB[16]= {&FLASH_S->SECBB1R1, &FLASH_S->SECBB1R2, &FLASH_S->SECBB1R3, &FLASH_S->SECBB1R4, &FLASH_S->SECBB1R5, &FLASH_S->SECBB1R6, &FLASH_S->SECBB1R7, &FLASH_S->SECBB1R8,
                                &FLASH_S->SECBB2R1, &FLASH_S->SECBB2R2, &FLASH_S->SECBB2R3, &FLASH_S->SECBB2R4, &FLASH_S->SECBB2R5, &FLASH_S->SECBB2R6, &FLASH_S->SECBB2R7, &FLASH_S->SECBB2R8};
  volatile uint32_t *ptr;
  uint32_t regwrite=0x0, index;
  uint32_t block_start = offset_start;
  uint32_t block_end =  offset_end;

  block_start = block_start / FLASH_SECTOR_SIZE;
  block_end = (block_end / FLASH_SECTOR_SIZE) ;

  /* 1f is for 32 bits */
  for (index = block_start & ~0x1f; index < FLASH_SECTOR_NB ; index++)
  { /* clean register on index aligned */
    if (!(index & 0x1f)){
       regwrite=0x0;
    }
    if ((index >= block_start) && (index <= block_end))
      regwrite = regwrite | ( 1 << (index & 0x1f));
    /* write register when 32 sub block are set or last block to set  */
    if ((index & 0x1f ) == 0x1f) {
      ptr = (uint32_t *)SecBB[index>>5];
      *ptr = regwrite;
    }
  }
}

/**
  * @brief  Sau idau configuration before jumping into loader
  * @retval None
  */
CMSE_NS_ENTRY void SECURE_loader_run(void)
{
  uint32_t i = 0U;

  /* configuration stage */
  __HAL_RCC_GTZC1_CLK_ENABLE();

  /* Allow secure to access to non secure */
  GTZC_MPCBB1_S->CR |= GTZC_MPCBB_CR_SRWILADIS_Msk;
  /* All bocks of SRAM1 configured non secure / privileged (default value) */
  for (i = 0; i < GTZC_MPCBB1_NB_VCTR; i++)
  {
    /*SRAM1 -> MPCBB1*/
    GTZC_MPCBB1_S->SECCFGR[i] = GTZC_MPCBB_ALL_NSEC;
    GTZC_MPCBB1_S->PRIVCFGR[i] = GTZC_MPCBB_ALL_NPRIV;
  }
  /* All bocks of SRAM3 configured non secure / privileged (default value) */
  for (i = 0; i < GTZC_MPCBB3_NB_VCTR; i++)
  {
    /*SRAM3 -> MPCBB3*/
    GTZC_MPCBB3_S->SECCFGR[i] = GTZC_MPCBB_ALL_NSEC;
    GTZC_MPCBB3_S->PRIVCFGR[i] = GTZC_MPCBB_ALL_NPRIV;
  }
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Required GPIO configured non secure */
  GPIOA_S->SECCFGR = ~GPIOA_MASK_SECCFG;
  GPIOB_S->SECCFGR = ~GPIOB_MASK_SECCFG;
  GPIOC_S->SECCFGR = ~GPIOC_MASK_SECCFG;
  GPIOD_S->SECCFGR = ~GPIOD_MASK_SECCFG;

  /* disable MPU */
   MPU->CTRL = 0;
   MPU_NS->CTRL = 0;

  /* Required peripherals configured non secure (default value) / privileged */
  GTZC_TZSC1_S->PRIVCFGR1 = ~TZSC_MASK_R1;
  GTZC_TZSC1_S->PRIVCFGR2 = ~TZSC_MASK_R2;
  GTZC_TZSC1_S->PRIVCFGR3 = ~TZSC_MASK_R3;

  GTZC_TZSC1_S->SECCFGR1 = ~TZSC_MASK_R1;
  GTZC_TZSC1_S->SECCFGR2 = ~TZSC_MASK_R2;
  GTZC_TZSC1_S->SECCFGR3 = ~TZSC_MASK_R3;

  for (i = 0U; i < ARRAY_SIZE(sau_load_cfg); i++)
  {
    SAU->RNR = sau_load_cfg[i].RNR;
    SAU->RBAR = sau_load_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (sau_load_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                SAU_RLAR_ENABLE_Msk;
  }

  secure_internal_flash(0x00, S_IMAGE_SECONDARY_PARTITION_OFFSET-1);

  /* Force memory writes before continuing */
  __DSB();
  /* Flush and refill pipeline with updated permissions */
  __ISB();
  /* Enable SAU */
  TZ_SAU_Enable();

   /* Enable HardFault/busFault and NMI exception in ns.
   * It is up to BL to drive non-secure faults
   * Do not enter in secure on non-secure fault
   */
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                           (SCB->AIRCR & 0x0000FFFFU) |
                           SCB_AIRCR_BFHFNMINS_Msk);

  NVIC->ITNS[0U] = RSS_NVIC_INIT_ITNS0_VAL;
  NVIC->ITNS[1U] = RSS_NVIC_INIT_ITNS1_VAL;
  NVIC->ITNS[2U] = RSS_NVIC_INIT_ITNS2_VAL;
  NVIC->ITNS[3U] = RSS_NVIC_INIT_ITNS3_VAL;
  NVIC->ITNS[4U] = RSS_NVIC_INIT_ITNS4_VAL;
  NVIC->ITNS[5U] = RSS_NVIC_INIT_ITNS5_VAL;

  /* TAMP */
  TAMP->SECCFGR  = TAMP_INIT_SECCFGR_VAL;

  /* Stop systick before jumping */
  HAL_SuspendTick();

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

/**
  * @}
  */

/**
  * @}
  */
