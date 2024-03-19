## <b>FDCAN_Power_Down Example Description</b>

This example describes the functionality of the power down mode in the FDCAN peripheral.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.

Then the *SystemClock_Config()* function is used to configure the system clock (SYSCLK) to run at 250 MHz.

The example is divided into several sections:<br>

  - In the first section, the *HAL_FDCAN_Init()* function is called to configure the module in external loopback mode,  with a **Nominal Bit Rate of 1 MBit/s** and a **sampling point at 75%**.<br>
    Additionally, it has a **Data Bit Rate of 2 MBit/s** and a **sampling point at 80%**.<br>
    Reception filters are then configured with *HAL_FDCAN_ConfigFilter()*, to receive:<br>
      - messages with pre-defined standard ID to Rx FIFO 0<br>

    Then FDCAN module is started with *HAL_FDCAN_Start()*.<br>
  - In section 2, the following messages is sent:<br>
      - one standard ID message matching Rx FIFO 0 filter<br>
    The application then checks that the message was received and as expected.
  - In section 3, the FDCAN module enters power down mode.
  - In section 4, the application tries to send an FDCAN message, and checks that it remains pending until power down mode is excited.
  - In section 5, the FDCAN clock is disabled. At this point, the energy consumption is a bit reduced as the module is in power down mode.
  - In section 6, the application exits power down mode of the FDCAN module and re-enable it's clock.
  - In section 7, the application checks the message was received and as expected, indicating that the power down mode did not interfere with the FDCAN configuration.

The FDCAN peripheral configuration is ensured by the *HAL_FDCAN_Init()* function.
This later is calling the *HAL_FDCAN_MspInit()* function which core is implementing
the configuration of the needed FDCAN resources according to the used hardware (CLOCK, GPIO, NVIC and DMA).
User may update this function to change FDCAN configuration.


NUCLEO-H563ZI's LEDs can be used to monitor the transfer status:

  - LED1 is ON when all messages were successfully transmitted and received.
  - LED3 toggle slowly when there is an error in transmission/reception process.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, CAN/FDCAN, Loopback, Polling, CAN FIFO, CAN Filter, Power Down

### <b>Directory contents</b>

File | Description
 --- | ---
FDCAN/FDCAN_Power_Down/Inc/stm32h5xx_nucleo_conf.h    | BSP configuration file
FDCAN/FDCAN_Power_Down/Inc/stm32h5xx_hal_conf.h   | HAL configuration file
FDCAN/FDCAN_Power_Down/Inc/stm32h5xx_it.h         | Header for stm32h5xx_it.c
FDCAN/FDCAN_Power_Down/Inc/main.h                 | Header for main.c module
FDCAN/FDCAN_Power_Down/Src/stm32h5xx_it.c         | Interrupt handlers
FDCAN/FDCAN_Power_Down/Src/main.c                 | Main program
FDCAN/FDCAN_Power_Down/Src/stm32h5xx_hal_msp.c    | HAL MSP module
FDCAN/FDCAN_Power_Down/Src/system_stm32h5xx.c     | stm32h5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up:
    No FDCAN connector available on this board. FDCAN RX/TX signals are accessible through Arduino connector.
      - FDCAN RX : PD0
      - FDCAN TX : PD1

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
