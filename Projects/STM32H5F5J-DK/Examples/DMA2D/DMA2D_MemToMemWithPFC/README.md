## <b>DMA2D_MemToMemWithPFC Example Description</b>

  This example provides a description of how to configure DMA2D peripheral in Memory_to_Memory transfer mode with Pixel Format conversion and display the result on LCD.

  The source buffer in format **RGB565** and size **240×130** located in SRAM is copied in Memory to Memory with PFC mode by the DMA2D to the SDRAM LCD frame buffer for display. 
  The DMA2D output offset and destination address are calculated to center the image on the LCD screen.


  **The example acts as follows:**

  1. First, the LCD screen is initialized.
  2. Then, in an infinite while loop:
      - The DMA2D is configured to perform a transfer from the input RGB565 image to the output ARGB8888 display frame buffer.
      - Start the DMA2D and wait for the end of the transfer.
      - Wait for 2 seconds and go to the next loop.


  **How to calculate the size of the transferred data?**

  Selected color format gives the number of bits per pixel. For example:

  RGB565 ⇒ 16 bits/pixel

  Given the number of pixels per line and the number of lines:

  data_size = (bits per pixel) × (pixels per line + output offset) × (number of lines)
 

  STM32H5F5J-DK board's LEDs can be used to monitor the transfer status:

  - **LED3(LED_GREEN)** is ON when the DMA2D transfer is complete.
  - **LED2(LED_ORANGE)** is ON when there is a DMA2D transfer error.
  - **LED1(LED_RED)** is ON when there is an error in LTDC transfer/Init process.

#### <b>Notes</b>

 1.  Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
     based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
     a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
     than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
     To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2.  The example needs to ensure that the SysTick time base is always set to 1 millisecond
     to have correct HAL operation.


### <b>Keywords</b>

Display, Graphics, DMA2D, LCD, SRAM, RGB565, Blending, Memory to memory, Pixel Format Conversion,
Red Blue swap, LTDC, Pixel

### <b>Directory contents</b>

        - DMA2D/DMA2D_MemToMemWithPFC/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
        - DMA2D/DMA2D_MemToMemWithPFC/Inc/stm32h5xx_hal_conf.h            HAL configuration file
        - DMA2D/DMA2D_MemToMemWithPFC/Inc/stm32h5xx_it.h                  Interrupt handlers header file
        - DMA2D/DMA2D_MemToMemWithPFC/Inc/main.h                          Header for main.c module
        - DMA2D/DMA2D_MemToMemWithPFC/Inc/RGB565_240x120.h                RGB565 input image to be transferred by DMA2D
        - DMA2D/DMA2D_MemToMemWithPFC/Src/stm32h5xx_it.c                  Interrupt handlers
        - DMA2D/DMA2D_MemToMemWithPFC/Src/main.c                          Main program
        - DMA2D/DMA2D_MemToMemWithPFC/Src/stm32h5xx_hal_msp.c             HAL MSP module
        - DMA2D/DMA2D_MemToMemWithPFC/Src/system_stm32h5xx.c              STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.
  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
