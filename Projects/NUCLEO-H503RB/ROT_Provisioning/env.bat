@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"

:: ==============================================================================
::                            OEMiROT Appli boot path
:: Select application project below
::set oemirot_appli_path_project=Templates_ROT/OEMiROT_Appli
set oemirot_appli_path_project=Applications/ROT/OEMiROT_Appli

:: ==============================================================================
::                            OEMiROT boot path
:: ==============================================================================
:: Select OEMiROT Boot project below
set oemirot_boot_path_project=Applications/ROT/OEMiROT_Boot

:: ==============================================================================

:: ==============================================================================
::               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
:: ==============================================================================
set PROJECT_GENERATED_BY_CUBEMX=false
set cube_fw_path=%~dp0..\..\..\
set oemirot_appli=rot_app_init_sign.hex
set rot_provisioning_path="%~dp0"
