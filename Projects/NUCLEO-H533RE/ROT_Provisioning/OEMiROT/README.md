## <b>ROT_Provisioning OEMiROT Description</b>

This section provides available configuration scripts for OEMiROT example.

To ease the configuration process, ob_flash_programming, obkey_programming and provisioning scripts are used.
The ob_flash_programming script is in charge of removing the protections, initializing and configuring the option bytes and download the images.
The obkey_programming script is in charge of configuring option byte keys (OBKeys).
The provisioning script is in charge of target provisioning.


### <b>Keywords</b>

OEMiROT, TrustZone, boot path, Root Of Trust, Security, mcuboot


### <b>Directory contents</b>

- OEMiROT/provisioning.bat/.sh                  Target provisioning.
- OEMiROT/obkey_programming.bat/.sh             Configure option byte keys(OBKeys).
- OEMiROT/ob_flash_programming.bat/.sh          Option bytes Initialization and Image download.
- OEMiROT/Binary                                It contains generated binary data images and obk binary.
- OEMiROT/Images/OEMiROT_S_Code_Image.xml       Configuration file for Secure Firmware image generation.
- OEMiROT/Images/OEMiROT_S_Data_Image.xml       Configuration file for Secure Data image generation.
- OEMiROT/Images/OEMiROT_NS_Code_Image.xml      Configuration file for NonSecure Firmware image generation.
- OEMiROT/Images/OEMiROT_NS_Data_Image.xml      Configuration file for NonSecure Data image generation.
- OEMiROT/Keys/OEMiRoT_Authentication_S.pem     Private key for secure authentication.
- OEMiROT/Keys/OEMiRoT_Authentication_NS.pem    Private key for nonsecure authentication.
- OEMiROT/Keys/OEMiRoT_Encryption.pem           Private key for encryption.
- OEMiROT/Keys/OEMiRoT_Encryption_Pub.pem       Public key for encryption.
- OEMiROT/Config/OEMiRoT_Config.xml             Configuring keys for authentication and encryption.


### <b>Hardware and Software environment</b>

- This example runs on STM32H533xx devices with security enabled (TZEN=B4).
- This example has been tested with STMicroelectronics NUCLEO-H533RE (MB1814)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files
of project as well as the postbuild commands will be automatically updated.

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

The **provisioning process** (OEMiROT/provisioning.bat/.sh) is divided into 3 majors steps :

  Step 1 : Configuration management

    * OEMiROT_Config.obk generation : Using TPC, regenerate the keys through
      OEMiROT_Config.xml.(ROT_Provisioning/OEMiROT/Config/OEMiRoT_Config.xml)

    * DA_Config.obk generation : Using TPC, generate certificates, update the keys and permissions through
      DA_Config.xml.(ROT_Provisioning/DA/Config/DA_Config.xml)

  Step 2 : Image generation

    * Boot firmware image generation : Build the OEMiROT_Boot project with the preferred toolchain.

    * Code firmware image generation : Build the OEMiROT_Appli_TrustZone project with the preferred toolchain.

    * Data generation : Data image generation with TrustedPackageCreator, if data image enabled.

  Step 3 : Provisioning

    * Program the option bytes and flash the images using ob_flash_programming.bat/.sh.

    * Define the final product state value. (OPEN/PROVISIONED/CLOSED/LOCKED)

The provisioning script is relying on ob_flash_programming and obkey_programming scripts.

It is possible to run ob_flash_programming.bat/.sh and obkey_programming.bat/.sh directly (advanced mode).

- The **ob_flash_programming process** (OEMiROT/ob_flash_programming.bat/.sh) includes :

  Step 1 : Initialize option bytes

  Step 2 : Configure the option Bytes

    * Remove Protection and erase all.
    * Configure option Bytes.

  Step 3 : Download Images

    * Download boot image.
    * Download Appli image.
    * Download data image, if data image enabled.


## Environment Setup

  All scripts rely on env.bat and env.sh for setting the necessary tools path and application path.
    - File path : ROT_Provisioning/env.bat/.sh.
    - Purpose : Sets the necessary tools path and application path for Windows, Linux and Mac operating system.
    - Usage : Exporting the Environment Variables, Path Configuration, Dependency Setup, Perform initial setup tasks.


## Additional Resources

  * [OEMiRoT_STM32H533] (https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H533)
