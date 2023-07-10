## <b>OEMiROT_Appli_TrustZone application Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT boot path after authenticity and the integrity checks of the project firmware and project data
images.

This project is composed of two sub-projects:

- One for the secure application part (Project_s)

- One for the non-secure application part (Project_ns).


Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application. When the secure application is started the MPU
is already configured (by OEMiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the secure
application to adapt the security configuration to its needs. In this example, the MPU is simply disabled.

This project mainly shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories.
The split of the internal Flash depends on OEMiROT configuration done through the provisioning steps.
The split of the internal SRAM memories is independent from provisioning process and can be updated
at any time before compilation steps.

The non-secure application will display a menu on the console allowing to download through standard bootloader a new version
of the application firmware and the application data images.
At next reset, the OEMiROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.

The maximum system clock frequency at 250Mhz is configured in non-secure application.

### <b>Keywords</b>

TrustZone, OEMiROT, boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - ROT/OEMiROT_Appli_TrustZone/Secure/Src/low_level_flash.c              Secure Low level flash driver
  - ROT/OEMiROT_Appli_TrustZone/Secure/Src/main.c                         Secure Main program
  - ROT/OEMiROT_Appli_TrustZone/Secure/Src/secure_nsc.c                   Secure Non-Secure Callable (NSC) module
  - ROT/OEMiROT_Appli_TrustZone/Secure/Src/stm32h5xx_it.c                 Secure Interrupt handlers
  - ROT/OEMiROT_Appli_TrustZone/Secure/Src/system_stm32h5xx_s.c           Secure STM32H5xx system clock configuration file
  - ROT/OEMiROT_Appli_TrustZone/Secure/Inc/low_level_flash.h              Secure Low level flash driver header file
  - ROT/OEMiROT_Appli_TrustZone/Secure/Inc/main.h                         Secure Main program header file
  - ROT/OEMiROT_Appli_TrustZone/Secure/Inc/partition_stm32h573xx.h        STM32H5xx Device System Configuration file
  - ROT/OEMiROT_Appli_TrustZone/Secure/Inc/stm32h5xx_hal_conf.h           Secure HAL Configuration file
  - ROT/OEMiROT_Appli_TrustZone/Secure/Inc/stm32h5xx_it.h                 Secure Interrupt handlers header file
  - ROT/OEMiROT_Appli_TrustZone/Secure_nsclib/appli_flash_layout.h        Flash layout header file
  - ROT/OEMiROT_Appli_TrustZone/Secure_nsclib/secure_nsc.h                Secure Non-Secure Callable (NSC) module header file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/com.c                       Non-secure Communication
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/common.c                    Non-secure common
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_flash.c           Non-Secure Low level flash driver
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/main.c                      Non-secure Main program
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/stm32h5xx_it.c              Non-secure Interrupt handlers
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/system_stm32h5xx_ns.c       Non-secure STM32H5xx system clock configuration file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Inc/com.h                       Non-secure Communication header file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Inc/common.h                    Non-secure common header file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Src/low_level_flash.h           Non-Secure Low level flash driver header file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Inc/main.h                      Non-secure Main program header file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32h5xx_hal_conf.h        Non-secure HAL Configuration file
  - ROT/OEMiROT_Appli_TrustZone/NonSecure/Inc/stm32h5xx_it.h              Non-secure Interrupt handlers header file
  - ROT/OEMiROT_Appli_TrustZone/Binary                                    Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

This project is targeted to boot through <b>OEMiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
of project_s and project_ns as well as the postbuild commands will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/OEMiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED, TZ-CLOSED or CLOSED, it is still possible to open the debug or to execute a full/partial regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security.

After application startup, check in your "UART console" the menu is well displayed:
  ```
  =================== Main Menu =============================
   Start BootLoader -------------------------------------- 1
   Display Data ------------------------------------------ 2
   Selection :
  ```

To update the application firmware and/or the application data image version, you must:

  - Select the function "Start Bootloader"
  - Connect STM32CubeProgrammer through supported device (USART/SPI/I2C/I3C/USB/FDCAN)
  - Download the new image(s)
  - Reset the board
  - After authenticity and intergrity checks the new images are decrypted and installed.

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



