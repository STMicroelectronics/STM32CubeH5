## <b>DAC_SignalsGeneration Example Description</b>

How to use the DAC peripheral to generate several signals using the DMA
controller and the DAC internal wave generator.

For each press on USER push-button, a signal has been selected and can be monitored on the DAC channel one :

    - Triangle waveform (Channel 1), amplitute: ~1V, frequency: order of Hz.
    - Escalator waveform (Channel 1) using DMA transfer, amplitute: ~3V, frequency: order of KHz.

NUCLEO-H563ZI board's LEDs can be used to monitor the process status:

  - LED3 is ON and example is stopped (using infinite loop) when there is an error during process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Analog, DAC, Digital to Analog, Continuous conversion, DMA, Escalator generation, Timer Trigger

### <b>Directory contents</b>

  - DAC/DAC_SignalsGeneration/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - DAC/DAC_SignalsGeneration/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - DAC/DAC_SignalsGeneration/Inc/stm32h5xx_it.h          DAC interrupt handlers header file
  - DAC/DAC_SignalsGeneration/Inc/main.h                  Header for main.c module  
  - DAC/DAC_SignalsGeneration/Src/stm32h5xx_it.c          DAC interrupt handlers
  - DAC/DAC_SignalsGeneration/Src/main.c                  Main program
  - DAC/DAC_SignalsGeneration/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - DAC/DAC_SignalsGeneration/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up :

      - Connect PA4 (DAC Channel1) (pin 32 in CN4) to an oscilloscope.
      
      - Use USER push-button connected to PC.13.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
