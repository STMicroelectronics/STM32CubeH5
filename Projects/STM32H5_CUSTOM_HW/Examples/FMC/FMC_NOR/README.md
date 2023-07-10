

## <b>FMC_NOR Example Description</b>

This example describes how to configure the FMC controller to access the NOR memory..

The NOR memory is M29W128GL70ZA6E.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 250 MHz.

The goal of this example is to explain the different steps to configure the FMC 
and make the NOR device ready for access( with the user own HW) , without using the MSP layer .       

In this example, the NOR device is configured and initialized explicitly 
following all initialization sequence steps. After initializing the device, user 
can perform erase/read NOR ID/read data/write data operations on it. 
A data buffer is written to the NOR memory, then read back and checked to verify 
its correctness.

If the data is read correctly from NOR, the LED1 is ON, otherwise the LED2 is ON.
In case of HAL initialization issue, LED3 will be ON.
  
#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

FMC/FSMC, Memory, NOR, Erase, Read, Write, Access

### <b>Directory contents</b>

 - FMC/FMC_NOR/Inc/stm32h563ZI_nucleo_conf.h     BSP configuration file
 - FMC/FMC_NOR/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
 - FMC/FMC_NOR/Inc/main.h                      Header for main.c module 
 - FMC/FMC_NOR/Inc/stm32h5xx_it.h              Header for stm32h5xx_it.c
 - FMC/FMC_NOR/Src/main.c                      Main program
 - FMC/FMC_NOR/Src/stm32h5xx_hal_msp.c         HAL MSP module  
 - FMC/FMC_NOR/Src/stm32h5xx_it.c              Interrupt handlers
 - FMC/FMC_NOR/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file 
    

### <b>Hardware and Software environment</b>
 - This example runs on STM32H563ZITx devices.

 - This example has been tested with HW other than NUCLEO-H563ZI board on which only compilation is done. 
      
### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

