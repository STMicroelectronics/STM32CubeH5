## <b>FLASH_EDATA_EraseProgram Example Description</b>

How to configure and use the FLASH HAL API to erase and program the FLASH high-cycle data area.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 250 MHz.

After Reset, the Flash memory and option bytes controller registers are locked. Dedicated functions
are used to enable both the FLASH control register and FLASH option control register access.
Before programming the desired addresses, an erase operation is performed using
the flash erase sector feature. The erase procedure is done by filling the erase init
structure giving the starting erase sector and the number of sectors to erase.
At this stage, all these sectors will be erased one by one separately.

###### <b>Note</b>
  - If problem occurs on a sector, erase will be stopped and faulty sector will be returned to user (through variable 'SectorError').

Once this operation is finished, a FLASH high-cycle data area configuration will be performed to enable the EDATA
area of BANK1 using the option structure giving the starting sector and the number of sectors to set as EDATA area.
This configuration will be followed by a HAL_FLASH_OB_Launch() function to ensure the update of the related 
option bytes.

Once this configuration step is finished, a half-word programming operation will be performed in the Flash high-cycle 
data area. The written data is then read back and checked.

At final step, an option byte configuration is performed to disable the EDATA area.

The NUCLEO-H563ZI board LEDs can be used to monitor the example status:

 - **LED_GREEN is ON** when there are no errors detected after finishing the whole FLASH high-cycle data example.
 - **LED_RED is ON** when any error occurs (EDATA Erase/Program or configuration).

###### <b>Notes</b>
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, FLASH, Erase, Program, Sector, High-cycle data

### <b>Directory contents</b>

      - FLASH/FLASH_EDATA_EraseProgram/Inc/stm32h5xx_nucleo_conf.h    BSP configuration file
      - FLASH/FLASH_EDATA_EraseProgram/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - FLASH/FLASH_EDATA_EraseProgram/Inc/stm32h5xx_it.h             Header for stm32h5xx_it.c
      - FLASH/FLASH_EDATA_EraseProgram/Inc/main.h                     Header for main.c module
      - FLASH/FLASH_EDATA_EraseProgram/Src/stm32h5xx_it.c             Interrupt handlers
      - FLASH/FLASH_EDATA_EraseProgram/Src/main.c                     Main program
      - FLASH/FLASH_EDATA_EraseProgram/Src/stm32h5xx_hal_msp.c        MSP initialization and de-initialization
      - FLASH/FLASH_EDATA_EraseProgram/Src/system_stm32h5xx.c         STM32H5xx system clock configuration file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563xx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
