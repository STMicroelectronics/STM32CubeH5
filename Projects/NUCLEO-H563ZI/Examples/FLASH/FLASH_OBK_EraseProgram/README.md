## <b>FLASH_OBK_EraseProgram Example Description</b>

How to configure and use the FLASH HAL API to erase and program the FLASH OBKeys: secure key storage area.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 MHz.

After startup, ICACHE is enabled and an MPU configuration is executed to disable local cacheability in OBK area, as not
practical for this region.
The Flash memory and Flash OBK registers are locked. Dedicated functions are used to enable both
the FLASH control register and FLASH OBK register access.

Before programming the desired OBK addresses, an erase operation is performed using the flash OBK erase feature.
The erase procedure is done by filling the erase initialization structure giving the erase type
(Erase OBK alternate area in this case).
At this stage, the OBK alternate sector will be erased.

Once this operation is finished, a FLASH swap operation will be performed to copy the HDPL0 data from current
to alternate OBK area using the defined "HAL_FLASHEx_OBK_Swap" function.

###### <b>Notes</b>
The swap operation will copy the OBKeys data from current to alternate OBK sector except for area of alternate OBK
sector where content is different from 0xFF.


Once this swap step is finished, a check of OBK HDPL1 area will be done to verify that data is equal to 0xFF and
then a quad-word alternate programming operation will be performed in the alternate HDPL1 area in polling mode.
A swap operation will be executed to copy HDPL0/1 data from current to alternate OBK sector
The written data is then read back and checked.

Incrementing the OBK-HDPL value makes the HDPL2 accessible for read and write operations, a program operation will be
performed in current area using the interrupt mode and then a check will be executed to verify the written data,

At final step, the OBK-HDPL value is incremented in order to access HDPL3 OBK area., a program operation will be
performed in current OBK sector using the polling mode and then a check will be executed to verify the written data.

The NUCLEO-H563ZI board LEDs can be used to monitor the example status:

 - **LED_GREEN is ON** when there are no errors detected when there is no error during OBK area operations.
 - **LED_RED is ON** when an error occurs (OBK Erase/Program/swap or data check).

###### <b>Notes</b>
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, FLASH, Erase, Program, OBK area, swap, alternate, current

### <b>Directory contents</b>

      - FLASH/FLASH_OBK_EraseProgram/Inc/stm32h5xx_nucleo_conf.h    BSP configuration file
      - FLASH/FLASH_OBK_EraseProgram/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - FLASH/FLASH_OBK_EraseProgram/Inc/stm32h5xx_it.h             Header for stm32h5xx_it.c
      - FLASH/FLASH_OBK_EraseProgram/Inc/main.h                     Header for main.c module
      - FLASH/FLASH_OBK_EraseProgram/Src/stm32h5xx_it.c             Interrupt handlers
      - FLASH/FLASH_OBK_EraseProgram/Src/main.c                     Main program
      - FLASH/FLASH_OBK_EraseProgram/Src/stm32h5xx_hal_msp.c        MSP initialization and de-initialization
      - FLASH/FLASH_OBK_EraseProgram/Src/system_stm32h5xx.c         STM32H5xx system clock configuration file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563xx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
