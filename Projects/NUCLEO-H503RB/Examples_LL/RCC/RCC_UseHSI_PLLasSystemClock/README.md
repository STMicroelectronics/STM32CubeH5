## <b>RCC_UseHSI_PLLasSystemClock Example Description</b>

Modification of the PLL parameters in run time.

In this example, the toggling frequency of the green LED2 depends on the system clock
frequency and, each time the USER push-button is pressed, the PLL switches between two configurations.

This make the **LED2 blinking** speed to highlight the system clock frequency changes.

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSI as clock source.
Then, when pressing USER push-button, an automatic switch is done between PLL with HSI as clock source (SYSCLK
set to 64MHz) and PLL with HSI as clock source (SYSCLK set to 250MHz).
LED2 will toggle differently between the 2 configurations (quick toggle with SYSCLK configuration at 250MHz).

To detect a problem with PLL configuration, switch USE_TIMEOUT can be enabled. Then in case of issues,
LED2 will toggle every 1 second.

**Note:** "uwFrequency" variable can be added in LiveWatch to monitor the system clock frequency.

### <b>Keywords</b>

System, RCC, PLL, PLLCLK, SYSCLK, HSE, Clock, Oscillator, HSI

### <b>Directory contents</b>

  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/main.h                  Header for main.c module
  - RCC/RCC_UseHSI_PLLasSystemClock/Inc/stm32_assert.h          Template file to include assert_failed function
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/stm32h5xx_it.c          Interrupt handlers
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/main.c                  Main program
  - RCC/RCC_UseHSI_PLLasSystemClock/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

