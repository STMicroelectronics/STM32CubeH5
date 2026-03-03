## <b>PLAY_Emergency_Alarm Example Description</b>

How to configure and use the HAL PLAY driver to implement a complex logic with multiple interconnected lookup tables. This examples demonstrates an emergency alarm system that is outputting a morse SOS signal without using the CPU, in STOP mode.

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

Step 2: The system enters STOP mode to demonstrate that the alarm signal is fully handled by the PLAY hardware.
            During STOP mode, the PLAY LUT output flags are not functional.
            To wakeup the system, this example configures the PLAY EXTI output so that the falling edge of LUT 15
            connected to the PLAY_OUT14 will trigger the EXTI, waking up the system.

Step 3: The user can change the PLAY1_IN12 (PE9 - CN9.8) input signal to trigger the alarm signal.
            As the PLAY peripheral handles the alarm signal, the application does not need to do anything in this step.
            When the user changes the input signal a second time, the PLAY peripheral will stop the alarm signal.
            The alarm stop will trigger a falling edge EXTI event, which will wake the system.
            The application will then turn the LED on for one second to indicate that the alarm has been stopped.
            Returns to step 2 indefinitely if no error occurs.

End of example: The example run indefinitely or until an error occurs.

2. Example configuration

The PLAY peripheral is configured to output a morse SOS signal (three short signals, three long signals, three short signals) on the PLAY1_OUT0 (PF3 - CN9.7) output pin. This sequence operates with the MCU in stop mode.
This PLAY configuration uses 13 interconnected lookup tables (LUTs) to generate the morse SOS signal.
The PLAY1_IN12 (PE9 - CN9.8) input signal is used to trigger and stop the alarm signal.
When the alarm stops, the PLAY peripheral generates a falling edge on the PLAY output pin connected to the EXTI, which will wake the system from STOP mode.


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

      - PLAY/PLAY_Emergency_Alarm/Src/main.c                     Main program
      - PLAY/PLAY_Emergency_Alarm/Src/system_stm32h5xx.c         STM32H5x system source file
      - PLAY/PLAY_Emergency_Alarm/Src/stm32h5xx_it.c             Interrupt handlers
      - PLAY/PLAY_Emergency_Alarm/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - PLAY/PLAY_Emergency_Alarm/Inc/main.h                     Header for main.c module
      - PLAY/PLAY_Emergency_Alarm/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - PLAY/PLAY_Emergency_Alarm/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - PLAY/PLAY_Emergency_Alarm/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

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
  - Stimulate the PLAY input pin and observe the PLAY output pin

