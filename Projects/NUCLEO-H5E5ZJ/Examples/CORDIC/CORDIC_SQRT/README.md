## <b>CORDIC_SQRT Example Description</b>

How to use the CORDIC peripheral to calculate array of SQRT.

In this example, the CORDIC peripheral is configured in SQRT function, q1.31
format for both input and output data, and with 3 cycles of precision.
The input data provided to CORDIC peripheral are values from range [0.027, 0.75[, in q1.31 format (stored in aSqrtArg_Scale0_q1_31 array).
The output data are square root in q1.31 format.
For 3 cycles of precision, the maximal expected residual error of the calculated values is 2^-19.


The calculated square roots are stored in aCalculatedSqrt[] array.
The residual error of calculation results is verified, by comparing to reference values in aRefSqrt_q1_31[] obtained by computing the sqrt of "aSqrtArg_Scale0_q1_31" values with the "C" sqrt function.

STM32 board LEDs are used to monitor the example status:
  - LED1(GREEN) is ON when correct CORDIC Sqrt results are calculated.
  - LED2(RED) is blinking (1 second period) when exceeding residual error on CORDIC sqrt results is detected or when there is an initialization error.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Mathematic, CORDIC, Square root

### <b>Directory contents</b>

    - CORDIC/CORDIC_SQRT/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - CORDIC/CORDIC_SQRT/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - CORDIC/CORDIC_SQRT/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - CORDIC/CORDIC_SQRT/Inc/main.h                      Header for main.c module
    - CORDIC/CORDIC_SQRT/Src/stm32h5xx_it.c              Interrupt handlers
    - CORDIC/CORDIC_SQRT/Src/main.c                      Main program
    - CORDIC/CORDIC_SQRT/Src/stm32h5xx_hal_msp.c         HAL MSP module 
    - CORDIC/CORDIC_SQRT/Src/system_stm32h5xx.c          STM32H5xx system source file

     
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

