## <b>Templates_Board Example Description</b>

This project provides a reference template for the NUCLEO-H533RE board based on the STM32Cube HAL API and the
BSP drivers that can be used to build any firmware application.

It was created from STM32CubeMX using the 'Start My project from ST board' feature.

This template shows how to use the BSP drivers for the leds, user push-button and VCP COM port of the NUCLEO-H533RE
board thanks to BSP resources initialization and demonstration codes:

  - BSP Leds: LED_GREEN is initialized and switched on.

  - BSP Push-button: BUTTON_USER is initialized in interrupt mode and each key-press is detected through
    the corresponding EXTI13_IRQHandler(). All leds are toggled at each key press.

  - BSP COM:  Virtual COM port associated to ST-LINK is initialized and used to redirect the C library printf(). 
    As an example it outputs the "Welcome to STM32 world !\n\r" message on a terminal.
    The USART is configured as follows:
      - Baudrate = 115200 bps
      - Word length = 8 bits (7-data bit + 1 stop bit)
      - One stop bit
      - No parity
      - Hardware flow control disabled (RTS and CTS signals)

At the beginning of the main program, the HAL_Init() function is called to initialize the systick used as 1ms HAL timebase.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, Template, Start, Board, BSP

### <b>Directory contents</b>

File | Description
 --- | ---
  Templates_Board/Inc/main.h                    |  Header for main.c module
  Templates_Board/Inc/stm32h5xx_hal_conf.h      |  HAL Configuration file
  Templates_Board/Inc/stm32h5xx_it.h            |  Interrupt handlers header file
  Templates_Board/Inc/stm32h5xx_nucleo_conf.h   |  BSP Configuration file
  Templates_Board/Src/main.c                    |  Main program
  Templates_Board/Src/stm32h5xx_hal_msp.c       |  HAL MSP module
  Templates_Board/Src/stm32h5xx_it.c            |  Interrupt handlers
  Templates_Board/Src/system_stm32h5xx.c        |  STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H533x devices.

  - This template has been tested with STMicroelectronics NUCLEO-H533RE
    boards and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
