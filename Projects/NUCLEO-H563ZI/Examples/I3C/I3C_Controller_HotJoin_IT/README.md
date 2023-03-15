## <b>I3C_Controller_HotJoin_IT Example Description</b>
How to handle a HOTJOIN procedure between an I3C Controller and I3C Targets

      - Board: NUCLEO-H563ZI's (embeds a STM32H563ZI device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize
the Flash interface and the systick. Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit()function which core is implementing the configuration of the needed I3C
resources according to the used hardware (CLOCK, GPIO and NVIC).
User may update this function to change I3C configuration.

The I3C communication is then initiated.
The project is split in two workspaces: the Controller Board (I3C_Controller_HotJoin_IT) and the Target
Boards (I3C_Target_HotJoin_IT)

- Controller Board
  At first step, the HAL_I3C_SetConfigFifo(), the HAL_I3C_Ctrl_Config() and the HAL_I3C_ActivateNotification()
  functions allow respectively the configuration of the internal hardware FIFOs, the configuration of the
  Controller Stall and own specific parameter, and the enabling of Asynchronous catching event in Controller
  mode using Interrupt.

  At second step, the HAL_I3C_Ctrl_DynAddrAssign_IT() and the HAL_I3C_Ctrl_SetDynamicAddress() functions allow
  respectively the reception of the Targets payload connected on the Bus and assign a proper Dynamic address
  for each of them using Interrupt mode.

For this example, the TargetDesc1, TargetDesc2 are predefined related to Target descriptor.

Controller side:
In a first step, until the HotJoin procedure is requested from the Target, the Controller waits in a no activity loop.

Then, at reception of a HotJoin event request from a Target, the I3C Controller starts the classic Enter Dynamic Address
procedure by sending the ENTDAA CCC Command through HAL_I3C_Ctrl_DynAddrAssign_IT() to I3C Targets which
receive the Command and treat it by sending their own payload.
The communication run at 12.5Mhz during Push-pull phases based on I3C source clock which is at 250 MHz.

Each time a Target, which has no Dynamic Address associated, sends its complete payload, Controller is informed at
reception of it through HAL_I3C_TgtReqDynamicAddrCallback().
At this step, Controller computes/assigns a Dynamic address to the Target, and sends it to Target
through HAL_I3C_Ctrl_SetDynamicAddress().

Then on hardware side, another frame Dynamic Address Assignment is automatically sent on the I3C Bus to request other
payload of any other Target without a dynamic address connected on the Bus.

This means, as example, if we have three Targets without a dynamic address which two of them with HotJoin capability
(sTgtConfig.HotJoinRequest = ENABLE), only two Targets will respond to the ENTDAA process: the one that made the
HotJoin request and the one without HotJoin capability.

The Controller assignment procedure is terminated when no additional Target respond to Dynamic Address Assignment
procedure and at reception of the callback HAL_I3C_CtrlDAACpltCallback().


Target side:
In a first step after the user presses the USER push-button on a Target Board, I3C Target starts the communication by
sending the HotJoin request to the Controller.
Target then waits for a Dynamic address association from the Controller.

At reception of the Dynamic Address Assignment procedure from the Controller, each Target start the sending of its
payload until it loses the communication. Where a Target is lost, it must quit the communication.

A lost of communication, or better naming arbitration lost, arrived on Target which have lowest number of bits “0” in
their payload.

The end of this Dynamic Address Assignment is monitored by the reception of HAL_I3C_CtrlDAACpltCallback() on
Controller side.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is toggle each time an HotJoin process is completed
 - LED1 is toggle slowly when there is an error in HotJoin or Dynamic Address Assignment process.

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, HotJoin, Transmission,
Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_HotJoin_IT/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_HotJoin_IT/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_HotJoin_IT/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Controller_HotJoin_IT/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Controller_HotJoin_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_HotJoin_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_HotJoin_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_HotJoin_IT/Src/main.c                    Main program
  - I3C/I3C_Controller_HotJoin_IT/Src/system_stm32h5xx.c        stm32h5xx system source file
  - I3C/I3C_Controller_HotJoin_IT/Src/stm32h5xx_hal_msp.c       HAL MSP file

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
    - Connect GND of Controller board to GND of Target Boards.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_HotJoin_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_HotJoin_IT)
 - Run the Controller before run the Target.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
