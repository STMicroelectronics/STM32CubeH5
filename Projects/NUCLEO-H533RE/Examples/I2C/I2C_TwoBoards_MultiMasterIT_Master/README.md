## <b>I2C_TwoBoards_MultiMasterIT_Master Example Description</b>
How to handle I2C data buffer communication between two boards, using an interrupt and two Masters and one Slave.

      - Board: NUCLEO-H533RE's (embeds a STM32H533xx device)
      - SCL Master1 Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin 3)
      - SDA Master1 Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin 5)
      - SCL Master2 Pin: PB10 (Arduino D6 CN9 pin 7, Morpho CN10 pin 25)
      - SDA Master2 Pin: PB3 (Arduino D4 CN9 pin 4, Morpho CN10 pin 31)
      - SCL Slave   Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin 3)
      - SDA Slave   Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin 5)
      Connect GND between each board.

   _________________________                       _________________________
  |           ______________|                     |______________           |
  |          |I2C1          |                     |          I2C1|          |
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
  |          |I2C2          |         |   |
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

The TIM peripheral configuration is ensured by the HAL_TIM_Init() function. This later is calling the HAL_TIM_MspInit()
function which core is implementing the configuration of the needed TIM resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change TIM configuration.

The TIM peripheral is used to simulate a RTOS scheduling, between Master1 manage through main() function and Master2
through HAL_TIM_PeriodElapsedCallback(). Every millisecond, the timer will send an interrupt to give the Master2 the
upperhand for the communication with the Slave.

For this example the aTxBufferMaster1 and aTxBufferMaster2 are predefined with different text value which can easily
identified through the Terminal IO of the Slave.

In a first step after initialization of I2C Master1, Master2 and Timer.
Each time the timer generate an interrupt, the associated callback HAL_TIM_PeriodElapsedCallback() is called
and a Master2 starts the communication by sending aTxBufferMaster2 through HAL_I2C_Master_Transmit_IT()
to I2C Slave which receives aRxBuffer through HAL_I2C_Slave_Receive_IT().

Then when user press the User push-button on the Master Board, I2C Master1 starts the communication by sending
aTxBufferMaster1 through HAL_I2C_Master_Transmit_IT() to I2C Slave which receives aRxBuffer through
HAL_I2C_Slave_Receive_IT().

Each time the slave retrieve a buffer, associated variable are incremented.
User can easily add to live watch the variable nbMst1RxCplt and nbMst2RxCplt to catch the Multimaster buffer reception
on Slave side.

And user can easily add to live watch the variable Master1BusBusyCpt and Master2BusBusyCpt to catch the Multimaster
bandwidth detection on Master side.

NUCLEO-H533RE's LED can be used to monitor the transfer status:
 - LED2 toggle each time the timer period elapsed.
 - LED2 is slowly blinking (1 sec. period) in case of error in initialization or transmission process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL
      operation.

### <b>Keywords</b>
I2C, Master, Slave, TIM, Multimaster, Communication, Interrupt, Transmission, Reception

### <b>Directory contents</b>
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Inc/stm32h5xx_it.h          I2C interrupt handlers header file
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Inc/main.h                  Main program header file
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Src/stm32h5xx_it.c          I2C interrupt handlers
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Src/main.c                  Main program
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Src/system_stm32h5xx.c      STM32H5xx system clock configuration file
  - I2C/I2C_TwoBoards_MultiMasterIT_Master/Src/stm32h5xx_hal_msp.c     HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H533xx devices.

  - This example has been tested with NUCLEO-H533RE board and can be easily tailored to any other supported device and
    development board.

  - NUCLEO-H533RE Set-up
    - Connect Master board PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin 3) to Slave Board PB6
      (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin 3)
    - Connect Master board PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin 5) to Slave Board PB7
      (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin 5)
    - Connect Master board PB10 (Arduino D6 CN9 pin 7, Morpho CN10 pin 25) to Slave Board PB6
      (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin 3)
    - Connect Master board PB3 (Arduino D4 CN9 pin 4, Morpho CN10 pin 31) to Slave Board PB7
      (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin 5)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
