## <b>STiROT_Appli Template Description</b>

This project provides a STiROT boot path reference template. Boot is performed through STiROT boot path after authenticity and the integrity checks of the project firmware and data
images.

This project template is based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=B4).

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock (SYSCLK)

This project is targeted to build a <u>full secure application</u>. When the secure application is started the MPU
is already configured (by STiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the secure
application to adapt the security configuration to its needs. In this template, the MPU is simply disabled.

The maximum system clock frequency at 250Mhz is configured.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

TrustZone, STiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - STiROT_Appli/Inc/main.h                      Header for main.c module
  - STiROT_Appli/Inc/stm32h5xx_hal_conf.h        HAL configuration file
  - STiROT_Appli/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - STiROT_Appli/Inc/stm32h573i_discovery_conf.h BSP configuration file
  - STiROT_Appli/Src/main.c                      Main program
  - STiROT_Appli/Src/stm32h5xx_hal_msp.c         HAL MSP file
  - STiROT_Appli/Src/stm32h5xx_it.c              Interrupt handlers
  - STiROT_Appli/Src/system_stm32h5xx.c          STM32H5xx system source file
  - STiROT_Appli/Binary                          Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This template runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This template has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

This project is targeted to boot through <b>STiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
as well as the postbuild command of the project will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through stirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/STiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED, TZ-CLOSED or CLOSED, it is still possible to open the debug or to execute a full/partial regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security

#### <b>Notes:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in the Open product state by setting with
     STM32CubeProgrammer the UBE to 0xB4 and the SECBOOTADD to (0x0C000400 + offset of the firmware execution slot).

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist



