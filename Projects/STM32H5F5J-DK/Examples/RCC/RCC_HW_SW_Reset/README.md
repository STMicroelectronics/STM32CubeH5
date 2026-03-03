## <b>RCC_HW_SW_Reset Example Description</b>

How to save data to RTC backup registers and perform a software reset triggered by a user button press using the HAL APIs.

In this example:

The system clock is configured to run at maximum frequency using the PLL with HSE as the clock source.
The USER push-button is configured to generate an external interrupt on each press.
When the button is pressed, the application saves a predefined data value into RTC backup registers, then triggers a software reset.
After reset, the application reads the software reset flag.
If the software reset flag is set:

  - Read the saved data from the backup register.
  - If the saved data matches the expected value, LED3 is turned on to confirm expected data.
  - If the saved data does not match the expected value, LED1 is turned on to indicate unexpected data.
  - Clear the backup registers.

Note: At startup, before any user button press, the software reset flag is set by default. LED1 will indicate unexpected data, since the backup register has no saved data, 

On STM32H5F5J-DK:

External line 8 is connected to the USER push-button.

In this example, HCLK is configured at 250 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, GPIO, EXTI, RTC, Backup Registers, Software Reset, Input, Output, Push-pull, Toggle

### <b>Directory contents</b>

  - RCC/RCC_HW_SW_Reset/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
  - RCC/RCC_HW_SW_Reset/Inc/stm32h5xx_hal_conf.h            HAL configuration file
  - RCC/RCC_HW_SW_Reset/Inc/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - RCC/RCC_HW_SW_Reset/Inc/main.h                          Header for main.c module
  - RCC/RCC_HW_SW_Reset/Src/stm32h5xx_it.c                  Interrupt handlers
  - RCC/RCC_HW_SW_Reset/Src/stm32h5xx_hal_msp.c             HAL MSP file
  - RCC/RCC_HW_SW_Reset/Src/main.c                          Main program
  - RCC/RCC_HW_SW_Reset//Src/system_stm32h5xx.c             STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.

  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

#### <b>Note :</b>

<b>STM32CubeIDE</b>

 - After completing the build, open the Debug configuration.
 - Go to the Debugger section and select "**Software system reset**" under "**Reset behavior**".
 - Run the example.

