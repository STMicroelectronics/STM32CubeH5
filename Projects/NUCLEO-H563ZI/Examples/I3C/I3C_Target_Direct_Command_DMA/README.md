## <b>I3C_Target_Direct_Command_DMA Example Description</b>

How to handle a Direct Command procedure between an I3C Controller and an I3C Target,
using Controller in DMA.

      - Board: NUCLEO-H563ZI's (embeds a STM32H563ZI device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN6 pin 2, Morpho CN5 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN6 pin 4, Morpho CN5 pin5)
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

The I3C communication is then initiated.
The project is split in two workspaces:
the Controller Board (I3C_Controller_Direct_Command_DMA) and the Target Board (I3C_Target_Direct_Command_DMA)

- Target Board
  The HAL_I3C_ActivateNotification() function allow the Target application to be inform when the Controller
  assign a Dynamic Address, Get or Set CCC command code.

  The HAL_I3C_Tgt_Config(), the HAL_I3C_SetConfigFifo(), and the HAL_I3C_ActivateNotification() functions
  allow respectively the configuration of the Target descriptor,
  the configuration of the internal hardware FIFOs and the enabling of Asynchronous catching event in Target mode using Interrupt.

  The Controller send data at 12.5Mhz during Push-pull phase.


In a first step the Target wait the assignation of a proper associated Dynamic Address from the Controller.

In a second step, until Common Command Code procedure from the Controller,
the Target wait in a no activity loop.
Then, at reception of the first Common Code procedure from the Controller,
the Target start the sending or receiving of the CCC additional data depending of Common Command Code.

This sending or receiving of additional data is totally manage by internal hardware state machine,
only end of CCC procedure is treated on software part through the callback HAL_I3C_NotifyCallback().

Each time a Set CCC command is confirmed treated through HAL_I3C_NotifyCallback(),
the associated data is compared with the expected value.

Each time a Get or Set CCC command is confirmed treated through HAL_I3C_NotifyCallback(), the LED1 is toggle.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is toggle each time a Command Code process is treated on hardware side.
 - LED1 is toggle slowly when there is an error in Command Code process.  

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, DMA, Controller, Target, Dynamic Address Assignment, Direct Command,
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Target_Direct_Command_DMA/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Target_Direct_Command_DMA/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Target_Direct_Command_DMA/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Target_Direct_Command_DMA/Inc/main.h                    Header for main.c module
  - I3C/I3C_Target_Direct_Command_DMA/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Target_Direct_Command_DMA/Src/main.c                    Main program
  - I3C/I3C_Target_Direct_Command_DMA/Src/system_stm32h5xx.c        STM32H5xx system source file
  - I3C/I3C_Target_Direct_Command_DMA/Src/stm32h5xx_hal_msp.c       HAL MSP file

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

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Direct_Command_DMA)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Direct_Command_DMA)
 - Run the Controller in debug mode before run the Target in normal mode.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
