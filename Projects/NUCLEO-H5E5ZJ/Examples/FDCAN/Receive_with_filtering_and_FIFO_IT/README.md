## <b>Receive_with_filtering_and_FIFO_IT Example Description</b>

This example demonstrates how to configure the FDCAN peripheral to receive messages with filtering and FIFO using interrupt-driven (IT) communication between two FDCAN units on the **STM32H5E5ZJ** device.

> **_NOTE:_** This example requires the same application to be run on two STM32H5E5ZJ boards connected together.

> **_NOTE:_** This example needs an external FDCAN transceiver: **MIKROE** - [**MCP2542 CLICK**](https://www.mikroe.com/mcp2542-click).

At the beginning of the main program, the `HAL_Init()` function is called to reset all peripherals and initialize the systick used as 1ms HAL timebase.  
Then the `SystemClock_Config()` function configures the system clock (SYSCLK) to run at **250 MHz**.

The example is divided into several steps:

- **FDCAN Initialization:**  
  `HAL_FDCAN_Init()` configures the module with:  
  - Nominal Bit Rate: 1 MBit/s, sampling point at 75%  
  - Data Bit Rate: 5 MBit/s, sampling point at 80%  
  Reception filters are configured with `HAL_FDCAN_ConfigFilter()` to allow only specific messages.  
  Tx delay compensation is set for Bit Rate Switching mode.  
  The FDCAN module is started with `HAL_FDCAN_Start()`.

- **Interrupt-driven Reception:**  
  The application does nothing in the main loop; received messages are handled in the IRQ handler.  
  Only messages passing the configured filters are received and processed from the FIFO.

- **Transmission and Reception:**  
  When the USER push-button is pressed on any board, its LED1 is turned OFF and its FDCAN module sends a message with Bit Rate Switching.  
  On the other board, after receiving the message in the IRQ handler, the received payload is compared to expected data. If OK, LED1 is turned ON.

- **Error Handling:**  
  If an error is encountered, LED2 starts blinking.

The FDCAN peripheral configuration is ensured by the `HAL_FDCAN_Init()` function, which calls `HAL_FDCAN_MspInit()` to configure the needed FDCAN resources (CLOCK, GPIO, NVIC, DMA).  
You can update this function to change the FDCAN configuration.

STM32 board LEDs are used to monitor the example status:

- LED1 is OFF on one board when the message is transmitted; ON on the other board when the message is received.  
- LED2 blinks slowly on any board when there is an error in transmission/reception.

#### <b>Notes</b>

1. Care must be taken when using `HAL_Delay()`. This function provides accurate delay (in milliseconds) based on a variable incremented in SysTick ISR.  
   If `HAL_Delay()` is called from a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower) than the peripheral interrupt.  
   To change the SysTick interrupt priority you have to use `HAL_NVIC_SetPriority()` function.

2. The example needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Connectivity, CAN/FDCAN, IT, Filtering, FIFO, Bit rate switching, CAN Filter, MIKROE, MCP2542

### <b>Directory contents</b>

    - FDCAN/Receive_with_filtering_and_FIFO_IT/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Inc/main.h                      Header for main.c module
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Src/stm32h5xx_it.c              Interrupt handlers
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Src/main.c                      Main program
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Src/stm32h5xx_hal_msp.c         HAL MSP module
    - FDCAN/Receive_with_filtering_and_FIFO_IT/Src/system_stm32h5xx.c          STM32H5xx system source file

### <b>Hardware and Software environment</b> 

  - This example runs on STM32H5E5ZJ series devices.  
  - This example has been tested with STM32H5E5ZJ (or compatible STM32H5 boards) and can be tailored to other supported devices and boards.  
  - Toolchain: STM32CubeIDE or compatible ARM GCC toolchain.  
  - HAL/LL Drivers: STM32CubeH5 package.

#### Board Set-up

  - FDCAN RX/TX signals can be accessed through the Arduino or morpho connector.  
  - Connect an external FDCAN transceiver to these pins.  
  - This example has been tested with the [**MCP2542 CLICK**](https://www.mikroe.com/mcp2542-click) CAN FD transceiver module from MIKROE.

  **Wiring:**  
  - Connect GND, 5V, and 3V3 on the MCP2542 CLICK board.  
  - Connect MCP2542 CLICK **TX** pin to FDCAN **RX** pin (e.g., PE0).  
  - Connect MCP2542 CLICK **RX** pin to FDCAN **TX** pin (e.g., PD5).  
  - Repeat for the second board.  
  - Connect both MCP2542 CLICK modules together using a CAN-FD cable.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain (e.g., STM32CubeIDE).  
 - Rebuild all files and load your image into the target memory of both boards.  
 - Connect both CAN-FD connectors from each board setup with a CAN-FD cable.  
 - Run the example on each board, and press the USER push-button on one of the boards.