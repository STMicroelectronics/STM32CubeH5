## <b>UART_ParityCheck Example Description</b>

How to use the HAL UART API for UART transmission with ewith user interaction.
  - Board : STM32H5F5J-DK
  - UART Instance : USART2
  - Tx Pin: PA2
  - Rx Pin: PA3

LED1 toggles when parity error has been detected.
LED3 toggles when the transfer is successful.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated. Thanks to a first message sent from STM32 side,
using the expected parity convention (Odd), user will be requested to send 10 charcaters through the hyperterminal.
UART peripheral is reconfigured to use Even parity convention. Data reception on STM32 side will detect parity error
on each received character. The board will then select the correct parity convention (Odd) and ask again to send
10 charcaters through the hyperterminal. Received characters will be sent back alongside the end message.

The USART is configured as follows:

    - BaudRate = 115200 baud
    - Word Length = 9 Bits (8 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

#### <b>Notes</b>

1. When the parity is enabled, the computed parity is inserted at the MSB
   position of the transmitted data.

2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, Printf, baud rate, RS-232, HyperTerminal, full-duplex, Interrupt,
Transmitter, Receiver, Asynchronous, Error management

### <b>Directory contents</b>

  - UART/UART_ParityCheck/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - UART/UART_ParityCheck/Inc/stm32h5xx_hal_conf.h        HAL configuration file
  - UART/UART_ParityCheck/Inc/stm32h5xx_it.h              IT interrupt handlers header file
  - UART/UART_ParityCheck/Inc/main.h                      Header for main.c module
  - UART/UART_ParityCheck/Src/stm32h5xx_it.c              Interrupt handlers
  - UART/UART_ParityCheck/Src/main.c                      Main program
  - UART/UART_ParityCheck/Src/stm32h5xx_hal_msp.c         HAL MSP module
  - UART/UART_ParityCheck/Src/system_stm32h5xx.c          STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.

  - This example has been tested with STM32H5F5J-DK board embedding a
    STM32H5F5LJHxQ and can be easily tailored to any other supported device
    and development board.

  - STM32H5F5J-DK Set-up
     - Connect ST-Link cable to the PC USB port to send/receive data between PC and board.
       A virtual COM port will then appear in the HyperTerminal.

  - Hyperterminal configuration:
    - Data Length = 8 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 115200 baud
    - Flow control: None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example