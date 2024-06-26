## <b>GPIO_EXTI Example Description</b>

How to configure external interrupt lines.

Then the *SystemClock_Config()* function is used to configure the system clock (SYSCLK) to run at 250 MHz.

In this example, one EXTI line (External line 13) is configured to generate
an interrupt on each falling edge.
In the interrupt routine a led connected to a specific GPIO pin is toggled.

In this example:

    - External line 13 is connected to PC.13 pin
    - when falling edge is detected on External line 13 by pressing USER push-button, LED2 toggles once

On NUCLEO-H533RE:

    - External line 13 is connected to USER push-button

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

System, GPIO, EXTI, Output, Alternate function, Push-pull, Toggle

### <b>Directory contents</b>

  - GPIO/GPIO_EXTI/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - GPIO/GPIO_EXTI/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - GPIO/GPIO_EXTI/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - GPIO/GPIO_EXTI/Inc/main.h                  Header for main.c module  
  - GPIO/GPIO_EXTI/Src/stm32h5xx_it.c          Interrupt handlers
  - GPIO/GPIO_EXTI/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - GPIO/GPIO_EXTI/Src/main.c                  Main program
  - GPIO/GPIO_EXTI/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H533RETx devices.

  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


