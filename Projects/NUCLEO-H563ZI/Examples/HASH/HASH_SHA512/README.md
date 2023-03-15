## <b>HASH_SHA512 Example Description</b>

This example provides a short description of how to use the HASH peripheral to 
hash data using SHA512 Algorithms.

**HASH_SHA512 configuration:**

For this example, DMA is not used for data transfer, the CPU is using HASH peripheral in
polling mode.

The SHA512 message digest result is a 512 bit data.

Since we are using NIST vector, the expected HASH digests is already given.
The digests value is compared to the computed by the HASH peripheral.

 - In case of digest computation mismatch or initialization issue the LED3 is blinking (200 ms period).
 - In case the SHA512 digest is computed correctly the LED1 is turned ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, Security, HASH, HMAC, SHA512, digest

### <b>Directory contents</b>

      - HASH/HASH_SHA512/Inc/stm32h5xx_nucleo_conf.h    BSP configuration file
      - HASH/HASH_SHA512/Inc/stm32h5xx_hal_conf.h       HAL configuration file
      - HASH/HASH_SHA512/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - HASH/HASH_SHA512/Inc/main.h                     Header for main.c module  
      - HASH/HASH_SHA512/Src/stm32h5xx_it.c             Interrupt handlers
      - HASH/HASH_SHA512/Src/main.c                     Main program
      - HASH/HASH_SHA512/Src/stm32h5xx_hal_msp.c        HAL MSP module
      - HASH/HASH_SHA512/Src/system_stm32h5xx.c         STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with a STM32H563ZITx embedded on an
    NUCLEO-H563ZI board and can be easily tailored to any other supported
    device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 