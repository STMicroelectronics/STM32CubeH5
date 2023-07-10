#!/bin/bash -
source ../env.sh

sec1_start=0
sec1_end=0xE
sec2_start=0x7F
sec2_end=0x0
wrpgrp1=0xFFFFFFF8
wrpgrp2=0xFFFFFFFF
hdp1_start=0
hdp1_end=0x0
hdp2_start=0x7F
hdp2_end=0x0
boot_lck=0xB4
bootaddress=0xC000400
bootob=0xC0004
app_image_number=2
s_data_image_number=0
ns_data_image_number=0

s_code_image=$oemirot_appli_secure
ns_code_image=$oemirot_appli_non_secure
one_code_image=$oemirot_appli_assembly_sign
s_data_image="s_data_enc_sign.hex"
ns_data_image="ns_data_enc_sign.hex"
oemurot_image="rot_enc_sign.hex"

connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

image_number=2

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Error function
script_error_file="error"
error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" > $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}
# =============================================== Remove protections and initialize Option Bytes ==========================================
remove_protect="-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 BOOT_UBE=0xB4"
erase_all="-e all"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
else
   appli_dir="../../$oemirot_boot_path_project"
fi
# =============================================== Configure Option Bytes ====================================================================
action="Set TZEN = 1"
echo "$action"
# Trust zone enabled is mandatory to execute ST-iRoT
"$stm32programmercli" $connect_no_reset -ob TZEN=0xB4
if [ $? -ne 0 ]; then error; return 1; fi

action="Set SECBOOT_LOCK option byte to 0xC3 (unlock secure boot address by default)"
echo $action
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_reset -ob SECBOOT_LOCK=0xC3
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove Protection and erase All"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_reset $remove_protect $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

action="Set SecureBoot address"
echo "$action"
"$stm32programmercli" $connect_reset
"$stm32programmercli" $connect_reset -ob SECBOOTADD=$bootob
if [ $? -ne 0 ]; then error; return 1; fi

echo "Set SRAM 2 configuration"
# Recommended configuration for secure boot is :
#   - SRAM2 erased in case of reset ==> SRAM2_RST=0
#   - SRAM2 ECC activated. Hack tentative detection enabled ==> SRAM2_ECC=0
"$stm32programmercli" $connect_no_reset -ob SRAM2_RST=0 SRAM2_ECC=0
if [ $? -ne 0 ]; then error; return 1; fi

action="Define secure area through watermarks"
echo "$action"
# This configuration depends on user mapping but the watermarks should cover at least the secure area part of the firmware execution slot.
# The secure area can also be larger in order to include additional sectors. For example the secure firmware will have to manage user data.
"$stm32programmercli" $connect_no_reset -ob SECWM1_STRT=0x0 SECWM1_END=$sec1_end
if [ $? -ne 0 ]; then error; return 1; fi
"$stm32programmercli" $connect_no_reset -ob SECWM2_STRT=0x7F SECWM2_END=0x0
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== Download images ====================================================================

echo "Application images programming in download slots"

if [ "$app_image_number" == "2" ]; then
    action="Write Appli Secure"
    echo "$action"
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
    if [ ! -f "$rot_provisioning_path/STiROT_OEMuROT/Binary/s_data_enc_sign.hex" ]; then
        echo "Error: s_data_enc_sign.hex does not exist! use TPC to generate it"
        error
        return 1
    fi
    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/STiROT_OEMuROT/Binary/$s_data_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
fi


if [ $ns_data_image_number -eq 1 ]; then
    action="Write non Secure Data"
    echo "$action"
    if [ ! -f "$rot_provisioning_path/STiROT_OEMuROT/Binary/ns_data_enc_sign.hex" ]; then
        echo "Error: ns_data_enc_sign.hex does not exist! use TPC to generate it"
        error
        return 1
    fi
    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/STiROT_OEMuROT/Binary/$ns_data_image" -v
    if [ $? -ne 0 ]; then error; return 1; fi
fi

action="Write OEMuROT_Boot"
echo "$action"
"$stm32programmercli" $connect_no_reset -d "$cube_fw_path/Projects/STM32H573I-DK/Applications/ROT/OEMiROT_Boot/Binary/rot_enc_sign.hex" -v
if [ $? -ne 0 ]; then error; return 1; fi
echo "OEMuROT_Boot Written"

action="Set UBE for ST-iRoT"
echo "$action"
# Unique boot entry is set to ST-iRoT to force ST-iRoT execution at each reset
"$stm32programmercli" $connect_reset -ob BOOT_UBE=0xC3
if [ $? -ne 0 ]; then error; return 1; fi
echo "UBE set to ST-iRoT"

action="Set SECBOOT_LOCK option byte"
echo "$action"
# SECBOOT_LOCK should be set to 0xB4 (locked) to be compliant with certification document
"$stm32programmercli" $connect_no_reset -ob SECBOOT_LOCK=0xB4
if [ $? -ne 0 ]; then error; return 1; fi
echo "SECBOOT_LOCK set"

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL;  fi

return
