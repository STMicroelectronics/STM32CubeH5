## <b>OEMiROT_Appli Template Description</b>

This project provides a OEMiROT boot path reference template. Boot is performed through OEMiROT boot path after authenticity and integrity checks of the project firmware and project data
images.

This project template is based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=B4).

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock (SYSCLK)

This project is targeted to build a <u>full secure application</u>. When the secure application is started the MPU
is already configured (by OEMiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
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

TrustZone, OEMiROT, Boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - OEMiROT_Appli/Inc/main.h                         Header file for main.c
  - OEMiROT_Appli/Inc/partition_stm32h573xx.h        STM32H5xx Device System Configuration file
  - OEMiROT_Appli/Inc/stm32h5xx_hal_conf.h           HAL configuration file
  - OEMiROT_Appli/Inc/stm32h5xx_it.h                 Header file for stm32h5xx_it.h
  - OEMiROT_Appli/Inc/stm32h573i_discovery_conf.h    BSP configuration file
  - OEMiROT_Appli/Src/main.c                         Secure Main program
  - OEMiROT_Appli/Src/stm32h5xx_hal_msp.c            Secure HAL MSP file
  - OEMiROT_Appli/Src/stm32h5xx_it.c                 Secure Interrupt handlers
  - OEMiROT_Appli/Src/system_stm32h5xx.c             Secure STM32H5xx system clock configuration file
  - OEMiROT_Appli/EWARM/startup_stm32h5xx.s          Startup file
  - OEMiROT_Appli/Binary                             Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This template runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This template has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

This project is targeted to boot through <b>OEMiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
as well as the postbuild command of the project will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_appli_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/OEMiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED, TZ-CLOSED or CLOSED, it is still possible to open the debug or to execute a full/partial regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security

#### <b>Notes:</b>

  1. The most efficient way to develop and debug an application is to boot directly on user flash in the Open product state by setting with
     STM32CubeProgrammer the UBE to 0xB4 and the SECBOOTADD to (0x0C000400 + offset of the firmware execution slot).

  2. AppliCfg.py (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning process.
     It is needed to have python and some python packages installed in your environment.
     Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.



