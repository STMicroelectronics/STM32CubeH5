#!/bin/bash -
source ../env.sh

script_error_file="error"
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`

if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi

# Boards connections
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

# Value updated automatically
image_number=1
# =============================================== Remove protections and erase the user flash ===============================================

remove_protect="-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3"
erase_all="-e all"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$stirot_boot_path_project
else
   appli_dir="../../$stirot_boot_path_project"
fi

error()
{
  echo "        Error when trying to $action" > $script_error_file
  echo "        Programming aborted" >> $script_error_file
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

# =============================================== Configure Option Bytes ====================================================================
action="Set TZEN = 1"
echo "$action"
# Trust zone enabled is mandatory to execute ST-iRoT
"$stm32programmercli" $connect_no_reset -ob TZEN=0xB4
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove Protection and erase All"
echo "$action"
"$stm32programmercli" $connect_reset $remove_protect $erase_all

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
"$stm32programmercli" $connect_no_reset -ob SECWM1_STRT=0x0 SECWM1_END=0xF
if [ $? -ne 0 ]; then error; return 1; fi
"$stm32programmercli" $connect_no_reset -ob SECWM2_STRT=0x7F SECWM2_END=0x0
if [ $? -ne 0 ]; then error; return 1; fi

# ==================================================== Download images ====================================================================
action="Download the code image in the download slots"
echo "$action"

if [ ! -f $appli_dir/Binary/$stirot_appli ]; then
    echo "Error: appli_enc_sign.hex does not exist! use TPC to generate it"
    error
fi
"$stm32programmercli" $connect_no_reset -d $appli_dir/Binary/$stirot_appli
if [ $? -ne 0 ]; then error; return 1; fi

if [ $image_number -eq 2 ]; then
    action="Download the data image in the download slots"
    echo "$action"
    
    if [ ! -f "$project_dir/Binary/data_enc_sign.hex" ]; then
        echo "Error: data_enc_sign.hex does not exist! use TPC to generate it"
        error
    fi
    
    "$stm32programmercli" $connect_no_reset -d $project_dir/Binary/data_enc_sign.hex
    if [ $? -ne 0 ]; then error; return 1; fi
fi

echo "Set UBE for ST-iROT"
# Unique boot entry is set to ST-iRoT to force ST-iRoT execution at each reset
"$stm32programmercli" $connect_reset -ob BOOT_UBE=0xC3
if [ $? -ne 0 ]; then error; return 1; fi

echo "Set SECBOOT_LOCK option byte"
# SECBOOT_LOCK should be set to 0xB4 (locked) to be compliant with certification document
"$stm32programmercli" $connect_no_reset -ob SECBOOT_LOCK=0xB4
if [ $? -ne 0 ]; then error; return 1; fi

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL;  fi

return
