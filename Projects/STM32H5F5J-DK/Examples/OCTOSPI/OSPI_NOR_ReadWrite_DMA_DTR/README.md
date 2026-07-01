## <b>OSPI_NOR_ReadWrite_DMA_DTR Example Description</b>
How to use a OSPI NOR memory in DMA mode.

This example describes how to erase part of a OSPI NOR memory, write data in DMA mode, read data in DMA mode 
and compare the result in an infinite loop. The memory is configured in octal DTR mode.

- LED_GREEN toggles each time a new comparison is good
- LED_RED is on as soon as a comparison error occurs
- LED_RED is on as soon as an error is returned by HAL API

In this example, HCLK is configured at 250 MHz.

#### <b>Notes</b>
 - Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 - The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, OCTOSPI, NOR, DMA, DTR Mode

### <b>Directory contents</b>

    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Src/main.c                       Main program
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Src/system_stm32h5xx.c           STM32H5xx system clock configuration file
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Src/stm32h5xx_it.c               Interrupt handlers 
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Src/stm32h5xx_hal_msp.c          HAL MSP module
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Inc/main.h                       Main program header file  
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Inc/stm32h5xx_hal_conf.h         HAL Configuration file
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Inc/stm32h5xx_it.h               Interrupt handlers header file
    - OCTOSPI/OSPI_NOR_ReadWrite_DMA_DTR/Inc/stm32h5f5j_discovery_conf.h  HAL configuration file

        
### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.
    
  - This example has been tested with STM32H5F5J-DK board and can be 
    easily tailored to any other supported device and development board.

  
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example