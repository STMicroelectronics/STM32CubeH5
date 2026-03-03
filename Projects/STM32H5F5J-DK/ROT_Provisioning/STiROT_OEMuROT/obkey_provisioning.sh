#!/bin/bash -
source ../env.sh

# Getting the CubeProgammer_cli path
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

# Local variable to manage alone script execution
product_state="unknown"
script_error_file="error"

# Update local variable thanks to argument use with the command script executed
if [ $# -ge 1 ]; then
  script_mode=$1;
  product_state=$2;
else
  script_mode=MANUAL;
fi

# Error function
error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Provisioning aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

if [[ "$product_state" == "OPEN" ]]; then
  action="Set SECBOOT_LOCK option byte to 0xC3 (unlock secure boot address)"
  echo "$action"
  # SECBOOT_LOCK reset to 0xC3 value is mandatory to change BOOT_UBE option byte (this is done only in OPEN product_state)
  "$stm32programmercli" $connect_no_reset -ob SECBOOT_LOCK=0xC3
  if [ $? -ne 0 ]; then error; return 1; fi

  action="Set UBE ==> OEM-iRoT (User Flash)"
  echo "$action"
  # Unique boot entry is set to OEM-iRoT (User Flash) to be able to configure OBKeys in product state Open
  "$stm32programmercli" $connect_no_reset -ob BOOT_UBE=0xB4
  if [ $? -ne 0 ]; then error; return 1; fi
fi

# =============================================== Configure OB Keys =========================================================================
action="Configure OBKeys HDPL1-DA config area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./../DA/Binary/DA_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-ST-iRoT config area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./Binary/STiRoT_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL1-ST-iRoT data area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./Binary/STiRoT_Data.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL2-OEMuRoT config area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./Binary/OEMuRoT_Config.obk
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure OBKeys HDPL2-OEMuRoT data area"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp ./Binary/OEMuRoT_Data.obk
if [ $? -ne 0 ]; then error; return 1; fi

# =============================================== Boot on ST-iRoT ==========================================================================
if [[ "$product_state" == "OPEN" ]]; then
  action="Set UBE ==> ST-iRoT"
  echo "$action"
  # Unique boot entry is set to ST-iRoT to force ST-iRoT execution at each reset
  "$stm32programmercli" $connect_reset
  "$stm32programmercli" $connect_no_reset -ob BOOT_UBE=0xC3
  if [ $? -ne 0 ]; then error; return 1; fi
  
  action="Set SECBOOT_LOCK to 0xB4 (lock secure boot address)"
  echo "$action"
  "$stm32programmercli" $connect_no_reset -ob SECBOOT_LOCK=0xB4
  if [ $? -ne 0 ]; then error; return 1; fi
fi


echo "Successful option bytes programming and images flashing"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return