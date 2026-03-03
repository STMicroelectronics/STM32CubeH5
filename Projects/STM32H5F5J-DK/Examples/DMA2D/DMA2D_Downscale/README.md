## <b>DMA2D_Downscale Example Description</b>

This example demonstrates how to utilize the STM32 DMA2D peripheral to perform efficient 
image downscaling and blending operations using the HAL API.

The DMA2D hardware accelerator is configured in memory-to-memory blending mode to combine two images
with alpha blending, while simultaneously applying horizontal and vertical downscaling.
This allows dynamic resizing of images with minimal CPU involvement, enabling smooth graphics rendering.

- At the start of the program, the HAL_Init() function resets all peripherals, initializes the Flash interface, and configures the system tick timer.
- The SystemClock_Config() function sets the system clock (SYSCLK) to 250 MHz to ensure optimal performance.

- The example performs progressive downscaling of a foreground image ("LifeAugmented") blended over a background image (ST logo).
The downscaling ratios for horizontal and vertical dimensions are incremented in a loop from 1 to 4, demonstrating scaling effects.

- The DMA2D peripheral is configured in M2M_BLEND mode with RGB888 color format and a fixed alpha value of 0x7F (50% transparency) applied to both layers.
The scaled and blended image is continuously updated on the LCD in an infinite loop.
Dynamically LTDC layer window size and position are adjusted to center the scaled image.

- The RED LED toggles in case of any error detected during initialization or DMA2D operations.

#### <b>Notes</b>

1. Care must be taken with HAL_Delay() usage, as it depends on the SysTick interrupt priority relative to other peripheral interrupts.
   Ensure SysTick interrupt priority is higher (numerically lower) than other peripheral interrupts to avoid blocking.
   Use HAL_NVIC_SetPriority() to configure priorities if necessary.

2. The SysTick time base must be configured to 1 millisecond for accurate HAL timing functions.

### <b>Keywords</b>

Graphic, Display, DMA2D, LTDC, Blending, Downscaling, RGB888, Alpha Blending, Blender Scaling, Layers, Dynamic window

### <b>Directory contents</b>

- DMA2D/DMA2D_Downscaling/Inc/stm32h5f5j_discovery_conf.h   BSP configuration file  
- DMA2D/DMA2D_Downscaling/Inc/stm32h5xx_hal_conf.h          HAL configuration file  
- DMA2D/DMA2D_Downscaling/Inc/stm32h5xx_it.h                Interrupt handlers header file  
- DMA2D/DMA2D_Downscaling/Inc/main.h                        Header for main.c module  
- DMA2D/DMA2D_Downscaling/Inc/STLogo.h                      Background image (ST logo) in RGB888 format  
- DMA2D/DMA2D_Downscaling/Inc/LifeAugmented.h               Foreground image in RGB888 format  
- DMA2D/DMA2D_Downscaling/Src/stm32h5xx_it.c                Interrupt handlers  
- DMA2D/DMA2D_Downscaling/Src/stm32h5xx_hal_msp.c           HAL MSP file  
- DMA2D/DMA2D_Downscaling/Src/main.c                        Main program  
- DMA2D/DMA2D_Downscaling/Src/system_stm32h5xx.c            STM32H5xx system source file  

### <b>Hardware and Software environment</b>

- This example runs on STM32H5F5LJHxQ devices.  
- It has been tested on the STM32H5F5J-DK development board and can be adapted to other supported devices and boards.

### <b>How to use it ?</b>

To run this example:

- Open your preferred STM32 development toolchain.  
- Build all source files and flash the binary to the target device.  
- Run the application to observe the foreground image being progressively downscaled and blended over the background image on the LCD.  