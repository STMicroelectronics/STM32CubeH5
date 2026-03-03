## <b>BSP Example Description</b>

How to use the different BSP drivers of the board.

This project is targeted to run on STM32H5F5xx devices on STM32H5F5J-DK board from STMicroelectronics.

The project configures the maximum system clock frequency at 250Mhz.

Red LED toggles every 500ms whenever any error is detected.

This example could be tested by selecting the required module from the menu dislyaed on the LCD.

#### <b>LED</b>

- This example shows how to switch on, switch off and toggle all leds.
- Use GoBack icon, displayed on top right, to go back to main menu.

#### <b>LCD</b>
- This example shows how to use the different LCD features to display string with different fonts, 
  to display different shapes.
- Use GoBack icon, displayed on top right, to go back to main menu.

#### <b>TOUCHSCREEN</b>
- This example shows how to use the touchscreen.
- Use GoBack icon, displayed on top right, to go back to main menu.

#### <b>SD</b>
- This example shows how to erase, write and read the SD card and also how to detect the presence of the card.
- Use GoBack icon, displayed on top right, to go back to main menu.

#### <b>XSPI</b>
- This example shows how to erase, write and read data available on the Octal Flash memory available on the board 
(STR and DTR in Normal mode and MemoryMapped).
- Use GoBack icon, displayed on top right, to go back to main menu.


#### Notes
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

 3. After exiting audio tests, a noise may remain for some times until the HW stabilizations.
      
### <b>Keywords</b>

BSP, Led, Button, LCD, TS, Touchscreen, OSPI, SD 

### <b>Directory contents</b>

    - BSP/Src/main.c                      Main program
    - BSP/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file
    - BSP/Src/stm32h5xx_it.c              Interrupt handlers
    - BSP/Src/stm32h5xx_hal_msp.c         HAL MSP module
    - BSP/Src/led.c                       Test Led features
    - BSP/Src/lcd.c                       Test Lcd features
    - BSP/Src/ts.c                        Test Touch Screen features
    - BSP/Src/sd.c                        Test SDMMC features
    - BSP/Src/ospi.c                      Test OCTOSPI features
    - BSP/Src/audio.c                     Test AUDIO features  
    - BSP/Inc/main.h                      Main program header file
    - BSP/Inc/stm32h5f5j_discovery_conf.h BSP Configuration file
    - BSP/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
    - BSP/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - BSP/Inc/mx25lm51245g_conf.h         MX25LM51245G Configuration file
 
  
### <b>Hardware and Software environment</b>

 - This example runs on STM32H5F5LJHxQ devices.
 - This example has been tested with STM32H5F5J-DK board and can be 
   easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

