## <b>I3C_Controller_Direct_Command_Polling Example Description</b>

How to handle a Direct Command procedure between an I3C Controller and an I3C Target,
using DMA.

      - Board: NUCLEO-H503RB (embeds a STM32H503RB device)
      - SCL Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3)
      - SDA Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration and I3C resources (CLOCK and GPIO) are ensured by the MX_I3C1_Init() function.
User may update this function to change I3C configuration.

To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_Direct_Command_Polling) and the Target Board (I3C_Target_Direct_Command_Polling)

- Controller Board
    The Handle_ENTDAA_Controller() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 250 MHz.

  The MX_I3C1_Init(), the Prepare_GET_CCC_Transfer(), Prepare_SET_CCC_Transfer and the Handle_CCC_Controller() functions
  allow respectively the configuration of the different I3C hardware settings like timing, internal hardware FIFOs,
  the preparation of the message buffer which contain the different caracteristics of array aGET_CCCList,
  the preparation of the message buffer which contain the different caracteristics of array aSET_CCCList,
  and the treatment of associated Command value using Polling mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 250 MHz.

For this example the aSET_CCCList and aGET_CCCList are predefined related to Common Command Code descriptor
treated in this example.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through Handle_ENTDAA_Controller().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of aTargetDesc
in the field TARGET_BCR_DCR_PID.

Then controller waiting user action.

The user press the USER push-button on the Controller Board to start the communication by sending the first
then all Get CCC element of the aGET_CCCList through Handle_CCC_Controller()
to I3C Targets which receive the Command and treat it by sending the associated data.

At this step, Controller compute the data receive through aRxBuffer and print it through Terminal I/O through DisplayCCCValue().

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.
Depending of IDE, to watch content of Terminal I/O note that
 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In Debug configuration : 
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup,add the command "monitor arm semihosting enable"

At next USER push-button press, the Controller switch to sending the first then all Set CCC element of the aSet_CCCList
through Handle_CCC_Controller() to I3C Target which receive the Command and treat it by receiving the associated data.

NUCLEO-H503RB's LEDs can be used to monitor the transfer status:
 - LED2 is toggle quickly to wait for USER push-button press.
 - LED2 is toggle at ENTDAA reception and each time the Command Code process is completed.
 - LED2 is toggle slowly when there is an error in Command Code process.  

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.
Depending of IDE, to watch content of Terminal I/O note that
 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In Debug configuration : 
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup,add the command "monitor arm semihosting enable"

### <b>Keywords</b>

Connectivity, Communication, I3C, Polling, Controller, Target, Dynamic Address Assignment, Direct Command,
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Direct_Command_Polling/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Direct_Command_Polling/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_Direct_Command_Polling/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Direct_Command_Polling/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Direct_Command_Polling/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_Direct_Command_Polling/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Direct_Command_Polling/Src/main.c                    Main program
  - I3C/I3C_Controller_Direct_Command_Polling/Src/system_stm32h5xx.c        STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RB devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to I3C_SCL line of
      Target Board (PB6 Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3).
    - Connect I3C_SDA line of Controller board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to I3C_SDA line of
      Target Board (PB7 Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5).
    - Connect GND of Controller board to GND of Target Board.

  - Launch the program in debug mode on Controller board side, and in normal mode on Target side
    to benefit of Terminal I/O information on Controller side.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Direct_Command_Polling)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Direct_Command_Polling)
 - Run the Controller in debug mode before run the Target in normal mode.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
