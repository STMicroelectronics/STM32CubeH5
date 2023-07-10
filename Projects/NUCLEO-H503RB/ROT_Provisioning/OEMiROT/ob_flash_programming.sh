#!/bin/bash -
source ../env.sh
script_error_file="error"

# Define script mode execution
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Data updated with the postbuild of OEMiROT-Boot
wrpgrp1=0x3
wrpgrp2=0xFE
hdp1_start=0
hdp1_end=0x7
hdp2_start=0x0
hdp2_end=0x0
bootob=0x80044
bootaddress=0x8004000
app_image_number=1
data_image_number=0
code_image=$oemirot_appli
data_image=data_enc_sign.hex

# CubeProgammer connection
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# =============================================== Remove protections and initialize Option Bytes  ==========================================
remove_protect_init="-ob WRPSGn1=0xff WRPSGn2=0xff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 NSBOOT_LOCK=0xC3 SRAM2_RST=0 SRAM2_ECC=0 SRAM1_RST=0 SRAM1_ECC=0 NSBOOTADD=$bootob SWAP_BANK=0"
# =============================================== Erase the user flash =====================================================================
erase_all="-e all"

# ================================================ hardening ===============================================================================
hide_protect="HDP1_STRT=$hdp1_start HDP1_END=$hdp1_end HDP2_STRT=$hdp2_start HDP2_END=$hdp2_end"
write_protect="WRPSGn1=$wrpgrp1 WRPSGn2=$wrpgrp2"
ns_boot_lock=NSBOOT_LOCK=0xB4

# Error function
error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
#  exit 1
}
# ============================================== Configure Option Bytes ==============================================================================

action="Remove Protection and erase All"
echo "$action"
"$stm32programmercli" $connect_reset $remove_protect_init $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== Download images ====================================================================
echo "Application images programming in download slots"

if [ "$app_image_number" == "1" ]; then
    action="Write OEMiROT_Appli Code"
    echo "$action"
    "$stm32programmercli" $connect_no_reset -d "../../$oemirot_boot_path_project/Binary/$code_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli Written"
fi

if [ "$data_image_number" == "1" ]; then
    action="Write OEMiROT_Appli Data"
    echo "$action"
    
    if [ ! -f "$rot_provisioning_path/OEMiROT/Binary/$data_image" ]; then
        echo "Error: $data_image does not exist! use TPC to generate it"
        error
    fi

    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/OEMiROT/Binary/$data_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
fi


action="Write OEMiROT_Boot"
echo "$action"
"$stm32programmercli" "$connect_no_reset" -d "$cube_fw_path/Projects/NUCLEO-H503RB/Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot.bin" $bootaddress -v
if [ $? -ne 0 ]; then error; return 1; fi
echo "OEMiROT_Boot Written"

echo "Configure Secure option Bytes: Write Protection, Hide Protection and boot lock"
"$stm32programmercli" $connect_no_reset -ob $write_protect $hide_protect $ns_boot_lock
if [ $? -ne 0 ]; then error; return 1; fi

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL;  fi



