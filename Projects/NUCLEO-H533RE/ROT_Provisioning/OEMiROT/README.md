## <b>ROT_Provisioning OEMiROT Description</b>

This section provides an overview of the available scripts for OEMiROT boot path.

OEMiROT stands for "OEM Immutable Root of Trust" and it provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- OEMiROT/Binary                               Output binaries and generated images.
- OEMiROT/Config                               OEMiROT configuration files.
- OEMiROT/Image                                Image configuration files.
- OEMiROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- OEMiROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- OEMiROT/obkey_programming.bat/.sh            Programs OBKeys on the device.
- OEMiROT/provisioning.bat/.sh                 Performs device provisioning process.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics NUCLEO-H533RE (MB1814)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

To use OEMiRoT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts and the application files will
be automatically updated according to OEMiRoT configuration, and user answers.

The **provisioning process** (OEMiROT/provisioning.bat/.sh) is divided into 3 majors steps:

-   Step 1: Configuration management
-   Step 2: Images generation
-   Step 3: Provisioning

The provisioning script is relying on ob_flash_programming, obkey_provisioning and obkey_provisioning_open scripts.

For more details, refer to STM32H533 Wiki articles:

  - [OEMiRoT OEMuRoT for STM32H5](https://wiki.st.com/stm32mcu/wiki/Security:OEMiRoT_OEMuRoT_for_STM32H5)
  - [How to start with OEMiRoT on STM32H533](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H533)
  - [How_to_create_h5_example] (https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_ROT_examples_for_STM32H5)

### <b>Notes</b>

AppliCfg.py (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning process.
It is needed to have python and some python packages installed in your environment.
Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.
