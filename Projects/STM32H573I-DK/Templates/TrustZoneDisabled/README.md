## <b>Templates_TrustZoneDisabled Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when security is not enabled (TZEN=C3).  
- This project is targeted to run on STM32H573xx devices on STM32H573I-DK boards from STMicroelectronics.  
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 Mhz.
- The template project calls also CACHE_Enable() function in order to enable the Instruction
and Data Caches. This function is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.  
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, Template, TrustZone disabled

### <b>Directory contents</b>

       - Templates/TrustZoneDisabled/Src/main.c                      Main program
       - Templates/TrustZoneDisabled/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file
       - Templates/TrustZoneDisabled/Src/stm32h5xx_it.c              Interrupt handlers
       - Templates/TrustZoneDisabled/Src/stm32h5xx_hal_msp.c         HAL MSP module
       - Templates/TrustZoneDisabled/Inc/main.h                      Main program header file
       - Templates/TrustZoneDisabled/Inc/stm32h573i_discovery_conf.h BSP Configuration file
       - Templates/TrustZoneDisabled/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
       - Templates/TrustZoneDisabled/Inc/stm32h5xx_it.h              Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H573xx devices without security enabled (TZEN=C3).  
  - This template has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

