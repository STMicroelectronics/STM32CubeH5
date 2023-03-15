## <b>OSPI_NOR_ReadWrite_DMA_DTR Example Description</b>

How to use a OSPI NOR memory in DMA mode.

This example describes how to erase part of a OSPI NOR memory, write data in DMA mode, read data in DMA mode 
and compare the result in an infinite loop. The memory is configured in octal DTR mode.

- LED1 toggles each time a new comparison is good
- LED2 is on as soon as a comparison error occurs
- LED3 is on as soon as an error is returned by HAL API

In this example, HCLK is configured at 250 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, OCTOSPI, NOR, DMA, DTR Mode

### <b>Directory contents</b>

  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/main.c                      Main program
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/stm32h5xx_it.c              Interrupt handlers 
  - OSPI/OSPI_NOR_ReadWrite_DMA/Src/stm32h5xx_hal_msp.c         HAL MSP module
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/main.h                      Main program header file  
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - OSPI/OSPI_NOR_ReadWrite_DMA/Inc/stm32h573i_discovery_conf.h HAL configuration file  

        
### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ devices.

  - This example has been tested with STM32H573I-DK board and can be 
    easily tailored to any other supported device and development board.

  
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


