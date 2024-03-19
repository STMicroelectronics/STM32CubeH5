## <b>I2C_Sensor_Private_Command_IT Example Description</b>

How to handle I2C data buffer transmission/reception between STM32H5xx Nucleo and X-NUCLEO-IKS4A1,
using an interrupt.

Goal of this example is to demonstrate a simple communication with the sensor LSM6DSV16X present on X-NUCLEO-IKS4A1.

       Board: NUCLEO-H533RE (embeds a STM32H533xx device)
       - SCL Pin: PB6 (Arduino SCL/D15 CN5 pin 10, Morpho CN10 pin3)
       - SDA Pin: PB7 (Arduino SDA/D14 CN5 pin 9, Morpho CN10 pin5)
       - Plug a X-NUCLEO-IKS4A1 (https://www.st.com/en/ecosystems/x-nucleo-iks4a1.html) expansion board
         on arduino connector


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK,
GPIO and NVIC). You may update this function to change I2C configuration.

In a first communication step, the I2C master initiate the communication by writing in CTRL1_XL_Register address the
aLSM6DSV16X_Config through HAL_I2C_Mem_Write_IT() function to config the LSM6DSV16X.

The end of this step is monitored through the HAL_I3C_GetState() function result.

Then master waiting user action.
The user press the USER push-button on the master Board.

Then, in an infinite loop, the I2C master intiate sensor communication to retrieve the LSM6DSV16X Temperature,
Gyroscope and Accelerometer data.

The HAL_I2C_Mem_Read_IT() function allow the master to send register address and receive an amount of data
from the LSM6DSV16X Temperature, Gyroscope and Accelerometer.

The end of this step are monitored through the HAL_I3C_GetState() function result.

At this step, Controller compute the data receive through aRxBuffer and
print it through Terminal I/O through DisplayValue() at each display refresh time.


NUCLEO-H533RE's LED can be used to monitor the transfer status:

 - LED2 is ON when the transmission process is complete.
 - LED2 is OFF when the reception process is complete.
 - LED2 toggle slowly when there is an error in transmission/reception process.

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.
Depending of IDE, to watch content of Terminal I/O note that
 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In Debug configuration :
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup,add the command "monitor arm semihosting enable"

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, Communication, I2C, Interrupt, Master, Slave, Transmission, Reception, Fast mode plus, Sensor, LSM6DSV16X

### <b>Directory contents</b>

  - I2C/I2C_Sensor_Private_Command_IT/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_Sensor_Private_Command_IT/Inc/stm32h5xx_hal_conf.h        HAL configuration file
  - I2C/I2C_Sensor_Private_Command_IT/Inc/stm32h5xx_it.h              I2C interrupt handlers header file
  - I2C/I2C_Sensor_Private_Command_IT/Inc/main.h                      Header for main.c module
  - I2C/I2C_Sensor_Private_Command_IT/Src/stm32h5xx_it.c              I2C interrupt handlers
  - I2C/I2C_Sensor_Private_Command_IT/Src/main.c                      Main program
  - I2C/I2C_Sensor_Private_Command_IT/Src/system_stm32h5xx.c          STM32H5xx system source file
  - I2C/I2C_Sensor_Private_Command_IT/Src/stm32h5xx_hal_msp.c         HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H533xx devices.

  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H533RE Set-up


- Plug a X-NUCLEO-IKS4A1 (https://www.st.com/en/ecosystems/x-nucleo-iks4a1.html) expansion board
      on arduino connector.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
