## <b>CRS_Synchronization_IT example Description</b>
How to configure the clock recovery service in IT mode through the
STM32H5xx CRS LL API. The peripheral initialization uses LL unitary
service functions for optimization purposes (performance and size).

### <b>Notes</b>

At the beginning of the main program the SystemClock_Config() function is used to 
configure the system clock (SYSCLK) to run at 250 MHz.

Then, HSI48 and LSE oscillators are enabled. 
In this example, after pressing the user button, as a first step a synchronization miss is raised 
when using a wrong synchronization source i.e. GPIO with nothing connected (instead of LSE). 
In a 2nd step, a reconfiguration is done to use LSE source and reach the correct synchronization.
HSI48 synchronize to LSE (32768Hz), adjusting the trimmer such to get the counting to 48000000/32768.

In this test, this is the scenario:

  - set the source to GPIO and then enable IT
  - Wait for USER push-button press to start the test (Fast toggle LED1)
  - to provoke a SYNC event, call the software event generation function
  - Interrupt is raised to indicate a SYNC MISS and then power on LED3
  - in main program, wait for 1 sec and reconfigure source to LSE
  - Interrupt is raised to indicate a SYNC OK and then power on LED1
  - LED2 is power on when SYNC WARN is detected
  - Increase tolerance (FELIM value) when SYNC WARN is detected
  - At the end of the test LED1 should stay ON


### <b>Keywords</b>

CRS, interrupt, HSI48, LSE

### <b>Directory contents</b>

      - CRS/CRS_Synchronization_IT/Inc/stm32h5xx_it.h          Interrupt handlers header file
      - CRS/CRS_Synchronization_IT/Inc/main.h                  Header for main.c module
      - CRS/CRS_Synchronization_IT/Inc/stm32_assert.h          Template file to include assert_failed function
      - CRS/CRS_Synchronization_IT/Src/stm32h5xx_it.c          Interrupt handlers
      - CRS/CRS_Synchronization_IT/Src/main.c                  Main program
      - CRS/CRS_Synchronization_IT/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with a STM32H563ZITx embedded on an
    NUCLEO-H563ZI board and can be easily tailored to any other supported
    device and development board.

  - NUCLEO-H563ZI Set-up
    - Connect the MCO pin to an oscilloscope to monitor HSI48 frequency:
      - PA.08: connected to pin 23 of CN5 connector for Nucleo-144  (MB1404)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain 
  - Rebuild all files and load your image into target memory
  - Run the example
