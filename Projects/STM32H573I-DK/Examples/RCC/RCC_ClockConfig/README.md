## <b>RCC_ClockConfig Example Description</b>

Configuration of the system clock (SYSCLK) and modification of the clock settings in Run mode, using the RCC HAL API.

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
HSI as clock source, the USER push-button (connected to External line 13) will be
used to change the PLL source:

- from CSI to HSE
- from HSE to HSI
- from HSI to CSI

Each time the USER push-button is pressed External line 13 interrupt is generated and in the ISR
the PLL oscillator source is checked using __HAL_RCC_GET_PLL_OSCSOURCE() macro:

- If the HSE oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to HSI oscillator:

     a. Switch the system clock source to HSE to allow modification of the PLL configuration
     b. Enable HSI Oscillator, select it as PLL source and finally activate the PLL
     c. Select the PLL as system clock source and configure bus clocks dividers
     d. Disable the HSE oscillator 

- If the CSI oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to HSE oscillator:

     a. Switch the system clock source to CSI to allow modification of the PLL configuration
     b. Enable HSE Oscillator, select it as PLL source and finally activate the PLL
     c. Select the PLL as system clock source and configure bus clocks dividers
     d. Disable the CSI oscillator

- If the HSI oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to CSI oscillator:
     a- Switch the system clock source to HSI to allow modification of the PLL configuration
     b- Enable CSI Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure bus clocks dividers
     d- Disable the HSI oscillator

LED1 is toggled with a timing defined by the HAL_Delay() API.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

RCC, System, Clock Configuration, HSE bypass mode, HSI, CSI, System clock, Oscillator, PLL

### <b>Directory contents</b>

  - RCC/RCC_ClockConfig/Inc/stm32h573i_discovery_conf.h   BSP configuration file
  - RCC/RCC_ClockConfig/Inc/stm32h5xx_hal_conf.h          HAL configuration file
  - RCC/RCC_ClockConfig/Inc/stm32h5xx_it.h                Interrupt handlers header file
  - RCC/RCC_ClockConfig/Inc/main.h                        Header for main.c module
  - RCC/RCC_ClockConfig/Src/stm32h5xx_it.c                Interrupt handlers
  - RCC/RCC_ClockConfig/Src/main.c                        Main program
  - RCC/RCC_ClockConfig/Src/system_stm32h5xx.c            STM32H5xx system source file
  - RCC/RCC_ClockConfig/Src/stm32h5xx_hal_msp.c           HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573xx devices.

  - This example has been tested with STM32H573I-DK
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

