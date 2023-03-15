## <b>I3C_Target_I2C_ComDMA Example Description</b>

How to handle I2C data buffer transmission/reception between two boards,
using an interrupt.

      - Board: NUCLEO-H503RB (embeds a STM32H503RB device)
      - SCL Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3)
      - SDA Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.

The project is split in two workspace:
the Controller Board (I3C_Controller_I2C_ComDMA) and the Target Board (I3C_Target_I2C_ComDMA)

- Target Board :
  The HAL_I2C_Slave_Receive_IT() and the HAL_I2C_Slave_Transmit_IT() functions
  allow respectively the reception and the transmission of a predefined data buffer
  in Target mode using interrupt.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I2C Controller starts the communication by sending aTxBuffer
to I2C Target which receives aRxBuffer through HAL_I2C_Slave_Receive_IT().
The second step starts when the user press the USER push-button on the Controller Board,
the I2C Target sends aTxBuffer through HAL_I2C_Slave_Transmit_IT()
to the I2C Controller which receives aRxBuffer.
The end of this two steps are monitored through the HAL_I2C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H503RB's LED can be used to monitor the transfer status:

 - LED2 is ON when the transmission process is complete.
 - LED2 is OFF when the reception process is complete.
 - LED2 is slowly blinking (1 sec. period) in case of error in initialization or
transmission/reception process

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I2C, Interrupt, Controller, Target, Transmission, Reception, Fast mode plus

### <b>Directory contents</b>

  - I2C/I3C_Target_I2C_ComDMA/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - I2C/I3C_Target_I2C_ComDMA/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - I2C/I3C_Target_I2C_ComDMA/Inc/stm32h5xx_it.h          I2C interrupt handlers header file
  - I2C/I3C_Target_I2C_ComDMA/Inc/main.h                  Header for main.c module
  - I2C/I3C_Target_I2C_ComDMA/Src/stm32h5xx_it.c          I2C interrupt handlers
  - I2C/I3C_Target_I2C_ComDMA/Src/main.c                  Main program
  - I2C/I3C_Target_I2C_ComDMA/Src/system_stm32h5xx.c      STM32H5xx system source file
  - I2C/I3C_Target_I2C_ComDMA/Src/stm32h5xx_hal_msp.c     HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RB devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication.
    - Connect I3C_SCL line of Controller board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to I3C_SCL line of Target Board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3).
    - Connect I3C_SDA line of Controller board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to I3C_SDA line of Target Board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5).
    - Connect GND of Controller board to GND of Target Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_I2C_ComDMA)
 - Then rebuild all files and load your image into Target memory (I3C_Target_I2C_ComDMA)
 - Run the Controller before run the Target, this will let a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
