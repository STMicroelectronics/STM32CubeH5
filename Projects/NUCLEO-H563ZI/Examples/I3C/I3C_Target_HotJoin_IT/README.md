## <b>I3C_Target_HotJoin_IT Example Description</b>
How to handle a HOTJOIN procedure to an I3C Controller

      - Board: NUCLEO-H563ZI's (embeds a STM32H563ZI device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize the
Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 MHz.

The I3C peripheral configuration is ensured by the HAL_I3C_Init() function.
This later is calling the HAL_I3C_MspInit()function which core is implementing the configuration of the needed
I3C resources according to the used hardware (CLOCK, GPIO and NVIC).
User may update this function to change I3C configuration.

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_HotJoin_IT) and the Target Boards (I3C_Target_HotJoin_IT).

- Target Boards
  At first step, the HAL_I3C_Tgt_Config(), the HAL_I3C_SetConfigFifo(), and the HAL_I3C_Tgt_HotJoinReq_IT()
  functions allow respectively the configuration of the Target payload value, the configuration of the internal hardware
  FIFOs and the request of the HotJoin request event in Target mode using Interrupt.

To simulate a different Target with the same project, user can change value of DEVICE_ID, to change the MIPIIdentifier
part of the payload data.
Then download the different project on associated Target board.

Target side:
In a first step, after the user presses the USER push-button on a Target Board, I3C Target starts the communication by
sending the HotJoin request through HAL_I3C_Tgt_HotJoinReq_IT() to the Controller.

This first action can be done independently on one or other Targets.
In fact, after starting HotJoin procedure, the I3C Controller starts the Dynamic Address Assignment procedure to all
Targets.
All Targets without a dynamic address must answer to the Controller by sending their proper Payload, but the ones with
HotJoin capability (sTgtConfig.HotJoinRequest = ENABLE) don't answer to this ENTDAA process.

This means, as example, if we have three Targets without a dynamic address which two of them with HotJoin capability,
only two Targets will respond to the ENTDAA process : the one that made the HotJoin request and the one without HotJoin
capability.

This payload sending procedure is totally managed by internal hardware state machine, only the end of
HotJoin procedure is treated on software part through the callback HAL_I3C_TgtHotJoinCallback().

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is ON when the HotJoin and Dynamic Address Assignment process is complete.
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

  - I3C/I3C_Target_HotJoin_IT/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Target_HotJoin_IT/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Target_HotJoin_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Target_HotJoin_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Target_HotJoin_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Target_HotJoin_IT/Src/main.c                    Main program
  - I3C/I3C_Target_HotJoin_IT/Src/system_stm32h5xx.c        stm32h5xx system source file
  - I3C/I3C_Target_HotJoin_IT/Src/stm32h5xx_hal_msp.c       HAL MSP file

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
