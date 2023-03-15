## <b>I3C_Controller_InBandInterrupt_IT Example Description</b>
How to handle an In-Band-Interrupt event between an I3C Controller and I3C Targets

      - Board: NUCLEO-H563ZI's (embeds a STM32H563ZI device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3) to PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5) to PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5)
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

The project is split in two workspaces:
the Controller Board (I3C_Controller_InBandInterrupt_IT) and the Target Boards (I3C_Target_InBandInterrupt_IT)

- Controller Board
  At first step, the MX_I3C1_Init() function allow the configuration of the internal hardware FIFOs,
  the configuration of the Controller Stall and own specific parameter, and the enabling of Asynchronous
  catching event in Controller mode using Interrupt.

  At second step, the Handle_ENTDAA_Controller() and the Target_Request_DynamicAddrCallback() functions allow
  respectively the reception of the Targets payload connected on the Bus and assign a proper Dynamic address
  for each of them using Interrupt mode.

For this example, the TargetDesc1, TargetDesc2 are predefined related to Target descriptor.

Controller side:
In a first step, after the user press the USER push-button on the Controller Board,
the Controller initiate the sending of the ENTDAA CCC Command through Handle_ENTDAA_Controller()
to I3C Targets which receive the Command and treat it by sending its own payload.

Then when ENTDAA is terminated thanks to reception of the completion callback Controller_Complete_Callback(),
the controller store the Target capabilities in the peripheral hardware register side
through LL_I3C_ConfigDeviceCapabilities().

Then Controller wait in a no activity loop.

Then, at reception of an In-Band-Interrupt event request from a Target,
the I3C Controller retrieve Target Dynamic Address with associated data if any through LL_I3C_GetIBIPayload(),
LL_I3C_GetNbIBIAddData() and LL_I3C_GetIBITargetAddr().

User can verify through debug the payload value by watch the content of uwNbIBITargetAddr, uwIBIPayload
and uwNbIBIData.

Those information will help the Controller to starts a private communication or a Direct communication
to I3C Target which sent the In-Band-Interrupt.

The Controller In-Band-Interrupt procedure is terminated when the IBI event treatment is completed
by calling the callback Controller_Notification_Callback().

The whole IBI process is placed in an infinite loop for the Controller to be able to receive
and treat any new IBI request from the Target.

Target side:
In a first step after retrieve a Dynamic address,
the user press the USER push-button on a Target Board,
I3C Target starts the communication by sending the In-Band-Interrupt request to the Controller.

This first action on Target side can be done at the same time or independently on one or other Targets.
In fact, after this starting In-Band-Interrupt procedure, the I3C Controller catch the event and
the associated information like IBI additional data.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is toggle quickly to wait for USER push-button press.
 - LED1 is toggle each time an In-Band-Interrupt process is completed.
 - LED1 is toggle slowly when there is an error in In-Band-Interrupt process. .

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, In Band Interrupt

### <b>Directory contents</b>

  - I3C/I3C_Controller_InBandInterrupt_IT/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_InBandInterrupt_IT/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_InBandInterrupt_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_InBandInterrupt_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_InBandInterrupt_IT/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_InBandInterrupt_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_InBandInterrupt_IT/Src/main.c                    Main program
  - I3C/I3C_Controller_InBandInterrupt_IT/Src/system_stm32h5xx.c        stm32h5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZI devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3) to I3C_SCL line of Target Board (PB8 Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3).
    - Connect I3C_SDA line of Controller board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5) to I3C_SDA line of Target Board (PB9 Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5).
    - Connect GND of Controller board to GND of Target Boards.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_InBandInterrupt_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_InBandInterrupt_IT)
 - Run the Controller before run the Target.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
