## <b>PLAY_Counter_3bits Example Description</b>

How to configure the PLAY peripheral to act as a 3 bits counter. This example shows how to use the HAL drivers to configure the PLAY peripheral to perform a counter using interconnected Lookup Tables (LUTs) and the PLAY input signal.

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

Step 2: The user can change the PLAY1_IN12 (PE9 - CN9.8) input signal to see how the outputs changes accordingly.
            The PLAY peripheral counts with 3 bits on each rising edge of the PLAY1_IN12 (PE9 - CN9.8) input signal. Each bit is output on a separate PLAY1_OUT0/13/14 (PF3 - CN9.D6 / PE12 - CN5.6 / PE14 - CN5.4) signal.
            When the counter reaches the maximum value, it wraps around to zero and triggers an interrupt that toggles the LED on the board to indicate the counter overflow.
            Returns to step 2 indefinitely if no error occurs.

End of example: The example run indefinitely or until an error occurs.


2. Example configuration

The PLAY peripheral is configured to count with 3 bits, using the PLAY1_IN12 (PE9 - CN9.8) input signal as a clock source. The PLAY1_OUT0/13/14 (PF3 - CN9.D6 / PE12 - CN5.6 / PE14 - CN5.4) output signals are used to output the counter bits.
The external input signal is connected to the PLAY1_IN12 (PE9 - CN9.8) pin, which is configured to trigger the counter on its rising edge.
Three lookup tables (LUTs) are used to implement the counter logic. Each LUT is configured to use its own registered output and that of the preceding LUT, with the PLAY1_IN12 (PE9 - CN9.8) input signal serving as the clock gate.
The PLAY1_OUT0/13/14 (PF3 - CN9.D6 / PE12 - CN5.6 / PE14 - CN5.4) output signals are connected to each LUT and is used to output each bit of the counter.
The PLAY peripheral is configured to generate an interrupt when the counter reset to zero (falling edge of most significant bit's LUT), which is used to toggle the status LED on the board.


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

      - PLAY/PLAY_Counter_3bits/Src/main.c                     Main program
      - PLAY/PLAY_Counter_3bits/Src/system_stm32h5xx.c         STM32H5x system source file
      - PLAY/PLAY_Counter_3bits/Src/stm32h5xx_it.c             Interrupt handlers
      - PLAY/PLAY_Counter_3bits/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - PLAY/PLAY_Counter_3bits/Inc/main.h                     Header for main.c module
      - PLAY/PLAY_Counter_3bits/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - PLAY/PLAY_Counter_3bits/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - PLAY/PLAY_Counter_3bits/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

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
  - Stimulate the PLAY input pins and observe the PLAY output pins
    to verify the counter logic function.

