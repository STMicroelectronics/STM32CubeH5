## <b>I3C_Controller_WakeUpFromStop Example Description</b>

How to handle I3C as Controller data buffer transmission/reception between a Target in Stop Mode, using interrupt.

      - Board: NUCLEO-H563ZI (embeds a STM32H563ZI device)
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
The project is split in two workspace:
the Controller Board (I3C_Controller_WakeUpFromStop) and the Target Board (I3C_Target_WakeUpFromStop)

- Controller Board
    The Handle_ENTDAA_Controller() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 250 MHz.

  The MX_I3C1_Init(), the Prepare_Private_Transfer(), and Handle_Private_Controller() functions
  allow respectively the configuration of the different I3C hardware settings like timing, internal hardware FIFOs,
  the preparation of the message buffer which contain the different caracteristics of array aPrivateDescriptor,
  and the start of private message using IT mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 250 MHz.

  The Handle_Repeat_Private_Controller() function allow the repetition of a previous transfer in case error detected,
  like a non acknowledge from the Target.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through Handle_ENTDAA_Controller().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

The second step starts when the user press the USER push-button on the Controller Board,
the I3C Controller sends aTxBuffer through Handle_Private_Controller() to I3C Target.

The sending of buffer have effect to wake up the Target. Depends on Target wakeup time,
this communication can be done several time, until Target is waked-up.

The third step starts when the user press the USER push-button on the Controller Board,
the I3C Target sends aTxBuffer to the I3C Controller which receives aRxBuffer through Handle_Private_Controller().

If an error is detected due to wakeup latency on Target side during the TX transfer,
the repetition of the sending of aTxBuffer is manage through Handle_Repeat_Private_Controller().
Then communication can continue with RX transfer step.

The end of this two steps are monitored through the Controller_Complete_Callback() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:

 - LED1 is toggle quickly to wait for USER push-button press.
 - LED1 is ON when the transmission process is complete.
 - LED1 is OFF when the reception process is complete.
 - LED1 toggle slowly when there is an error in transmission/reception process.

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Private,
Transmission, Reception, Low power, Wakeup, Stop mode

### <b>Directory contents</b>

  - I3C/I3C_Controller_WakeUpFromStop/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_WakeUpFromStop/Inc/stm32h5xx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_WakeUpFromStop/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_WakeUpFromStop/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_WakeUpFromStop/Src/stm32h5xx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_WakeUpFromStop/Src/main.c                    Main program
  - I3C/I3C_Controller_WakeUpFromStop/Src/system_stm32h5xx.c        STM32H5xx system source file

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
 - Rebuild all files and load your image into Controller memory (I3C_Controller_WakeUpFromStop)
 - Then rebuild all files and load your image into Target memory (I3C_Target_WakeUpFromStop)
 - Run the Controller before run the Target, this will let a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
