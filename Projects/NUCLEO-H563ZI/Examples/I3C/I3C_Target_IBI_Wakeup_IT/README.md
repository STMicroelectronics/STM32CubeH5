## <b>I3C_Target_IBI_Wakeup_IT Example Description</b>
How to handle a In Band Interrupt procedure to an I3C Controller in Stop Mode

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
the Controller Board (I3C_Controller_IBI_Wakeup_IT) and the Target Boards (I3C_Target_IBI_Wakeup_IT).

- Target Boards
  The HAL_I3C_Tgt_Config(), the HAL_I3C_SetConfigFifo(), and the HAL_I3C_ActivateNotification() functions
  allow respectively the configuration of the Target descriptor,
  the configuration of the internal hardware FIFOs and the enabling of Asynchronous catching event in Target mode using Interrupt.

  The Controller send data at 12.5Mhz during Push-pull phase.

  The HAL_I3C_Tgt_IBIReq_IT() functions allow the request of the In-Band-Interrupt request in Target mode using Interrupt.

Target side:
In a first step until Dynamic Address Assignment procedure from the Controller, the Target wait in a no activity loop.
Then, at reception of the Dynamic Address Assignment procedure from the Controller, the Target start the sending of its payload.
The end of reception of a Dynamic address is monitored by the reception of HAL_I3C_NotifyCallback() on Target side.

In a second step, the user press the USER push-button on a Target Board,
I3C Target starts the communication by sending the In-Band-Interrupt request through HAL_I3C_Tgt_IBIReq_IT()
to the Controller which is in Low power mode.

For this specific example, the In Band Interrupt have not payload. As on STM32H5xx product,
only In Band Interrupt without payload is supported for wakeup from low power mode.

This second step can be done at the same time or independently on one or other Targets.
In fact, after this starting In-Band-Interrupt procedure, the I3C Controller catch the event and request
a private communication with the Target which have send and have get acknowledge of the In-Band-Interrupt event.

The end of IBI communication is monitored by the reception of HAL_I3C_NotifyCallback() on Target side.

To simulate a different Target with the same project, user can change value of DEVICE_ID, to change the MIPIIdentifier
part of the payload data.
Then download the different project on associated Target board.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:
 - LED1 is ON when Dynamic Address Assignment process is completed.
 - LED1 is OFF when the In Band Interrupt is completed.
 - LED1 is toggle slowly when there is an error in In Band Interrupt or Dynamic Address Assignment process.

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, In Band Interrupt, Transmission,
Reception, Low Power Mode, Wakeup

### <b>Directory contents</b>

  - I3C/I3C_Target_IBI_Wakeup_IT/Inc/stm32h5xx_nucleo_conf.h   BSP configuration file
  - I3C/I3C_Target_IBI_Wakeup_IT/Inc/stm32h5xx_hal_conf.h      HAL configuration file
  - I3C/I3C_Target_IBI_Wakeup_IT/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Target_IBI_Wakeup_IT/Inc/main.h                    Header for main.c module
  - I3C/I3C_Target_IBI_Wakeup_IT/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Target_IBI_Wakeup_IT/Src/main.c                    Main program
  - I3C/I3C_Target_IBI_Wakeup_IT/Src/system_stm32h5xx.c        stm32h5xx system source file
  - I3C/I3C_Target_IBI_Wakeup_IT/Src/stm32h5xx_hal_msp.c       HAL MSP file

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
 - Rebuild all files and load your image into Controller memory (I3C_Controller_IBI_Wakeup_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_IBI_Wakeup_IT)
 - Run the Controller before run the Target.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
