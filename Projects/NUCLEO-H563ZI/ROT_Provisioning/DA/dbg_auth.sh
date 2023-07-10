#!/bin/bash -
source ../env.sh

# Define script mode execution
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "dbg_auth script failed"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# Command with both key/certificate (if TZEN enabled) and password (if TZEN disabled)
"$stm32programmercli" -c port=SWD speed=fast key=./Keys/key_3_leaf.pem cert=./Certificates/cert_leaf_chain.b64 pwd=.\Binary\password.bin debugauth=1
if [ $? -ne 0 ]; then error; return 1; fi

echo "dbg_auth script success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi

return

