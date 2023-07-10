#!/bin/bash -
# Absolute path to this script
if [ $# -ge 1 ] && [ -d $1 ]; then
    projectdir=$1
else
    projectdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi
# ==============================================================================
#                               General
# ==============================================================================
#Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
    stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
    stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
else
    PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
    stm32programmercli="STM32_Programmer_CLI"
    stm32tpccli="STM32TrustedPackageCreator_CLI"
fi

# ==============================================================================
#                            OEMiROT bootpath
# ==============================================================================
# Select application project below
#oemirot_boot_path_project=Templates/ROT/OEMiROT_Appli_TrustZone
oemirot_boot_path_project=Applications/ROT/OEMiROT_Appli_TrustZone
# ==============================================================================

# ==============================================================================
#               !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
# ==============================================================================
PROJECT_GENERATED_BY_CUBEMX=false
cube_fw_path=$projectdir/../../../
oemirot_appli_secure=rot_tz_s_app_enc_sign.hex
oemirot_appli_non_secure=rot_tz_ns_app_enc_sign.hex
oemirot_appli_assembly_sign=rot_tz_app_enc_sign.hex
rot_provisioning_path=$projectdir
