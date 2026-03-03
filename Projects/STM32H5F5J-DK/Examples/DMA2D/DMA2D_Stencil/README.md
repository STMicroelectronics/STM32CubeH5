## <b>DMA2D_Stencil Example Description</b> 

- This example demonstrates how to leverage the STM32 DMA2D peripheral to efficiently shape image composition<br/>
using the HAL API to activate the stencil and blending stages.<br/>

<pre>
The stencil buffer is a mask that controls pixel visibility by defining transparency levels<br/>
on the foreground image. It uses an 8-bit alpha channel, providing 256 levels of transparency (0–255),<br/>
which enables smooth blending with soft edges rather than simple binary masking.<br/>
</pre>

- At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 Mhz.

- In this example, the stencil buffer is configured to render the text "STMICROELECTRONICS" as an alpha mask.<br/>
This mask selectively reveals the foreground image only through the text shapes, while other areas remain transparent.<br/>

- The DMA2D blending operation uses this resulting masked foreground image with a background image<br/> (ST logo). 
  The stencil buffer can be applied to the foreground, background,<br/>
  or the final blended output, offering flexible control over the composition.<br/>

- The process flow is as follows:<br/>
  - Stencil Buffer Setup: The stencil buffer is prepared with the alpha mask representing the desired shape (text).<br/>
  - Foreground Layer: The foreground image is combined with the stencil buffer to apply transparency.<br/>
  - Background Layer: One background image is used.<br/>
  - Blending Stage: The DMA2D hardware blends the masked foreground (two images are used for this and continuously
                updated in an infinite loop) with the background image during the blending stage..<br/>
<br/>
The final image(480x272) is displayed on the LCD.<br/>
<span style="color:red">RED Led</span> toggles wen any error occurs.<br/>

<pre>
This approach demonstrates dynamic image composition where the same foreground stencil mask is applied<br/>
over different backgrounds efficiently. The DMA2D peripheral accelerates the blending operation,<br/>
minimizing CPU load and ensuring smooth graphics rendering.</pre>

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Graphic, Display, DMA2D, LTDC, Blending , Foreground, background , ARGB444, Stencil, Transparency(Alpha), A8

### <b>Directory contents</b>

  - DMA2D/DMA2D_Stencil/Inc/stm32h5f5j_discovery_conf.h   BSP configuration file
  - DMA2D/DMA2D_Stencil/Inc/stm32h5xx_hal_conf.h          HAL configuration file
  - DMA2D/DMA2D_Stencil/Inc/stm32h5xx_it.h                Interrupt handlers header file
  - DMA2D/DMA2D_Stencil/Inc/main.h                        Header for main.c module
  - DMA2D/DMA2D_Stencil/Inc/BGND_Blue_ARGB444.h           Image 480x272 in ARGB4444 used for DMA2D foreground layer
  - DMA2D/DMA2D_Stencil/Inc/BGND_Wood_ARGB4444.h          Image 480x272 in ARGB4444 used for DMA2D foreground layer
  - DMA2D/DMA2D_Stencil/Inc/ST_LOGO_ARGB4444.h            Image 480x272 in ARGB4444 used for DMA2D background layer
  - DMA2D/DMA2D_Stencil/Inc/ST_Txt_Stencil.h              Image 480x272 in ARGB4444 used as stencil buffer for the DMA2D
  - DMA2D/DMA2D_Stencil/Src/stm32h5xx_it.c                Interrupt handlers
  - DMA2D/DMA2D_Stencil/Src/stm32h5xx_hal_msp.c           HAL MSP file
  - DMA2D/DMA2D_Stencil/Src/main.c                        Main program
  - DMA2D/DMA2D_Stencil/Src/system_stm32h5xx.c            STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.

  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
