## <b>ADC_SingleConversion_TriggerSW_DMA Example Description</b>

How to use an ADC peripheral to perform a single ADC conversion on a channel 
at each software start. Converted data is transferred by DMA
into a table in RAM memory.
This example is based on the STM32H5xx ADC HAL API.

Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.

DMA is configured to transfer conversion data in an array (size 4 elements),
in circular mode.

**Note:** In this example, array size can be set to 1 (equivalent to a single variable)
      to have a scenario comparable to other ADC examples using programming model
      polling or interruption.
      Size of array can be modified with literal ADC_CONVERTED_DATA_BUFFER_SIZE.

**Note:** In this example, only DMA interruption of transfer complete is used
      (interruption of half transfer is available, refer to other ADC examples
      with trigger from timer)

Example execution:

From the main program execution, the ADC group regular converts the
selected channel at each software start. DMA transfers conversion data to the array,
indefinitely (DMA in circular mode).

When conversion and data transfer are completed (DMA full buffer),
interruption occurs and data is processed (for this example purpose,
convert digital data to physical value in mV).

LED1 is used to monitor program execution status:

- Normal operation: ADC group regular activity can be observed with LED toggle:

  - At each ADC conversion: LED toggle once (every 1sec)
  - At each buffer completed: LED toggle 4 times quickly (10Hz) (every 4sec)

- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData": ADC group regular buffer conversion data (resolution 12 bits)
- "uhADCxConvertedData_Voltage_mVolt": ADC group regular buffer conversion data computed to voltage value (unit: mV)

Connection needed:
Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

**Note:** If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

- 1 GPIO for LED1
- 1 GPIO for analog input: ADC channel 10 on pin PC.00 (Arduino A1 pin 3 CN9, Morpho PIN 38 CN4)
- 1 DMA channel

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, DMA transfer

### <b>Directory contents</b>

  - ADC/ADC_SingleConversion_TriggerSW_DMA/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Inc/main.h                  Header for main.c module
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Src/stm32h5xx_it.c          Interrupt handlers
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Src/main.c                  Main program
  - ADC/ADC_SingleConversion_TriggerSW_DMA/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.
    
  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

