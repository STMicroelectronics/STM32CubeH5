#!/bin/bash -
source ../env.sh

script_error_file="error"
sec1_start=0
sec1_end=0xE
sec2_start=0x7F
sec2_end=0x0
wrpgrp1=0xFFFFFFF8
wrpgrp2=0xFFFFFFFF
hdp1_start=0
hdp1_end=0xB
hdp2_start=0x7F
hdp2_end=0x0
boot_lck=0xB4
bootaddress=0xC000000
bootob=0xC0000
app_image_number=2
s_data_image_number=0
ns_data_image_number=0

s_code_image=$oemirot_appli_secure
ns_code_image=$oemirot_appli_non_secure
one_code_image=$oemirot_appli_assembly_sign
s_data_image="s_data_enc_sign.hex"
ns_data_image="ns_data_enc_sign.hex"

connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
else
   appli_dir="../../$oemirot_boot_path_project"
fi

# =============================================== Remove protections and initialize Option Bytes ==========================================
remove_protect_init="-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3 SECBOOTADD=$bootob SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 BOOT_UBE=0xB4"

# =============================================== Erase the user flash =====================================================================
erase_all="-e all"

# =============================================== Hardening ===============================================================================
hide_protect="HDP1_STRT=$hdp1_start HDP1_END=$hdp1_end HDP2_STRT=$hdp2_start HDP2_END=$hdp2_end"
write_protect="WRPSGn1=$wrpgrp1 WRPSGn2=$wrpgrp2"
sec_water_mark="SECWM1_STRT=$sec1_start SECWM1_END=$sec1_end SECWM2_STRT=$sec2_start SECWM2_END=$sec2_end"
boot_lock="-ob SECBOOT_LOCK=$boot_lck"

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

error()
{
  echo "        Error when trying to "$action"" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Configure Option Bytes ====================================================================
action="Set TZEN = 1"
echo "$action"
# Trust zone enabled is mandatory in order to execute OEM-iRoT
"$stm32programmercli" $connect_no_reset -ob TZEN=0xB4
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove Protection and erase All"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_reset $remove_protect_init $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

action="Configure Secure Water Mark"
echo "$action"
"$stm32programmercli" $connect_no_reset -ob $sec_water_mark
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== Download images ====================================================================
echo "Application images programming in download slots"

if [ "$app_image_number" == "2" ]; then
    action="Write Appli Secure"
    echo "$action $appli_dir/Binary/$s_code_image"
    "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$s_code_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli Secure Written"
    action="Write Appli NonSecure"
    echo "$action"
    "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$ns_code_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli NonSecure Written"
fi

if [ "$app_image_number" == "1" ]; then
    action="Write One image Appli"
    echo "$action"
    "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$one_code_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli Written"
fi

if [ "$s_data_image_number" == "1" ]; then
    action="Write Secure Data"
    echo "$action"
    
    if [ ! -f "$rot_provisioning_path/OEMiROT/Binary/$s_data_image" ]; then
        echo "Error: s_data_enc_sign.hex does not exist! use TPC to generate it"
        error
    fi
    
    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/OEMiROT/Binary/$s_data_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
fi

if [ $ns_data_image_number -eq 1 ]; then
    action="Write non Secure Data"
    echo "$action"

    if [ ! -f "$rot_provisioning_path/OEMiROT/Binary/$ns_data_image" ]; then
        echo "Error: ns_data_enc_sign.hex does not exist! use TPC to generate it"
        error
    fi

    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/OEMiROT/Binary/$ns_data_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
fi

action="Write OEMiROT_Boot"
echo "$action"
"$stm32programmercli" "$connect_no_reset" -d "$cube_fw_path/Projects/NUCLEO-H563ZI/Applications/ROT/OEMiROT_Boot/Binary/OEMiROT_Boot.bin" $bootaddress -v
if [ $? -ne 0 ]; then error; return 1; fi
echo "OEMiROT_Boot Written"

# ======================================================= Extra board protections =========================================================
action="Configure Secure option Bytes: Write Protection, Hide Protection and boot lock"
echo "$action"
"$stm32programmercli" $connect_no_reset -ob $write_protect $hide_protect $boot_lock
if [ $? -ne 0 ]; then error; return 1; fi

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL;  fi

return


