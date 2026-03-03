## <b>EXTI_NVIC Example Description</b>

How to configure external interrupt lines and manage interrupt masking and enabling using the HAL API.

In this example, a timer (Timer 2) is configured to generate an interrupt and toggle a LED (LED3) at each interrupt. Additionally,
an EXTI line (External line 8), connected to GPIO pin PA.8, is configured to generate an interrupt on every rising edge of this pin.
Within the EXTI line 8 interrupt routine, the timer interrupt is enabled or masked based on the USER push-button press.

In this example:

LED3 toggles every 2 seconds using Timer 2.
When a rising edge is detected on External line 8 by pressing the USER push-button, the timer interrupt is masked, and LED3 is turned off.
Pressing the button again reactivates the timer interrupt, allowing LED3 to toggle once more.

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

System, GPIO, EXTI, TIMER, Input, Output, Alternate function, Push-pull, Toggle, push-button

### <b>Directory contents</b>

  - EXTI/EXTI_NVIC/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
  - EXTI/EXTI_NVIC/Inc/stm32h5xx_hal_conf.h            HAL configuration file
  - EXTI/EXTI_NVIC/Inc/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - EXTI/EXTI_NVIC/Inc/main.h                          Header for main.c module
  - EXTI/EXTI_NVIC/Src/stm32h5xx_it.c                  Interrupt handlers
  - EXTI/EXTI_NVIC/Src/stm32h5xx_hal_msp.c             HAL MSP file
  - EXTI/EXTI_NVIC/Src/main.c                          Main program
  - EXTI/EXTI_NVIC//Src/system_stm32h5xx.c             STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.

  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

