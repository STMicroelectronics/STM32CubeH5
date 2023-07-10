#!/bin/bash -
source ../env.sh
script_error_file="error"


# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# =============================================== Remove protections and initialize Option Bytes ===========================================
remove_protect_init="-ob WRPSGn1=0xff WRPSGn2=0xff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 NSBOOT_LOCK=0xC3 SRAM2_RST=0 SRAM2_ECC=0 SRAM1_RST=0 SRAM1_ECC=0 NSBOOTADD=0x80000 SWAP_BANK=0"
# =============================================== Erase the user flash =====================================================================
erase_all="-e all"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
}

# =============================================== Configure Option Bytes ====================================================================

action="Remove Protection and erase All"
echo $action
"$stm32programmercli" $connect_reset $remove_protect_init $erase_all
if [ "$?" -ne "0" ]; then error; return 1; fi


echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
return 0
