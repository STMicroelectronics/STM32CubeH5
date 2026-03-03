#!/bin/bash -
source ../env.sh

if [ $# -ge 2 ]; then script_mode=$2; else script_mode=MANUAL; fi

# Define TrustZone Configuration
if [ $# -eq 0 ]; then
    read -p "The Trust Zone configuration is enable ? [ y | n ]:" tzen_state;
    tzen_state=$(echo $tzen_state | tr '[:upper:]' '[:lower:]');
else
    tzen_state=$1;
fi

if [ $tzen_state == "y" ]; then 
  da_file="DA_Config"
else
  da_file="DA_ConfigWithPassword"
fi

SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`
# Define the obk file to provisionned
da_obk_file="$da_file.obk"
# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

script_error_file="error"

error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Provisioning aborted" > $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Configure OB Keys ========================================================
# Start provisioning
# Verify if obk file exists
if [ ! -f $projectdir/Binary/$da_obk_file ]; then
    echo "     Error: $da_obk_file does not exist! use TPC to generate it" 2>&1
    error
fi

# Provisioning the obk file 
action="Provisioning the obk file"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_no_reset -sdp "./Binary/$da_obk_file"
if [ $? -ne 0 ]; then error; return 1; fi

# Reset the board after provisioning
action="Reset the board after provisioning"
echo "$action"
"$stm32programmercli" $connect_reset
if [ $? -ne 0 ]; then error; return 1; fi

echo "Provisioning success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return
