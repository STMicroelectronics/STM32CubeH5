## <b>USART_WakeUpFromStop_Init Example Description</b>

Configuration of GPIO and USART3 peripherals to allow the characters received on USART_RX pin to wake up the MCU from low-power mode.
This example is based on the STM32H5xx USART3 LL API. The peripheral
initialization uses LL unitary service functions for optimization purposes (performance and size).

USART3 Peripheral is configured in asynchronous mode (9600 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
USART3 Clock is based on HSI.

Example execution:

After startup from reset and system configuration, LED1 is blinking quickly during 3 sec,
then MCU  enters "Stop 0" mode (LED1 off).
On first  character reception by the USART3 from PC Com port (ex: using HyperTerminal)
after "Stop 0" Mode period, MCU wakes up from "Stop 0" Mode.
Received character value is checked :

- On a specific value ('S' or 's'), LED1 is turned On and program ends.
- If different from 'S' or 's', program performs a quick LED1 blinks during 3 sec and
  enters again "Stop 0" mode, waiting for next character to wake up.

In case of errors, LED1 is slowly blinking (1 sec period).

### <b>Keywords</b>

Connectivity, UART/USART, Asynchronous, baud rate, Interrupt, HyperTerminal,
Receiver, Asynchronous, Low Power, Wake Up

### <b>Directory contents</b>

  - USART/USART_WakeUpFromStop_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - USART/USART_WakeUpFromStop_Init/Inc/main.h                  Header for main.c module
  - USART/USART_WakeUpFromStop_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_WakeUpFromStop_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - USART/USART_WakeUpFromStop_Init/Src/main.c                  Main program
  - USART/USART_WakeUpFromStop_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5E5ZJTx devices.

  - This example has been tested with NUCLEO-H5E5ZJ board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H5E5ZJ Set-up
    - Connect STM32 MCU board USART3 TX pin (GPIO PD9) to PC COM port RX signal
    - Connect STM32 MCU board USART3 RX pin (GPIO PD8) to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled
    on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
      (9600 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example