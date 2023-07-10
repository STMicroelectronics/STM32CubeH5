#!/bin/bash -
source ../env.sh

# Define script mode execution
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "regression script failed"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"
# Close debug if opened
"$stm32programmercli" $connect_no_reset debugauth=3 >/dev/null

# In case of Provisioning Product state, try to disable TZEN and provision DA with password
"$stm32programmercli" $connect_reset -ob TZEN=0xC3 >/dev/null
"$stm32programmercli" $connect_reset >/dev/null
"$stm32programmercli" $connect_no_reset -sdp ./Binary/DA_ConfigWithPassword.obk >/dev/null

# Regression with certificates (if TZEN enabled) and password (if TZEN disabled)
"$stm32programmercli" -c port=SWD per=a key=./Keys/key_3_leaf.pem cert=./Certificates/cert_leaf_chain.b64 pwd=./Binary/password.bin debugauth=1
if [ $? -ne 0 ]; then error; return 1; fi


echo "regression script success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return