## <b>I3C_Target_HotJoin_IT Example Description</b>
How to handle a HOTJOIN procedure to an I3C Controller

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

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_HotJoin_IT) and the Target Boards (I3C_Target_HotJoin_IT).

- Target Boards
  The MX_I3C1_Init() function allow the configuration of the internal hardware FIFOs,
  the configuration of the Target own specific parameter, and the enabling of Asynchronous
  catching event in Target mode using Interrupt.
  The configuration of Interrupt allow the Target application to be inform when the Controller
  assign a Dynamic Address.

  At first step, the Handle_HOTJOIN_Target() function allow the request of the HotJoin request event
  in Target mode using Interrupt.

To simulate a different Target with the same project, user can change value of DEVICE_ID, to change the MIPIIdentifier
part of the payload data.
Then download the different project on associated Target board.

Target side:
In a first step, after the user presses the USER push-button on a Target Board, I3C Target starts the communication by
sending the HotJoin request through Handle_HOTJOIN_Target() to the Controller.

This first action can be done independently on one or other Targets.
In fact, after starting HotJoin procedure, the I3C Controller starts the Dynamic Address Assignment procedure to all
Targets.
All Targets without a dynamic address must answer to the Controller by sending their proper Payload, but the ones with
HotJoin capability (LL_I3C_EnableHotJoin()) don't answer to this ENTDAA process.

This means, as example, if we have three Targets without a dynamic address which two of them with HotJoin capability,
only two Targets will respond to the ENTDAA process : the one that made the HotJoin request and the one without HotJoin
capability.

This payload sending procedure is totally managed by internal hardware state machine, only the end of
HotJoin procedure is treated on software part through the callback Target_Notification_Callback().

NUCLEO-H503RB's LEDs can be used to monitor the transfer status:
 - LED2 is toggle quickly to wait for USER push-button press.
 - LED2 is ON when the HotJoin and Dynamic Address Assignment process is complete.
 - LED2 is toggle slowly when there is an error in HotJoin or Dynamic Address Assignment process.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, HotJoin, Transmission,
Reception

### <b>Directory contents</b>

  - I3C/I3C_Target_HotJoin_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Target_HotJoin_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Target_HotJoin_IT/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Target_HotJoin_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Target_HotJoin_IT/Src/main.c                    Main program
  - I3C/I3C_Target_HotJoin_IT/Src/system_stm32h5xx.c        stm32h5xx system source file

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
