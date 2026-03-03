## <b>CORDIC_EXP Example Description</b>

How to use the CORDIC peripheral to calculate the Exponential of a value.

The exponential function, exp(x), can be obtained as the sum of sinh(x) and cosh(x).

In this example, the CORDIC peripheral is configured in HCosine function, q1.31
format for both input and output data, and with 6 cycles of precision.
The input data provided to CORDIC peripheral are values from -0.559 to 0.559 (in aRefValues array) converted
in q1.31 format (in aRefValues_q1_31 array) with arm_float_to_q31 function.
The output data are hyperbolic cosines and hyperbolic sines in q1.31 format.
For 6 cycles of precision, the maximal expected residual error of the calculated hcosines & hsines is 2^-18.

The calculated hyperbolic cosine & hyperbolic sine values are stored in aCalculatedValues_q1_31[] array.
The residual error of calculation results is verified, by comparing the exponential result of 'C' Exp function with the sum of the 2 hyperbolic results (cosh + sinh).

STM32 board LEDs are used to monitor the example status:

  - LED1(GREEN) is ON when exponential value is well computed with the sum of CORDIC hyperbolic cosine & hyperbolic sine results of each value.
  - LED2(RED) is blinking (1 second period) when exceeding residual error on CORDIC Hyperbolic results is detected or when there is an initialization error.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Mathematic, CORDIC, Exponential,

### <b>Directory contents</b>

    - CORDIC/CORDIC_EXP/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - CORDIC/CORDIC_EXP/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - CORDIC/CORDIC_EXP/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - CORDIC/CORDIC_EXP/Inc/main.h                      Header for main.c module
    - CORDIC/CORDIC_EXP/Src/stm32h5xx_it.c              Interrupt handlers
    - CORDIC/CORDIC_EXP/Src/main.c                      Main program
    - CORDIC/CORDIC_EXP/Src/stm32h5xx_hal_msp.c         HAL MSP module 
    - CORDIC/CORDIC_EXP/Src/system_stm32h5xx.c          STM32H5xx system source file

     
### <b>Hardware and Software environment</b> 

  - This example runs on STM32H5E5ZJTx devices.
  
  - This example has been tested with an STMicroelectronics NUCLEO-H5E5ZJ
    board and can be easily tailored to any other supported device 
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

