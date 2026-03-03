#!/bin/bash -
# Absolute path to this script
if [ $# -ge 1 ] && [ -d $1 ]; then
    projectdir=$1
else
    projectdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi
#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)

echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo ""
  echo " =========================================="
  echo "| For Windows OS, please use .bat scripts. |"
  echo " =========================================="
  echo ""
  echo "       Press any key to continue..."
  echo
  read -p "" -n1 -s
  exit 0
fi

if [ "$OS" == "Linux" ]; then
  echo "HOST OS : Linux detected"
elif [ "$OS" == "Darwin" ]; then
  echo "HOST OS : MacOS detected"
else
  echo "!!!HOST OS not supported : >$OS<!!!"
  exit 1
fi

#=================================================================================================
# Managing HOST OS diversity : end
#=================================================================================================

# ==============================================================================
#                               General
# ==============================================================================
#Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
    stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
    stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
    stm32programmercli_path="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
elif [[ "$OS" == "Linux" ]]; then
    stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
    PATH=$stm32programmercli_path:$PATH
    stm32programmercli="STM32_Programmer_CLI"
    stm32tpccli="STM32TrustedPackageCreator_CLI"
elif [[ "$OS" == "Darwin" ]]; then
    stm32programmercli=/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI
    stm32tpccli=/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32TrustedPackageCreator.app/Contents/MacOs/bin/STM32TrustedPackageCreator_CLI
else
  echo "OS not supported : >$OS<"
  return
fi

#==============================================================================
#                           OEMiROT Appli boot path
# Select application project below
#oemirot_appli_path_project=Templates_ROT/OEMiROT_Appli
oemirot_appli_path_project=Applications/ROT/OEMiROT_Appli

#==============================================================================
#                           OEMiROT bootpath
#==============================================================================
#Select OEMiROT Boot project below
oemirot_boot_path_project=Applications/ROT/OEMiROT_Boot

#==============================================================================
#              !!!! DOT NOT EDIT --- UPDATED AUTOMATICALLY !!!!
#==============================================================================
PROJECT_GENERATED_BY_CUBEMX=false
cube_fw_path=$projectdir/../../../
oemirot_appli=rot_app_init_sign.hex
rot_provisioning_path=$projectdir
