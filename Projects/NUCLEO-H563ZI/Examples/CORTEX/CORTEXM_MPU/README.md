## <b>CORTEXM_MPU example Description</b>
Presentation of the MPU features. This example configures MPU attributes of different
MPU regions then configures a memory area as privileged read-only, and attempts to
perform read and write operations in different modes.

If the access is permitted LED1 is toggling. If the access is not permitted, 
a memory management fault is generated and LED3 is ON.

To generate an MPU memory fault exception due to an access right error, uncomment
the following line "PrivilegedReadOnlyArray[0] = 'e';" in the "stm32_mpu.c" file.

### <b>Notes</b>

  - Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  - The application needs to ensure that the SysTick time base is always set to 1 millisecond
to have correct HAL operation.

### <b>Keywords</b>

System, Cortex, Memory regions, Privileged area, permitted access, memory management fault, MPU memory fault

### <b>Directory contents</b>

      - CORTEX/CORTEXM_MPU/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
      - CORTEX/CORTEXM_MPU/Inc/stm32h5xx_hal_conf.h    HAL configuration file
      - CORTEX/CORTEXM_MPU/Inc/stm32h5xx_it.h          Interrupt handlers header file
      - CORTEX/CORTEXM_MPU/Inc/main.h                  Header for main.c module
      - CORTEX/CORTEXM_MPU/Inc/stm32_mpu.h             MPU management
      - CORTEX/CORTEXM_MPU/Src/stm32h5xx_it.c          Interrupt handlers
      - CORTEX/CORTEXM_MPU/Src//stm32h5xx_hal_msp.c     HAL MSP file
      - CORTEX/CORTEXM_MPU/Src/main.c                  Main program
      - CORTEX/CORTEXM_MPU/Src/system_stm32h5xx.c      stm32h5xx system source file
      - CORTEX/CORTEXM_MPU/Src/stm32_mpu.c             MPU management


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