#!/bin/bash -
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
echo $project_dir
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source $provisioningdir/env.sh "$provisioningdir"

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > $current_log_file

# arg1 is the config type (Debug, Release)
config=$1

applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo "AppliCfg with windows executable"
  python=""
else
  #line for python
  echo "AppliCfg with python script"
  applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  #determine/check python version command
  python="python "
fi

auto_rot_update="$project_dir/../auto_rot_update.sh"
map_properties="$project_dir/../map.properties"
preprocess_bl2_file="$project_dir/image_macros_preprocessed_bl2.c"
appli_dir="../../../../$oemirot_boot_path_project"

provisioning="$project_dir/../../../../ROT_Provisioning/img_config.sh"
ns_main="$appli_dir/NonSecure/Inc/main.h"
s_main="$appli_dir/Secure/Inc/main.h"
appli_flash_layout="$appli_dir/Secure_nsclib/appli_flash_layout.h"
appli_postbuild="$appli_dir/STM32CubeIDE/postbuild.sh"

$python$applicfg flash --layout $preprocess_bl2_file -b oemurot_enable -m  RE_OEMUROT_ENABLE --decimal $auto_rot_update --vb >> $current_log_file
$command

if [ $? != 0 ]; then error; fi

source $auto_rot_update

update=$provisioningdir/$bootpath/"ob_flash_programming.sh"
s_code_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Image.xml"
ns_code_xml=$provisioningdir/$bootpath/Images/$project"_NS_Code_Image.xml"
s_data_xml=$provisioningdir/$bootpath/Images/$project"_S_Data_Image.xml"
ns_data_xml=$provisioningdir/$bootpath/Images/$project"_NS_Data_Image.xml"
auth_s="Authentication secure key"
auth_ns="Authentication non secure key"
xml_fw_app_item_name="Firmware binary input file"
xml_fw_data_item_name="Data binary input file"
xml_output_item_name="Image output file"
xml_enc_item_name="Encryption key"
s_ld_file="$appli_dir/STM32CubeIDE/Secure/STM32H573IIKXQ_FLASH.ld"
ns_ld_file="$appli_dir/STM32CubeIDE/NonSecure/STM32H573IIKXQ_FLASH.ld"

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

$python$applicfg flash --layout $preprocess_bl2_file -b S_CODE_REGION_START -m  RE_ADDRESS_SECURE_START $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b S_CODE_REGION_SIZE -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NS_CODE_REGION_START -m  RE_ADDRESS_NON_SECURE_START $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NS_CODE_REGION_SIZE -m  RE_IMAGE_NON_SECURE_IMAGE_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg modifyfilevalue --variable bootPath --delimiter = --value $project $map_properties --str --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b S_DATA -m RE_S_DATA_IMAGE_NUMBER $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NS_DATA -m RE_NS_DATA_IMAGE_NUMBER $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b CODE_IMAGE_ASSEMBLY -m RE_CODE_IMAGE_ASSEMBLY $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b MCUBOOT_OVERWRITE_ONLY -m RE_OVER_WRITE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b TRAILER_SIZE -m RE_TRAILER_MAX_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b S_DATA_REGION_START -m RE_AREA_4_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b S_DATA_REGION_SIZE -m RE_AREA_4_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NS_DATA_REGION_START -m RE_AREA_5_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b NS_DATA_REGION_SIZE -m RE_AREA_5_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ROT_REGION_START -m RE_FLASH_AREA_BL2_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ROT_REGION_SIZE -m RE_FLASH_AREA_BL2_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b SCRATCH_REGION_START -m RE_FLASH_AREA_SCRATCH_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b SCRATCH_REGION_SIZE -m RE_FLASH_AREA_SCRATCH_SIZE $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DOWNLOAD_S_CODE_REGION_START -m RE_AREA_2_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DOWNLOAD_NS_CODE_REGION_START -m RE_AREA_3_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DOWNLOAD_S_DATA_REGION_START -m RE_AREA_6_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b DOWNLOAD_NS_DATA_REGION_START -m RE_AREA_7_OFFSET $map_properties --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b bootob -m  RE_BL2_BOOT_ADDRESS  -d 0x100 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file  -b bootaddress -m  RE_BL2_BOOT_ADDRESS $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b sec1_end -m  RE_BL2_SEC1_END -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b sec2_start -m  RE_BL2_SEC2_START -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b sec2_end -m  RE_BL2_SEC2_END -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp1_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp2_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg setob --layout $preprocess_bl2_file -b hdp2_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b s_data_image_number -m  RE_S_DATA_IMAGE_NUMBER --decimal $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ns_data_image_number -m  RE_NS_DATA_IMAGE_NUMBER --decimal $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal $appli_postbuild --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal $update --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal $provisioning --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b s_data_image_number -m  RE_S_DATA_IMAGE_NUMBER --decimal $provisioning --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ns_data_image_number -m  RE_NS_DATA_IMAGE_NUMBER --decimal $provisioning --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b image_s_size -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE $appli_postbuild --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n S_CODE_OFFSET $s_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n S_CODE_OFFSET $ns_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE $s_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE $ns_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE $ns_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE $ns_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg linker --layout $preprocess_bl2_file -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE $s_ld_file --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlname --layout $preprocess_bl2_file -m RE_APP_IMAGE_NUMBER -n "$auth_ns" -sn "$auth_s" -v 1 -c k $ns_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_UPDATE -c x $s_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_UPDATE -sm RE_IMAGE_FLASH_SECURE_UPDATE -v 0 -c x $ns_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_DATA_SECURE_UPDATE -c x $s_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE -c x $ns_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S $s_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S $ns_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S $s_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S $ns_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $s_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $ns_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $s_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $ns_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $s_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $ns_code_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $s_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $ns_data_xml --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n S_CODE_OFFSET $s_main --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE $s_main --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE $s_main --vb >> $current_log_file
if [ $? != 0 ]; then error; fi

# Bypass configuration of appli_flash_layout file if not present
if [ -f $appli_flash_layout ]; then
  $python$applicfg setdefine --layout $preprocess_bl2_file -m RE_NS_DATA_IMAGE_NUMBER -n NS_DATA_IMAGE_EN -v 1 $ns_main --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg setdefine --layout $preprocess_bl2_file -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi

  $python$applicfg definevalue --layout $preprocess_bl2_file -m RE_FLASH_B_SIZE -n FLASH_B_SIZE $appli_flash_layout --vb >> $current_log_file
  $command
  if [ $? != 0 ]; then error; fi
fi

cp -a $project_dir/$config/STM32H573I-DK_OEMiROT_Boot.bin $project_dir/../Binary/OEMiROT_Boot.bin >> $current_log_file
if [ $? != 0 ]; then error; fi

if [ "$oemurot_enable" == "1" ]; then
    "$stm32tpccli" -pb "$rot_provisioning_path/STiROT_OEMuROT/Images/STiRoT_Code_Image.xml"
fi

exit 0
