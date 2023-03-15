## <b>I3C_Controller_Private_Command_IT Example Description</b>

How to handle I3C as Controller data buffer transmission/reception between two boards, using interrupt.

      - Board: NUCLEO-H563ZI (embeds a STM32H563ZI device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3) to PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5) to PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit()function which core is implementing
the configuration of the needed I3C resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change I3C configuration.
To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspace:
the Controller Board (I3C_Controller_Private_Command_IT) and the Target Board (I3C_Target_Private_Command_IT)

- Controller Board
    The Handle_ENTDAA_Controller() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 250 MHz.

  The MX_I3C1_Init(), the Prepare_Private_Transfer(), and Handle_Private_Controller() functions
  allow respectively the configuration of the different I3C hardware settings like timing, internal hardware FIFOs,
  the preparation of the message buffer which contain the different caracteristics of array aPrivateDescriptor,
  and the start of private message using IT mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 250 MHz.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through Handle_ENTDAA_Controller().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

The second step starts when the user press the USER push-button on the Controller Board,
through Handle_Private_Controller() to I3C Target.

The third step starts when the user press the USER push-button on the Controller Board,
the I3C Target sends aTxBuffer to the I3C Controller which receives aRxBuffer through Handle_Private_Controller().

The end of this two steps are monitored through the Controller_Complete_Callback() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:

 - LED1 is ON when the Dynamic address assignment is complete.
 - LED1 is OFF when the transmission process is complete.
 - LED1 is ON when the reception process is complete.
 - LED1 toggle slowly when there is an error in transmission/reception process.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Private,
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Private_Command_IT/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Private_Command_IT/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Controller_Private_Command_IT/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Controller_Private_Command_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Private_Command_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Private_Command_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Private_Command_IT/Src/main.c                    Main program
  - I3C/I3C_Controller_Private_Command_IT/Src/system_stm32h5xx.c        STM32H5xx system source file
  - I3C/I3C_Controller_Private_Command_IT/Src/stm32h5xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZI devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3) to I3C_SCL line of Target Board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3).
    - Connect I3C_SDA line of Controller board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5) to I3C_SDA line of Target Board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5).
    - Connect GND of Controller board to GND of Target Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Private_Command_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Private_Command_IT)
 - Run the Controller before run the Target, this will let a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
