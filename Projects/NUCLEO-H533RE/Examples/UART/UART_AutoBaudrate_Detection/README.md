## <b>UART_AutoBaudrate_Detection Example Description</b>

How to use the HAL UART API for detecting automatically the baud rate.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz for STM32H5xx Devices.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change UART configuration.

After initialisation step, the UART instance (USART2) is configured as follows:

 - Baud rate = 115200 bauds
 - Word Length = 8 bits (8 data bits, no parity bit)
 - One Stop Bit
 - Hardware flow control disabled (RTS and CTS signals)
 - Reception and transmission are enabled

Please make sure to configure Hyperterminal connection on PC side with same characteristics.

Goal of this example is to illustrate the different methods supported by STM32 UART peripherals
for automatically detecting baud rate used on a connection. This automatic detection is based on
the reception of one character.

There are 4 auto baud rate detection modes, based on different patterns of received character :

- Mode 0: Start Bit
  In this mode, the USART measures the duration of the start bit (falling edge to rising edge).
  Expected character : Any character starting with a bit0 at 1 (example "A")

- Mode 1: Falling Edge
  In this mode, the USART measures the duration of the Start and of the 1st data bit. The
  measurement is done falling edge to falling edge, to ensure a better accuracy in the
  case of slow signal slopes.
  Expected character : Any character starting with a 10xx bit pattern (example "1")

- Mode 2: A 0x7F character frame (it may be a 0x7F character in LSB first mode or a
  0xFE in MSB first mode).
  In this mode, the baud rate is updated first at the end of the start bit, then at the
  end of bit 6 (based on the measurement done from falling edge to falling edge).
  Expected character : 0x7F character, i.e. DEL key in LSB first mode

- Mode 3: A 0x55 character frame.
  In this mode, the baud rate is updated first at the end of the start bit, then at the
  end of bit0 (based on the measurement done from falling edge to falling edge),
  and finally at the end of bit6.
  Expected character : 0x55 character, i.e. "U" in LSB first mode

### <b>Example execution</b>

After UART peripheral initialisation, the UART/Hyperterminal communication is initiated (Board is sending an information message to PC Terminal console).

As goal of this example is to illustrate use of all auto baud rate detection modes, same procedure will be executed in loop for each of the 4 modes. This procedure could be described as follows :

a) User is invited to choose a new baud rate value, and configure the HyperTerminal accordingly
   (communication seen from HyperTerminal will now use the new baud rate value)
   Prompt also indicate the expected character to be sent from HyperTerminal in order to trig auto baud rate detection mechanism.

b) USART2 is initialised and auto baud rate detection mode is selected

c) Reception is started on STM32 UART instance (1 char expected).
   While waiting, LED2 is blinking (toggle every 100ms).

d) Once received, UART flags are checked to ensure no error occurs during auto baud rate detection procedure.
   If successful, baud rate has been automatically updated on STM32 UART side (BRR value).
   This value is retrieved, and message is sent to user using the newly computed baud rate value.

e) If " ... baud rate value has been adjusted in BRR to XXX ..." message is properly received on HyperTerminal,
   this means that detected baud rate value is exact (baud rate used to send the character from HyperTerminal properly detected).
   Value could be slightly different from initial value set in HyperTerminal configuration.

   In case of successful execution, LED2 is turned On.
   In case of encountered error, LED2 is toggling every 500ms.

### <b>Notes</b>

1. When the parity is enabled, the computed parity is inserted at the MSB position of the transmitted data.

2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, baud rate, RS-232, full-duplex, parity, stop bit, Receiver, Asynchronous, Auto baud rate detection

### <b>Directory contents</b>

  - UART/UART_AutoBaudrate_Detection/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - UART/UART_AutoBaudrate_Detection/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - UART/UART_AutoBaudrate_Detection/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - UART/UART_AutoBaudrate_Detection/Inc/main.h                  Header for main.c module
  - UART/UART_AutoBaudrate_Detection/Src/stm32h5xx_it.c          Interrupt handlers
  - UART/UART_AutoBaudrate_Detection/Src/main.c                  Main program
  - UART/UART_AutoBaudrate_Detection/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - UART/UART_AutoBaudrate_Detection/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H533RE devices.
  - This example has been tested with one NUCLEO-H533RE board embedding
    a STM32H533RE device and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-H533RE Set-up
     - UART Instance : USART2 (Tx Pin: PA2, Rx Pin: PA3)
     - Connect ST-Link cable to the PC USB port to send/receive data between PC and board.
       A virtual COM port will then appear in the HyperTerminal.
     - Hyperterminal configuration:
       - Data Length = 8 Bits
       - One Stop Bit
       - No parity
       - Baud rate = 115200 baud
       - Flow control: None


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example and follow instructions as described in Example execution paragraph
