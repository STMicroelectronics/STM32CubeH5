@ECHO OFF

:: ==============================================================================
::                               General
:: ==============================================================================
:: Configure tools installation path
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"

:: Configure Virtual Com Port
set com_port=COM5

:: ==============================================================================
::                            STiRoT boot path
:: ==============================================================================
:: Select application project below
::set stirot_boot_path_project=%~dp0\..\Templates\ROT\STiROT_Appli_TrustZone
::set stirot_boot_path_project=%~dp0\..\Templates\ROT\STiROT_Appli
::set stirot_boot_path_project=%~dp0\..\Applications\ROT\STiROT_Appli_TrustZone
set stirot_boot_path_project=%~dp0\..\Applications\ROT\STiROT_Appli
:: ==============================================================================

:: ==============================================================================
::                            OEMiROT boot path
:: ==============================================================================
:: Select application project below
::set oemirot_boot_path_project=%~dp0\..\Templates\ROT\OEMiROT_Appli_TrustZone
set oemirot_boot_path_project=%~dp0\..\Applications\ROT\OEMiROT_Appli_TrustZone
:: ==============================================================================

:: ==============================================================================
::                         STiROT_OEMuROT boot path
:: ==============================================================================
:: Application is the same as for OEMiROT boot path

:: ==============================================================================
::                            Secure Manager boot path
:: ==============================================================================
:: Select application project below
::set sm_ns_app_boot_path_project=%~dp0\..\Templates\ROT\SMAK_Appli
set sm_ns_app_boot_path_project=%~dp0\..\Applications\ROT\SMAK_Appli
::set sm_ns_app_boot_path_project=%~dp0\..\Applications\ROT\SMDK_Appli
set sm_module_0_boot_path_project=
::set sm_module_0_boot_path_project=%~dp0\..\Applications\ROT\SMDK_Module

:: ==============================================================================

:: ==============================================================================
::               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
:: ==============================================================================
set PROJECT_GENERATED_BY_CUBEMX=false
set cube_fw_path=%~dp0..\..\..\
set stirot_appli=appli_enc_sign.hex
set oemirot_appli_secure=rot_tz_s_app_enc_sign.hex
set oemirot_appli_non_secure=rot_tz_ns_app_enc_sign.hex
set rot_provisioning_path=%~dp0