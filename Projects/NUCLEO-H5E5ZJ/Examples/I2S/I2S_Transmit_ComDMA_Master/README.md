## <b>I2S_Transmit_ComDMA_Master Example Description</b>

This example demonstrates how to configure and use the I2S peripheral in transmit mode with communication managed by DMA (ComDMA) on the NUCLEO-H5E5ZJ board.

**Objective:**

- Verify that the I2S interface is properly configured in the selected modes and that data transmission is correctly performed using DMA.
- LED1(GREEN) will turn on when the data transfer is complete, indicating successful transmission.
- LED2(RED) will turn on when error occurred.

The *SystemClock_Config()* function is used to configure the system clock (SYSCLK) to run at 250 MHz.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, GPIO, I2S, DMA

### <b>Directory contents</b>

    - I2S/I2S_Transmit_ComDMA_Master/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - I2S/I2S_Transmit_ComDMA_Master/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - I2S/I2S_Transmit_ComDMA_Master/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - I2S/I2S_Transmit_ComDMA_Master/Inc/main.h                      Header for main.c module  
    - I2S/I2S_Transmit_ComDMA_Master/Src/stm32h5xx_it.c              Interrupt handlers
    - I2S/I2S_Transmit_ComDMA_Master/Src/stm32h5xx_hal_msp.c         HAL MSP file
    - I2S/I2S_Transmit_ComDMA_Master/Src/main.c                      Main program
    - I2S/I2S_Transmit_ComDMA_Master/Src/system_stm32h5xx.c          STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5E5ZJTx devices.

  - This example has been tested with NUCLEO-H5E5ZJ board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


