## <b>ADC_Regular_injected_TriggerTimer Example Description</b>

ADC_Regular_injected_groups ADC conversion example showing the usage of the 2 ADC groups: regular group for ADC conversions on main stream and injected group for ADC conversions limited on specific events (conversions injected within main conversions stream),
using related peripherals (GPIO, DMA, Timer), voltage input from DAC, user control by push button and LED

**Example Description:**

This example provides a short description of how to use the ADC peripheral to
perform conversions using the two ADC groups: regular group for ADC conversions
on main stream and injected group for ADC conversions limited on specific events
(conversions injected within main conversions stream).

Other peripherals related to ADC are used:
 - GPIO peripheral is used in analog mode to drive signal from device pin to ADC input.
 - Timer peripheral is used to trigger ADC conversions.
 - DMA peripheral is used to transfer ADC converted data.

**ADC settings:**
 - Regular group:
   Conversions are triggered by external event (timer at 1kHz).
 - Continuous mode is disabled (and sequencer disabled: only 1 channel selected) to yield only 1 conversion at each conversion trigger.
 - Injected group:
   Conversions are software-triggered by a mere call to HAL_ADCEx_InjectedStart_IT()
   to convert VREFINT channel at each press on Tamper button.

**ADC conversion results:**
 - ADC regular conversions results are transferred automatically by DMA, into variable
   array "aADCxConvertedValues".
 - DMA and ADC are configured to operate continuously, in circular mode.
   When DMA transfer half-buffer and full buffer lengths are reached, callbacks
   HAL_ADC_ConvHalfCpltCallback() and HAL_ADC_ConvCpltCallback() are called.
 - ADC injected conversions results are retrieved upon JEOC interrupt by
   HAL_ADCEx_InjectedConvCpltCallback() function.
 - uhADCxConvertedValue_Injected variable contains the  ADC conversion result of injected group.

**Board settings:**
 - ADC is configured to convert ADC_CHANNEL_2 on pin PF.11(Connector CN7 pin 38).
 - Channel configured on regular group:
   User has to connect the Nucleo ADC1 ADC_CHANNEL_2 PF.11 pin
   to the DAC1 output PA.04 pin.
 - Voltage is increasing at each click on push button, from 0 to maximum range in 4 steps.
   Clicks on push button follow circular cycles: At clicks counter maximum value reached, counter is set back to 0.
 - Channel configured on injected group:
   The voltage input is provided from internal reference voltage VrefInt.

   NUCLEO-H5E5ZJ board LEDs are used to monitor the program execution status:
 - Normal operation: LED1(LED_GREEN) is turned-on/off in function of ADC conversion
   result.
    - Turned-off if voltage measured by injected group is below voltage measured by regular group (average of results table)
    - Turned-on if voltage measured by injected group is above voltage measured by regular group (average of results table)
    - Error : LED2(LED_RED) remaining turned on
### <b>Keywords</b>

Analog, ADC, Analog to Digital, Regular conversion, Injected conversion, Continuous conversion, software-triggered, Timer-triggered, Measurement, Voltage, Button

### <b>Directory contents</b>

    - ADC/ADC_Regular_injected_TriggerTimer/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
    - ADC/ADC_Regular_injected_TriggerTimer/Inc/stm32h5xx_hal_conf.h    HAL configuration file
    - ADC/ADC_Regular_injected_TriggerTimer/Inc/stm32h5xx_it.h          Interrupt handlers header file
    - ADC/ADC_Regular_injected_TriggerTimer/Inc/main.h                  Header for main.c module
    - ADC/ADC_Regular_injected_TriggerTimer/Src/stm32h5xx_it.c          Interrupt handlers
    - ADC/ADC_Regular_injected_TriggerTimer/Src/stm32h5xx_hal_msp.c     HAL MSP module
    - ADC/ADC_Regular_injected_TriggerTimer/Src/main.c                  Main program
    - ADC/ADC_Regular_injected_TriggerTimer/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5E5ZJTx devices.

  - This example has been tested with NUCLEO-H5E5ZJ board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

