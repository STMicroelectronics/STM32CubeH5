## <b>I2C_TwoBoards_MultiMasterIT_Slave Example Description</b>
How to handle I2C data buffer communication between two boards, using an interrupt and two Masters and one Slave.

      - Board: NUCLEO-H5E5ZJ's (embeds a STM32H5F5ZJ device)
      - SCL Master1 Pin: PB6 (Morpho CN12 pin 3)
      - SDA Master1 Pin: PB7 (Morpho CN12 pin 1)
      - SCL Master2 Pin: PF5 (Arduino D15 CN5 pin 10, Morpho CN10 pin 3)
      - SDA Master2 Pin: PF15 (Morpho CN7 pin 4)
      - SCL Slave   Pin: PF5 (Arduino D15 CN5 pin 10, Morpho CN10 pin 3)
      - SDA Slave   Pin: PF15 (Morpho CN7 pin 4)
      Connect GND between each board.

   _________________________                       _________________________
  |           ______________|                     |______________           |
  |          |I2C1          |                     |          I2C4|          |
  |          |              |                     |              |          |
  |          |    MST1  SCL |_____________________| SCL          |          |
  |          |              |         |           |              |          |
  |          |              |         |           |              |          |
  |          |              |         |           |              |          |
  |          |    MST1  SDA |_________|___________| SDA          |          |
  |          |              |         |   |       |              |          |
  |          |______________|         |   |       |______________|          |
  |                         |         |   |       |                         |
  |                      GND|_________|___|_______|GND                      |
  |                         |         |   |       |_STM32_Board 2___________|
  |           ______________|         |   |
  |          |I2C4          |         |   |
  |          |              |         |   |
  |          |    MST2  SCL |_________|   |
  |          |              |             |
  |          |              |             |
  |          |              |             |
  |          |    MST2  SDA |_____________|
  |          |              |
  |          |______________|
  |                         |
  |                      GND|
  |_STM32_Board 1___________|
  
At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize the
Flash interface and the systick. Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function. This later is calling the HAL_I2C_MspInit()
function which core is implementing the configuration of the needed I2C resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change I2C configuration.

In a first step after initialization of I2C Master1, Master2 and Timer.
Each time the timer generate an interrupt, the associated callback HAL_TIM_PeriodElapsedCallback() is called and a
Master2 starts the communication by sending aTxBufferMaster2 through HAL_I2C_Master_Transmit_IT() to I2C Slave which
receives aRxBuffer through HAL_I2C_Slave_Receive_IT().

Then when user press the User push-button on the Master Board, I2C Master1 starts the communication by sending
aTxBufferMaster1 through HAL_I2C_Master_Transmit_IT() to I2C Slave which receives aRxBuffer through
HAL_I2C_Slave_Receive_IT().

Each time the slave retrieve a buffer, associated variable are incremented.
User can easily add to live watch the variable nbMst1RxCplt and nbMst2RxCplt to catch the Multimaster buffer reception
on Slave side.

NUCLEO-H5E5ZJ's LED can be used to monitor the transfer status:
 - LED1 toggle each time the timer period elapsed.
 - LED1 is slowly blinking (1 sec. period) in case of error in initialization or transmission process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>
I2C, Master, Slave, TIM, Multimaster, Communication, Interrupt, Transmission, Reception

### <b>Directory contents</b>
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Inc/stm32h5xx_it.h            PSSI interrupt handlers header file
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Inc/main.h                    Header for main.c module
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Src/stm32h5xx_it.c            PSSI interrupt handlers
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Src/main.c                    Main program
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Src/system_stm32h5xx.c        stm32h5xx system source file
  - I2C/I2C_TwoBoards_MultiMasterIT_Slave/Src/stm32h5xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5ZJ devices.

  - This example has been tested with NUCLEO-H5E5ZJ board and can be easily tailored to any other supported device and
    development board.

  - NUCLEO-H5E5ZJ Set-up
    - Connect Master board PB6 (Morpho CN12 pin 3) to Slave Board PF5
      (Arduino D15 CN5 pin 10, Morpho CN10 pin 3)
    - Connect Master board PB7 (Morpho CN12 pin 1) to Slave Board PF15
      (Morpho CN7 pin 4)
    - Connect Master board PF5 (Arduino D15 CN5 pin 10, Morpho CN10 pin 3) to Slave Board PF5
      (Arduino D15 CN5 pin 1, Morpho CN10 pin 3)
    - Connect Master board PF15 (Morpho CN7 pin 4) to Slave Board PF15
      (Morpho CN7 pin 4)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
