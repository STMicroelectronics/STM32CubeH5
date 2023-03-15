## <b>RCC_CRS_Synchronization_IT Example Description</b>

Configuration of the clock recovery system (CRS) in Interrupt mode, using the RCC/CRS HAL APIs.

At the beginning of the main program the HAL_Init() function is called to reset all the peripherals, initialize the Flash interface and the systick. Then the SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 MHz. 

HSI48 and LSE oscillators are enabled during CRS Initialization. As a first step a synchronization miss is raised when using a wrong synchronization source (GPIO instead of LSE which is not configured). In a 2nd step, a reconfiguration is done to use LSE and the correct synchronization.

- This is the scenario of the example:

     - Set the source to GPIO and then enable the CRS interrupt
     - To provoke a SYNC event, call the software event generation function
     - Interrupt is raised to indicate a SYNC MISS and then power on **LED3**
     - In main program, wait for 1 sec and reconfigure synchronization source to LSE
     - Power on **LED2** when SYNC WARN is detected
     - Increase tolerance (FELIM value) when SYNC WARN is detected
     - At the end of the example, **LED1** should stay ON

**Note**: HSI48 frequency can be checked with oscilloscope using MCO1 PIN PA.08


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

RCC, System, Clock Configuration, CRS, System clock, Oscillator, PLL

### <b>Directory contents</b>

      - RCC/RCC_CRS_Synchronization_IT/Inc/stm32h5xx_nucleo_conf.h    BSP configuration file
      - RCC/RCC_CRS_Synchonization_IT/Inc/stm32h5xx_hal_conf.h        HAL configuration file
      - RCC/RCC_CRS_Synchonization_IT/Inc/stm32h5xx_it.h              Interrupt handlers header file
      - RCC/RCC_CRS_Synchonization_IT/Inc/main.h                      Header for main.c module
      - RCC/RCC_CRS_Synchonization_IT/Src/stm32h5xx_it.c              Interrupt handlers
      - RCC/RCC_CRS_Synchonization_IT/Src/main.c                      Main program
      - RCC/RCC_CRS_Synchonization_IT/Src/system_stm32h5xx.c          STM32H5xx system source file
      - RCC/RCC_CRS_Synchonization_IT/Src/stm32h5xx_hal_msp.c         HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

