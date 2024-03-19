## <b>FDCAN_Adaptive_Bitrate_Receiver Example Description</b>

This example describes how to configure the FDCAN peripheral to adapt to different CAN bit rates using restricted mode.

> **_NOTE:_**  This example needs an external FDCAN transceiver: **MIKROE** - [**MCP2542 CLICK**](https://www.mikroe.com/mcp2542-click).

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

Then the *SystemClock_Config()* function is used to configure the system clock (SYSCLK) to run at 250 MHz.

The example manages in a loop, the detection of a functional BitRate, in Restricted Operation mode.<br>
In this mode, the node is able to receive data and remote frames and to give acknowledgement to valid frames, but it does not send any frames.

The example is divided into several steps:<br>

  - In the first step, the *HAL_FDCAN_Init()* function is called to configure the bitrate to try to receive the message from the transmitter board.<br>
    Reception filters are then configured with *HAL_FDCAN_ConfigFilter()*, and FDCAN module is started with *HAL_FDCAN_Start()*.<br>
    The application attempts to receive a valid message and repeats this step with another bitrate if nothing is received.<br>
    If all bitrates were tested and no message was received, the application ends and LED3 blinks.<br>
  - In the second step, the application exits the Restricted Operation Mode with *HAL_FDCAN_ExitRestrictedOperationMode()*.<br>
  - In the third step, the board sends an acknowledge message to the transmitter board with *HAL_FDCAN_AddMessageToTxFifoQ()*.<br>

  - If the result is OK, the application ends and LED1 stays ON.
  - If the transmitter bitrate was not found, or the message is not correctly received, or if at any time of the process an error is encountered, LED3 starts blinking.

The FDCAN peripheral configuration is ensured by the *HAL_FDCAN_Init()* function.
This later is calling the *HAL_FDCAN_MspInit()* function which core is implementing
the configuration of the needed FDCAN resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change FDCAN configuration.


NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:

  - LED1 is ON when the expected bitrate is found as expected and acknowledge message is transmitted.
  - LED3 toggle slowly when there is an error in transmission/reception process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, CAN/FDCAN, Polling, Adaptive-bitrate, Restricted-mode
, MIKROE, MCP2542


### <b>Directory contents</b>

File | Description
 --- | ---
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Inc/stm32h5xx_nucleo_conf.h    | BSP configuration file
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Inc/stm32h5xx_hal_conf.h   | HAL configuration file
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Inc/stm32h5xx_it.h         | Header for stm32h5xx_it.c
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Inc/main.h                 | Header for main.c module
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Src/stm32h5xx_it.c         | Interrupt handlers
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Src/main.c                 | Main program
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Src/stm32h5xx_hal_msp.c    | HAL MSP module
FDCAN/FDCAN_Adaptive_Bitrate_Receiver/Src/system_stm32h5xx.c     | stm32h5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up:
    - FDCAN RX/TX signals can be accessed through the Arduino or morpho connector.<br>
      To use this example, please connect an external FDCAN transceiver to these pins.<br>
      This example has been tested with [**MCP2542 CLICK**](https://www.mikroe.com/mcp2542-click) CAN FD transceiver module from MIKROE.<br>

      To connect the module, please follow these wiring indications:<br>
      - Connect GND, 5V **and** 3V3 on the MCP2545 CLICK board.
      - Connect MCP2545 CLICK **TX** pin to FDCAN **RX** pin PD0.
      - Connect MCP2545 CLICK **RX** pin to FDCAN **TX** pin PD1.

      This must also be done for the other board, running the FDCAN_Adaptive_Bitrate_Transmitter example.
      Both MCP2545 CLICK must be connected to each other using a CAN-FD cable.

    

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

  - Open your preferred toolchain
  - Rebuild all files from this example "FDCAN_Adaptive_Bitrate_Receiver" and load your image into target memory of board A.
  - Rebuild all files from other example "FDCAN_Adaptive_Bitrate_Transmitter" and load your image into target memory of board B.
  - Connect both CAN-FD connectors from each board setup, with a CAN-FD cable.
  - Run the examples on each board, ensuring that **"FDCAN_Adaptive_Bitrate_Transmitter" is the first one to be executed**.
