## <b>OEMiROT_Appli Template Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT bootpath after authenticity and integrity checks of the project firmware and project data 
images.
    
This project template is based on the STM32Cube HAL API.

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock (SYSCLK)
    
The maximum system clock frequency at 250Mhz is configured in non-secure application.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

OEMiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - OEMiROT_Appli/Inc/main.h                  Header file for main.c
  - OEMiROT_Appli/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - OEMiROT_Appli/Inc/stm32h5xx_it.h          Header file for stm32h5xx_it.h
  - OEMiROT_Appli/Src/main.c                  Secure Main program
  - OEMiROT_Appli/Src/stm32h5xx_hal_msp.c     Secure HAL MSP file
  - OEMiROT_Appli/Src/stm32h5xx_it.c          Secure Interrupt handlers
  - OEMiROT_Appli/Src/system_stm32h5xx_s.c    Secure STM32H5xx system clock configuration file
  - OEMiROT_Appli/EWARM/startup_stm32h5xx.s   Startup file
  - OEMiROT_Appli/Binary                      Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503xx devices.  
  - This example has been tested with STMicroelectronics NUCLEO-H503RB (MB1814)
    board and can be easily tailored to any other supported device
    and development board.  
	
### <b>How to use it ?</b>

This project is targeted to boot through <b>OEMiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker file
as well as the postbuild command will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/OEMiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED or CLOSED, it is still possible to execute a regression
with the Debug Authentication feature. To do it, scripts (regression.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security

#### <b>Notes:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in the Open product state by setting with 
     STM32CubeProgrammer the UBE to 0xB4 and the NSBOOTADD to (0x08000400 + offset of the firmware execution slot)

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It 
     is possible to switch to python version by:  
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.  
        ```
        pip install -r requirements.txt  
        ```
        - having python in execution path variable  
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist
     
     
     