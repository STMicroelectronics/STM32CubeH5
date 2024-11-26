## <b>ROT_Provisioning OEMiROT Description</b>

This section provides available configuration scripts for OEMiROT example.

To ease the configuration process, keygen, ob_flash_programming and provisioning scripts are used.
The keygen script is in charge of preparing the authentication and encryption keys.
The ob_flash_programming script is in charge of removing the protections, initializing and configuring the option bytes and download the images.
The provisioning script is in charge of target provisioning.


### <b>Keywords</b>

OEMiROT, boot path, Root of Trust, Security, mcuboot


### <b>Directory contents</b>

- OEMiROT/keygen.bat/.sh                     Configuring keys for authentication and encryption.
- OEMiROT/ob_flash_programming.bat/.sh       Option bytes Initialization and Image download.
- OEMiROT/provisioning.bat/.sh               Target provisioning.
- OEMiROT/Binary                             It contains generated binary data images.
- OEMiROT/Images/OEMiROT_Code_Image.xml      Configuration file for Firmware image generation.
- OEMiROT/Images/OEMiROT_Data_Image.xml      Configuration file for Data image generation.
- OEMiROT/Keys/OEMiRoT_Authentication.pem    Private key for authentication.
- OEMiROT/Keys/OEMiRoT_Encryption.pem        Private key for encryption.
- OEMiROT/Keys/OEMiRoT_Encryption_Pub.pem    Public key for encryption.


### <b>Hardware and Software environment</b>

- This example runs on STM32H503xx devices.
- This example has been tested with STMicroelectronics NUCLEO-STM32H503RB (MB1814)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

Before compiling the project, you should first start the provisioning process. During the provisioning process, the linker files
of project as well as the postbuild commands will be automatically updated.

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

The **provisioning process** (OEMiROT/provisioning.bat/.sh) is divided into 3 majors steps :

  Step 1 : Configuration management

    * OEM Keys generation : Run the keygen.bat/.sh script to regenerate the keys randomly.

  Step 2 : Image generation

    * Boot firmware image generation : Build the OEMiROT_Boot project with the preferred toolchain.

    * Code firmware image generation : Build the OEMiROT_Appli project with the preferred toolchain.

    * Data generation : Data image generation with TrustedPackageCreator, if data image enabled.

  Step 3 : Provisioning password

    * Programming the option bytes and flash the images using ob_flash_programming.bat/.sh.

    * Password provisioning. (The password is provisioned in OTP)

    * Password board creation using user_password.bin. (ROT_Provisioning/DA/user_password.bin)

    * Create the user password using create_password.bat/.sh. (ROT_Provisioning/DA/create_password.bat/.sh)

    * Define the final product state value. (OPEN/PROVISIONED/CLOSED/LOCKED)

    * Provisioning the password using password_provisioning.bat/.sh. (ROT_Provisioning/DA/password_provisioning.bat/.sh)

The provisioning script is relying on ob_flash_programming and keygen scripts.

It is possible to run ob_flash_programming.bat/.sh and keygen.bat/.sh directly (advanced mode).

- The **keygen process** (OEMiROT/keygen.bat/.sh) include :

  * ECC 256 authentication key generation.
  * Generation of private key to encode images.

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

  * [OEMiRoT_STM32H503](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_OEMiRoT_on_STM32H503)
