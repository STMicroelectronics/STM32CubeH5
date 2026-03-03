## <b>ROT_Provisioning STiROT_OEMuROT Description</b>

This section provides an overview of the available scripts for STiROT_OEMuROT boot path.

STiROT stands for "ST Immutable Root of Trust".<br>
OEMuROT stands for "OEM Updatable Root of Trust".<br>
The 2 boot stages provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

STiRoT, OEMuROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- STiROT_OEMuROT/Binary                               Output binaries and generated images.
- STiROT_OEMuROT/Config                               STiRoT and OEMuROT configuration files.
- STiROT_OEMuROT/Image                                Image configuration files.
- STiROT_OEMuROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- STiROT_OEMuROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- STiROT_OEMuROT/obkey_provisioning.bat/.sh           Programs OBKeys on the device.
- STiROT_OEMuROT/provisioning.bat/.sh                 Performs device provisioning process.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics NUCLEO-H533RE (MB1814)
  board and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

To use STiRoT_OEMuRoT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts, the OEMuRoT and the application files will
be automatically updated according to STiRoT_OEMuRoT configuration, and user answers.

The **provisioning process** (STiRoT_OEMuRoT/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 1: Configuration management
- Step 2: Images generation
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming and obkey_provisioning scripts.

For more details, refer to STM32H533 Wiki articles:

  - [STiRoT OEMuRoT for STM32H5](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32H5)
  - [How to start with STiRoT OEMiRoT on STM32H573](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_OEMuRoT_on_STM32H573)

### <b>Notes</b>

AppliCfg.py (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning process.
It is needed to have python and some python packages installed in your environment.
Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.
