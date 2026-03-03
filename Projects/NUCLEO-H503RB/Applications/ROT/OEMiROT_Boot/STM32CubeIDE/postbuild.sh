#!/bin/bash -
#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)

echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo "=================================="
  echo "HOST OS : Windows detected"
  echo ""
  echo ">>> Running ../postbuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../
  cmd.exe /C postbuild.bat $@
  # Return OK if no error detected during .bat script
  exit 0
fi

if [ "$OS" == "Linux" ]; then
  echo "HOST OS : Linux detected"
elif [ "$OS" == "Darwin" ]; then
  echo "HOST OS : MacOS detected"
else
  echo "!!!HOST OS not supported : >$OS<!!!"
  exit 1
fi

#=================================================================================================
# Managing HOST OS diversity : end
#=================================================================================================
echo "=================================="
echo ">>> Running $0 $@"
echo ""

# arg1 is the config type (Debug, Release)
config=$1
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source $provisioningdir/env.sh "$provisioningdir"

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > $current_log_file

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Check if Python is installed
python3 --version >/dev/null 2>&1
if [ $? -ne 0 ]; then
  python --version >/dev/null 2>&1
  if [ $? -ne 0 ]; then
  echo "Python installation missing. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md"
  exit 1
  fi
  python="python "
else
  python="python3 "
fi

# Environment variable for AppliCfg
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"

#postbuild
preprocess_bl2_file="$project_dir/image_macros_preprocessed_bl2.c"
appli_dir="../../../../$oemirot_appli_path_project"
update="$project_dir/../../../../ROT_Provisioning/OEMiROT/ob_flash_programming.sh"

#Environment variable for AppliCfg
isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX
appli_flash_layout="$appli_dir/Inc/appli_flash_layout.h"
appli_linker_file="$appli_dir/STM32CubeIDE/STM32H503RBTX_FLASH.ld"
map_properties="$project_dir/../../OEMiROT_Boot/map.properties"

#======================================================================================
#image xml configuration files
#======================================================================================
code_xml="$project_dir/../../../../ROT_Provisioning/OEMiROT/Images/OEMiROT_Code_Image.xml"
data_xml="$project_dir/../../../../ROT_Provisioning/OEMiROT/Images/OEMiROT_Data_Image.xml"
code_init_xml="$project_dir/../../../../ROT_Provisioning/OEMiROT/Images/OEMiROT_Code_Init_Image.xml"
data_init_xml="$project_dir/../../../../ROT_Provisioning/OEMiROT/Images/OEMiROT_Data_Init_Image.xml"

$python$applicfg flash --layout $preprocess_bl2_file -b CODE_REGION_START -m RE_IMAGE_FLASH_ADDRESS $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b CODE_REGION_SIZE -m RE_IMAGE_FLASH_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DATA -m RE_DATA_IMAGE_NUMBER $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DATA_REGION_START -m RE_IMAGE_FLASH_ADDRESS_DATA $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DATA_REGION_SIZE -m RE_IMAGE_FLASH_DATA_IMAGE_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ROT_REGION_START -m RE_FLASH_AREA_BL2_OFFSET $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ROT_REGION_SIZE -m RE_FLASH_AREA_BL2_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b HASH_REF_AREA_START -m RE_FLASH_HASH_REF_AREA_OFFSET $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b HASH_REF_AREA_SIZE -m RE_FLASH_HASH_REF_AREA_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NVCNT_AREA_START -m RE_FLASH_BL2_NVCNT_AREA_OFFSET $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NVCNT_AREA_SIZE -m RE_FLASH_BL2_NVCNT_AREA_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b AREA_PERSO_START -m RE_FLASH_AREA_PERSO_OFFSET $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b AREA_PERSO_SIZE -m RE_FLASH_AREA_PERSO_SIZE $map_properties --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

#Bypass configuration of application in case of cubemx
if [ $isGeneratedByCubeMX != "true" ]; then

    $python$applicfg linker --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n CODE_OFFSET $appli_linker_file --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then  error; fi

    $python$applicfg linker --layout $preprocess_bl2_file -m RE_AREA_0_SIZE -n CODE_SIZE $appli_linker_file --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then  error; fi
fi

# Bypass configuration of appli_flash_layout file if not present
if [ -f $appli_flash_layout ]; then

    $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_DATA_IMAGE_NUMBER -n MCUBOOT_DATA_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then  error; fi

    $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_ADDRESS -n CODE_IMAGE_PRIMARY_PARTITION_ADDRESS $appli_flash_layout --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then  error; fi

    $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_ADDRESS_DATA -n DATA_IMAGE_PRIMARY_PARTITION_ADDRESS $appli_flash_layout --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then  error; fi
fi

$python$applicfg setob --layout $preprocess_bl2_file -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp1_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp2_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp2_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b bootob -m RE_BL2_BOOT_ADDRESS -d 0x100 $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b bootaddress -m RE_BL2_PERSO_ADDRESS $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SIZE -c S $code_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_ADDRESS -c x $code_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_DATA_UPDATE -c x $data_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_DATA_IMAGE_SIZE -c S $data_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b data_image_number -m RE_DATA_IMAGE_NUMBER --decimal $update --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

#xml for init image generation

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_ADDRESS -c x $code_init_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_DATA_UPDATE -c x $data_init_xml --vb >> $current_log_file 2>&1
if [ $? != 0 ]; then  error; fi

exit 0