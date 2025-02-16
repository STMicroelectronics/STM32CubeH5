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
    stm32programmercli_path="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
else
    stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
    PATH=$stm32programmercli_path:$PATH
    stm32programmercli="STM32_Programmer_CLI"
    stm32tpccli="STM32TrustedPackageCreator_CLI"
fi

#==============================================================================
#                           OEMiROT bootpath
#==============================================================================
#Select application project below
#oemirot_boot_path_project=Templates_ROT/OEMiROT_Appli
oemirot_boot_path_project=Applications/ROT/OEMiROT_Appli

#==============================================================================
#              !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
#==============================================================================
PROJECT_GENERATED_BY_CUBEMX=false
cube_fw_path=$projectdir/../../../
oemirot_appli=rot_app_init_sign.hex
rot_provisioning_path=$projectdir
