## <b>Templates Example Description</b>

- This project provides a reference template through the HAL API that can be used
to build any firmware application.
- This project is targeted to run on STM32H553xx devices on NUCLEO-H553ZG boards from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the SysTick.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 250 MHz.
- The template project also calls CACHE_Enable() in order to enable the Instruction
and Data Caches. This function is provided as a template implementation that the user may
integrate into their application in order to enhance performance.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on a variable incremented in the SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.  
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Reference, template

### <b>Directory contents</b>

      - Templates/TrustZoneDisabled/Src/main.c                     Main program
      - Templates/TrustZoneDisabled/Src/system_stm32h5xx.c         STM32H5x system source file
      - Templates/TrustZoneDisabled/Src/stm32h5xx_it.c             Interrupt handlers
      - Templates/TrustZoneDisabled/Src/stm32h5xx_hal_msp.c        Board specific setup file
      - Templates/TrustZoneDisabled/Inc/main.h                     Header for main.c module
      - Templates/TrustZoneDisabled/Inc/stm32h5xx_it.h             Interrupt handlers header file
      - Templates/TrustZoneDisabled/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
      - Templates/TrustZoneDisabled/Inc/stm32h5xx_nucleo_conf.h    Nucleo BSP configuration file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H553xx devices.

  - This template has been tested with STMicroelectronics NUCLEO-H553ZG (MB2321)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it?</b>

In order to make the program work, you must do the following:

  - Open your preferred toolchain
  - Rebuild all files
  - Load images into target memory
  - Run the example

#### <span style="color: #e6007e;"><b>ARM-STUDIO</b></span>

This project is set up for an Arm CMSIS-Toolbox workflow (CMSIS Solution / *csolution* + *cbuild*) and targets STM32H553xx on the NUCLEO-H553ZG.

#### <b>1) How to install Arm Studio in Visual Studio Code</b>

This repository is designed to be used with **Visual Studio Code** and the Arm/Keil tooling (often referred to as “Arm Studio” in internal documentation).

- Install **Visual Studio Code**.
- Install the Arm/CMSIS extensions:
  - In VS Code, open **Extensions** (Ctrl+Shift+X).
  - Search for and install **Arm Keil Studio Pack**.
  - Ensure **CMSIS-Toolbox / CMSIS Solution** support is enabled (these provide `csolution`, `cbuild`, and the CMSIS views/commands).
- Install required toolchain prerequisites:
  - **Arm Compiler 6 (AC6)** (this project is configured for `compiler: AC6` in the CMSIS solution).
  - **minimum Python version 3.27** 
  - **pyOCD** *(used as the debug adapter for flashing/debug; install via `pip install pyocd` if not already available)*
  - **ST-LINK USB driver** (for NUCLEO boards).

Where to find/enable it in VS Code:
- **Extensions** view for installation.
- **View → Command Palette** for CMSIS actions (search for “CMSIS”).
- **Terminal → Run Task…** for the provided `CMSIS *` tasks.
- **Run and Debug** for the `STLink@pyOCD (launch)` and `STLink@pyOCD (attach)` debug configurations.

> If you meant **Microsoft Visual Studio** (the full IDE): this repo does not include a dedicated Visual Studio extension. Use VS Code with the Arm/CMSIS extensions above (recommended).

#### <b>2) ARM-STUDIO folder</b>

The `ARM-Studio/` folder contains the CMSIS Solution workspace and the files VS Code uses to build, flash, and debug this project.

When you open the `ARM-Studio/` folder in VS Code, click the CMSIS icon; it should recognize the CMSIS Solution and load the available build contexts.

Key items to know:
- `Project.csolution.yml`, `Project.cproject.yml`, `Project.cbuild-*.yml`: CMSIS Solution and build configuration (defines target, device, toolchain, and build contexts).
- `.vscode/`:
  - `tasks.json`: pyOCD tasks for erase/load/run.
  - `launch.json`: debug configurations for **launch** (flash + debug) and **attach** (attach to an already-running gdbserver).
- `RTE/`: CMSIS Run-Time Environment configuration (notably `RTE_Components.h`).
- `out/`: build outputs (for example `out/Project/Templates_TrustZoneDisabled/Project.axf`).
- `tmp/`: generated intermediate build files.


#### <b>3) How to build and load the application</b>

##### <b>Build</b>

1. Open the `ARM-Studio/` folder in VS Code.
2. Build using one of the following:
  - Right-click `Project.csolution.yml` → **Build Solution**.
  - Alternatively, click the build icon in the CMSIS Solution explorer.
   
Expected output after a successful build:
- `out/Project/Templates_TrustZoneDisabled/Project.axf` (application image)
- `out/Project/Templates_TrustZoneDisabled/Project.axf.map` (linker map)

##### <b>Load / Flash to target (ST-LINK + pyOCD)</b>

1. Connect the NUCLEO-H553ZG board via ST-LINK USB.
2. Right-click `Project.csolution.yml` → **Load and Run** or **Load and Debug**.
3. Alternatively, use the Run/Debug icons from the CMSIS Solution explorer.




