/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.6.0 startup_ARMv81MML.c
 * Git SHA: b5f0603d6a584d1724d952fd8b0737458b90d62b
 */

#include "stm32h5xx.h"
/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern void __PROGRAM_START(void) __NO_RETURN;

extern void Error_Handler(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler  (void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
#define DEFAULT_IRQ_HANDLER(handler_name)  \
void handler_name(void); \
__WEAK void handler_name(void) { \
    while(1); \
}

/* Exceptions */
DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SecureFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

DEFAULT_IRQ_HANDLER(WWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(PVD_AVD_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(RTC_S_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(TAMP_IRQHandler)
DEFAULT_IRQ_HANDLER(RAMCFG_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(FLASH_S_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(GTZC_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(RCC_S_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(EXTI0_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI2_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI4_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI5_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI6_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI7_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI8_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI9_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI10_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI11_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI12_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI13_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI14_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI15_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel0_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA1_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(IWDG_IRQHandler)
#ifdef STM32H573xx
DEFAULT_IRQ_HANDLER(SAES_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(ADC1_IRQHandler)
DEFAULT_IRQ_HANDLER(DAC1_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN1_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM1_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM3_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(TIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM5_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(TIM6_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM7_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI3_IRQHandler)
DEFAULT_IRQ_HANDLER(USART1_IRQHandler)
DEFAULT_IRQ_HANDLER(USART2_IRQHandler)
DEFAULT_IRQ_HANDLER(USART3_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(UART4_IRQHandler)
DEFAULT_IRQ_HANDLER(UART5_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(LPUART1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM1_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(TIM8_BRK_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_UP_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_TRG_COM_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM8_CC_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC2_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(LPTIM2_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(TIM15_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM16_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM17_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(OTG_FS_IRQHandler)
DEFAULT_IRQ_HANDLER(CRS_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(UCPD1_IRQHandler)
DEFAULT_IRQ_HANDLER(FMC_IRQHandler)
DEFAULT_IRQ_HANDLER(OCTOSPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SDMMC1_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI4_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI5_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI6_IRQHandler)
DEFAULT_IRQ_HANDLER(USART6_IRQHandler)
DEFAULT_IRQ_HANDLER(USART10_IRQHandler)
DEFAULT_IRQ_HANDLER(USART11_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI2_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(GPDMA2_Channel0_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(GPDMA2_Channel7_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(UART7_IRQHandler)
DEFAULT_IRQ_HANDLER(UART8_IRQHandler)
DEFAULT_IRQ_HANDLER(UART9_IRQHandler)
DEFAULT_IRQ_HANDLER(UART12_IRQHandler)
DEFAULT_IRQ_HANDLER(SDMMC2_IRQHandler)
#else
DEFAULT_IRQ_HANDLER(COMP1_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C2_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C2_ER_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(FPU_IRQHandler)
DEFAULT_IRQ_HANDLER(ICACHE_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(DCACHE_IRQHandler)
DEFAULT_IRQ_HANDLER(ETH_IRQHandler)
DEFAULT_IRQ_HANDLER(ETH_WKUP_IRQHandler)
DEFAULT_IRQ_HANDLER(DCMI_PSSI_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT0_IRQHandler)
DEFAULT_IRQ_HANDLER(FDCAN2_IT1_IRQHandler)
DEFAULT_IRQ_HANDLER(CORDIC_IRQHandler)
DEFAULT_IRQ_HANDLER(FMAC_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(DTS_IRQHandler)
DEFAULT_IRQ_HANDLER(RNG_IRQHandler)
#ifdef STM32H573xx
DEFAULT_IRQ_HANDLER(OTFDEC1_IRQHandler)
DEFAULT_IRQ_HANDLER(AES_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(HASH_IRQHandler)
#ifdef STM32H573xx
DEFAULT_IRQ_HANDLER(PKA_IRQHandler)
#endif
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(CEC_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM12_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM13_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM14_IRQHandler)
#endif
DEFAULT_IRQ_HANDLER(I3C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I3C1_ER_IRQHandler)
#if defined(STM32H573xx) || defined (STM32H563xx)
DEFAULT_IRQ_HANDLER(I2C4_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C4_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM3_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM4_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM5_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM6_IRQHandler)
#endif

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const pFunc __VECTOR_TABLE[];
       const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (pFunc)(&__INITIAL_SP),           /*      Initial Stack Pointer */
  Reset_Handler,                    /*      Reset Handler */
  NMI_Handler,                      /* -14: NMI Handler */
  Error_Handler,                    /* -13: Hard Fault Handler */
  Error_Handler,                    /* -12: MPU Fault Handler */
  Error_Handler,                    /* -11: Bus Fault Handler */
  Error_Handler,                    /* -10: Usage Fault Handler */
  Error_Handler,                    /*  -9: Secure Fault Handler */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  SVC_Handler,                      /*  -5: SVCall Handler */
  DebugMon_Handler,                 /*  -4: Debug Monitor Handler */
  0,                                /*      Reserved */
  PendSV_Handler,                   /*  -2: PendSV Handler */
  SysTick_Handler,                  /*  -1: SysTick Handler */
  WWDG_IRQHandler,                  /*   0: Window WatchDog */
  PVD_AVD_IRQHandler,               /*   1: PVD/AVD through EXTI Line detection Interrupt */
  RTC_IRQHandler,                   /*   2: RTC non-secure interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  RTC_S_IRQHandler,                 /*   3: RTC secure interrupt */
#else
  0,                                /*   3: Reserved */
#endif
  TAMP_IRQHandler,                  /*   4: Tamper non-secure interrupt  */
  Error_Handler,                    /*   5: RAMCFG global */
  FLASH_IRQHandler,                 /*   6: FLASH non-secure global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  FLASH_S_IRQHandler,               /*   7: FLASH secure global interrupt */
#else
  0,                                /*   7: Reserved */
#endif
  GTZC_IRQHandler,                  /*   8: Global TrustZone Controller interrupt */
  RCC_IRQHandler,                   /*   9: RCC non-secure global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  RCC_S_IRQHandler,                 /*  10: RCC secure global interrupt */
#else
  0,                                /*  10: Reserved */
#endif
  EXTI0_IRQHandler,                 /*  11: EXTI Line0 interrupt */
  EXTI1_IRQHandler,                 /*  12: EXTI Line1 interrupt */
  EXTI2_IRQHandler,                 /*  13: EXTI Line2 interrupt */
  EXTI3_IRQHandler,                 /*  14: EXTI Line3 interrupt */
  EXTI4_IRQHandler,                 /*  15: EXTI Line4 interrupt */
  EXTI5_IRQHandler,                 /*  16: EXTI Line5 interrupt */
  EXTI6_IRQHandler,                 /*  17: EXTI Line6 interrupt */
  EXTI7_IRQHandler,                 /*  18: EXTI Line7 interrupt */
  EXTI8_IRQHandler,                 /*  19: EXTI Line8 interrupt */
  EXTI9_IRQHandler,                 /*  20: EXTI Line9 interrupt */
  EXTI10_IRQHandler,                /*  21: EXTI Line10 interrupt */
  EXTI11_IRQHandler,                /*  22: EXTI Line11 interrupt */
  EXTI12_IRQHandler,                /*  23: EXTI Line12 interrupt */
  EXTI13_IRQHandler,                /*  24: EXTI Line13 interrupt */
  EXTI14_IRQHandler,                /*  25: EXTI Line14 interrupt */
  EXTI15_IRQHandler,                /*  26: EXTI Line15 interrupt */
  GPDMA1_Channel0_IRQHandler,       /*  27: GPDMA1 Channel 0 global interrupt */
  GPDMA1_Channel1_IRQHandler,       /*  28: GPDMA1 Channel 1 global interrupt */
  GPDMA1_Channel2_IRQHandler,       /*  29: GPDMA1 Channel 2 global interrupt */
  GPDMA1_Channel3_IRQHandler,       /*  30: GPDMA1 Channel 3 global interrupt */
  GPDMA1_Channel4_IRQHandler,       /*  31: GPDMA1 Channel 4 global interrupt */
  GPDMA1_Channel5_IRQHandler,       /*  32: GPDMA1 Channel 5 global interrupt */
  GPDMA1_Channel6_IRQHandler,       /*  33: GPDMA1 Channel 6 global interrupt */
  GPDMA1_Channel7_IRQHandler,       /*  34: GPDMA1 Channel 7 global interrupt */
  IWDG_IRQHandler,                  /*  35: IWDG global interrupt */
#ifdef STM32H573xx
  SAES_IRQHandler,                  /*  36: SAES global interrupt */
#else
  0,                                /*  36: Reserved */
#endif
  ADC1_IRQHandler,                  /*  37: ADC1 global interrupt */
  DAC1_IRQHandler,                  /*  38: DAC1 global interrupt */
  FDCAN1_IT0_IRQHandler,            /*  39: FDCAN1 Interrupt 0 */
  FDCAN1_IT1_IRQHandler,            /*  40: FDCAN1 Interrupt 1 */
  TIM1_BRK_IRQHandler,              /*  41: TIM1 Break interrupt */
  TIM1_UP_IRQHandler,               /*  42: TIM1 Update interrupt */
  TIM1_TRG_COM_IRQHandler,          /*  43: TIM1 Trigger and Commutation interrupt */
  TIM1_CC_IRQHandler,               /*  44: TIM1 Capture Compare interrupt */
  TIM2_IRQHandler,                  /*  45: TIM2 global interrupt */
  TIM3_IRQHandler,                  /*  46: TIM3 global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  TIM4_IRQHandler,                  /*  47: TIM4 global interrupt */
  TIM5_IRQHandler,                  /*  48: TIM5 global interrupt */
#else
  0,                                /*  47: Reserved */
  0,                                /*  48: Reserved */
#endif
  TIM6_IRQHandler,                  /*  49: TIM6 global interrupt */
  TIM7_IRQHandler,                  /*  50: TIM7 global interrupt */
  I2C1_EV_IRQHandler,               /*  51: I2C1 event interrupt */
  I2C1_ER_IRQHandler,               /*  52: I2C1 error interrupt */
  I2C2_EV_IRQHandler,               /*  53: I2C2 event interrupt */
  I2C2_ER_IRQHandler,               /*  54: I2C2 error interrupt */
  SPI1_IRQHandler,                  /*  55: SPI1 global interrupt */
  SPI2_IRQHandler,                  /*  56: SPI2 global interrupt */
  SPI3_IRQHandler,                  /*  57: SPI3 global interrupt */
  USART1_IRQHandler,                /*  58: USART1 global interrupt */
  USART2_IRQHandler,                /*  59: USART2 global interrupt */
  USART3_IRQHandler,                /*  60: USART3 global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  UART4_IRQHandler,                 /*  61: UART4 global interrupt */
  UART5_IRQHandler,                 /*  62: UART5 global interrupt */
#else
  0,                                /*  61: Reserved */
  0,                                /*  62: Reserved */
#endif
  LPUART1_IRQHandler,               /*  63: LPUART1 global interrupt */
  LPTIM1_IRQHandler,                /*  64: LPTIM1 global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  TIM8_BRK_IRQHandler,              /*  65: TIM8 Break interrupt */
  TIM8_UP_IRQHandler,               /*  66: TIM8 Update interrupt */
  TIM8_TRG_COM_IRQHandler,          /*  67: TIM8 Trigger and Commutation interrupt */
  TIM8_CC_IRQHandler,               /*  68: TIM8 Capture Compare interrupt */
  ADC2_IRQHandler,                  /*  69: ADC2 global interrupt */
#else
  0,                                /*  65: Reserved */
  0,                                /*  66: Reserved */
  0,                                /*  67: Reserved */
  0,                                /*  68: Reserved */
  0,                                /*  69: Reserved */
#endif
  LPTIM2_IRQHandler,                /*  70: LPTIM2 global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  TIM15_IRQHandler,                 /*  71: TIM15 global interrupt */
  TIM16_IRQHandler,                 /*  72: TIM16 global interrupt */
  TIM17_IRQHandler,                 /*  73: TIM17 global interrupt */
#else
  0,                                /*  71: Reserved */
  0,                                /*  72: Reserved */
  0,                                /*  73: Reserved */
#endif
  OTG_FS_IRQHandler,                /*  74: USB OTG FS global interrupt */
  CRS_IRQHandler,                   /*  75: CRS global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  UCPD1_IRQHandler,                 /*  76: UCPD1 global interrupt */
  FMC_IRQHandler,                   /*  77: FMC global interrupt */
  OCTOSPI1_IRQHandler,              /*  78: OctoSPI1 global interrupt */
  SDMMC1_IRQHandler,                /*  79: SDMMC1 global interrupt */
  I2C3_EV_IRQHandler,               /*  80: I2C3 event interrupt */
  I2C3_ER_IRQHandler,               /*  81: I2C3 error interrupt */
  SPI4_IRQHandler,                  /*  82: SPI4 global interrupt */
  SPI5_IRQHandler,                  /*  83: SPI5 global interrupt */
  SPI6_IRQHandler,                  /*  84: SPI6 global interrupt */
  USART6_IRQHandler,                /*  85: USART6 global interrupt */
  USART10_IRQHandler,               /*  86: USART10 global interrupt */
  USART11_IRQHandler,               /*  87: USART11 global interrupt */
  SAI1_IRQHandler,                  /*  88: Serial Audio Interface 1 global interrupt */
  SAI2_IRQHandler,                  /*  89: Serial Audio Interface 2 global interrupt */
#else
  0,                                /*  76: Reserved */
  0,                                /*  77: Reserved */
  0,                                /*  78: Reserved */
  0,                                /*  79: Reserved */
  0,                                /*  80: Reserved */
  0,                                /*  81: Reserved */
  0,                                /*  82: Reserved */
  0,                                /*  83: Reserved */
  0,                                /*  84: Reserved */
  0,                                /*  85: Reserved */
  0,                                /*  86: Reserved */
  0,                                /*  87: Reserved */
  0,                                /*  88: Reserved */
  0,                                /*  89: Reserved */
#endif
  GPDMA2_Channel0_IRQHandler,       /*  90: GPDMA2 Channel 0 global interrupt */
  GPDMA2_Channel1_IRQHandler,       /*  91: GPDMA2 Channel 1 global interrupt */
  GPDMA2_Channel2_IRQHandler,       /*  92: GPDMA2 Channel 2 global interrupt */
  GPDMA2_Channel3_IRQHandler,       /*  93: GPDMA2 Channel 3 global interrupt */
  GPDMA2_Channel4_IRQHandler,       /*  94: GPDMA2 Channel 4 global interrupt */
  GPDMA2_Channel5_IRQHandler,       /*  95: GPDMA2 Channel 5 global interrupt */
  GPDMA2_Channel6_IRQHandler,       /*  96: GPDMA2 Channel 6 global interrupt */
  GPDMA2_Channel7_IRQHandler,       /*  97: GPDMA2 Channel 7 global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  UART7_IRQHandler,                 /*  98: UART7 global interrupt */
  UART8_IRQHandler,                 /*  99: UART8 global interrupt */
  UART9_IRQHandler,                 /* 100: UART9 global interrupt */
  UART12_IRQHandler,                /* 101: UART12 global interrupt */
  SDMMC2_IRQHandler,                /* 102: SDMMC2 global interrupt */
#else
  COMP1_IRQHandler,                 /*  98: COMP1 global interrupt */
  0,                                /*  99: Reserved */
  I3C2_EV_IRQHandler,               /* 100: I2C3 event interrupt */
  I3C2_ER_IRQHandler,               /* 101: I2C3 error interrupt */
  0,                                /* 102: Reserved */
#endif
  FPU_IRQHandler,                   /* 103: FPU global interrupt */
  ICACHE_IRQHandler,                /* 104: Instruction cache global interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  DCACHE_IRQHandler,                /* 105: Data cache global interrupt */
  ETH_IRQHandler,                   /* 106: Ethernet global interrupt */
  ETH_WKUP_IRQHandler,              /* 107: Ethernet Wakeup global interrupt */
  DCMI_PSSI_IRQHandler,             /* 108: DCMI PSSI global interrupt */
  FDCAN2_IT0_IRQHandler,            /* 109: FDCAN2 Interrupt 0 */
  FDCAN2_IT1_IRQHandler,            /* 110: FDCAN2 Interrupt 1 */
  CORDIC_IRQHandler,                /* 111: CORDIC global interrupt */
  FMAC_IRQHandler,                  /* 112: FMAC global interrupt */
#else
  0,                                /* 105: Reserved */
  0,                                /* 106: Reserved */
  0,                                /* 107: Reserved */
  0,                                /* 108: Reserved */
  0,                                /* 109: Reserved */
  0,                                /* 110: Reserved */
  0,                                /* 111: Reserved */
  0,                                /* 112: Reserved */
#endif
  DTS_IRQHandler,                   /* 113: DTS global interrupt */
  RNG_IRQHandler,                   /* 114: RNG global interrupt */
#ifdef STM32H573xx
  OTFDEC1_IRQHandler,               /* 115: OTFDEC1 global interrupt */
  AES_IRQHandler,                   /* 116: AES global interrupt */
#else
  0,                                /* 115: Reserved */
  0,                                /* 116: Reserved */
#endif
  HASH_IRQHandler,                  /* 117: HASH global interrupt */
#ifdef STM32H573xx
  PKA_IRQHandler,                   /* 118: PKA global interrupt */
#else
  0,                                /* 118: Reserved */
#endif
#if defined(STM32H573xx) || defined (STM32H563xx)
  CEC_IRQHandler,                   /* 119: CEC global interrupt */
  TIM12_IRQHandler,                 /* 120: TIM12 global interrupt */
  TIM13_IRQHandler,                 /* 121: TIM13 global interrupt */
  TIM14_IRQHandler,                 /* 122: TIM14 global interrupt */
#else
  0,                                /* 119: Reserved */
  0,                                /* 120: Reserved */
  0,                                /* 121: Reserved */
  0,                                /* 122: Reserved */
#endif
  I3C1_EV_IRQHandler,               /* 123: I3C1 event interrupt */
  I3C1_ER_IRQHandler,               /* 124: I3C1 error interrupt */
#if defined(STM32H573xx) || defined (STM32H563xx)
  I2C4_EV_IRQHandler,               /* 125: I2C4 event interrupt */
  I2C4_ER_IRQHandler,               /* 126: I2C4 error interrupt */
  LPTIM3_IRQHandler,                /* 127: LPTIM3 global interrupt */
  LPTIM4_IRQHandler,                /* 128: LPTIM4 global interrupt */
  LPTIM5_IRQHandler,                /* 129: LPTIM5 global interrupt */
  LPTIM6_IRQHandler,                /* 130: LPTIM6 global interrupt */
#else
  0,                                /* 125: Reserved */
  0,                                /* 126: Reserved */
  0,                                /* 127: Reserved */
  0,                                /* 128: Reserved */
  0,                                /* 129: Reserved */
  0,                                /* 130: Reserved */
#endif
};
#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

#ifdef OEMIROT_DEV_MODE
#ifdef __ICCARM__
__no_init volatile uint32_t TamperEventCleared;
#else
  volatile uint32_t TamperEventCleared  __attribute__((section(".bss.NoInit")));
#endif /* __ICCARM__ */
#endif /* OEMIROT_DEV_MODE */
/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
  /* CMSIS System Initialization */
  SystemInit();
  /* enable access to tamper register */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_RTC_ENABLE();
  __HAL_RCC_RTC_CLK_ENABLE();
   /* Get tamper status */
  if (READ_REG(TAMP->SR))
  {
#ifdef OEMIROT_DEV_MODE
#if 1
    /* avoid several re-boot in DEV_MODE with Tamper active, clean tamper configuration */
    __HAL_RCC_BACKUPRESET_FORCE();
    __HAL_RCC_BACKUPRESET_RELEASE();
    /* wait for event being cleared */
    while(READ_REG(TAMP->SR));
#else
    /* clear tamper event */
    WRITE_REG(TAMP->SCR, READ_REG(TAMP->SR));
#endif
    /* memorize for log that event has been cleared */
    TamperEventCleared = 1;
#else
    /* VBAT and VDD must be set to zero to allow board to restart */
    Error_Handler();
#endif /* OEMIROT_DEV_MODE */
  }
  /*  Enable TAMP IRQ , to catch tamper interrupt in TAMP_IRQHandler
   *  else a stack in SRAM2 is cleaned a HardFault can occur, at every pop of
   *  function */
#ifndef OEMIROT_DEV_MODE
  HAL_NVIC_EnableIRQ(TAMP_IRQn);
#endif
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
