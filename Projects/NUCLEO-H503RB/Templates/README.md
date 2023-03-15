## <b>Templates Example Description</b>

- This projects provides a reference template through the HAL API that can be used
to build any firmware application.
- This project is targeted to run on STM32H503xx devices on NUCLEO-H503RB boards from STMicroelectronics.
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

Reference, template

### <b>Directory contents</b>

      - Templates/Src/system_stm32h5xx.c         STM32H5x system source file
      - Templates/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - Templates/Inc/main.h                     Header for main.c module
      - Templates/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file
      - Templates/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - Templates/Src/stm32h5xx_it.c             Interrupt handlers
      - Templates/Src/main.c                     Main program

### <b>Hardware and Software environment</b>

  - This template runs on STM32H503xx devices.

  - This template has been tested with STMicroelectronics NUCLEO-H503RB (MB1814)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files
  - Load images into target memory
  - Run the example

