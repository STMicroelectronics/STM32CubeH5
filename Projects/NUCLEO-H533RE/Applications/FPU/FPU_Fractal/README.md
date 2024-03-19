## <b>FPU_Fractal Floating Point Unit Application Description</b>

  This application demonstrates the benefits brought by the STM32H5 floating-point unit (FPU). The Cortex-M33 FPU is
  an implementation of the single precision variant of the ARMv8-M Floating-point extension, FPv5 architecture.

  The application computes a simple mathematical fractal: the Julia set.
  The generation algorithm for such a mathematical object is quite simple:
  for each point of the complex plan, we are evaluating the divergence speed of a defined sequence. 
  The Julia set equation for the sequence is: z(n+1) = z(n)^2 + c.

  Two workspaces are available to activate or not the FPU during the compilation phase:

    - Without using the FPU, these operations are done by software through the C compiler
      library and are not visible to the programmer; but the performances are very low.
    - When enabling the FPU, all of the real numbers calculations are entirely done by hardware in a
      single cycle, for most of the instructions. The C compiler does not use its own floating-point
      library but directly generates FPU native instructions.

  User might change the number of iterations done while calculating the fractal by changing the ITERATION define in main.h

  NUCLEO-H533RE board's LED2 can be used to monitor the application status:

    - LED2 is toggling with a frequency that corresponds to the fractal calculation speed. So when the FPU is not used,
      the LED2 toggles slowly. However, when the FPU is used, the LED2 toggles quickly.
    - LED2 is ON in case of error.
    The variable "score_fpu" could also be used, in debug mode, to check the duration of the fractal calculation in ms.

  For more information on how to use floating-point units (FPU) in STM32 microcontrollers, refer to [AN4044](http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00047230.pdf)

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, Cortex, floating-point unit, FPU, Double-precision, Fractal, Julia set, Equation, Complex plan, Real numbers

### <b>Directory contents</b>

    - FPU/FPU_Fractal/Inc/stm32h5xx_nucleo_conf.h        BSP configuration file
    - FPU/FPU_Fractal/Inc/stm32h5xx_hal_conf.h           HAL Configuration file
    - FPU/FPU_Fractal/Inc/stm32h5xx_it.h                 Interrupt handlers header file
    - FPU/FPU_Fractal/Inc/main.h                         Main program header file
    - FPU/FPU_Fractal/Src/stm32h5xx_it.c                 Interrupt handlers
    - FPU/FPU_Fractal/Src/main.c                         Main program
    - FPU/FPU_Fractal/Src/system_stm32h5xx.c             STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H533xx devices.

  - This application has been tested with STMicroelectronics NUCLEO-H533RE
    evaluation boards and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Select **FPU-On** or **FPU-Off** workspace
  - Rebuild all files and load your image into target memory
  - Run the application


