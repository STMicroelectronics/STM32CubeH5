## <b>OEMiROT_Appli application Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT boot path after authenticity and the integrity checks of the project firmware and project data
images.

This project is targeted to build a <u>full secure application</u>. When the secure application is started the MPU
is already configured (by OEMiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the secure
application to adapt the security configuration to its needs. In this example, the MPU is simply disabled.

The secure application will display a menu on the console allowing to download through standard bootloader a new version
of the application firmware and the application data images.
At next reset, the OEMiROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.

The maximum system clock frequency at 250Mhz is configured.

### <b>Keywords</b>

TrustZone, OEMiROT, boot path, Root Of Trust, Security, MPU

### <b>Directory contents</b>

  - ROT/OEMiROT_Appli/Src/com.c                                 UART low level interface
  - ROT/OEMiROT_Appli/Src/loader.c                              System configuration before calling bootloader
  - ROT/OEMiROT_Appli/Src/low_level_device.c                    Flash Low level device setting
  - ROT/OEMiROT_Appli/Src/low_level_flash.c                     Secure Low level flash driver
  - ROT/OEMiROT_Appli/Src/main.c                                Secure Main program
  - ROT/OEMiROT_Appli/Src/stm32h5xx_it.c                        Secure Interrupt handlers
  - ROT/OEMiROT_Appli/Src/system_stm32h5xx.c                    Secure STM32H5xx system clock configuration file
  - ROT/OEMiROT_Appli/Inc/low_level_flash.h                     Secure Low level flash driver header file
  - ROT/OEMiROT_Appli/Inc/main.h                                Secure Main program header file
  - ROT/OEMiROT_Appli/Inc/partition_stm32h573xx.h               STM32H5xx Device System Configuration file
  - ROT/OEMiROT_Appli/Inc/stm32h5xx_hal_conf.h                  Secure HAL Configuration file
  - ROT/OEMiROT_Appli/Inc/stm32h5xx_it.h                        Secure Interrupt handlers header file
  - ROT/OEMiROT_Appli/Secure_nsclib/appli_flash_layout.h        Flash layout header file
  - ROT/OEMiROT_Appli/Binary                                    Application firmware image (application binary + MCUBoot header and metadata)

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
as well as the postbuild command of the project will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_appli_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/OEMiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED, TZ-CLOSED or CLOSED, it is still possible to open the debug or to execute a full/partial regression
with the Debug Authentication feature. To do it, scripts (regression.bat & dbg_auth.bat) are available in the ROT_provisioning/DA folder.

For more details, refer to Wiki article available here : https://wiki.st.com/stm32mcu/wiki/Category:Security.

After application startup, check in your "UART console" the menu is well displayed:
  ```
  =================== Main Menu =============================
   Start BootLoader -------------------------------------- 1
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

  2. AppliCfg.py (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning process.
     It is needed to have python and some python packages installed in your environment.
     Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.



