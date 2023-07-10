## <b>OEMiROT_Appli application Description</b>

This project provides a OEMiROT boot path application example. Boot is performed through OEMiROT boot path after authenticity and the integrity checks of the project firmware and project data
images.

The application will display a menu on the console allowing to download through standard bootloader a new version
of the application firmware and the application data images.
At next reset, the OEMiROT will decrypt and install these new versions after successful check of the authenticity and the integrity of each image.

The maximum system clock frequency at 250Mhz is configured.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, MPU

### <b>Directory contents</b>

  - OEMiROT_Appli/Src/com.c                       Communication
  - OEMiROT_Appli/Src/main.c                      Main program
  - OEMiROT_Appli/Src/startup_stm32h503xx.s       Startup file
  - OEMiROT_Appli/Src/stm32h5xx_it.c              Interrupt handlers
  - OEMiROT_Appli/Src/system_stm32h5xx.c          STM32H5xx system clock configuration file
  - OEMiROT_Appli/Src/appli_flash_layout.h        Flash layout header file
  - OEMiROT_Appli/Src/com.h                       Communication header file
  - OEMiROT_Appli/Inc/main.h                      Main program header file
  - OEMiROT_Appli/Inc/stm32h5xx_hal_conf.h        HAL Configuration file
  - OEMiROT_Appli/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - OEMiROT_Appli/Binary                          Application firmware image (application binary + MCUBoot header and metadata)

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503xx devices.
  - This example has been tested with STMicroelectronics NUCLEO-H503RB (MB1814)
    board and can be easily tailored to any other supported device
    and development board.
  - To print the application menu in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it ?</b>

This project is targeted to boot through <b>OEMiROT boot path</b>.

<u>Before compiling the project, you should first start the provisioning process</u>. During the provisioning process, the linker files
of project as well as the postbuild commands will be automatically updated.

Before starting the provisioning process, select the application project to use (application example or template),
through oemirot_boot_path_project variable in ROT_Provisioning/env.bat or env.sh.
Then start provisioning process by running ROT_Provisioning/OEMiROT/provisioning.bat or provisioning.sh, and follow its instructions.

If the product state set to PROVISIONED or CLOSED, it is still possible to open the debug or to execute a regression
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
     STM32CubeProgrammer the NSBOOTADD to (0x08000400 + offset of the firmware execution slot).

  2. Two versions of ROT_AppliConfig are available: windows executable and python version. By default, the windows executable is selected. It
     is possible to switch to python version by:
        - installing python (Python 3.10 or newer) with the required modules listed in requirements.txt.
        ```
        pip install -r requirements.txt
        ```
        - having python in execution path variable
        - deleting main.exe in Utilities\PC_Software\ROT_AppliConfig\dist



