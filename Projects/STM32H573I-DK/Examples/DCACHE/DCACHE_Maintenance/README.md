## <b>DCACHE_Maintenance Example Description</b>

How to do Data-Cache maintenance on a shared memory buffer accessed by 2 masters (CPU and DMA).

This project is targeted to run on STM32H573xx devices on STM32H573I-DK board from STMicroelectronics.

This example includes two MPU configurations of External FLASH, and only one configuration
should be chosen in main.h:

- MPU_WRITE_THROUGH.
- MPU_WRITE_BACK_WRITE_ALLOCATE.

In this examples, two DMA transfers are configured using **GPDMA1_Channel7**:

**First Transfer:**

 * MPU_WRITE_BACK_WRITE_ALLOCATE
   - From internal SRAM to external memory
   - Destination buffer is put in DCACHE, before starting the transfer (Cache coherency issue after DMA transfer)
   - Green LED OFF

 * MPU_WRITE_THROUGH
   - From internal SRAM to external memory
   - Destination buffer is updated in the External memory, before starting the transfer (No Cache coherency issue after DMA transfer)
   - Green LED ON

**Second Transfer:**

 * MPU_WRITE_BACK_WRITE_ALLOCATE
   - From external memory cacheable buffer to internal SRAM non-cacheable buffer
   - Source buffer modified by CPU in DCACHE, before starting the transfer (Cache coherency issue after DMA transfer)
   - Green LED OFF

 * MPU_WRITE_THROUGH
   - From external memory cacheable buffer to internal SRAM non-cacheable buffer
   - Source buffer is updated in the external memory without bringing that block to the cache, before starting the transfer (No Cache coherency issue after DMA transfer)
   - Red LED ON

Then, these 2 DMA transfers are started, then source and destination buffers are compared.
LEDs colors will indicate the buffer comparison results.
In the first step, only one or two transfer comparison results could be wrong, depending
on the chosen MPU configuration.
Then, User has to press on User button to do a Cache maintenance, in order to
ensure the cache coherency between CPU and DMA.
Thereafter, a new buffer comparison is done to indicate transfer status after Cache maintenance,
and LEDs colors should indicate the correct comparison status for the 2 transfers.

STM32H573I-DK board's LEDs can be used to monitor the transfer status:

 *For the first transfer:*

 - **LED_GREEN is ON** when the transfer is completed and buffers comparison is correct.
 - **LED_GREEN is OFF** when the transfer is completed and buffers comparison is wrong.

 *For the second transfer:*

 - **LED_RED is ON** when the transfer is completed and buffers comparison is correct.
 - **LED_RED is OFF** when the transfer is completed and buffers comparison is wrong.

**Expected behavior:**

LED_GREEN and LED_RED will be ON to indicating that the 2 transfers are correct, after pressing on USER button

Cache Maintenance:

* It is recommended to enable the cache and maintain its coherence, but depending
on the use case, it is also possible to configure the MPU as "Write through",
to guarantee the write access coherence. In that case, the MPU must be configured
as Cacheable/Not bufferable/Not Shareable. Even though the user must manage
the cache coherence for read accesses.

* Be careful before using the Invalidate all data cache in Write-Back policy:
   * Risk to lose all the modification which are not yet evicted.
   * Use always Cache Clean before the Cache Invalidate.

* Clean all Data-Cache shall not cause a coherency problem but will impact performance,
since it will fetch all dirty lines in the DCACHE and evicted even the non-shareable data.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Memory, DCACHE, XSPI, Read, Write, Initialization, Access, TrustZone disabled

### <b>Directory contents</b>

  - DCACHE/DCACHE_Maintenance/Src/main.c                       Main program
  - DCACHE/DCACHE_Maintenance/Src/system_stm32h5xx.c           STM32H5xx system clock configuration file
  - DCACHE/DCACHE_Maintenance/Src/stm32h5xx_it.c               Interrupt handlers
  - DCACHE/DCACHE_Maintenance/Src/stm32h5xx_hal_msp.c          HAL MSP module
  - DCACHE/DCACHE_Maintenance/Src/external_memory_helper.c     External memory Helper
  - DCACHE/DCACHE_Maintenance/Inc/main.h                       Main program header file
  - DCACHE/DCACHE_Maintenance/Inc/stm32h573i_discovery_conf.h  BSP Configuration file
  - DCACHE/DCACHE_Maintenance/Inc/stm32h5xx_hal_conf.h         HAL Configuration file
  - DCACHE/DCACHE_Maintenance/Inc/stm32h5xx_it.h               Interrupt handlers header file
  - DCACHE/DCACHE_Maintenance/Inc/external_memory_helper.h     External memory Helper header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573xx devices without security enabled (TZEN=0xC3).
  - This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example