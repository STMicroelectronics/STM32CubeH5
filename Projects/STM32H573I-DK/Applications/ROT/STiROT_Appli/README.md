## <b>STiROT_Appli application Description</b>

This project provides a STiROT boot path application example. Boot is performed through STiROT boot path after authenticity and the integrity checks of the project firmware and project data
images.

This project is targeted to build a <u>full secure application</u>. When the secure application is started the MPU
is already configured (by STiROT) to limit the execution area to the project firmware execution slot. This is done in order to avoid
any malicious code execution from an unauthorised area (RAM, out of execution slot in user flash ...). Once started, it is up to the secure
application to adapt the security configuration to its needs. In this example, the MPU is simply disabled.

The application will display a menu on the console allowing to download through standard bootloader a new version
of the application firmware and the application data images.
At next reset, the STiROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.

The maximum system clock frequency at 250Mhz is configured.

### <b>Keywords</b>

TrustZone, STiROT, boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - ROT/STiROT_Appli/Src/com.c                       UART low level interface
  - ROT/STiROT_Appli/Src/loader.c                    System configuration before calling bootloader
  - ROT/STiROT_Appli/Src/low_level_obkeys.c          Obkeys Low level services
  - ROT/STiROT_Appli/Src/low_level_security.c        Security Low level services
  - ROT/STiROT_Appli/Src/main.c                      Main program
  - ROT/STiROT_Appli/Src/startup_stm32h573xx.s       Startup file
  - ROT/STiROT_Appli/Src/stm32h5xx_it.c              Interrupt handlers
  - ROT/STiROT_Appli/Src/system_stm32h5xx.c          STM32H5xx system source file
  - ROT/STiROT_Appli/Src/test_versions.c             Print application version
  - ROT/STiROT_Appli/Inc/com.h                       Header file for com.c
  - ROT/STiROT_Appli/Inc/low_level_obkeys.h          Header file for low_level_obkeys.c
  - ROT/STiROT_Appli/Inc/low_level_security.h        Header file for low_level_security.c
  - ROT/STiROT_Appli/Inc/main.h                      Header for main.c module
  - ROT/STiROT_Appli/Inc/stm32h5xx_hal_conf.h        HAL configuration file
  - ROT/STiROT_Appli/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - ROT/STiROT_Appli/Inc/test_versions.h             Header for test_versions.c file
  - ROT/STiROT_Appli/Binary                          Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

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

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist



