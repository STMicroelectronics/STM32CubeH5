## <b>SPI_TwoBoards_FullDuplex_IT_Slave_Init Example Description</b>

Data buffer transmission and receptionvia SPI using Interrupt mode. This
example is based on the STM32H5xx SPI LL API. The peripheral
initialization uses LL unitary service functions for optimization purposes (performance and size).

The communication is done with 2 boards through SPI.

This example shows how to configure GPIO and SPI peripherals
to use a Full-Duplex communication using IT mode through the STM32H5xx COM_INSTANCE1_TYPE LL API.

This example is split in two projects, Master board and Slave board:

Master Board :

- SPI1 Peripheral is configured in Master mode.
- SPI1_IRQn activated.
- RXP and TXP Interrupts SPI peripheral activated.
- GPIO associated to USER push-button is linked with EXTI.

Slave Board :

- SPI1 Peripheral is configured in Slave mode.
- SPI1_IRQn activated.
- RXP and TXP Interrupts SPI peripheral activated.

Example execution :

On BOARD MASTER, LED2 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD MASTER start a Full-Duplex communication through IT.
On MASTER side, Clock will be generated on SCK line, Transmission(MOSI Line) and reception (MISO Line)
will be done at the same time. 
SLAVE SPI will received  the Clock (SCK Line), so Transmission(MISO Line) and reception (MOSI Line) will be done also.

LED2 is On on both boards if data is well received.

In case of errors, LED2 is blinking Slowly (1s).

#### <b>Notes</b>

 1. You need to perform a reset on Master board, then perform it on Slave board
    to have the correct behaviour of this example.

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Transmission, Reception, Slave, MISO, MOSI, Interrupt

### <b>Directory contents</b>

  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Inc/main.h                  Header for main.c module
  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Src/main.c                  Main program
  - SPI/SPI_TwoBoards_FullDuplex_IT_Slave_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up

    - Connect Master board PC0 to Slave Board PC0 (connected to pin 38 of CN7 connector)
    - Connect Master board PC2 to Slave Board PC2 (connected to pin 35 of CN7 connector)
    - Connect Master board PC3 to Slave Board PC3 (connected to pin 37  of CN7 connector)
    - Connect Master board GND to Slave Board GND

@par How to use it ?

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
    - Load the Master project in Master Board
    - Load the Slave project in Slave Board
 - Run the example

