@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"

:: ==============================================================================
::                            OEMiROT boot path
:: ==============================================================================
:: Select application project below
::set oemirot_boot_path_project=Templates/ROT/OEMiROT_Appli_TrustZone
set oemirot_boot_path_project=Applications/ROT/OEMiROT_Appli_TrustZone
:: ==============================================================================

:: ==============================================================================
::               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
:: ==============================================================================
set PROJECT_GENERATED_BY_CUBEMX=false
set cube_fw_path=%~dp0..\..\..\
set oemirot_appli_secure=rot_tz_s_app_enc_sign.hex
set oemirot_appli_non_secure=rot_tz_ns_app_enc_sign.hex
set oemirot_appli_assembly_sign=rot_tz_app_enc_sign.hex
set rot_provisioning_path=%~dp0
