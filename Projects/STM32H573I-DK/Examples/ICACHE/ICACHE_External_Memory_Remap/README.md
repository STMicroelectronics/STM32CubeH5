## <b>ICACHE_External_Memory_Remap Example Description</b>

How to execute code from an external Flash remapped region configured through the ICACHE HAL driver.

This example describes how to remap the MX25LM51245G external Flash to C-bus and execute code located in this memory.

- First, a piece of code which blinks once the **Green led** is executed from external Flash before remapping.
- After remapping to C-Bus, the **Green led** will blink infinitely.

This project is targeted to run on STM32H573xx devices on STM32H573I-DK board from STMicroelectronics.

At the beginning of the main program the HAL_Init() function is called initialize the systick.

The SystemClock_Config() function is used to configure the system clock for STM32H573IIKxQ devices :
The CPU at 250MHz

STM32H573I-DK board's LEDs can be used to monitor the example execution:
 - **Green LED is blinking** when code has been correctly executed before and after remapping.
 - **Red LED is ON** when any error occurred.

#### <b>Notes</b>

Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds) based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower) than the peripheral interrupt. Otherwise the caller ISR process will be blocked. To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

The application needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.

The instruction cache (ICACHE) must be enabled by software to get a zero wait-state execution from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

System, ICACHE, XSPI, FLASH, Memory Remap

### <b>Directory contents</b>

  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Src/main.c                             Main program
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Src/stm32h5xx_hal_msp.c                MSP initializations and de-initializations
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Src/system_stm32h5xx.c                 STM32H5xx system source file
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Src/stm32h5xx_it.c                     Interrupt handlers
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Inc/main.h                             Main program header file
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Inc/stm32h5xx_hal_conf.h               HAL configuration file
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Inc/stm32h573i_discovery_conf.h        Board configuration file
  - ICACHE/ICACHE_EXTERNAL_Memory_Remap/Inc/stm32h5xx_it.h                     Interrupt handlers header file


### <b>Hardware and Software environment</b>
  - This example runs on STM32H573xx devices without security enabled ( TZEN=0xC3 ).

  - This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example