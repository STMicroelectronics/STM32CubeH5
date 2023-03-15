## <b>DTS_GetTemperature Example Description</b>

How to configure and use the DTS to get the temperature of the die.

      - Board: NUCLEO-H563ZI (embeds a STM32H563ZI device)

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The DTS is configured to continuously get the temperature.
The Temperature variable will hold the temperature value in °C unit.
The Temperature value can be displayed on the debugger in "Temperature" variable.

NUCLEO-H563ZI board's LEDs can be used to monitor the process status:
  - LED3 toggles and example is stopped (using infinite loop) when there is an error during process.

### <b>Keywords</b>

DTS, Sensor, Temperature, Die

### <b>Directory contents</b>

  - DTS/DTS_GetTemperature/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - DTS/DTS_GetTemperature/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - DTS/DTS_GetTemperature/Inc/stm32h5xx_it.h          HAL interrupt handlers header file
  - DTS/DTS_GetTemperature/Inc/main.h                  Header for main.c module
  - DTS/DTS_GetTemperature/Src/stm32h5xx_it.c          HAL interrupt handlers
  - DTS/DTS_GetTemperature/Src/main.c                  Main program
  - DTS/DTS_GetTemperature/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - DTS/DTS_GetTemperature/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZI devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - To get the temperature information, put the "Temperature" variable in the debugger watch.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
