## <b>DMA2D_MemToMemWithBlending Example Description</b>

  This example provides a description of how to configure DMA2D peripheral in 

  This example provides a description of how to configure the DMA2D peripheral in Memory_to_Memory with blending transfer mode.


  **LCD Initialization:**

  - The LCD is initialized using the BSP functions.
  - LCD layer 0 is configured for display with color format set to ARGB8888.

  **Images Used:**

  1. **Image 1**
      - Input memory address: Flash memory (static image array coded in FLASH)
      - Format: RGB565
      - Size: 240×130
  2. **Image 2**
      - Input memory address: Flash memory (static image array coded in FLASH)
      - Format: RGB565
      - Size: 240×130


  **Main Loop Sequence:**

  1. **Step 1:**
      - DMA2D is used in Pixel Format Conversion mode (PFC) to transfer "Image 1" from the input buffer in Flash to the LCD frame buffer.
      - DMA2D destination address and output offset are set to center the image on the LCD.
      - Input color format: RGB565 (input image)
      - Output color format: ARGB8888 (LCD display)

  2. **Step 2:**
      - DMA2D is used in Pixel Format Conversion mode (PFC) to transfer "Image 2" from the input buffer in Flash to the LCD frame buffer.
      - DMA2D destination address and output offset are set to center the image on the LCD.
      - Input color format: RGB565 (input image)
      - Output color format: ARGB8888 (LCD display)

  3. **Step 3:**
      - DMA2D is used in memory-to-memory with Blending mode (**M2M_BLEND**) to blend the foreground and background images.
      - DMA2D configuration:
          - **Foreground layer:**
              - Input pixel format: RGB565
              - Source address: "Image 1" input buffer in Flash
              - Input offset: 0
              - Alpha transparency: 0x7F (semi-transparent)
          - **Background layer:**
              - Input pixel format: ARGB8888
              - Source address: center of LCD frame buffer
              - Input offset: set to fetch background from center of LCD frame buffer
              - LCD frame buffer contains "Image 2" in ARGB8888 format (from previous step)
              - Alpha transparency: 0xFF (fully opaque)
          - **Output:**
              - Color format: ARGB8888 (LCD display)
              - Output offset and destination address: configured to place the result of blending in the center of the LCD frame buffer
              - Output offset: same as background layer input offset
              - Destination address: same as background layer source address


  **Display Messages for Each Step:**

  - Display Foreground Image: life.augmented
  - Display Background Image: ST Logo
  - Display Blended Image: ST Logo + life.augmented

  *A delay of 2 seconds is inserted after each step.*


  **Pixel Pipeline:**

  - Next step is the use of the LTDC with only 1 layer (output of DMA2D operation).
  - This shows on the LCD panel the result of blending the foreground and background.

  In this example:

In this example:

  - **Background object:** represents the ST logo.

  - **Foreground object:** represents the "life.augmented" slogan.


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

        - DMA2D/DMA2D_MemToMemWithBlending/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
        - DMA2D/DMA2D_MemToMemWithBlending/Inc/stm32h5xx_hal_conf.h            HAL configuration file
        - DMA2D/DMA2D_MemToMemWithBlending/Inc/stm32h5xx_it.h                  Interrupt handlers header file
        - DMA2D/DMA2D_MemToMemWithBlending/Inc/main.h                          Header for main.c module
        - DMA2D/DMA2D_MemToMemWithBlending/Inc/RGB565_240x120_1.h              RGB565 input image to be transferred by DMA2D
        - DMA2D/DMA2D_MemToMemWithBlending/Inc/RGB565_240x120_2.h              RGB565 input image to be transferred by DMA2D
        - DMA2D/DMA2D_MemToMemWithBlending/Src/stm32h5xx_it.c                  Interrupt handlers
        - DMA2D/DMA2D_MemToMemWithBlending/Src/main.c                          Main program
        - DMA2D/DMA2D_MemToMemWithBlending/Src/stm32h5xx_hal_msp.c             HAL MSP module
        - DMA2D/DMA2D_MemToMemWithBlending/Src/system_stm32h5xx.c              STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.
  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
