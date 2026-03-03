## <b>XSPI_MemoryToolbox_M2_W25Q16JV Example Description</b>

How to use a XSPI Winbond W25Q16JV memory in memory-mapped mode.

This example demonstrates how to use an XSPI Winbond W25Q16JV memory in memory-mapped mode on the STM32H5 series.
It provides a reference for integrating XSPI Winbond W25Q16JV memory in embedded applications requiring fast,
 flexible, and reliable external storage.
Memory used in this example is the Winbond W25Q16JV.

- This project is targeted to run on STM32H5E5xx devices on NUCLEO-H5E5ZJ boards from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 Mhz.
- The example project calls also HAL_ICACHE_Enable() function in order to enable the Instruction
Caches. This function is provided as template implementation that the User may
integrate in his application in order to enhance the performance.
- The Winbond W25Q16JV memory is present on the MB1928-33LB board that is connected to the M2 connector (CN19)
of the NUCLEO-H5E5ZJ board.

The project covers following key features:

- JEDEC ID reading
- SFDP table reading and parsing for device parameter discovery
- Write Enable and Write Disable command test
- Quad mode configuration for high-speed data transfers
- Erase, write, and read operations on XSPI NOR memory
- Memory-mapped mode for direct access to NOR memory
- Status and configuration register read/write support
- Data comparison and copy utilities

Implementation of each element of these features is provided thanks to unitary functions implemented in memory_toolbox.c/.h files,
and called from `main()` function.

#### <b>Detailed scenario</b>

The application executes the following example steps:

Init : Configures GPIO, USART3 (used for Trace system), LED, ... and configures and initializes the XSPI instance. 
Default frequency at 50 Mhz is used.

Step 1: Read memory JEDEC ID.

Step 2: Read memory SFDP Table.

Step 3: Check WEL bit value in Status Register after sending Write Enable and Write Disable commands 
        (to ensure Write Enable command is properly handled by memory).

Step 4: Memory specific configuration : Winbond W25Q16JV registers

- Status register reading
- Status register 3 reading + writing with updated value for Drive Strength (to be adapted according to used HW)
- Status register 2 reading + writing with updated value for Quad mode enabling
- Clock frequency update and DLYB calibration.

Step 5: Write and read data using indirect and memory mapped modes :

- Step 5.1 : Enable write operation (WE bit) and erase sector (and read a page to check data has been erased. 
             All data are expected to be 0xFF)
- Step 5.2 : Enable write operation (WE bit), program memory page
- Step 5.3 : Read data in Indirect mode and compare content of read buffer versus expected one
- Step 5.4 : Enable Memory Mapped mode
- Step 5.5 : Read data in Memory Mapped mode and compare data with written one. 
             In memory mapped mode, read buffer filling with memory content 
             could be achieved thanks to a memory to memory service as memcpy()
- Step 5.6 : Disable Memory Mapped mode

End of example: The example runs indefinitely in while(1) loop at end of main(). If an error occurs, Error_Handler() is called.

Visual feedback of project execution is provided via LEDs for operation status and error indication :

- LED_GREEN is ON when program execution completes successfully.
- LED_RED is ON as soon as an error is encountered.

#### <b>Trace and Debug</b>


This example also includes an optional trace/debug output system.
When the `EXAMPLE_TRACE_ENABLE` compilation switch is defined in project compilation options,
detailed trace messages are generated during memory operations.
The trace output can be directed either to a UART interface (USART3 on NUCLEO-H5E5ZJ board) or to the IDE debug window,
depending on whether the `EXAMPLE_TRACE_OUTPUT_UART` or `EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW` switch is enabled.
If Trace system is enabled, one and only one of those 2 switches should be defined.
This allows flexible selection of the trace destination for debugging and diagnostics.

If `EXAMPLE_TRACE_OUTPUT_UART` is selected (default project configuration):
Traces could be retrieved using any PC Terminal application 
connected to board Virtual COM Port with 115200 bauds/8-bit data/1 stop bit/no parity configuration.

If `EXAMPLE_TRACE_OUTPUT_DEBUG_WINDOW` is selected:
- When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O
- When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer with proper configuration:
   - Add the "__DBG_ITM" to MDK-ARM Settings --> C/C++ (AC6) --> Preprocessor Symbols --> Define
   - In Settings --> Debug --> Settings --> Trace, enable the "Trace Enable" option
   - In Settings --> Target, enable the "Use MicroLIB" option
   - Include "Retarget.c" and "Serial.c" files in target build
   - Build and run the example
 - When resorting to STM32CubeIDE (Debug and Release):
   Command Code is displayed on debugger as follows: Window--> Show View--> Console with proper configuration:
   - Exclude "syscall.c" file from build
   - Add "-specs=rdimon.specs -lc -lrdimon" flag to Project → Properties → C/C++ Build → Settings
   --> Tool Settings --> MCU GCC linker --> Miscellaneous
   - Compile the example (Debug ou Release configuration); the elf file is required to configure the debug profile
   - Open the menu [Run]->[Debug configuration] and double click on [STM32 C/C++ Application] (it creates a default configuration for the current project selected):
     - When using ST-LINK (OpenOCD): 
       - In [Debugger] tab, select the Debug probe : ST-LINK(OpenOCD)
       - In [Startup] tab, add the command "monitor arm semihosting enable"
	   - Click Apply to save the changes and start the debug session
     - When using ST-LINK (ST-LINK GDB server):
       - In [Debugger] tab, select the Debug probe : ST-LINK(ST-LINK GDB server) and select the semihosting: Enable terminal mode or Enable terminal and File I/O mode  
       - Click Apply to save the changes and start the debug session
Trace output system could be configured, allowing users to select the desired level of trace detail at runtime
using the `Memory_Toolbox_SetTraceLevel()` function.
Available trace levels include disabling trace output, logging only command and step information,
or enabling full trace with buffer contents.
This flexibility helps users tailor the verbosity of debug information to their needs, making it easier to monitor,
diagnose, and optimize memory operations during development and testing.

Aside from output traces, content of all Status and Configuration registers is available in `memory_toolbox_register_dump[]` array.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. Due to an issue related to semihosting management in Keil v5.38, Keil v5.39 or higher is recommended.
 
### <b>Keywords</b>

Memory, XSPI, Read, Write, Memory Mapped, NorFlash, W25Q16JV, M2

### <b>Directory contents</b>

      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/main.c                     Main program
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/system_stm32h5xx.c         STM32H5x system source file
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/stm32h5xx_it.c             Interrupt handlers
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/memory_toolbox.c           Memory management basic services
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Inc/main.h                     Header for main.c module
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/memory_toolbox.h           Header for memory_toolbox.c module
      - XSPI/XSPI_MemoryToolbox_M2_W25Q16JV/Src/m2_w25q16jv.h              Definitions for Winbond W25Q16JV memory

### <b>Hardware and Software environment</b>

  - This example runs on STM32H5E5xx devices .

  - This example has been tested with STMicroelectronics NUCLEO-H5E5ZJ board (MB2129) with
    MB1928-33LB board connected to the M2 connector (CN19) and can be easily tailored
    to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files
  - Load images into target memory
  - Run the example


