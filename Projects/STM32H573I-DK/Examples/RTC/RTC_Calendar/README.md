## <b>RTC_Calendar Example Description</b>

Configuration of the calendar using the RTC HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

- LSE oscillator clock is used as RTC clock source. 
    The user can use also LSE as RTC clock source.
    - The user uncomment the adequate line on the main.h file.

      @code

        #define RTC_CLOCK_SOURCE_LSI  

        /* #define RTC_CLOCK_SOURCE_LSE */

      @endcode

    - Open the ioc file with STM32CubeMX and select :
      LSE as "Crystal/Ceramic Resonator" in RCC configuration.
      LSE as RTC clock source in Clock configuration.
    - Generate code
    LSI oscillator clock is delivered by a 32 kHz.
    LSE (when available on board) is delivered by a 32.768 kHz crystal.

HAL_RTC_SetTime()and HAL_RTC_SetDate() functions are then called to initialize the 
time and the date.

A key value is written in backup data register 1 to indicate if the RTC is already configured.  
The program behaves as follows:

1. After startup the program checks the Initialization status flag (INITS). This bit is
   set by hardware when the calendar year field is different from 0
   (Backup domain reset state).

    - INITS is reset : Calendar has not been initialized, the RTC is
      configured and internal time stamp is enabled.
    
    - INITS is set: Calendar has been initialized and you can watch 
      the time and the date on the debugger watch window.

2. When a reset (except power on reset) occurs the BKP domain is not reset and the RTC 
   configuration is not lost. LED1 is ON.
   
3. When power on reset occurs:

    - The BKP domain is reset and the RTC configuration is lost.
    
    **Note:** During first debugger re-run, the power on reset already occurred just before 
    (at switch ON) then RTC configuration is not lost and LED1 is ON.

LED1 is turned ON when the RTC configuration is done correctly.

The current time and date are updated and displayed on the debugger in aShowTime 
and aShowDate variables.

#### <b>Notes</b>
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, RTC, Calendar, Backup Domain, Reset

### <b>Directory contents</b>

  - RTC/RTC_Calendar/Inc/stm32h573i_discovery_conf.h    BSP configuration file
  - RTC/RTC_Calendar/Inc/stm32h5xx_hal_conf.h    		HAL configuration file
  - RTC/RTC_Calendar/Inc/stm32h5xx_it.h          		Interrupt handlers header file
  - RTC/RTC_Calendar/Inc/main.h                  		Header for main.c module  
  - RTC/RTC_Calendar/Src/stm32h5xx_it.c          		Interrupt handlers
  - RTC/RTC_Calendar/Src/main.c                  		Main program
  - RTC/RTC_Calendar/Src/stm32h5xx_hal_msp.c     		HAL MSP module
  - RTC/RTC_Calendar/Src/system_stm32h5xx.c      		STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ Devices.
  - This example has been tested with STMicroelectronics STM32H573I-DK
    board and can be easily tailored to any other supported device and 
    development board. 

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

