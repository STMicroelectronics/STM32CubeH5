#!/bin/bash -
source ../env.sh

script_error_file="error"

#Path and input files
ob_flash_programming_local="ob_flash_programming.sh"
obk_cfg_file="./Config/STiRoT_Config.xml"

SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

#xml item nem contents
#This will allow to recover information from xml file
code_size="Firmware area size"
code_offset="Firmware execution area offset"
data_image_en="Number of images managed"
secure_area="Size of the secure area inside the firmware execution area"
sram2_rst="SRAM2 erasing in case of reset"
sram2_ecc="SRAM2 ECC management activation"
full_sec="Is the firmware full secure"

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
  echo "        Error when trying to $action" > $script_error_file
  echo "        Update script aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

AppliCfg="$python $applicfg"
#======================================================= Updating the Option bytes =======================================================
echo Updating the ob_flash_programming script ...
action="Updating Secure WaterMarks values"
$AppliCfg obscript --xml $obk_cfg_file -ob SECWM -sp "$secure_area" -cs "$code_size" -co "$code_offset" -fs "$full_sec" --vb $ob_flash_programming_local
if [ $? -ne 0 ]; then error; return 1; fi

action="Updating SRAM2_RST value"
$AppliCfg obscript --xml $obk_cfg_file -ob SRAM2_RST -sp "$sram2_rst" --vb $ob_flash_programming_local
if [ $? -ne 0 ]; then error; return 1; fi

action="Updating SRAM2_ECC value"
$AppliCfg obscript --xml $obk_cfg_file -ob SRAM2_ECC -sp "$sram2_ecc" --vb $ob_flash_programming_local
if [ $? -ne 0 ]; then error; return 1; fi

action="Updating image number value"
$AppliCfg flash -xml $obk_cfg_file --name "$data_image_en" -b image_number --decimal $ob_flash_programming_local --vb
if [ $? -ne 0 ]; then error; return 1; fi

echo 'Script successfully updated according to STiRoT_Config.obk'
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return
