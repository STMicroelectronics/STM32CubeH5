## <b>PLAY_Gate_XOR Example Description</b>

How to configure the PLAY peripheral to act as a XOR gate. This example shows how to use the HAL drivers to configure the PLAY peripheral to perform a logical XOR operation on two input signals: PLAY1_IN12 (PE9 - CN9.8) external input and Software Trigger 0 input.


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

Step 2: Toggles the PLAY Software Trigger 0 logical level each second.
            The result of the XOR operation is outputted on the PLA1_OUT0 (PF3 - CN9.7) signal.
            Returns to step 2 indefinitely if no error occurs.

NOTE: During this step, the user can change the PLAY1_IN12 (PE9 - CN9.8) input signal to see how the output changes accordingly.
             As the PLAY peripheral performs a logical XOR operation, the output will be high when one of the inputs is high and the other is low, and low when both inputs are either high or low.
             As the PLAY input is configured to be filtered by setting a minimal pulse width, the signal will be taken into account only if it is stable for a certain number of PLAY clock cycles, defined by the filter configuration.

End of example: The example run indefinitely or until an error occurs.

2. Example configuration

The PLAY peripheral is configured to perform a logical XOR operation on two inputs: the PLAY1_IN12 (PE9 - CN9.8) external input and the Software Trigger 0 input. The external input is filtered by setting a minimum pulse width, so it must be stable for a certain number of PLAY clock cycles to be taken into account.
One lookup table (LUT) is used to implement the XOR operation.
The PLA1_OUT0 (PF3 - CN9.7) output signal is connected to this LUT and is used to output the result of the XOR operation on the PLA1_OUT0 (PF3 - CN9.7) pin.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

PLAY, Gate, XOR, LUT, LogicArray

### <b>Directory contents</b>

      - PLAY/PLAY_Gate_XOR/Src/main.c                     Main program
      - PLAY/PLAY_Gate_XOR/Src/system_stm32h5xx.c         STM32H5x system source file
      - PLAY/PLAY_Gate_XOR/Src/stm32h5xx_it.c             Interrupt handlers
      - PLAY/PLAY_Gate_XOR/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - PLAY/PLAY_Gate_XOR/Inc/main.h                     Header for main.c module
      - PLAY/PLAY_Gate_XOR/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - PLAY/PLAY_Gate_XOR/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - PLAY/PLAY_Gate_XOR/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

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
  - Stimulate the PLAY input pins and observe the PLAY output pin
    to verify the XOR logic function.

