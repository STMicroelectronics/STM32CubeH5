#!/bin/bash -
source ../env.sh

if [ $# -ge 2 ]; then script_mode=$2; else script_mode=MANUAL; fi

# Define TruseZone Configuration
if [ $# -eq 0 ]; then
    read -p "The TrustZone feature is enable ? [ y | n ]:" tzen_state;
    tzen_state=$(echo $tzen_state | tr '[:upper:]' '[:lower:]');
else
    tzen_state=$1;
fi

if [ $tzen_state == "y" ]; then 
  tzen_ob=0xB4
else
  tzen_ob=0xC3
fi

# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# Initial configuration
sec_water_mark="-ob SECWM1_STRT=0x0 SECWM1_END=0x7F SECWM2_STRT=0x0 SECWM2_END=0x7F"

# =============================================== Remove protections and initialize Option Bytes ===========================================
remove_protect_init_non_secure="-ob WRPSGn1=0xffffffff WRPSGn2=0xffffffff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0"
remove_protect_init_secure="-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3 SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 NSBOOTADD=0x80000 SECBOOTADD=0xC0000 BOOT_UBE=0xB4"

# =============================================== Erase the user flash =====================================================================
erase_all="-e all"

end_programming()
{
  echo "Programming success"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
}

script_error_file="error"

error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" > $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}


# =============================================== Configure Option Bytes ====================================================================
# Configure TrustZone Option Bytes
action="Set TZEN configuration"
echo "$action"
"$stm32programmercli" $connect_no_reset -ob TZEN=$tzen_ob
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure Initial OB"
echo "$action"
if [ "$tzen_state" == "n" ]; then
    "$stm32programmercli" $connect_no_reset $remove_protect_init_non_secure $erase_all
    action="Configure non secure OB"
    if [ $? -ne 0 ]; then error; return 1; fi
fi

if [ "$tzen_state" == "y" ]; then
    "$stm32programmercli" $connect_no_reset $remove_protect_init_secure $erase_all
    action="Configure secure OB"
    if [ $? -ne 0 ]; then error; return 1; fi
fi

# Configure WaterMark Option Bytes (only when TZEN=1)
if [ "$tzen_state" == "n" ]; then end_programming; fi
action="Configure Secure Water Mark"
echo "$action"
"$stm32programmercli" $connect_reset $sec_water_mark
if [ $? -ne 0 ]; then error; return 1; fi

return

return
