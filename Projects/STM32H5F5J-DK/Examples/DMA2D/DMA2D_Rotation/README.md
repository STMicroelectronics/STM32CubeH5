## <b>DMA2D_Rotation Example Description</b>

- This example demonstrates how to use the STM32 DMA2D peripheral to perform image rotation and blending operations efficiently using the HAL API.

> The DMA2D peripheral is configured to rotate images at different angles (0°, 90°, 180°, 270°) and blend them with a background image that represents a compass dial.  
> The rotation can also be inverted (flipped) to achieve mirrored effects, allowing the compass needle to rotate in the opposite direction.  
> This is achieved using the DMA2D hardware's rotation and blending capabilities, minimizing CPU load and ensuring smooth and efficient graphics rendering.

- At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 Mhz.

- In this example:
  - The foreground image consists of **three images**:
    1. A base image of a compass needle.
    2. Two pre-rotated images of the compass needle at **30°** and **60°** angles.<br/>
  - The DMA2D hardware rotation is applied to these three images at **0°**, **90°**, **180°**, and **270°** to achieve a complete 360° rotation of the compass needle.<br/>
  - This combination of pre-rotated images and hardware rotation allows the needle to perform a **smooth and complete circular rotation**.<br/>
  - The foreground (rotated needle) is blended with a background image that shows a **compass dial**, resulting in a fully functional compass with a rotating needle.<br/>
  - When the user button is pressed, the rotation sense is inverted, causing the needle to rotate in the opposite direction.<br/>
  - The application runs in an infinite loop, continuously the final blended image is displayed on the LCD .

This example demonstrates dynamic image manipulation using the DMA2D peripheral, showcasing its ability to perform hardware-accelerated rotation and blending operations efficiently.

#### <b>Notes</b>

1. The `BSP_PB_Callback()` function is used to toggle the `useInverse` flag when the user button is pressed. This allows switching between normal and inverted rotations dynamically.
2. The DMA2D peripheral is configured for ARGB4444 color format, and the output is displayed on an LCD with a resolution of 270x270 pixels.
3. The `HAL_Delay()` function is used to introduce a delay between successive rotations.

### <b>Keywords</b>

Graphic, Display, DMA2D, LTDC, Rotation, Blending, Foreground, Background, ARGB4444, Inverted Rotation(Flip, Mirror)

### <b>Directory contents</b>

- DMA2D/DMA2D_Rotation/Inc/main.h: Header for the main program.
- DMA2D/DMA2D_Rotation/Src/main.c: Main program source file.
- DMA2D/DMA2D_Rotation/Inc/stm32h5xx_it.h: Interrupt handlers header file
- DMA2D/DMA2D_Rotation/Inc/CompassNeedle.h: Foreground image (compass needle) in ARGB4444 format.
- DMA2D/DMA2D_Rotation/Inc/CompassNeedle_Rot30.h: Pre-rotated foreground image (30°).
- DMA2D/DMA2D_Rotation/Inc/CompassNeedle_Rot60.h: Pre-rotated foreground image (60°).
- DMA2D/DMA2D_Rotation/Inc/CompassDial.h: Background image (compass dial) in ARGB4444 format.
- DMA2D/DMA2D_Rotation/Src/stm32h5xx_it.c: Interrupt handlers.
- DMA2D/DMA2D_Rotation/Src/system_stm32h5xx.c: STM32H5xx system source file.

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.

  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
