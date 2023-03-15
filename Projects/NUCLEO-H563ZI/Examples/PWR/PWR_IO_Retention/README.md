## <b>PWR_IO_Retention Example Description</b>

How to Use the Standby IO Retention.

In the associated software, the system clock is set to 250 MHz.

1- At the beginning of the program, LED1, LED2, LED3 and User BUTTON are initialized.
2- The LED1, LED2 and LED3 are turned On.
3- The Standby IO Retention is enabled.
4- At this stage, the program waits for user to press the button to enter the system to STANDBY mode.
Once button pressed, the Leds kept their state ON and the debug is lost.

To exit the STANDBY mode, reset the board so the program will check that the system resumed from STANDBY.
If so, the Standby IO retention is disabled. The Leds will be turned off for 2s as they lose their state.

After reset, the program runs steps 1, 2, 3 and remain in step 4 waiting user to press button.


LED1, LED2 and LED3 is used to monitor the system state as follows:

 - LED1, LED2 and LED3 on  : Before and after the user button press.
 - LED1, LED2 and LED3 off (during 2 seconds) : After the wake up of system from stanby using RESET.

#### <b>Notes</b>

 1. This example can not be used in DEBUG mode due to the fact
    that the Cortex-M33 core is no longer clocked during low power mode
    so debugging features are disabled.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, EXTI, Standby mode, IO_Retention, Wakeup, External reset

### <b>Directory contents</b>

  - PWR/PWR_IO_retention/Inc/stm32h5xx_nucleo_conf.h               BSP configuration file
  - PWR/PWR_IO_retention/Inc/stm32h5xx_conf.h                      HAL Configuration file
  - PWR/PWR_IO_retention/Inc/stm32h5xx_it.h                        Header for stm32h5xx_it.c
  - PWR/PWR_IO_retention/Inc/main.h                                Header file for main.c
  - PWR/PWR_IO_retention/Src/system_stm32h5xx.c                    STM32H5xx system clock configuration file
  - PWR/PWR_IO_retention/Src/stm32h5xx_it.c                        Interrupt handlers
  - PWR/PWR_IO_retention/Src/stm32h5xx_hal_msp.c                   HAL MSP module
  - PWR/PWR_IO_retention/Src/main.c                                Main program

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5xx devices

  - This example has been tested with STMicroelectronics NUCLEO-H563ZI
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-H563ZI set-up:
    - LED1 connected to PB.00 pin
    - LED2 connected to PF.04 pin
    - LED3 connected to PG.04 pin


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example