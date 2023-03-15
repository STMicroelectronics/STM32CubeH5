## <b>I2C_TwoBoards_ComDMA Example Description</b>

How to handle I2C data buffer transmission/reception between two boards via DMA.

      - Board   : NUCLEO-H563ZI (embeds a STM32H563ZI device)
      - SCL Pin : PF1 (CN9, pin 19 D69)
      - SDA Pin : PF0 (CN9, pin 21 D68)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.
The project is split in two parts (Master Board and the Slave Board) :

- Master Board :
  The HAL_I2C_Master_Receive_DMA() and the HAL_I2C_Master_Transmit_DMA() functions
  allow respectively the reception and the transmission of a predefined data buffer
  in Master mode using DMA.

- Slave Board :
  The HAL_I2C_Slave_Receive_DMA() and the HAL_I2C_Slave_Transmit_DMA() functions
  allow respectively the reception and the transmission of a predefined data buffer
  in Slave mode using DMA.

The user can choose between Master and Slave through "#define MASTER_BOARD" in the "main.c" file :

  - If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
  - If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Master Board,
I2C Master starts the communication by sending aTxBuffer through HAL_I2C_Master_Transmit_DMA()
to I2C Slave which receives aRxBuffer through HAL_I2C_Slave_Receive_DMA().
The second step starts when the user press the USER push-button on the Master Board,
the I2C Slave sends aTxBuffer through HAL_I2C_Slave_Transmit_DMA()
to the I2C Master which receives aRxBuffer through HAL_I2C_Master_Receive_DMA().
The end of this two steps are monitored through the HAL_I2C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status :

 - LED2 toggles quickly on master board waiting User push-button to be pressed.
 - LED2 turns OFF as soon as the push-button is pressed.
 - LED1 is ON when the Master transmission / Slave reception process is complete.
 - LED2 toggles quickly on master board waiting User push-button to be pressed.
 - LED2 turns OFF as soon as the push-button is pressed.
 - LED1 is OFF when the Master reception / Slave transmission process is complete.
 - LED3 is ON when there is an error in transmission/reception process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I2C, DMA, Master, Slave, Transmission, Reception, Fast mode plus

### <b>Directory contents</b>

  - I2C/I2C_TwoBoards_ComDMA/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - I2C/I2C_TwoBoards_ComDMA/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - I2C/I2C_TwoBoards_ComDMA/Inc/stm32h5xx_it.h          DMA and I2C interrupt handlers header file
  - I2C/I2C_TwoBoards_ComDMA/Inc/main.h                  Header for main.c module
  - I2C/I2C_TwoBoards_ComDMA/Inc/linked_list.h           Linked list header file
  - I2C/I2C_TwoBoards_ComDMA/Src/stm32h5xx_it.c          DMA and I2C interrupt handlers
  - I2C/I2C_TwoBoards_ComDMA/Src/main.c                  Main program
  - I2C/I2C_TwoBoards_ComDMA/Src/system_stm32h5xx.c      STM32H5xx system source file
  - I2C/I2C_TwoBoards_ComDMA/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - I2C/I2C_TwoBoards_ComDMA/Src/linked_list.c           Linked list source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up :
    - Connect I2C_SCL line of Master board (PF1, CN9, pin 19 D69) to I2C_SCL line of Slave Board (PF1, CN9, pin 19 D69).
    - Connect I2C_SDA line of Master board (PF0, CN9, pin 21 D68) to I2C_SDA line of Slave Board (PF0, CN9, pin 21 D68).
    - Connect GND of Master board to GND of Slave Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory :
    - Uncomment "#define MASTER_BOARD" and load the project in Master Board
    - Comment "#define MASTER_BOARD" and load the project in Slave Board
 - Run the example
