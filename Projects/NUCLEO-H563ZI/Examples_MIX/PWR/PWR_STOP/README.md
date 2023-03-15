## <b>PWR_STOP Example Description</b>

How to enter the STOP mode and wake up from this mode by using external 
reset or wakeup interrupt (all the RCC function calls use RCC LL API 
for minimizing footprint and maximizing performance).

In the associated software, the system clock is set to 250 MHz, an EXTI line
is connected to the user button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick 
interrupt handler, LED1 is toggled in order to indicate whether the MCU is in STOP mode 
or RUN mode.

5 seconds after start-up, the system automatically enters STOP mode and 
LED1 stops toggling.

The USER push-button can be pressed at any time to wake-up the system. 
The software then comes back in RUN mode for 5 sec. before automatically entering STOP mode again. 

Two leds LED1 and LED2 are used to monitor the system state as following:

 - LED1 toggling: system in RUN mode
 - LED1 off : system in STOP mode
 - LED1 and LED2 ON: configuration failed (system will go to an infinite loop)

These steps are repeated in an infinite loop.

#### <b>Notes</b>

 1. To measure MCU current consumption on board STM32H5 Nucleo,
    board configuration must be applied:
    - remove all jumpers on connector JP2 to avoid leakages between ST-Link circuitry and STM32H5 device.
    - remove jumper JP5 and connect an amperemeter to measure current between the 2 connectors of JP5.

 2. This example can not be used in DEBUG mode due to the fact 
    that the Cortex-M33 core is no longer clocked during low power mode 
    so debugging features are disabled.

 3. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 4. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, stop mode, wake-up, external reset, Interrupt, low power mode

### <b>Directory contents</b>

  - Examples_MIX/PWR/PWR_STOP/Inc/stm32h5xx_conf.h         HAL Configuration file
  - Examples_MIX/PWR/PWR_STOP/Inc/stm32h5xx_it.h           Header for stm32h5xx_it.c
  - Examples_MIX/PWR/PWR_STOP/Inc/main.h                   Header file for main.c
  - Examples_MIX/PWR/PWR_STOP/Src/system_stm32h5xx.c       STM32H5xx system clock configuration file
  - Examples_MIX/PWR/PWR_STOP/Src/stm32h5xx_it.c           Interrupt handlers
  - Examples_MIX/PWR/PWR_STOP/Src/main.c                   Main program
  - Examples_MIX/PWR/PWR_STOP/Src/stm32h5xx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5xx devices

  - This example has been tested with STMicroelectronics NUCLEO-H563ZI
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-H563ZI set-up:
    - Use LED1 and LED2 connected respectively to PB.00 and PF.04 pins
    - USER push-button connected to pin PC.13 (External line 13)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


