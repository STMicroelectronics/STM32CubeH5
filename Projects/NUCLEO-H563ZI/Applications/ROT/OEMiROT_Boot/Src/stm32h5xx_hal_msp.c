/**
  ******************************************************************************
  * @file    stm32h5xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
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
#include "boot_hal_cfg.h"

/** @addtogroup STM32H5xx_HAL_Examples
  * @{
  */

/** @defgroup RTC_ActiveTamper
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RCC_LSI_TIMEOUT_VALUE           ((uint32_t)2U)    /* 2 ms (minimum Tick + 1) */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constant ----------------------------------------------------------*/
/** @defgroup HAL_MSP_Private_Constant
  * @{
  */
const  RCC_OscInitTypeDef RCC_OscInitStruct_RTC =
{
    .OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE,
    .HSEState = 0,
#if defined(RTC_CLOCK_SOURCE_LSE)
    .LSEState = RCC_LSE_ON,
    .LSIState = RCC_LSI_OFF,
#elif defined (RTC_CLOCK_SOURCE_LSI)
    .LSEState = RCC_LSE_OFF,
    .LSIState = RCC_LSI_ON,
#else
#error
#endif /* defined (RTC_CLOCK_SOURCE_LSI) */
    .HSIState = 0,
    .HSIDiv = 0,
    .HSICalibrationValue = 0,
    .CSIState = 0,
    .CSICalibrationValue = 0,
    .HSI48State = 0,
    .PLL = {
        .PLLState = RCC_PLL_NONE,
        .PLLSource = 0,
        .PLLM = 0,
        .PLLN = 0,
        .PLLP = 0,
        .PLLQ = 0,
        .PLLR = 0,
        .PLLRGE = 0,
        .PLLVCOSEL = 0,
        .PLLFRACN = 0
    }
};

const RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct_RTC =
{
    .PeriphClockSelection = RCC_PERIPHCLK_RTC,
    .PLL2 = {0},
#if defined(RCC_CR_PLL3ON)
    .PLL3 = {0},
#endif /* RCC_CR_PLL3ON */
    .CkperClockSelection = 0,
    .Usart1ClockSelection = 0,
    .Usart2ClockSelection = 0,
    .Usart3ClockSelection = 0,
#if defined(UART4)
    .Uart4ClockSelection = 0,
#endif /* UART4 */
#if defined(UART5)
    .Uart5ClockSelection = 0,
#endif /* UART5 */
#if defined(USART6)
    .Usart6ClockSelection = 0,
#endif /* UART6 */
#if defined(UART7)
    .Uart7ClockSelection = 0,
#endif /* UART7 */
#if defined(UART8)
    .Uart8ClockSelection = 0,
#endif /* UART8 */
#if defined(UART9)
    .Uart9ClockSelection = 0,
#endif /* UART9 */
#if defined(USART10)
    .Usart10ClockSelection = 0,
#endif /* UART10 */
#if defined(USART11)
    .Usart11ClockSelection = 0,
#endif /* UART11 */
#if defined(USART12)
    .Uart12ClockSelection = 0,
#endif /* UART12 */
    .Lpuart1ClockSelection = 0,
    .I2c1ClockSelection = 0,
    .I2c2ClockSelection = 0,
#if defined(I2C3)
    .I2c3ClockSelection = 0,
#endif /* I2C3 */
#if defined(I2C4)
    .I2c4ClockSelection = 0,
#endif /* I2C4 */
    .I3c1ClockSelection = 0,
#if defined(I3C2)
    .I3c2ClockSelection = 0,
#endif /* I3C2 */
    .Lptim1ClockSelection = 0,
    .Lptim2ClockSelection = 0,
#if defined(LPTIM3)
    .Lptim3ClockSelection = 0,
#endif /* LPTIM3 */
#if defined(LPTIM4)
    .Lptim4ClockSelection = 0,
#endif /* LPTIM4 */
#if defined(LPTIM5)
    .Lptim5ClockSelection = 0,
#endif /* LPTIM5 */
#if defined(LPTIM6)
    .Lptim6ClockSelection = 0,
#endif /* LPTIM6 */
    .FdcanClockSelection = 0,
#if defined(SAI1)
    .Sai1ClockSelection = 0,
#endif /* SAI1 */
#if defined(SAI2)
    .Sai2ClockSelection = 0,
#endif /* SAI2 */
    .RngClockSelection = 0,
#if defined(SDMMC1)
    .Sdmmc1ClockSelection = 0,
#endif /* SDMMC1 */
#if defined(SDMMC2)
    .Sdmmc2ClockSelection = 0,
#endif /* SDMMC2 */
    .AdcDacClockSelection = 0,
    .DacLowPowerClockSelection = 0,
#if defined(OCTOSPI1)
    .OspiClockSelection = 0,
#endif
    .Spi1ClockSelection = 0,
    .Spi2ClockSelection = 0,
    .Spi3ClockSelection = 0,
#if defined(SPI4)
    .Spi4ClockSelection = 0,
#endif /* SPI4 */
#if defined(SPI5)
    .Spi5ClockSelection = 0,
#endif /* SPI5 */
#if defined(SPI6)
    .Spi6ClockSelection = 0,
#endif /* SPI6 */
#if defined(RTC_CLOCK_SOURCE_LSE)
    .RTCClockSelection = RCC_RTCCLKSOURCE_LSE,
#elif defined (RTC_CLOCK_SOURCE_LSI)
    .RTCClockSelection = RCC_RTCCLKSOURCE_LSI,
#else
#error
#endif /* RTC_CLOCK_SOURCE_LSE */
#if defined(CEC)
    .CecClockSelection = 0,
#endif /* CEC */
    .UsbClockSelection = 0,
    .TimPresSelection = 0
};
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef RCC_OscConfig(const RCC_OscInitTypeDef  *pOscInitStruct);
HAL_StatusTypeDef RCCEx_PeriphCLKConfig(const RCC_PeriphCLKInitTypeDef  *pPeriphClkInit);
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the RCC Oscillators according to the specified parameters in the
  *         RCC_OscInitTypeDef.
  * @param  RCC_OscInitStruct  pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC Oscillators.
  * @retval HAL status
  */
HAL_StatusTypeDef RCC_OscConfig(const RCC_OscInitTypeDef  *pOscInitStruct)
{
  uint32_t tickstart = 0;

  /* Check Null pointer */
  if (pOscInitStruct == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_RCC_OSCILLATORTYPE(pOscInitStruct->OscillatorType));

#if defined (RTC_CLOCK_SOURCE_LSI)
  /*------------------------------ LSI Configuration -------------------------*/
  if (((pOscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {

    /* Check the parameters */
    assert_param(IS_RCC_LSI(pOscInitStruct->LSIState));

    /* Update LSI configuration in Backup Domain control register    */

    /* Check the LSI State */
    if (pOscInitStruct->LSIState != RCC_LSI_OFF)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_ENABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is ready */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSIRDY) == 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_DISABLE();

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSI is disabled */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSIRDY) != 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* RTC_CLOCK_SOURCE_LSI */

#if defined(RTC_CLOCK_SOURCE_LSE)
  /*------------------------------ LSE Configuration -------------------------*/
  if (((pOscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSE(pOscInitStruct->LSEState));

    /* Update LSE configuration in Backup Domain control register    */
    /* Requires to enable write access to Backup Domain */
    if (HAL_IS_BIT_CLR(PWR->DBPCR, PWR_DBPCR_DBP))
    {
      /* Enable write access to Backup domain */
      SET_BIT(PWR->DBPCR, PWR_DBPCR_DBP);

      /* Wait for Backup domain Write protection disable */
      tickstart = HAL_GetTick();

      while (HAL_IS_BIT_CLR(PWR->DBPCR, PWR_DBPCR_DBP))
      {
        if ((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }

    /* Set the new LSE configuration -----------------------------------------*/
    __HAL_RCC_LSE_CONFIG(pOscInitStruct->LSEState);

    /* Check the LSE State */
    if (pOscInitStruct->LSEState != RCC_LSE_OFF)
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is ready */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      /* Wait till LSE is disabled */
      while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) != 0U)
      {
        if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
#endif /* RTC_CLOCK_SOURCE_LSE */

  return HAL_OK;
}

/**
  * @brief  Initialize the RCC extended peripherals clocks according to the specified
  *         parameters in the RCC_PeriphCLKInitTypeDef.
  * @param  PeriphClkInit  pointer to an RCC_PeriphCLKInitTypeDef structure that
  *         contains a field PeriphClockSelection which can be one of the following values:
  *            @arg @ref RCC_PERIPHCLK_RTC RTC peripheral clock
  *
  * @note   Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *         the RTC clock source: in this case the access to Backup domain is enabled.
  *
  * @retval HAL status
  */
HAL_StatusTypeDef RCCEx_PeriphCLKConfig(const RCC_PeriphCLKInitTypeDef  *pPeriphClkInit)
{
  uint32_t tmpregister;
  uint32_t tickstart;
  HAL_StatusTypeDef ret = HAL_OK;      /* Intermediate status */
  HAL_StatusTypeDef status = HAL_OK;   /* Final status */

  /* Check the parameters */
  assert_param(IS_RCC_PERIPHCLOCK(pPeriphClkInit->PeriphClockSelection));

  /*-------------------------- RTC clock source configuration ----------------------*/
  if (((pPeriphClkInit->PeriphClockSelection) & RCC_PERIPHCLK_RTC) == RCC_PERIPHCLK_RTC)
  {

    /* Check for RTC Parameters used to output RTCCLK */
    assert_param(IS_RCC_RTCCLKSOURCE(pPeriphClkInit->RTCClockSelection));

    /* Enable write access to Backup domain */
    SET_BIT(PWR->DBPCR, PWR_DBPCR_DBP);

    /* Wait for Backup domain Write protection disable */
    tickstart = HAL_GetTick();

    while (HAL_IS_BIT_CLR(PWR->DBPCR, PWR_DBPCR_DBP))
    {
      if ((HAL_GetTick() - tickstart) > RCC_DBP_TIMEOUT_VALUE)
      {
        ret = HAL_TIMEOUT;
        break;
      }
    }

    if (ret == HAL_OK)
    {
      /* Reset the Backup domain only if the RTC Clock source selection is modified from default */
      tmpregister = READ_BIT(RCC->BDCR, RCC_BDCR_RTCSEL);

      if ((tmpregister != RCC_RTCCLKSOURCE_NO_CLK) && (tmpregister != pPeriphClkInit->RTCClockSelection))
      {
        /* Store the content of BDCR register before the reset of Backup Domain */
        tmpregister = READ_BIT(RCC->BDCR, ~(RCC_BDCR_RTCSEL));
        /* RTC Clock selection can be changed only if the Backup Domain is reset */
        __HAL_RCC_BACKUPRESET_FORCE();
        __HAL_RCC_BACKUPRESET_RELEASE();
        /* Restore the Content of BDCR register */
        RCC->BDCR = tmpregister;
      }

      /* Wait for LSE reactivation if LSE was enable prior to Backup Domain reset */
      if (HAL_IS_BIT_SET(tmpregister, RCC_BDCR_LSEON))
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till LSE is ready */
        while (READ_BIT(RCC->BDCR, RCC_BDCR_LSERDY) == 0U)
        {
          if ((HAL_GetTick() - tickstart) > RCC_LSE_TIMEOUT_VALUE)
          {
            ret = HAL_TIMEOUT;
            break;
          }
        }
      }

      if (ret == HAL_OK)
      {
        /* Apply new RTC clock source selection */
        __HAL_RCC_RTC_CONFIG(pPeriphClkInit->RTCClockSelection);
      }
      else
      {
        /* set overall return value */
        status = ret;
      }
    }
    else
    {
      /* set overall return value */
      status = ret;
    }

  }
  return status;
}

/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    HAL_PWR_EnableBkUpAccess();
    /*  not required to be removed */
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);
    /* Reset the whole backup domain, RTC included */
    if (RCC_OscConfig((RCC_OscInitTypeDef *)&RCC_OscInitStruct_RTC) != HAL_OK)
    {
        Error_Handler();
    }
    if (RCCEx_PeriphCLKConfig((RCC_PeriphCLKInitTypeDef *)&PeriphClkInitStruct_RTC) != HAL_OK)
    {
        Error_Handler();
    }
    /* Enable RTC */
    __HAL_RCC_RTC_ENABLE();

    /* Enable RTC clock  */
    __HAL_RCC_RTC_CLK_ENABLE();

    HAL_NVIC_SetPriority(TAMP_IRQn, 0x4, 0);
    HAL_NVIC_EnableIRQ(TAMP_IRQn);
}

/**
  * @brief RTC MSP De-Initialization
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_RTC_DISABLE();
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
