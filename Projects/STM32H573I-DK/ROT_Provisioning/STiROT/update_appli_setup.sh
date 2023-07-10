#!/bin/bash -
source ../env.sh

script_error_file="error"
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# External script
ob_flash_programming="ob_flash_programming.sh"

# Environment variable for setting postbuild command with AppliCfg
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$stirot_boot_path_project
else
   appli_dir="../../$stirot_boot_path_project"
fi

# CubeIDE project Appli TrustZone files
ld_tz_s_file="$appli_dir/STM32CubeIDE/Secure/STM32H573IIKXQ_FLASH.ld"
ld_tz_ns_file="$appli_dir/STM32CubeIDE/NonSecure/STM32H573IIKXQ_FLASH.ld"

# CubeIDE project Appli Full Secure files
cube_appli_postbuild="$appli_dir/STM32CubeIDE/postbuild.sh"
ld_file="$appli_dir/STM32CubeIDE/STM32H573IIKXQ_FLASH.ld"


main_h="$appli_dir/Inc/main.h"
s_main_h="$appli_dir/Secure/Inc/main.h"

obk_cfg_file="$project_dir/Config/STiRoT_Config.xml"
code_image="$project_dir/Image/STiRoT_Code_Image.xml"

# Switch use case project Application TrustZone or Full secure
Full_secure=1

# General section need
code_size="Firmware area size"
code_offset="Firmware execution area offset"
data_image_en="Number of images managed"
fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
secure_code_size="Size of the secure area"

#Path adapted to IAR postbuild command
stirot_app_bin="../$appli_dir/Binary/appli.bin"
stirot_app_hex="../$appli_dir/Binary/appli_enc_sign.hex"

# Define AppliCfg to execute
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"

uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo AppliCfg with windows executable
  python=""
else
  #line for python
  echo AppliCfg with python script
  applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  python="python "
fi

error()
{
  echo "        Error when trying to $action" >$script_error_file
  echo "        Update script aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

AppliCfg="$python$applicfg"
# ================================================ Updating test Application files ========================================================
if [ "$Full_secure" == "1" ]; then

  action="Update STiROT_Code_Image.xml input binary file"
  $AppliCfg xmlval --name "$fw_in_bin" --value $stirot_app_bin --string --vb $code_image
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Update STiROT_Code_Image.xml output encrypted/signed hexadecimal file"
  $AppliCfg xmlval --name "$fw_out_bin" --value $stirot_app_hex --string --vb $code_image
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Updating Linker file"
  echo $action

  $AppliCfg linker -xml $obk_cfg_file -nxml "$code_size" -n S_CODE_SIZE --vb $ld_file
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg linker -xml $obk_cfg_file -nxml "$code_offset" -n S_CODE_OFFSET --vb $ld_file
  if [ $? -ne 0 ]; then error; return 1; fi


  action="Updating Data Image Define Flag"
  $AppliCfg setdefine -xml $obk_cfg_file -nxml "$data_image_en" -n DATA_IMAGE_EN -v 0x02 --vb $main_h
  #if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg definevalue -xml $obk_cfg_file -nxml "$code_size" -n S_CODE_SIZE --vb $main_h
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg definevalue -xml $obk_cfg_file -nxml "$code_offset" -n S_CODE_OFFSET --vb $main_h
  if [ $? -ne 0 ]; then error; return 1; fi

  echo "stm32h573xx_flash.icf and main.h successfully updated according to STiRoT_Config.obk"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

fi

if [ "$Full_secure" == "0" ]; then

  action="Update STiROT_Code_Image.xml input binary file"
  $AppliCfg xmlval --name "$fw_in_bin" --value $stirot_app_bin --string --vb $code_image

  if [ $? -ne 0 ]; then error; return 1; fi

  action="Update STiROT_Code_Image.xml output encrypted/signed hexadecimal file"
  $AppliCfg xmlval --name "$fw_out_bin" --value $stirot_app_hex --string --vb $code_image
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Update appli postbuild"

  $AppliCfg flash -xml $obk_cfg_file -nxml "$secure_code_size" -b image_size $cube_appli_postbuild --vb
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Updating Linker ld secure file"

  $AppliCfg linker -xml $obk_cfg_file -nxml "$secure_code_size" -n S_CODE_SIZE --vb $ld_tz_s_file
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg linker -xml $obk_cfg_file -nxml "$code_offset" -n S_CODE_OFFSET --vb $ld_tz_s_file
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Updating Linker non secure file"

  $AppliCfg linker -xml $obk_cfg_file -nxml "$secure_code_size" -n S_CODE_SIZE --vb $ld_tz_ns_file
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg linker -xml $obk_cfg_file -nxml "$code_offset" -n S_CODE_OFFSET --vb $ld_tz_ns_file
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg linker -xml $obk_cfg_file -nxml "$secure_code_size" -n NS_CODE_SIZE -e "(cons1 - val1)" -cons "0x00020000" --vb $ld_tz_ns_file
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg definevalue -xml $obk_cfg_file -nxml "$secure_code_size" -n S_CODE_SIZE $s_main_h --vb
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg definevalue -xml $obk_cfg_file -nxml "$code_offset" -n S_CODE_OFFSET $s_main_h --vb
  if [ $? -ne 0 ]; then error; return 1; fi

  $AppliCfg definevalue -xml $obk_cfg_file -nxml "$secure_code_size" -n NS_CODE_SIZE -e "(cons1 - val1)" -cons "0x00020000" $s_main_h --vb
  if [ $? -ne 0 ]; then error; return 1; fi


  echo "main.h successfully updated according to STiRoT_Config.obk"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return
fi

