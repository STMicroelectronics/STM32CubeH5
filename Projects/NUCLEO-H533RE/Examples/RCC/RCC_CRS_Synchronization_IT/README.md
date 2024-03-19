## <b>RCC_CRS_Synchronization_IT RCC Clock Recovery Service  Example Description</b>

Configuration of the clock recovery service (CRS) in Interrupt mode, using the RCC HAL APIs.


At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

Then, HSI48 and LSE oscillators are enabled.
In this example, as a first step a synchronization miss is raised when using a wrong synchronization
source (GPIO instead of LSE which is not configured). In a 2nd step, a reconfiguration is done to
use LSE and the correct synchronization.

In this example, this is the scenario:

    1. Set the source to GPIO and then enable IT

    2. To provoke a SYNC event, call the software event generation function

    3. Interrupt is raised to indicate a SYNC MISS and LED2 remains off

    4. In main program, wait for 1 sec and reconfigure source to LSE

    5. Quickly toggle LED2 when SYNC WARN is detected

    6. Increase tolerance (FELIM value) when SYNC WARN is detected

    7. At the end of the example, LED2 should stay ON

Note: HSI48 frequency can be checked with oscilloscope using MCO PIN PA.08

### <b>Keywords</b>

RCC, System, Clock Configuration, CRS, System clock, Oscillator, PLL

### <b>Directory contents</b>

  - RCC/RCC_CRS_Synchronization_IT/Inc/stm32h5xx_nucleo_conf.h        BSP configuration file
  - RCC/RCC_CRS_Synchonization_IT/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - RCC/RCC_CRS_Synchonization_IT/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - RCC/RCC_CRS_Synchonization_IT/Inc/main.h                        Header for main.c module
  - RCC/RCC_CRS_Synchonization_IT/Src/stm32h5xx_it.c          Interrupt handlers
  - RCC/RCC_CRS_Synchonization_IT/Src/main.c                        Main program
  - RCC/RCC_CRS_Synchonization_IT/Src/system_stm32h5xx.c      STM32H5xx system source file
  - RCC/RCC_CRS_Synchonization_IT/Src/stm32h5xx_hal_msp.c     HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H533RETx Devices.

  - This example has been tested with NUCLEO-H533RE board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain

 - Rebuild all files and load your image into target memory

 - Run the example