## <b>ADC_DifferentialMode Example Description</b>

This example describes how to configure and use the ADC1 to convert an external
analog input in Differential Mode, difference between external voltage on VinN and VinP.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 MHz.

The ADC1 is configured to convert continuously ADC_CHANNEL_1 (VinP) connected to PA.01 (PIN 30 CN7 (Morpho Arduino A1 pin 2 CN8))
and its corresponding VinN connected to PA.00 (PIN 28 CN7 (Morpho Arduino A0 pin 1 CN8)).

User can connect ADC_CHANNEL_1 pins (PA.01, PA.00) to external power supply  and vary voltage.

uhADCxConvertedData variable contains conversion data (voltage difference VinP - VinN)  and uhADCxConvertedData_Voltage_mVolt
yields the signed input voltage in mV.

### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Analog, ADC, ADC channel, Polling

### <b>Directory contents</b>

  - ADC/ADC_DifferentialMode/Appli/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - ADC/ADC_DifferentialMode/Appli/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - ADC/ADC_DifferentialMode/Appli/Inc/stm32h5xx_it.h          HAL interrupt handlers header file
  - ADC/ADC_DifferentialMode/Appli/Inc/main.h                    Header for main.c module
  - ADC/ADC_DifferentialMode/Appli/Src/stm32h5xx_it.c          HAL interrupt handlers
  - ADC/ADC_DifferentialMode/Appli/Src/main.c                    Main program
  - ADC/ADC_DifferentialMode/Appli/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - ADC/ADC_DifferentialMode/Appli/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H533RETx devices.

  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H533RE Set-up
    Use an external power supply, adjust supply voltage and connect it to pins PA.01 and PA.00.

  - To monitor the conversion result, put the "uhADCxConvertedData" (or it's voltage representation "uhADCxConvertedData_Voltage_mVolt")
    variable in the debugger live watch.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files from sub-project Appli and load your images into memories: This sub-project will first load Boot_XIP.hex in internal Flash,
   than load Appli part in External memory available on NUCLEO-H533RE board.
 - Run the example
 