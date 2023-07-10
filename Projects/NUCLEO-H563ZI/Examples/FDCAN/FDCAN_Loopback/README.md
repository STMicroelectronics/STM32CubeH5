## <b>FDCAN_Loopback Example Description</b>
How to configure the FDCAN to operate in loopback mode.

At the beginning of the main program the HAL_Init() function is called to reset 
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

Then:

  - FDCAN module is configured to operate in external loopback mode,
    with Nominal Bit Rate of 1 MBit/s and Data Bit Rate of 8 MBit/s.
  - FDCAN module is configured to receive:
    - messages with pre-defined standard ID to Rx FIFO 0
    - messages with pre-defined extended ID to Rx FIFO 1
  - FDCAN module is started.
  - The following messages are then sent:
    - one standard ID message matching Rx FIFO 0 filter
    - two extended ID messages matching Rx FIFO 1 filter

The FDCAN peripheral configuration is ensured by the HAL_FDCAN_Init() function.
This later is calling the HAL_FDCAN_MspInit()function which core is implementing
the configuration of the needed FDCAN resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change FDCAN configuration.

Main program checks that the three messages are received as expected.
If the result is OK, LED1 start blinking.

If the messages are not correctly received, or if at any time of the process an error is encountered,
LED3 is turned ON.

#### <b>Notes</b>
  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
  2. The application need to ensure that the SysTick time base is always set to 1 millisecond

### <b>Keywords</b>
Connectivity, CAN/FDCAN, Loopback, Polling

### <b>Directory contents</b>
  - FDCAN/FDCAN_Loopback/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - FDCAN/FDCAN_Loopback/Inc/stm32h5xx_it.h          Header for stm32h5xx_it.c
  - FDCAN/FDCAN_Loopback/Inc/main.h                  Header for main.c module
  - FDCAN/FDCAN_Loopback/Src/stm32h5xx_it.c          Interrupt handlers
  - FDCAN/FDCAN_Loopback/Src/main.c                  Main program
  - FDCAN/FDCAN_Loopback/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - FDCAN/FDCAN_Loopback/Src/system_stm32h5xx.c      stm32h5xx system source file

### <b>Hardware and Software environment</b>
  - This example runs on STM32H563ZITx devices.
  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.
  - NUCLEO-H563ZI Set-up:
    - No FDCAN connector available on this board. FDCAN RX/TX signals are accessible through Arduino connector.

### <b>How to use it ?</b>
In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
