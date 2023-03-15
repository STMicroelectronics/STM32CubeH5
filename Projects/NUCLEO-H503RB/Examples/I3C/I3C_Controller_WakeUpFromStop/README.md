## <b>I3C_Controller_WakeUpFromStop Example Description</b>

How to handle I3C as Controller data buffer transmission/reception between two boards, where Target is in Stop mode,
using interrupt.

      - Board: NUCLEO-H503RB (embeds a STM32H503RB device)
      - SCL Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3)
      - SDA Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit()function which core is implementing
the configuration of the needed I3C resources according to the used hardware (CLOCK, GPIO and NVIC).
User may update this function to change I3C configuration.
To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspace:
the Controller Board (I3C_Controller_WakeUpFromStop) and the Target Board (I3C_Target_WakeUpFromStop)

- Controller Board
  The HAL_I3C_Ctrl_DynAddrAssign_IT() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Targets connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 250 MHz.

  The HAL_I3C_Ctrl_Receive_IT() and the HAL_I3C_Ctrl_Transmit_IT() functions
  allow respectively the reception and the transmission of a predefined data buffer
  in Controller mode using Interrupt at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 250 MHz.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through HAL_I3C_Ctrl_DynAddrAssign_IT().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

In a second step after the user press the USER push-button on the Controller Board,
I3C Controller continue the communication by sending aTxBuffer
through HAL_I3C_Ctrl_Transmit_IT() to I3C Target which is in Stop Mode.
The wake up phase on Target side can take more time than Controller expected.
This mean an error management is added to repeat communication if a non acknowledge error is detected.

Then when Target wake up, it receives aRxBuffer through HAL_I3C_Tgt_Receive_IT().

The third step starts when the user press the USER push-button on the Controller Board,
the I3C Target sends aTxBuffer through HAL_I3C_Tgt_Transmit_IT()
to the I3C Controller which receives aRxBuffer through HAL_I3C_Ctrl_Receive_IT().

The end of this two steps are monitored through the HAL_I3C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H503RB's LEDs can be used to monitor the transfer status:

 - LED2 is ON when the Dynamic address assignment process is complete.
 - LED2 is OFF when the transmission process is complete.
 - LED2 is ON when the reception process is complete.
 - LED2 toggle slowly when there is an error in transmission/reception process.

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Private,
Transmission, Reception, Wakeup

### <b>Directory contents</b>

  - I3C/I3C_Controller_WakeUpFromStop/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_WakeUpFromStop/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Controller_WakeUpFromStop/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Controller_WakeUpFromStop/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_WakeUpFromStop/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_WakeUpFromStop/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_WakeUpFromStop/Src/main.c                    Main program
  - I3C/I3C_Controller_WakeUpFromStop/Src/system_stm32h5xx.c        STM32H5xx system source file
  - I3C/I3C_Controller_WakeUpFromStop/Src/stm32h5xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RB devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to I3C_SCL line of Target Board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3).
    - Connect I3C_SDA line of Controller board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to I3C_SDA line of Target Board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5).
    - Connect GND of Controller board to GND of Target Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_WakeUpFromStop)
 - Then rebuild all files and load your image into Target memory (I3C_Target_WakeUpFromStop)
 - Run the Controller before run the Target, this will let a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
