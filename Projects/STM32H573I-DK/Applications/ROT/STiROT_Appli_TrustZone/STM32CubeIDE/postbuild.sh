#!/bin/bash -
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source $provisioningdir/env.sh "$provisioningdir"

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > $current_log_file

# arg1 is the binary type (1 nonsecure, 2 secure)
signing=$1
# arg2 is the config type (Debug, Release)
config=$2


code_xml="$project_dir/../../../../ROT_Provisioning/STiROT/Image/STiRoT_Code_Image.xml"
data_xml="$project_dir/../../../../ROT_Provisioning/STiROT/Image/STiRoT_Data_Image.xml"
code_init_xml="$project_dir/../../../../ROT_Provisioning/STiROT/Image/STiRoT_Code_Init_Image.xml"
data_init_xml="$project_dir/../../../../ROT_Provisioning/STiROT/Image/STiRoT_Data_Init_Image.xml"
s_code_bin="$project_dir/Secure/$config/STiROT_Appli_TrustZone_Secure.bin"
ns_code_bin="$project_dir/NonSecure/$config/STiROT_Appli_TrustZone_NonSecure.bin"
s_ns_code_bin="$project_dir/../Binary/appli.bin"

# Variables for image xml configuration
appli_dir="../../../Applications/ROT/STiROT_Appli_TrustZone"
fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
stirot_app_bin="$appli_dir/Binary/appli.bin"
stirot_app_hex="$appli_dir/Binary/appli_enc_sign.hex"
stirot_app_init_hex="$appli_dir/Binary/appli_init_sign.hex"
stirot_data_hex="$provisioningdir/STiROT/Binary/data_enc_sign.hex"
stirot_data_init_bin="$provisioningdir/STiROT/Binary/data_init_sign.bin"

#Variables updated by update_appli_setup
image_size=0x00006000

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
  python="python3 "
fi

echo "Postbuild STiROT image" >> $current_log_file 2>&1

if  [ $signing == "nonsecure" ]; then
  # Create one image (assembly secure and non secure binaries)
  echo "Creating only one image" >> $current_log_file 2>&1
  $python$applicfg oneimage -fb $s_code_bin -o $image_size -sb $ns_code_bin -i 0x0 -ob $s_ns_code_bin --vb >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi

  # Create final image with Trust Package Creator
  echo "Creating STiROT image"  >> $current_log_file 2>&1

  # update xml file : input file
  $python$applicfg xmlval -v $stirot_app_bin --string -n "$fw_in_bin" $code_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi

  # update xml file : output file
  $python$applicfg xmlval -v $stirot_app_hex --string -n "$fw_out_bin" $code_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi

  "$stm32tpccli" -pb $code_xml >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi

  # update xml file : input file
  $python$applicfg xmlval -v $stirot_app_bin --string -n "$fw_in_bin" $code_init_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi

  # update xml file : output file
  $python$applicfg xmlval -v $stirot_app_init_hex --string -n "$fw_out_bin" $code_init_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi

  "$stm32tpccli" -pb $code_init_xml >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi

  echo "Creating STiROT data image"  >> $current_log_file 2>&1

  # update data xml file : output file
  $python$applicfg xmlval -v $stirot_data_hex --string -n "$fw_out_bin" $data_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi

  $python$applicfg xmlval -v $stirot_data_init_bin --string -n "$fw_out_bin" $data_init_xml --vb >> $current_log_file 2>&1
  if [ $? -ne 0 ]; then error; return 1; fi
fi
exit 0
