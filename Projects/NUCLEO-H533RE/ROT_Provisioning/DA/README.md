## <b>ROT_Provisioning DA Description</b>

This section provides an overview of the available scripts for provisioning, regression and discovery
along with their basic descriptions, usage and instructions.

Debug authentication(DA) controls debug opening and regressions.
Before using the debug authentication services, the user must provision STM32 with its credentials.
The debug authentication allows two types of credential : password or certificates,
  - The password method is only supported when TrustZone is disabled (TZEN=0xC3).
  - The certificate method is only supported when TrustZone is enabled (TZEN=0xB4).
Debug authentication can be used when the STM32 is in product state PROVISIONNING up to product state CLOSED.


### <b>Keywords</b>

DA, Password, Certificates, Security, Root of Trust


### <b>Directory contents</b>

- DA/discovery.bat/.sh                  It allows the user to get the information about the device state.
- DA/ob_programming.bat/.sh             It removes the protections, initializes and configures the option bytes.
- DA/provisioning.bat/.sh               It configures the option bytes, the password, and the final product state for the board.
- DA/regression.bat/.sh                 It erases the user stored content and sets the product to an open state.
- DA/dbg_auth.bat/.sh                   It automates certain processes related to debugging or authentication.
- DA/obk_provisioning.bat/.sh           It defines the TrustZone configuration and obk file to provision.
- DA/Binary                             It contains generated .obk and .bin files.
- DA/Certificates                       It contains generated certificates. (3 types - root, intermediate, leaf)
- DA/Config/DA_Config.xml               It is used to generate the keys if TZ=1 and configurations are based on certificates.
- DA/Config/DA_ConfigWithPassword.xml   It is used to generate the keys if TZ=0 and configurations are based on password.
- DA/Keys                               It contains generated keys. (3 types - root, intermediate, leaf)


### <b>Hardware and Software environment</b>

- This example runs on STM32H533xx devices with security enabled (TZEN=B4).
- This example has been tested with STMicroelectronics NUCLEO-H533RE (MB1814)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

* provisioning.bat/.sh :

  Run the provisioning.bat/.sh script by double clicking on it.

  Step 1 : Configuration management

    * DA_ConfigWithPassword.obk : If TrustZone is disabled, using TPC, it will generate the password and update the keys through
      DA_ConfigWithPassword.xml. (ROT_Provisioning/DA/Config/DA_ConfigWithPassword.xml)

    * DA_Config.obk generation : If TrustZone is enabled, using TPC, it will generate certificates, update the keys and permissions through
      DA_Config.xml. (ROT_Provisioning/DA/Config/DA_Config.xml)

  Step 2 : Initial Option Bytes programming

    * The script proceeds with the programming of option bytes, removing all the protections
      and erasing the user flash memory using ob_programming.bat/.sh.

  Step 3 : Images flashing

    * In this step, you will be able to flash your application with your preferred toolchain. (Rebuild the projects)

    * Define the final product state value. (OPEN/PROVISIONED/CLOSED/LOCKED)

* regression.bat/.sh :

  - Regression service to erase the user firmware and data within the user flash memory, SRAM and option bytes keys (OBK)
    when OBK are supported.

  - After a regression, STM32 falls back in product state open.

  - Launch the provided regression.bat/.sh script by double clicking on it.


## Environment Setup

  All scripts rely on env.bat and env.sh for setting the necessary tools path and application path.
    - File path : ROT_Provisioning/env.bat/.sh.
    - Purpose : Sets the necessary tools path and application path for Windows, Linux and Mac operating system.
    - Usage : Exporting the Environment Variables, Path Configuration, Dependency Setup, Perform initial setup tasks.


## Additional Resources

  * For DA_STM32H533, follow the same steps as STM32H563 or STM32H573:
    [DA_STM32H5x3]
    (https://wiki.st.com/stm32mcu/index.php?title=Security:How_to_start_with_DA_access_on_STM32H573_and_H563-TrustZone_disabled_-_stm32mcu)
