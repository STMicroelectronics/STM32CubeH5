## <b>OSPI_NOR_MemoryMapped Example Description</b>

How to use a OSPI NOR memory in memory-mapped mode.

This example describes how to erase a part of an OSPI NOR memory, write data in memory-mapped mode and 
access to OSPI NOR memory in memory-mapped mode to check the data in an infinite loop.

 - LED1 toggles each time the data have been checked
 - LED2 is on as soon as a a data is wrong
 - LED3 is on as soon as an error is returned by HAL API

In this example, HCLK is configured at 250 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, OCTOSPI, NOR, Memory Mapped

### <b>Directory contents</b>

  - OSPI/OSPI_NOR_MemoryMapped/Src/main.c                 		Main program
  - OSPI/OSPI_NOR_MemoryMapped/Src/system_stm32h5xx.c     		STM32H5xx system clock configuration file
  - OSPI/OSPI_NOR_MemoryMapped/Src/stm32h5xx_it.c         		Interrupt handlers 
  - OSPI/OSPI_NOR_MemoryMapped/Src/stm32h5xx_hal_msp.c    		HAL MSP module
  - OSPI/OSPI_NOR_MemoryMapped/Inc/main.h                 		Main program header file  
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32h5xx_hal_conf.h   		HAL Configuration file
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32h5xx_it.h         		Interrupt handlers header file
  - OSPI/OSPI_NOR_MemoryMapped/Inc/stm32h573i_discovery_conf.h  HAL configuration file  

        
### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ devices.

  - This example has been tested with STM32H573I-DK (MB1677 revB) board and can be 
    easily tailored to any other supported device and development board.

  - To run this example on STM32H573I-DK (MB1677 revA), /* #define STM32H573I_DK_REVA */ should be uncommented in main.h
  
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

