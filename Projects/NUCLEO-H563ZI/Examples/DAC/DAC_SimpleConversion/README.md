## <b>DAC_SimpleConversion Example Description</b>

How to use the DAC peripheral to do a simple conversion.

      - The example uses the DAC for a simple conversion in 8 bits right
        alignment of 0xFF value, the result of conversion can be seen by
        connecting PA4(DAC channel1) to an oscilloscope.
        The observed value is 3.3V.
      - The low power mode of DAC (sample and hold mode) can also be used while Cortex is in sleep mode.
      - The tests steps are:
      - Step 0:

        DAC:     normal power mode
        Cortex:  run mode
      - Step 1:

        DAC:     Low power mode
        Cortex:  Sleep mode

NUCLEO-H563ZI board's LEDs can be used to monitor the process status:

  - LED3 is ON and example is stopped (using infinite loop) when there is an error during process.

### <b>Keywords</b>

Analog, DAC, Digital to Analog, Single conversion, Sample and Hold

### <b>Directory contents</b>

  - DAC/DAC_SimpleConversion/Inc/stm32h5xx_nucleo_conf.h  BSP configuration file
  - DAC/DAC_Simple_Conversion/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - DAC/DAC_Simple_Conversion/Inc/stm32h5xx_it.h          DMA interrupt handlers header file
  - DAC/DAC_Simple_Conversion/Inc/main.h                  Header for main.c module  
  - DAC/DAC_Simple_Conversion/Src/stm32h5xx_it.c          DMA interrupt handlers
  - DAC/DAC_Simple_Conversion/Src/main.c                  Main program
  - DAC/DAC_Simple_Conversion/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - DAC/DAC_Simple_Conversion/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up

      - Connect PA4 (DAC Channel1) (pin 32 in CN4) to an oscilloscope.
      - Press user button to switch between steps.
      - You may redo the tests by changing the sample and hold parameters 
        of the DAC.
      

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
