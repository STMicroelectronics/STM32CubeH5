/**
  ******************************************************************************
  * @file    system_stm32h5xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  *
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

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Defines
  * @{
  */

#if !defined  (HSE_VALUE)
  #define HSE_VALUE    (25000000U) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    (4000000U)  /*!< Value of the Internal oscillator in Hz*/
#endif /* CSI_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    (64000000U) /*!< Value of the Internal oscillator in Hz */
#endif /* HSI_VALUE */

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */
/******************************************************************************/

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
static void SetSysClock(void);

/**
  * @}
  */

/** @addtogroup STM32H5xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system.
  * @param  None
  * @retval None
  */

void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
   SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));  /* set CP10 and CP11 Full Access */
  #endif

  /* Configure the System clock source, PLL Multiplier and Divider factors,
     AHB/APBx prescalers and Flash settings for System clock 250 MHz */
  SetSysClock();
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
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
  case 0x00:  /* HSI used as system clock source */
    SystemCoreClock = (uint32_t) (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV)>> 3));
    break;

  case 0x08:  /* CSI used as system clock  source */
    SystemCoreClock = CSI_VALUE;
    break;

  case 0x10:  /* HSE used as system clock  source */
    SystemCoreClock = HSE_VALUE;
    break;

  case 0x18:  /* PLL1 used as system clock source */
    /* PLL_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLLM) * PLLN
    SYSCLK = PLL_VCO / PLLR
    */
    pllsource = (RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1SRC);
    pllm = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1M)>> RCC_PLL1CFGR_PLL1M_Pos);
    pllfracen = ((RCC->PLL1CFGR & RCC_PLL1CFGR_PLL1FRACEN)>>RCC_PLL1CFGR_PLL1FRACEN_Pos);
    fracn1 = (float_t)(uint32_t)(pllfracen* ((RCC->PLL1FRACR & RCC_PLL1FRACR_PLL1FRACN)>> RCC_PLL1FRACR_PLL1FRACN_Pos));

      switch (pllsource)
      {
      case 0x00:  /* No clock sent to PLL*/
        pllvco = 0U;
        break;

      case 0x01:  /* HSI used as PLL clock source */
        hsivalue = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV)>> 3)) ;
        pllvco = ( (float_t)hsivalue / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x02:  /* CSI used as PLL clock source */
        pllvco = ((float_t)CSI_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      case 0x03:  /* HSE used as PLL clock source */
          pllvco = ((float_t)HSE_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_PLL1N) + (fracn1/(float_t)0x2000) +(float_t)1 );
        break;

      default:    /* No clock sent to PLL*/
          pllvco = 0U;

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
  * @brief  Configures the System clock source, PLL Multiplier and Divider factors,
  *         AHB/APBx prescalers and Flash settings for System clock 250 MHz
  * @retval None
  */
static void SetSysClock(void)
{
  /* Set the regulator supply output voltage */
  MODIFY_REG(PWR->SCCR, (PWR_SCCR_BYPASS), PWR_SCCR_LDOEN);
  MODIFY_REG(PWR->VOSCR, PWR_VOSCR_VOS, PWR_VOSCR_VOS);
  while ((PWR->VOSSR & PWR_VOSSR_VOSRDY) != PWR_VOSSR_VOSRDY)
  {
  }

  /* HSI Calibration with default value */
  MODIFY_REG(RCC->HSICFGR, RCC_HSICFGR_HSITRIM, 0x40 << RCC_HSICFGR_HSITRIM_Pos);
  MODIFY_REG(RCC->CR, RCC_CR_HSIDIV, RCC_HSI_DIV1);
  /* Enable HSI oscillator */
  SET_BIT(RCC->CR, RCC_CR_HSION);
  while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY)
  {
  }

  /* Set FLASH latency */
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);

  /* Configure PLL clock source */
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1SRC, RCC_PLL1CFGR_PLL1SRC_0);

  /* Main PLL configuration and activation */
  SET_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1PEN);
  SET_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1QEN);
  SET_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1REN);
  CLEAR_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1FRACEN);
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1RGE, RCC_PLL1CFGR_PLL1SRC_1 << RCC_PLL1CFGR_PLL1RGE_Pos);
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1VCOSEL, RCC_PLL1VCOWIDE << RCC_PLL1CFGR_PLL1VCOSEL_Pos);
  MODIFY_REG(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1M, RCC_PLL1CFGR_PLL1M_Pos  << RCC_PLL1CFGR_PLL1M_Pos);
  MODIFY_REG(RCC->PLL1DIVR, RCC_PLL1DIVR_PLL1N, (60U - 1UL) << RCC_PLL1DIVR_PLL1N_Pos);
  MODIFY_REG(RCC->PLL1DIVR, RCC_PLL1DIVR_PLL1P, (2U - 1UL) << RCC_PLL1DIVR_PLL1P_Pos);
  MODIFY_REG(RCC->PLL1DIVR, RCC_PLL1DIVR_PLL1Q, (2U - 1UL) << RCC_PLL1DIVR_PLL1Q_Pos);
  MODIFY_REG(RCC->PLL1DIVR, RCC_PLL1DIVR_PLL1R, (2U - 1UL) << RCC_PLL1DIVR_PLL1R_Pos);

  SET_BIT(RCC->CR, RCC_CR_PLL1ON);
  while((RCC->CR & RCC_CR_PLL1RDY) != RCC_CR_PLL1RDY)
  {
  }

  /* Set AHB, APB1, APB2 and APB3 prescalers */
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_HPRE, RCC_SYSCLK_DIV1);
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE1, RCC_HCLK_DIV1);
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE2, RCC_HCLK_DIV1);
  MODIFY_REG(RCC->CFGR2, RCC_CFGR2_PPRE3, RCC_HCLK_DIV1);

  /* Set PLL1 as System Clock Source */
  MODIFY_REG(RCC->CFGR1, RCC_CFGR1_SW, (RCC_CFGR1_SW_1 | RCC_CFGR1_SW_0));

  while((RCC->CFGR1 & RCC_CFGR1_SWS) != (RCC_CFGR1_SWS_1 | RCC_CFGR1_SWS_0))
  {
  }

  /* Enable the HSI48 oscillator (HSI48) for RNG peripheral */
  RCC->CR |= RCC_CR_HSI48ON;
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

