## <b>ROT_Provisioning DA Description</b>

This section provides an overview of the available scripts for provisioning, regression and discovery
along with their basic descriptions, usage and instructions.

Debug authentication(DA) controls debug opening and regressions.
Before using the debug authentication services, the user must provision STM32 with its credentials.
The debug authentication allows Password method (TZ=0), the user must provision a password hash (SHA256) within STM32.
Debug authentication can be used when the STM32 is in product state PROVISIONNING up to product state CLOSED.


### <b>Keywords</b>

DA, Password, Security, Root of Trust


### <b>Directory contents</b>

- DA/discovery.bat/.sh               It allows the user to get the information about the device state.
- DA/ob_programming.bat/.sh          It removes the protections, initializes and configures the option bytes.
- DA/create_password.bat/.sh         It creates a password for a board.
- DA/password_provisioning.bat/.sh   The password will be programmed on a specific address.
- DA/provisioning.bat/.sh            It configures the option bytes, the password, and the final product state for the board.
- DA/regression.bat/.sh              It erases the user stored content and sets the product to an open state.


### <b>Hardware and Software environment</b>

- This example runs on STM32H503xx devices
- This example has been tested with STMicroelectronics NUCLEO-STM32H503RB (MB1814)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

All scripts are relying on env.bat/env.sh for tools path and application path. (ROT_Provisioning/env.bat/.sh)

* provisioning.bat/.sh :

  Run the provisioning.bat/.sh script by double clicking on it.

  Step 1 : Programming of initial option bytes :

    - The script proceeds with the programming of option bytes, removing all the protections
      and erasing the user flash memory using ob_programming.bat/.sh.

  Step 2 : Generating and flashing the code image :

    - In this step, you will be able to flash your application with your preferred toolchain. (Rebuild the projects)

  Step 3 : Provisioning the password and setting the final product state :

    - If it is the first time you are provisioning your board,
      you have the possibility to update the default password in the user_password.bin file.

    - You can also use this default user_password.bin file without modifying it.

    - If you already created and provisioned password for your board,
      then you can proceed to jump to the setting of the final product state.

      * user_password.bin : the password (16 bytes) to be filled by User is defined here.
      * board_password.bin : HASH of user password, which will be provisioned in the chip.
      * da_password.bin : the output file opening the DA access for regression.

    - Board_password.bin and da_password.bin files are automatically updated with the
      new password saved in user_password.bin through create_password.bat/.sh.

    - Provisioning the password through password_provisioning.bat/.sh.

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

  * [DA_STM32H503](https://wiki.st.com/stm32mcu/wiki/Security:How_to_start_with_DA_access_on_STM32H503)
