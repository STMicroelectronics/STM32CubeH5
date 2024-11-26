## <b>STiROT_OEMuROT Description</b>

This section provides an overview of the available scripts for STiROT_OEMuROT.

It is used to first configure the STiRoT, debug authentication, and the OEMuRoT.
OEMuRoT is the updatable root of trust. It can be updated on a production device.


### <b>Keywords</b>

STiRoT, OEMuROT, TrustZone, boot path, Root Of Trust, Security


### <b>Directory contents</b>

- STiROT_OEMuROT/provisioning.bat/.sh                   Target provisioning.
- STiROT_OEMuROT/obkey_provisioning.bat/.sh             It configures Option byte keys(OBKeys).
- STiROT_OEMuROT/ob_flash_programming.bat/.sh           Option bytes Initialization and Image download.
- STiROT_OEMuROT/img_config.bat/.sh                     It configures the image number.
- STiROT_OEMuROT/Config/OEMuRoT_Config_Keys.xml         It contains the OEMuRoT configuration.
- STiROT_OEMuROT/Config/STiRoT_Config.xml               It contains the STiROT configuration.
- STiROT_OEMuROT/Keys/OEMuRoT_Authentication_NS.pem     Private key for NonSecure application authentication.
- STiROT_OEMuROT/Keys/OEMuRoT_Authentication_S.pem      Private key for Secure application authentication.
- STiROT_OEMuROT/Keys/OEMuRoT_Encryption.pem            Private key for application encryption.
- STiROT_OEMuROT/Keys/OEMuRoT_Encryption_Pub.pem        Public key for application encryption.
- STiROT_OEMuROT/Keys/STiRoT_Authentication.pem         Private key for OEMuROT authentication.
- STiROT_OEMuROT/Keys/STiRoT_Authentication_pub.pem     Public key for OEMuROT authentication.
- STiROT_OEMuROT/Keys/STiRoT_Encryption.pem             Private key for OEMuROT encryption.
- STiROT_OEMuROT/Keys/STiRoT_Encryption_pub.pem         Public key for OEMuROT encryption.
- STiROT_OEMuROT/Images/OEMuROT_NS_Code_Image.xml       Configuration file for NonSecure application Firmware image generation.
- STiROT_OEMuROT/Images/OEMuROT_NS_Data_Image.xml       Configuration file for NonSecure application Data image generation.
- STiROT_OEMuROT/Images/OEMuROT_S_Code_Image.xml        Configuration file for Secure application Firmware image generation.
- STiROT_OEMuROT/Images/OEMuROT_S_Data_Image.xml        Configuration file for Secure application Data image generation.
- STiROT_OEMuROT/Images/STiRoT_Code_Image.xml           Configuration file for OEMuROT Firmware image generation.
- STiROT_OEMuROT/Images/STiROT_Data_Image.xml           Configuration file for OEMuROT Data image generation.
- STiROT_OEMuROT/Binary                                 It contains generated binary data images and obk binary.
- STiROT_OEMuROT/ST/OEMuRoT_ST_Settings_1.xml           To generate the OEMuROT_Config.bin.
- STiROT_OEMuROT/ST/OEMuRoT_ST_Settings_2.xml           To generate the OEMuROT_Config.obk


### <b>Hardware and Software environment</b>

- This example runs on STM32H573xx devices with security enabled (TZEN=B4).
- This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it?</b>

Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files
of project as well as the postbuild commands will be automatically updated.

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

The **provisioning process** (STiROT_OEMuROT/provisioning.bat/.sh) is divided into 3 majors steps :

  Step 1 : Configuration management

    * STiRoT_Config.obk generation : Using TPC, regenerate your own keys and update the configuration through
      STiROT_Config.xml(ROT_Provisioning/STiROT_OEMuROT/Config/STiRoT_Config.xml)

    * DA_Config.obk generation : Using TPC, update the keys and permissions then regenerate the certificate and update the configuration through
      DA_Config.xml.(ROT_Provisioning/DA/Config/DA_Config.xml)

    * OEMuRoT_Config.obk generation : Using TPC, regenerate your own keys and update the configuration through
      OEMuRoT_Config_Keys.xml(ROT_Provisioning/STiROT_OEMuROT/Config/OEMuRoT_Config_Keys.xml)

  Step 2 : Images generation

    * Boot firmware image generation : Build the OEMiROT_Boot project with the preferred toolchain.

    * Code firmware image generation : Build the OEMiROT_Appli_TrustZone project with the preferred toolchain.

    * Data generation : Data image generation with TrustedPackageCreator, if data image enabled.

  Step 3 : Provisioning

    * Program the option bytes and flash the images using ob_flash_programming.bat/.sh.

    * Define the final product state value. (OPEN/PROVISIONED/CLOSED/LOCKED)

The provisioning script is relying on ob_flash_programming and obkey_provisioning scripts.

It is possible to run ob_flash_programming.bat/.sh and obkey_provisioning.bat/.sh directly (advanced mode).

- The **ob_flash_programming process** (STiROT_OEMuROT/ob_flash_programming.bat/.sh) includes :

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

  * [STiRoT_OEMuRoT_STM32H573](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_OEMuRoT_on_STM32H573)
