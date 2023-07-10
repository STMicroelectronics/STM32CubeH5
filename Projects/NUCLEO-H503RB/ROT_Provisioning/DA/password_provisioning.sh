#!/bin/bash -
source ../env.sh

# Define script mode execution
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

#CubeProgammer path and input files
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
board_password="$project_dir\board_password.bin"
otp_data_soc_mask="$project_dir\data_soc_mask.bin"

address_password=0x8FFF000
address_data_soc_mask=0x8FFF020

# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

error()
{
  echo "       Error when trying to $action 2>&1"
  echo "       Programming aborted 2>&1"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
}

# =========================================== Write data soc mask =========================================================================
action="Write data soc mask"
echo $action

# Write OTP data soc mask
"$stm32programmercli" $connect_no_reset -w $otp_data_soc_mask $address_data_soc_mask

# =============================================== Write password =========================================================================
action="Write Password"
echo "$action"

"$stm32programmercli" $connect_no_reset -w $board_password $address_password
"$stm32programmercli" $connect_reset

action="Lock OTP with write protection"
echo $action

# Lock OTP values to prevent data corruption (case of double provisioning with different password)
"$stm32programmercli" $connect_no_reset -ob LOCKBL=0x1
if [ $? -ne 0 ]; then error; return 1; fi
"$stm32programmercli" $connect_reset

echo "       Programming password success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0

