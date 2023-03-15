## <b>RCC_CRS_Synchronization_Polling example Description</b>
Configuration of the clock recovery system (CRS) in Polling mode, using the RCC HAL API.

### <b>Notes</b>

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

Then, HSI48 and LSE oscillators are enabled. In this example CRS will use LSE
oscillator to calibrate HSI48 frequency.
When HSI48 and LSE are started, automatic calibration starts and program call the
polling function to wait for synchronization status.

In this test, there are results which could occur:

 - few loops occurs up to SYNC OK event (if SYNC WARN, HSI48 trimming value will be
 automatically modified)
 - in case of SYNC MISS or SYNC ERROR, there is an issue with synchronization input
 parameters. In this case, user need to apply new parameters after checking synchronization
 information and restart the synchronization.
 - LED3 power on means that a timeout occurred during polling procedure. It may
 due to an issue with LSE synchronization frequency.
 - LED1 will finally power on if SYNC OK is returned by CRS.

Note: HSI48 frequency can be checked with oscilloscope using MCO PIN


### <b>Keywords</b>

RCC, System, Clock Configuration, CRS, System clock, HSI48, LSE, Oscillator, PLL, Polling

### <b>Directory contents</b>

      - RCC/RCC_CRS_Synchronization_Polling/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
      - RCC/RCC_CRS_Synchonization_Polling/Inc/stm32h5xx_hal_conf.h    HAL configuration file
      - RCC/RCC_CRS_Synchonization_Polling/Inc/stm32h5xx_it.h          Interrupt handlers header file
      - RCC/RCC_CRS_Synchonization_Polling/Inc/main.h                  Header for main.c module
      - RCC/RCC_CRS_Synchonization_Polling/Src/stm32h5xx_it.c          Interrupt handlers
      - RCC/RCC_CRS_Synchonization_Polling/Src/main.c                  Main program
      - RCC/RCC_CRS_Synchonization_Polling/Src/system_stm32h5xx.c      STM32H5xx system source file
      - RCC/RCC_CRS_Synchonization_Polling/Src/stm32h5xx_hal_msp.c     HAL MSP module


### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.
    
  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up
    - Connect the MCO pin to an oscilloscope to monitor HSI48 frequency:
      - PA.08: connected to pin 23 of CN5 connector for Nucleo-144  (MB1404)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
