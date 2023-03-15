## <b>SPI_FullDuplex_ComPolling_Slave Example Description</b>

Data buffer transmission/reception between two boards via SPI using Polling mode.

    - Board    : NUCLEO-H503RB (embeds a STM32H503RB device)
    - CLK Pin  : PC0 (pin 38 on CN7 connector)
    - MISO Pin : PC2 (pin 35 on CN7 connector)
    - MOSI Pin : PC3 (pin 37 on CN7 connector)

HAL architecture allows user to easily change code to move to IT or DMA mode. 
To see others communication modes please check following examples:

 - SPI_FullDuplex_ComDMA_Master
 - SPI_FullDuplex_ComDMA_Slave
 - SPI_FullDuplex_ComIT_Master
 - SPI_FullDuplex_ComIT_Slave

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK & 
GPIO). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive() function allows the reception and the 
transmission of a predefined data buffer at the same time (Full Duplex Mode).
If the Master board is used, the project SPI_FullDuplex_ComPolling_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComPolling_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button, SPI Master starts the
communication by sending aTxBuffer and receiving aRxBuffer through 
HAL_SPI_TransmitReceive(), at the same time SPI Slave transmits aTxBuffer 
and receives aRxBuffer through HAL_SPI_TransmitReceive(). 
The end of this step is monitored through the HAL_SPI_GetState() function
result.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.

STM32 board LED can be used to monitor the transfer status:

 - LED2 toggles quickly on master board waiting USER push-button to be pressed.
 - LED2 turns ON if transmission/reception is complete and OK.
 - LED2 toggles slowly when there is a timeout or an error in transmission/reception process.   

#### <b>Notes</b>

 1. Timeout is set to 5 seconds which means that if no communication occurs during 5 seconds,
    a timeout error is generated.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Polling, Transmission, Reception, Master, Slave, MISO, MOSI

### <b>Directory contents</b> 

  - SPI/SPI_FullDuplex_ComPolling_Slave/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - SPI/SPI_FullDuplex_ComPolling_Slave/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - SPI/SPI_FullDuplex_ComPolling_Slave/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComPolling_Slave/Inc/main.h                  Header for main.c module  
  - SPI/SPI_FullDuplex_ComPolling_Slave/Src/stm32h5xx_it.c          Interrupt handlers
  - SPI/SPI_FullDuplex_ComPolling_Slave/Src/main.c                  Main program
  - SPI/SPI_FullDuplex_ComPolling_Slave/Src/system_stm32h5xx.c      stm32h5xx system source file
  - SPI/SPI_FullDuplex_ComPolling_Slave/Src/stm32h5xx_hal_msp.c     HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up

    - Connect Master board PC0 (pin 38 on CN7 connector) to Slave Board PC0 (pin 38 on CN7 connector)
    - Connect Master board PC2 (pin 35 on CN7 connector) to Slave Board PC2 (pin 35 on CN7 connector)
    - Connect Master board PC3 (pin 37 on CN7 connector) to Slave Board PC3 (pin 37 on CN7 connector)
    - Connect Master board GND  to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files (master project) and load your image into target memory
    - Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    - Load the project in Slave Board
 - Run the example

