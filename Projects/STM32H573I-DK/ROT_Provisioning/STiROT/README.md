## <b>ROT_Provisioning STiROT Description</b>

This section provides an overview of the available scripts for STiROT.

STiROT stands for "ST Immutable Root of Trust" and it provides two services :
  - Secure Boot : Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update : If a new firmware image is available on detection, the SFU checks its authenticity.
                             Once the firmware is decrypted, the SFU checks and confirms the integrity of the code before installing it.


### <b>Keywords</b>

STiROT, TrustZone, boot path, Root Of Trust


### <b>Directory contents</b>

- STiROT/provisioning.bat/.sh                 Target provisioning.
- STiROT/update_appli_setup.bat/.sh           It updates test Application files.
- STiROT/update_ob_setup.bat/.sh              It updates the Option bytes setup on the device.
- STiROT/ob_flash_programming.bat/.sh         Option bytes Initialization and Image download.
- STiROT/obkey_provisioning.bat/.sh           It configures Option byte keys(OBKeys).
- STiROT/Image/STiRoT_Code_Image.xml          Configuration file for Firmware image generation.
- STiROT/Image/STiRoT_Data_Image.xml          Configuration file for Data image generation.
- STiROT/Keys/STiRoT_Authentication.pem       Private key for authentication.
- STiROT/Keys/STiRoT_Authentication_pub.pem   Public key for authentication.
- STiROT/Keys/STiRoT_Encryption.pem           Private key for encryption.
- STiROT/Keys/STiRoT_Encryption_pub.pem       Public key for encryption.
- STiROT/Binary                               It contains generated binary data images and obk binary.
- STiROT/Config/STiROT_Config.xml             It contains the STiROT configuration.


### <b>Hardware and Software environment</b>

- This example runs on STM32H573xx devices with security enabled (TZEN=B4).
- This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it?</b>

Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files
of project as well as the postbuild commands will be automatically updated.

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

The **provisioning process** (STiROT/provisioning.bat/.sh) is divided into 3 majors steps :

  Step 1 : Configuration management

    * STiRoT_Config.obk generation : Using TPC, regenerate your own keys and update the configuration through
      STiROT_Config.xml(ROT_Provisioning/STiROT/Config/STiRoT_Config.xml)

    * DA_Config.obk generation : Using TPC, update the keys and permissions then regenerate the certificate and update the configuration
      through DA_Config.xml.(ROT_Provisioning/DA/Config/DA_Config.xml)

    * updateAppliSetup script update : Update full secure value in updateAppliSetup.bat/.sh according to
      STiRoT_Config.xml.(ROT_Provisioning/STiROT/Config/STiRoT_Config.xml)

    * ob_flash_programming script update : Updating the Option bytes in update_ob_setup.bat/.sh according to
      STiRoT_Config.xml.(ROT_Provisioning/STiROT/Config/STiRoT_Config.xml)

    * STiROT_Appli or STiROT_Appli_TrustZone project files (main.h, .icf) update: stm32h573xx_flash.icf and main.h updated according to
      STiRoT_Config.xml.(ROT_Provisioning/STiROT/Config/STiRoT_Config.xml)

  Step 2 : Images generation

    * Code firmware image generation :
      - If full_secure = 1 : Build the STiROT_Appli project with the preferred toolchain.
      - If full_secure = 0 : Build the STiROT_Appli_TrustZone with the preferred toolchain.

    * Data generation : Data image generation with TrustedPackageCreator, if data image enabled.

  Step 3 : Provisioning

    * Program the option bytes and flash the images using ob_flash_programming.bat/.sh.

    * Define the final product state value. (OPEN/PROVISIONED/CLOSED/LOCKED)

The provisioning script is relying on ob_flash_programming, obkey_provisioning, update_appli_setup and update_ob_setup scripts.

It is possible to run ob_flash_programming.bat/.sh, obkey_provisioning.bat/.sh, update_appli_setup.bat/.sh
and update_ob_setup.bat/.sh directly (advanced mode).

- The **ob_flash_programming process** (STiROT/ob_flash_programming.bat/.sh) includes :

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

  * [STiRoT_STM32H573] (https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_STiRoT_on_STM32H573)
