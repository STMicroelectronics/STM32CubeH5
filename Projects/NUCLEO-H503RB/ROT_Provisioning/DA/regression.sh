#!/bin/bash -
source ../env.sh

password_programming="password_provisioning.sh"

connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

error()
{
  echo "regression script failed"
  echo "Please unplug, then plug your board and try again"
  echo "press key"
  if [ "$script_mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  return 1
}

# In case of Provisioning Product state, try to provision DA with password
command="source $password_programming AUTO"
$command > /dev/null

# Regression with password
"$stm32programmercli" -c port=SWD pwd=password.bin debugauth=1
if [ $? -ne 0 ]; then error; fi

echo "regression script done, press key"
if [ "$script_mode" != "AUTO" ]; then read -p "" -n1 -s; exit 0; fi
return 0

