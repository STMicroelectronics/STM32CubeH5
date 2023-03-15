## <b>I3C_Controller_HotJoin_IT Example Description</b>
How to handle a HOTJOIN procedure between an I3C Controller and I3C Targets

      - Board: NUCLEO-H503RB's (embeds a STM32H503RB device)
      - SCL Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3) to PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3)
      - SDA Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5) to PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration and I3C resources (CLOCK, GPIO and NVIC) are ensured by the MX_I3C1_Init() function.
User may update this function to change I3C configuration.

To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspaces: the Controller Board (I3C_Controller_HotJoin_IT) and the Target
Boards (I3C_Target_HotJoin_IT)

- Controller Board
  At first step, the MX_I3C1_Init() function allow the configuration of the internal hardware FIFOs,
  the configuration of the Controller Stall and own specific parameter, and the enabling of Asynchronous
  catching event in Controller mode using Interrupt.

  At second step, the Handle_ENTDAA_Controller() and the Target_Request_DynamicAddrCallback() functions allow
  respectively the reception of the Targets payload connected on the Bus and assign a proper Dynamic address
  for each of them using Interrupt mode.

For this example, the TargetDesc1, TargetDesc2 are predefined related to Target descriptor.

Controller side:
In a first step, until the HotJoin procedure is requested from the Target, the Controller waits in a no activity loop.

Then, at reception of a HotJoin event request from a Target, the I3C Controller starts the classic Enter Dynamic Address
procedure by sending the ENTDAA CCC Command through Handle_ENTDAA_Controller() to I3C Targets which
receive the Command and treat it by sending their own payload.
The communication run at 12.5Mhz during Push-pull phases based on I3C source clock which is at 250 MHz.

Each time a Target, which has no Dynamic Address associated, sends its complete payload, Controller is informed at
reception of it through Controller_Transmit_Callback() and I3C_DynamicAddressTreatment().
At this step, Controller computes/assigns a Dynamic address to the Target, and sends it to Target
through Target_Request_DynamicAddrCallback().

Then on hardware side, another frame Dynamic Address Assignment is automatically sent on the I3C Bus to request other
payload of any other Target without a dynamic address connected on the Bus.

This means, as example, if we have three Targets without a dynamic address which two of them with HotJoin capability
(LL_I3C_EnableHotJoin()), only two Targets will respond to the ENTDAA process: the one that made the
HotJoin request and the one without HotJoin capability.

The Controller assignment procedure is terminated when no additional Target respond to Dynamic Address Assignment
procedure and at reception of the callback Controller_Complete_Callback().


Target side:
In a first step after the user presses the USER push-button on a Target Board, I3C Target starts the communication by
sending the HotJoin request to the Controller.
Then waits for a Dynamic address association from the Controller.

At reception of the Dynamic Address Assignment procedure from the Controller, each Target start the sending of its
payload until it loses the communication. Where a Target is lost, it must quit the communication.

A lost of communication, or better naming arbitration lost, arrived on Target which have lowest number of bits “0” in
their payload.

The end of this Dynamic Address Assignment is monitored by the reception of HAL_I3C_CtrlDAACpltCallback() on
Controller side.

NUCLEO-H503RB's LEDs can be used to monitor the transfer status:
 - LED2 is toggle quickly to wait for USER push-button press.
 - LED2 is toggle each time an HotJoin process is completed
 - LED2 is toggle slowly when there is an error in HotJoin or Dynamic Address Assignment process.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, HotJoin, Transmission,
Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_HotJoin_IT/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_HotJoin_IT/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_HotJoin_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_HotJoin_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_HotJoin_IT/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_HotJoin_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_HotJoin_IT/Src/main.c                    Main program
  - I3C/I3C_Controller_HotJoin_IT/Src/system_stm32h5xx.c        stm32h5xx system source file

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
    - Connect GND of Controller board to GND of Target Boards.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_HotJoin_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_HotJoin_IT)
 - Run the Controller before run the Target.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
