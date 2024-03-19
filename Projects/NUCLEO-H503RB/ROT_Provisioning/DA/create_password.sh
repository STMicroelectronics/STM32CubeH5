#!/bin/bash -
source ../env.sh
script_error_file="error"

# Define script mode execution
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Absolute path to this script
SCRIPT=$(readlink -f $0)
da_projectdir=`dirname $SCRIPT`
# CubeProgammer path and input files
user_password="$da_projectdir/../DA/user_password.bin"
da_password="$da_projectdir/../DA/da_password.bin"
board_password="$da_projectdir/../DA/board_password.bin"

# Environment variable for AppliCfg
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
  #determine/check python version command
  python="python3 "
fi
AppliCfg=$python$applicfg

error()
{
  echo "       Error when trying to $action" > $script_error_file
  echo "       Provisioning aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
}

# ======================================================= Create board password =========================================================
action="Create password file"
echo "$action"
$AppliCfg hashcontent -i $user_password $board_password --password --da_bin_file=$da_password -h 0x000000800C000000
if [ $? -ne 0 ]; then error; return 1; fi

echo "Password script creation success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0

