## <b>SPI_TwoBoards_FullDuplex_DMA_Master_Init Example Description</b>

Data buffer transmission and reception via SPI using DMA mode. This example is
based on the STM32H5xx SPI LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

The communication is done with 2 boards through SPI.

This example shows how to configure GPIO and SPI peripherals
to use a Full-Duplex communication using DMA Transfer mode through the STM32H5xx SPI LL API.

This example is split in two projects, Master board and Slave board:

- Master Board
  SPI3 Peripheral is configured in Master mode.
  GPDMA1_Channel0 and GPDMA1_Channel1 configured to transfer Data via SPI peripheral
  GPIO associated to USER push-button is linked with EXTI.

- Slave Board
  SPI3 Peripheral is configured in Slave mode.
  GPDMA1_Channel0 and GPDMA1_Channel1 configured to transfer Data via SPI peripheral

### <b>Example execution:</b>

- On BOARD MASTER, LED2 is blinking Fast (200ms) and wait USER push-button action.
- Press USER push-button on BOARD MASTER start a Full-Duplex communication through DMA.
- On MASTER side, Clock will be generated on SCK line, Transmission(MOSI Line) and reception (MISO Line) will be done at the same time.
- SLAVE SPI will received  the Clock (SCK Line), so Transmission(MISO Line) and reception (MOSI Line) will be done also.
- LED2 is On on both boards if data is well received.
- In case of errors, LED2 is blinking Slowly (1s).

### <b>Note</b>

You need to perform a reset on Master board, then perform it on Slave board to have the correct behaviour of this example.

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Interrupt, Transmission, Reception, Master, Slave, MISO, MOSI, DMA

### <b>Directory contents</b>

  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Inc/main.h                  Header for main.c module
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Src/main.c                  Main program
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Master_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on NUCLEO-H533RE devices.
  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.
  - NUCLEO-H533RE Set-up :
    - Connect Master board PB1 to Slave Board PB1 (connected to pin D3 of CN9 connector)
    - Connect Master board PB0 to Slave Board PB0 (connected to pin D12 of CN5 connector)
    - Connect Master board PB5 to Slave Board PB5 (connected to pin D11 of CN5 connector)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
    - Load the Master project in Master Board
    - Load the Slave project in Slave Board
 - Run the example

