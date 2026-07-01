#!/bin/bash -
source ../env.sh

# Define permission
permission=""
if [ $# -ge 1 ]; then permission="per=$1"; fi
# Define script mode execution
if [ $# -ge 2 ]; then script_mode=$2; else script_mode=MANUAL; fi

error()
{
  echo "dbg_auth script failed"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

# Close debug if opened
"$stm32programmercli" $connect_no_reset debugauth=3 >/dev/null

# Command with both key/certificate (if TZEN enabled) and password (if TZEN disabled)
"$stm32programmercli" -c port=SWD speed=fast ${permission} key=./Keys/key_3_leaf.pem cert=./Certificates/cert_leaf_chain.b64 pwd=./Binary/password.bin debugauth=1
if [ $? -ne 0 ]; then error; fi

echo "dbg_auth script success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

exit 0
