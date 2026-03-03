## <b>CORDIC_LN Example Description</b>

How to use the CORDIC peripheral to calculate the Natural Logarithm of a value.

In this example, the CORDIC peripheral is configured in Natural Logarithm function with a scale factor of 1, q1.31
format for both input and output data, and with 6 cycles of precision.
The input data provided to CORDIC peripheral are values in range [0.107, 1.0[ (in aRefValues array), which are scaled
by a factor of 2^(-1) (in aRefScaledValues) and then converted in q1.31 format (in aRefValues_q1_31 array) with arm_float_to_q31 function.

The output data are natural logarithm in q1.31 format.
For 6 cycles of precision, the maximal expected residual error of the calculated natural logarithm is 2^-18.


The calculated natural logarithm values are stored in aCalculatedNaturalLog_q1_31 array and then converted in float format (in aCalculatedNaturalLog_float array).
The residual error of calculation results is verified, by comparing the logarithm result of 'C' "logf" function with the CORDIC results in float.
To simplify the code of checking result, the result of "logf" function is divided by 2^(scale_factor + 1), 
instead of multiple it to the floated CORDIC result (as recommended in the Reference Manual).

STM32 board LEDs are used to monitor the example status:
  - LED1(GREEN) is ON when natural logarithm value is well computed.
  - LED2(RED) is blinking (1 second period) when exceeding residual error or when there is an initialization error.


#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Mathematic, CORDIC, Natural Logarithm

### <b>Directory contents</b>

    - CORDIC/CORDIC_LN/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - CORDIC/CORDIC_LN/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - CORDIC/CORDIC_LN/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - CORDIC/CORDIC_LN/Inc/main.h                      Header for main.c module
    - CORDIC/CORDIC_LN/Src/stm32h5xx_it.c              Interrupt handlers
    - CORDIC/CORDIC_LN/Src/main.c                      Main program
    - CORDIC/CORDIC_LN/Src/stm32h5xx_hal_msp.c         HAL MSP module 
    - CORDIC/CORDIC_LN/Src/system_stm32h5xx.c          STM32H5xx system source file

     
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

