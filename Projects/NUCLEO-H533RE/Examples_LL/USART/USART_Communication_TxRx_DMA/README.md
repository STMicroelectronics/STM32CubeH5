## <b>USART_Communication_TxRx_DMA Example Description</b>

This example shows how to configure GPIO, USART2 and GPDMA1 peripherals
to send characters asynchronously to/from an HyperTerminal (PC) in
DMA mode. This example is based on STM32H5xx USART and DMA LL API. Peripheral
initializations are done using LL unitary services functions for optimization
purpose (performance and size).

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.

GPIO associated to User push-button is linked with EXTI. 
Virtual Com port feature of STLINK is used for UART communication between board and PC.

### <b>Example execution</b>

After startup from reset and system configuration, LED2 is blinking quickly and wait User push-button action.
USART2 and GPDMA1 are configured.
On press on push button , TX DMA transfer to PC is initiated. 
RX DMA transfer is also ready to retrieve characters from PC.
"END" string is expected to be entered by user on PC side (HyperTerminal).

In case of both transfers successfully completed, LED2 is turned on.

In case of errors, LED2 is blinking (1sec period).

### <b>Keywords</b>

Connectivity, UART/USART, Asynchronous, RS-232, baud rate, DMA, HyperTerminal, 
Transmitter, Receiver

### <b>Directory contents</b>

  - USART/USART_Communication_TxRx_DMA/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - USART/USART_Communication_TxRx_DMA/Inc/main.h                  Header for main.c module
  - USART/USART_Communication_TxRx_DMA/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_Communication_TxRx_DMA/Src/stm32h5xx_it.c          Interrupt handlers
  - USART/USART_Communication_TxRx_DMA/Src/main.c                  Main program
  - USART/USART_Communication_TxRx_DMA/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>
  - This example runs on STM32H533RETx devices.
  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.
  - NUCLEO-H533RE Set-up:
    In order use of Virtual Com port feature of STLINK for connection between NUCLEO-H533RE and PC,
    please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled 
    on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control). 

  - Launch the program. Press on User push button on board to initiate data transfer.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
