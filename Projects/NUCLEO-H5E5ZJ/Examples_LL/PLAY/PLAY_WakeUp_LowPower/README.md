## <b>PLAY_WakeUp_LowPower Example Description</b>

How to configure the PLAY peripheral to wake up the system from low-power STOP mode.

- This project is targeted to run on STM32H5E5xx devices on NUCLEO-H5E5ZJ boards from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 Mhz.
- The template project calls also CACHE_Enable() function in order to enable the Instruction
and Data Caches. This function is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

1. Detailed scenario

The application executes the following example steps:

Step 1: Configures, initializes, and starts the PLAY instance.

Step 2: Enters low power STOP mode with WaitForEvent parameter.

Step 3: The user create a falling edge on the PLAY input signal.
            This is routed through the PLAY peripheral to the EXTI line, which wakes up the system from low-power STOP mode.
            The system wakes up and turn the status LED on for one second.
            Returns to step 2 indefinitely if no error occurs.

End of example: The example run indefinitely or until an error occurs.


2. Example configuration

The PLAY peripheral is configured to wake up the system from low-power STOP mode when a falling edge is detected on the PLAY1_IN12 (PE9 - CN9.8) input signal.
The PLAY1_IN12 (PE9 - CN9.8) input signal is routed with one lookup table (LUT) to the PLAY output connected to the EXTI line, which wakes up the system from low-power STOP mode.

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

      - PLAY/PLAY_WakeUp_LowPower/Src/main.c                     Main program
      - PLAY/PLAY_WakeUp_LowPower/Src/system_stm32h5xx.c         STM32H5x system source file
      - PLAY/PLAY_WakeUp_LowPower/Src/stm32h5xx_it.c             Interrupt handlers
      - PLAY/PLAY_WakeUp_LowPower/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - PLAY/PLAY_WakeUp_LowPower/Inc/main.h                     Header for main.c module
      - PLAY/PLAY_WakeUp_LowPower/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - PLAY/PLAY_WakeUp_LowPower/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - PLAY/PLAY_WakeUp_LowPower/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H5E5xx devices.

  - This template has been tested with STMicroelectronics NUCLEO-H5E5ZJ (MB2129)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files
  - Load images into target memory
  - Run the example
  - Stimulate the PLAY input pin and observe the LED behavior and the PLAY output pin

