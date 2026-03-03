## <b>PLAY_Signal_Routing Example Description</b>

How to configure the PLAY peripheral to act as signal router. This example shows how to use the HAL drivers to configure the PLAY peripheral to perform a signal router from a TIM internal signal to an external GPIO.

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

Step 1: Configures, initializes, and starts the PLAY & TIM peripherals.

Step 2: Toggles the TIM TRGO internal signal each second.
            The signal is routed to the PLAY1_in12 input signal, which is output on the PLAY1_OUT0 (PF3 - CN9.7) signal.
            Returns to step 2 indefinitely if no error occurs.

End of example: The example run indefinitely or until an error occurs.


2. Example configuration

The PLAY peripheral is configured to route the TIM TRGO internal signal to the PLAY1_OUT0 (PF3 - CN9.7) output pin. The PLAY1_in12 input signal is used to trigger the routing.
One lookup table (LUT) is used to route the TIM TRGO signal to the PLAY1_OUT0 (PF3 - CN9.7) output.
The PLAY1_OUT0 (PF3 - CN9.7) output signal is connected to an external GPIO pin.


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

      - PLAY/PLAY_Signal_Routing/Src/main.c                     Main program
      - PLAY/PLAY_Signal_Routing/Src/system_stm32h5xx.c         STM32H5x system source file
      - PLAY/PLAY_Signal_Routing/Src/stm32h5xx_it.c             Interrupt handlers
      - PLAY/PLAY_Signal_Routing/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - PLAY/PLAY_Signal_Routing/Inc/main.h                     Header for main.c module
      - PLAY/PLAY_Signal_Routing/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - PLAY/PLAY_Signal_Routing/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - PLAY/PLAY_Signal_Routing/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

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
  - Observe the PLAY output pin

