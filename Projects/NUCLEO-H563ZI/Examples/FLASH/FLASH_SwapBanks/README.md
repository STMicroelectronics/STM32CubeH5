## <b>FLASH_SwapBanks Example Description</b>


Guide through the configuration steps to program internal Flash memory bank 1
and bank 2, and to swap between both of them by mean of the FLASH HAL API.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H563xx Devices :


Below are the steps to run this example:

1- Choose the FLASH_SwapBanks_Bank2 project and build it using your preferred toolchain (EWARM, MDK-ARM, STM32CubeIDE),then use STM32CubeProgrammer to flash the 
  generated binary file FLASH_BANK2.bin into the bank2 at the address 0x08100000.
##### <b>note:</b>
      You have to configure your preferred toolchain in order to generate the binary
      file after compiling the project.

2- Choose the FLASH_SwapBanks_Bank1 project and run it, this project will be loaded
   in the bank1 of the flash: at the address 0x08000000

3- Click the BUTTON_USER button to swap between the two banks,

- If program is executing from bank1 is selected, LED1 toggles,

- If program is executing from bank2 is selected, LED2 toggles,


##### <b>note</b>
      Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

##### <b>note </b>
       The example need to ensure that the SysTick time base is always set to 1 millisecond
       to have correct HAL operation.

##### <b>note</b>
       After executing this example , the SWAP_BANK option byte will be set ,
       Use STM32CubeProgrammer to change this option byte back to "Not swapped" if needed.


### <b> Keywords </b>

Memory, FLASH, Swap, Bank, Dual bank, Program, Sector, LED

### <b> Directory contents </b>

  - FLASH/FLASH_SwapBanks/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
  - FLASH/FLASH_SwapBanks/Inc/stm32h5xx_nucleo_conf.h     STM32H5xx_Nuleo board configuration file
  - FLASH/FLASH_SwapBanks/Inc/stm32h5xx_it.h              Header for stm32h5xx_it.c
  - FLASH/FLASH_SwapBanks/Inc/main.h                      Header for main.c module
  - FLASH/FLASH_SwapBanks/Src/stm32h5xx_it.c              Interrupt handlers
  - FLASH/FLASH_SwapBanks/Src/main.c                      Main program
  - FLASH/FLASH_SwapBanks/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file

### <b> Hardware and Software environment </b>

  - This example runs on STM32H563 devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b> How to use it ? </b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain.
   - Rebuild all files.
  - Load the binary file generated with FLASH_SwapBanks_Bank2 into
    the BANK2 at address 0x08100000 using STM32CubeProgrammer.
  - Choose the FLASH_SwapBanks_Bank1 project and run the example.

  <b>STM32CubeIDE</b>
 One linker script is needed for this example where the FLASH shall be defined at origin: 0x8000000
	This is because The CPU expects all code to be executed from this address.
	The SWAP_BANK bit simply “toggles” which bank is located at @0x8000000.
	To actually load one of the binaries into 0x8100000 you have to setup the debug config to load one bin-file with 0x100000:
	select "Debug configuration" or "Run configuration" in function of the active build FLASH_SwapBanks_Bank1 configuration
   - Double click on “STM32 Cortex-M C/C++ Application”
   - Select  “Startup” >  “Add” > 
   - Select the FLASH_SwapBanks_Bank2 project 
   - Uncheck Perform build option
   - Add 0x100000 to Use download offset hex
   - Click Debug/Run to debug/run the example
   STM32CubeIDE loses debug connection when the application toggles the SWAP_BANK option bit. Example can be compiled and executed on target, but debug does not work. 
   Target is not responding, retrying...
   Target is not responding, retrying...
   Shutting down...
