## <b>I3C_Controller_Switch_To_Target Example Description</b>

How to handle a Controller Role Request Direct Command procedure between an I3C Controller and an I3C Target,
using Interrupt.

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
the configuration of the needed I3C resources according to the used hardware (CLOCK, GPIO, NVIC and IT).
User may update this function to change I3C configuration.
To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_Switch_To_Target) and the Target Board (I3C_Target_Switch_To_Controller)

- Controller Board
    The HAL_I3C_SetConfigFifo() and the HAL_I3C_Ctrl_Config()
  allow respectively the configuration of the internal hardware FIFOs and
  the configuration of the Controller Stall and own specific parameter.

    The HAL_I3C_Ctrl_DynAddrAssign_IT() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 250 MHz.

    The HAL_I3C_ActivateNotification() and the HAL_I3C_GetCCCInfo() function allow respectively
  the enabling of Asynchronous catching event in Controller mode using Interrupt and
  to retrieve the Target/Secondary Controller associated Dynamic Address.

    Then current Controller sent to Target the command GETACCCR through HAL_I3C_Ctrl_ReceiveCCC_IT()
  to acknowledge the Controller Role Request.

    After those steps, the Controller have shared the Bus configuration to the new Controller and
  then freed the Bus for the new Controller which can take the control on the Bus.
  The previous Controller goes to Target role.

For this example the aGET_CCCList is predefined related to GETACCCR Common Command Code descriptor treated in this example.

Controller side:
In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through HAL_I3C_Ctrl_DynAddrAssign_IT().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

The Controller assignment procedure is terminated when no additional Target respond
to Dynamic Address Assignment procedure and at reception of HAL_I3C_CtrlDAACpltCallback().
The controller store the Target capabilities in the peripheral hardware register side through HAL_I3C_Ctrl_ConfigBusDevices().

Then Controller wait in a no activity loop.

Then, at reception of a Controller Role event request from a Target, through callback HAL_I3C_NotifyCallback(),
the I3C Controller retrieve Target Dynamic Address through HAL_I3C_GetCCCInfo().

This information will help the I3C Controller to send the GETACCCR command to acknowledge
the procedure through HAL_I3C_Ctrl_ReceiveCCC_IT().

The Controller Role Request procedure is terminated when the communication is completed
by calling the callback HAL_I3C_CtrlRxCpltCallback().
The Dynamic address received (shifted to 1bit, as LSB bit is the parity bit) is then compare to
the Target address which have request the controller role request.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is toggle at ENTDAA reception and Controller Role Request process is completed.
 - LED1 is toggle slowly when there is an error in ENTDAA and Controller Roler Request process.

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Direct Command,
Controller Role Request, Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Switch_To_Target/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Switch_To_Target/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_Switch_To_Target/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Controller_Switch_To_Target/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Controller_Switch_To_Target/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Switch_To_Target/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Switch_To_Target/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Switch_To_Target/Src/main.c                    Main program
  - I3C/I3C_Controller_Switch_To_Target/Src/system_stm32h5xx.c        STM32H5xx system source file
  - I3C/I3C_Controller_Switch_To_Target/Src/stm32h5xx_hal_msp.c       HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZI devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3) to I3C_SCL line of
      Target Board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3).
    - Connect I3C_SDA line of Controller board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5) to I3C_SDA line of
      Target Board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5).
    - Connect GND of Controller board to GND of Target Board.

  - Launch the program in debug mode on Controller board side, and in normal mode on Target side
    to benefit of Terminal I/O information on Controller side.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Switch_To_Target)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Switch_To_Controller)
 - Run the Controller in debug mode before run the Target in normal mode.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
