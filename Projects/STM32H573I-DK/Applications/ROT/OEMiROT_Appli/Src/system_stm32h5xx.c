/**
  ******************************************************************************
  * @file    system_stm32h5xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  *          to be used in secure application when the system implements
  *          the security.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32h5xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *   After each device reset the HSI (64 MHz) is used as system clock source.
  *   Then SystemInit() function is called, in "startup_stm32h5xx.s" file, to
  *   configure the system clock before to branch to main program.
  *
  *   This file configures the system clock as follows:
  *=============================================================================
  *-----------------------------------------------------------------------------
  *        System Clock source                     | HSI
  *-----------------------------------------------------------------------------
  *        SYSCLK(Hz)                              | 64000000
  *-----------------------------------------------------------------------------
  *        HCLK(Hz)                                | 64000000
  *-----------------------------------------------------------------------------
  *        AHB Prescaler                           | 1
  *-----------------------------------------------------------------------------
  *        APB1 Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB2 Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        APB3 Prescaler                          | 1
  *-----------------------------------------------------------------------------
  *        HSI Division factor                     | 1
  *-----------------------------------------------------------------------------
  *        PLL1_SRC                                | No clock
  *-----------------------------------------------------------------------------
  *        PLL1_M                                  | Prescaler disabled
  *-----------------------------------------------------------------------------
  *        PLL1_N                                  | 129
  *-----------------------------------------------------------------------------
  *        PLL1_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL1_Q                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL1_R                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL1_FRACN                              | 0
  *-----------------------------------------------------------------------------
  *        PLL2_SRC                                | No clock
  *-----------------------------------------------------------------------------
  *        PLL2_M                                  | Prescaler disabled
  *-----------------------------------------------------------------------------
  *        PLL2_N                                  | 129
  *-----------------------------------------------------------------------------
  *        PLL2_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL2_Q                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL2_R                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL2_FRACN                              | 0
  *-----------------------------------------------------------------------------
  *        PLL3_SRC                                | No clock
  *-----------------------------------------------------------------------------
  *        PLL3_M                                  | Prescaler disabled
  *-----------------------------------------------------------------------------
  *        PLL3_N                                  | 129
  *-----------------------------------------------------------------------------
  *        PLL3_P                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL3_Q                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL3_R                                  | 2
  *-----------------------------------------------------------------------------
  *        PLL3_FRACN                              | 0
  *-----------------------------------------------------------------------------
  *=============================================================================
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup STM32H5xx_system
  * @{
  */

/** @addtogroup STM32H5xx_System_Private_Includes
  * @{
  */

#include "stm32h5xx.h"
#include "main.h"

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Defines
  * @{
  */
#if !defined  (HSE_VALUE)
  #define HSE_VALUE    (25000000UL) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    (4000000UL)  /*!< Value of the Internal oscillator in Hz*/
#endif /* CSI_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    (64000000UL) /*!< Value of the Internal oscillator in Hz */
#endif /* HSI_VALUE */

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#if !defined(VECT_TAB_OFFSET)
#define VECT_TAB_OFFSET  0x00U /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */
#endif /* VECT_TAB_OFFSET */

/* Definitions for RCC PWR GTZC TZSC & TZIC ALL register values */
/* RCC ALL register values */
#define RCC_AHB1ENR_ALL         (0x013ED103UL)
#define RCC_AHB2ENR_ALL         (0xC13F1DFFUL)
#define RCC_AHB4ENR_ALL         (0x00111880UL)
#define RCC_APB1LENR_ALL        (0xDFFFC9FFUL)
#define RCC_APB1HENR_ALL        (0x0080022BUL)
#define RCC_APB2ENR_ALL         (0x017F7800UL)
#define RCC_APB3ENR_ALL         (0x00F0FBD2UL)
#define RCC_SECCFGR_ALL         (0x00003BFFUL)

/* GTZC ALL register values */

#define TZIC1_IER1_ALL         (0xFFFFFFFFUL)
#define TZIC1_IER2_ALL         (0xFF0FFF17UL)
#define TZIC1_IER3_ALL         (0x05FFFF57UL)
#define TZIC1_IER4_ALL         (0x3F1F1FDFUL)

#define TZSC1_SECCFGR1_ALL     (0xFFFFFFFFUL)
#define TZSC1_SECCFGR2_ALL     (0xFF0FFF17UL)
#define TZSC1_SECCFGR3_ALL     (0x05FFFF57UL)

/* PWR ALL register values */
#define PWR_SECCFGR_ALL        (0x0000F8FFUL)

/* GPIO ALL register values */
#define GPIOA_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOB_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOC_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOD_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOE_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOF_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOG_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOH_SECCFGR_ALL      (0x0000FFFFUL)
#define GPIOI_SECCFGR_ALL      (0x00000FFFUL)

/* SYSCFG ALL register values */
///////////#define SYSCFG_SECCFGR_ALL     (0x)

/* DMA ALL register values */
#define GPDMA1_SECCFGR_ALL     (0x000000FFUL)

/* EXTI ALL register values */
#define EXTI_SECCFGR1_ALL      (0xFFFFFFFFUL)

/* RTC ALL register values */
#define RTC_SECCFGR_ALL        (0x0000D00FUL)

/* TAMP ALL register values */
#define TAMP_SECCFGR_ALL       (0x80FF80FFUL)

/******************************************************************************/

static void flash_init_cfg(void);

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Variables
  * @{
  */
  /* The SystemCoreClock variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 64000000U;

  const uint8_t  AHBPrescTable[16] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};
  const uint8_t  APBPrescTable[8] =  {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U};
/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_FunctionPrototypes
  * @{
  */
/*
*/
void SecureSystemInit(void);
/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Functions
  * @{
  */

/*----------------------------------------------------------------------------
  * @brief  Initialize the flash to full secure.
  * @param  None
  * @retval None
 *----------------------------------------------------------------------------*/
static void flash_init_cfg(void)
{
  const uint32_t flash_bb_all_sec = 0xFFFFFFFF;
  __IO uint32_t *reg;
  uint32_t i = 0U;

  /* Configure Bank1 flash on Secure */
  reg = &(FLASH->SECBB1R1);

  for (i = 0U; i < FLASH_BLOCKBASED_NB_REG; i++)
  {
    *(reg + i) = flash_bb_all_sec;
  }

  /* Configure Bank2 flash on Secure */
  reg = &(FLASH->SECBB2R1);

  for (i = 0U; i < FLASH_BLOCKBASED_NB_REG; i++)
  {
    *(reg + i) = flash_bb_all_sec;
  }
}

/**
  * @brief  Setup the microcontroller system.
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  uint32_t reg_opsr;

  /* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));     /* set CP10 and CP11 Full Access */
#endif

  /* Init the flash to full secure -------------------------------------------*/
  flash_init_cfg();

  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR = RCC_CR_HSION;

  /* Reset CFGR register */
  RCC->CFGR1 = 0U;
  RCC->CFGR2 = 0U;

  /* Reset HSEON, HSECSSON, HSEBYP, HSEEXT, HSIDIV, HSIKERON, CSION, CSIKERON, HSI48 and PLLxON bits */
  RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSECSSON | RCC_CR_HSEBYP | RCC_CR_HSEEXT | RCC_CR_HSIDIV | RCC_CR_HSIKERON | \
               RCC_CR_CSION | RCC_CR_CSIKERON |RCC_CR_HSI48ON | RCC_CR_PLL1ON | RCC_CR_PLL2ON | RCC_CR_PLL3ON);

  /* Reset PLLxCFGR register */
  RCC->PLL1CFGR = 0U;
  RCC->PLL2CFGR = 0U;
  RCC->PLL3CFGR = 0U;

  /* Reset PLL1DIVR register */
  RCC->PLL1DIVR = 0x01010280U;
  /* Reset PLL1FRACR register */
  RCC->PLL1FRACR = 0x00000000U;
  /* Reset PLL2DIVR register */
  RCC->PLL2DIVR = 0x01010280U;
  /* Reset PLL2FRACR register */
  RCC->PLL2FRACR = 0x00000000U;
  /* Reset PLL3DIVR register */
  RCC->PLL3DIVR = 0x01010280U;
  /* Reset PLL3FRACR register */
  RCC->PLL3FRACR = 0x00000000U;

  /* Reset HSEBYP bit */
  RCC->CR &= ~(RCC_CR_HSEBYP);

  /* Disable all interrupts */
  RCC->CIER = 0U;

  /* Configure the Vector Table location ------------------*/
  SCB->VTOR = S_CODE_START; /* Vector Table Relocation in Internal FLASH */

  /* Check OPSR register to verify if there is an ongoing swap or option bytes update interrupted by a reset */
  reg_opsr = FLASH->OPSR & FLASH_OPSR_CODE_OP;
  if ((reg_opsr == FLASH_OPSR_CODE_OP) || (reg_opsr == (FLASH_OPSR_CODE_OP_2 | FLASH_OPSR_CODE_OP_1)))
  {
    /* Check FLASH Option Control Registers access */
    if ((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != 0U)
    {
      /* Authorizes the Option Byte register programming */
      FLASH->OPTKEYR = 0x08192A3BU;
      FLASH->OPTKEYR = 0x4C5D6E7FU;
    }
    /* Launch the option bytes change operation */
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTART;

    /* Lock the FLASH Option Control Register access */
    FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;
  }

  /* Set resources as secure */
  SecureSystemInit();

}


/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Depending on secure or non-secure compilation, the adequate RCC peripheral
  *         memory are is accessed thanks to RCC alias defined in stm32h5xxxx.h device file
  *         so either from RCC_S peripheral register mapped memory in secure or from
  *         RCC_NS peripheral register mapped memory in non-secure.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is CSI, SystemCoreClock will contain the CSI_VALUE(*)
  *
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(**)
  *
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(***)
  *             or HSI_VALUE(**) or CSI_VALUE(*) multiplied/divided by the PLL factors.
  *
  *         (*) CSI_VALUE is a constant defined in stm32h5xx_hal.h file (default value
  *             4 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (**) HSI_VALUE is a constant defined in stm32h5xx_hal.h file (default value
  *              64 MHz) but the real value may vary depending on the variations
  *              in voltage and temperature.
  *
  *         (***) HSE_VALUE is a constant defined in stm32h5xx_hal.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{
  uint32_t pllp, pllsource, pllm, pllfracen, hsivalue, tmp;
  float_t fracn1, pllvco;

  /* Get SYSCLK source -------------------------------------------------------*/
  switch (RCC->CFGR1 & RCC_CFGR1_SWS)
  {
  case 0x00UL:  /* HSI used as system clock source */
    SystemCoreClock = (uint32_t) (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV)>> 3));
    break;

  case 0x08UL:  /* CSI used as system clock  source */
    SystemCoreClock = CSI_VALUE;
    break;

  case 0x10UL:  /* HSE used as system clock  source */
    SystemCoreClock = HSE_VALUE;
    break;

  case 0x18UL:  /* PLL1 used as system clock source */
    /* PLL_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLLM) * PLLN
    SYSCLK = PLL_VCO / PLLR
    */
    pllsource = (RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1SRC);
    pllm = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1M)>> RCC_PLL1CFGR_PLL1M_Pos);
    pllfracen = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1FRACEN)>>RCC_PLL1CFGR_PLL1FRACEN_Pos);
    fracn1 = (float_t)(uint32_t)(pllfracen* ((RCC->PLL1FRACR & RCC_PLL1FRACR_PLL1FRACN)>> RCC_PLL1FRACR_PLL1FRACN_Pos));

      switch (pllsource)
      {
      case 0x01UL:  /* HSI used as PLL clock source */
        hsivalue = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV)>> 3)) ;
        pllvco = ((float_t)hsivalue / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + \
                  (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x02UL:  /* CSI used as PLL clock source */
        pllvco = ((float_t)CSI_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + \
                  (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x03UL:  /* HSE used as PLL clock source */
          pllvco = ((float_t)HSE_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + \
                    (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      default: /* No clock sent to PLL*/
          pllvco = (float_t) 0U;
        break;
      }

      pllp = (((RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1P) >>RCC_PLL1DIVR_PLL1P_Pos) + 1U ) ;
      SystemCoreClock =  (uint32_t)(float_t)(pllvco/(float_t)pllp);

      break;

  default:
    SystemCoreClock = HSI_VALUE;
    break;
  }
  /* Compute HCLK clock frequency --------------------------------------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR2 & RCC_CFGR2_HPRE) >> RCC_CFGR2_HPRE_Pos)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;

}

/**
  * @brief  Configure all securable resources as secure
  * @param  None
  * @retval None
  */
void SecureSystemInit(void)
{

  /* Clock enabling */
  RCC->AHB1ENR  |= RCC_AHB1ENR_ALL;
  RCC->AHB2ENR  |= RCC_AHB2ENR_ALL;
  RCC->AHB4ENR  |= RCC_AHB4ENR_ALL;
  RCC->APB1LENR |= RCC_APB1LENR_ALL;
  RCC->APB1HENR |= RCC_APB1HENR_ALL;
  RCC->APB2ENR  |= RCC_APB2ENR_ALL;
  RCC->APB3ENR  |= RCC_APB3ENR_ALL;
  RCC->SECCFGR  |= RCC_SECCFGR_ALL;

  /* Illegal access interrupts configuration */
  GTZC_TZIC1->IER1 |= TZIC1_IER1_ALL;
  GTZC_TZIC1->IER2 |= TZIC1_IER2_ALL;
  GTZC_TZIC1->IER3 |= TZIC1_IER3_ALL;
  GTZC_TZIC1->IER4 |= TZIC1_IER4_ALL;

  /* Securable peripherals security and privilege management */
  GTZC_TZSC1->SECCFGR1  |= TZSC1_SECCFGR1_ALL;
  GTZC_TZSC1->SECCFGR2  |= TZSC1_SECCFGR2_ALL;
  GTZC_TZSC1->SECCFGR3  |= TZSC1_SECCFGR3_ALL;

 /* TrustZone-aware peripherals security and privilege management */

  /* PWR */
  PWR->SECCFGR  |= PWR_SECCFGR_ALL;

  /* RCC */
  RCC->SECCFGR  |= RCC_SECCFGR_ALL;

  /* GPIO */
  GPIOA->SECCFGR |= GPIOA_SECCFGR_ALL;
  GPIOB->SECCFGR |= GPIOB_SECCFGR_ALL;
  GPIOC->SECCFGR |= GPIOC_SECCFGR_ALL;
  GPIOD->SECCFGR |= GPIOD_SECCFGR_ALL;
  GPIOE->SECCFGR |= GPIOE_SECCFGR_ALL;
  GPIOF->SECCFGR |= GPIOF_SECCFGR_ALL;
  GPIOG->SECCFGR |= GPIOG_SECCFGR_ALL;
  GPIOH->SECCFGR |= GPIOH_SECCFGR_ALL;
  GPIOI->SECCFGR |= GPIOI_SECCFGR_ALL;

  /* DMA */
  GPDMA1->SECCFGR  |= GPDMA1_SECCFGR_ALL;

  /* EXTI */
  EXTI->SECCFGR1  |= EXTI_SECCFGR1_ALL;

  /* RTC and TAMP */
  /* Registers accesses enabling */
  PWR->DBPCR     |= PWR_DBPCR_DBP;

  /* RTC */
  RTC->SECCFGR  |= RTC_SECCFGR_ALL;

  /* TAMP */
  TAMP->SECCFGR  |= TAMP_SECCFGR_ALL;

  /* Registers accesses disabling */
  PWR->DBPCR     &= ~PWR_DBPCR_DBP;

   /* Clock disabling */
  RCC->AHB1ENR  &= ~ (RCC_AHB1ENR_ALL & ~(RCC_AHB1ENR_SRAM1EN | RCC_AHB1ENR_TZSC1EN | RCC_AHB1ENR_FLITFEN));
  RCC->AHB2ENR  &= ~ (RCC_AHB2ENR_ALL & ~(RCC_AHB2ENR_SRAM2EN | RCC_AHB2ENR_SRAM3EN ));
  RCC->AHB4ENR  &= ~ RCC_AHB4ENR_ALL;
  RCC->APB1LENR &= ~ RCC_APB1LENR_ALL;
  RCC->APB1HENR &= ~ RCC_APB1HENR_ALL;
  RCC->APB2ENR  &= ~ RCC_APB2ENR_ALL;
  RCC->APB3ENR  &= ~ RCC_APB3ENR_ALL;
  RCC->SECCFGR  &= ~ RCC_SECCFGR_ALL;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
