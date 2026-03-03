## <b>DMA2D_MemoryToMemory Example Description</b>

This example provides a description of how to configure the DMA2D peripheral in 
Memory_to_Memory transfer mode.
The example transfers with DMA2D an internal SRAM static texture array aBufferInput[] of size 16x32 
and format ARGB4444 (16 bpp) to destination buffer in internal SRAM in array aBufferResult[].
The number of bytes transferred is then 16x32x2=1024 bytes.

In this basic example the goal is to explain the different fields of the DMA2D 
structure in the case of Memory_to_Memory transfer mode.

After DMA2D configuration, the data transfer is performed and then the transferred 
data are checked byte per byte in comparing the source and destination buffers aBufferInput[] and aBufferResult[].   

STM32H5F5J-DK board's LEDs can be used to monitor the transfer status:

- **LED3(LED_GREEN)** is ON when the DMA2D transfer is complete and the data have been correctly transferred.
- **LED1(LED_RED)** is ON when there is a DMA2D transfer error, when data are not correctly transferred, or when there is an error in the DMA2D transfer/init process.

If everything is all right at the end of the test, LED3 must be ON and LED1 must be OFF.

#### <b>Notes</b>

 1.  Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
     based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
     a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
     than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
     To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2.  The example needs to ensure that the SysTick time base is always set to 1 millisecond
     to have correct HAL operation.


### <b>Keywords</b>

Display, Graphics, DMA2D, LCD, SRAM, ARGB4444, Memory to Memory

### <b>Directory contents</b>

        - DMA2D/DMA2D_MemoryToMemory/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
        - DMA2D/DMA2D_MemoryToMemory/Inc/stm32h5xx_hal_conf.h            HAL configuration file
        - DMA2D/DMA2D_MemoryToMemory/Inc/stm32h5xx_it.h                  Interrupt handlers header file
        - DMA2D/DMA2D_MemoryToMemory/Inc/main.h                          Header for main.c module
        - DMA2D/DMA2D_MemoryToMemory/Src/stm32h5xx_it.c                  Interrupt handlers
        - DMA2D/DMA2D_MemoryToMemory/Src/main.c                          Main program
        - DMA2D/DMA2D_MemoryToMemory/Src/stm32h5xx_hal_msp.c             HAL MSP module
        - DMA2D/DMA2D_MemoryToMemory/Src/system_stm32h5xx.c              STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.
  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
