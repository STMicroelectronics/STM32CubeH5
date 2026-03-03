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

#======================================================================================
#image xml configuration files
#======================================================================================
code_image_file="$provisioningdir/OEMiROT/Images/OEMiROT_Code_Image.xml"
code_image_init_file="$provisioningdir/OEMiROT/Images/OEMiROT_Code_Init_Image.xml"
data_image_file="$provisioningdir/OEMiROT/Images/OEMiROT_Data_Image.xml"
data_image_init_file="$provisioningdir/OEMiROT/Images/OEMiROT_Data_Init_Image.xml"

#======================================================================================
#Variables for image xml configuration(ROT_Provisioning/OEMiROT/Images)
#relative path from ROT_Provisioning/OEMiROT/Images directory to retrieve binary files
#======================================================================================
bin_path_xml_field="../../../Applications/ROT/OEMiROT_Appli/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
app_bin_xml_field="$bin_path_xml_field/rot_app.bin"
app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_app_enc_sign.hex"
app_init_sign_hex_xml_field="$bin_path_xml_field/rot_app_init_sign.hex"
data_enc_sign_hex_xml_field="$provisioningdir/OEMiROT/Binary/data_enc_sign.hex"
data_init_sign_hex_xml_field="$provisioningdir/OEMiROT/Binary/data_init_sign.hex"

#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $bin_path_xml_field ]; then
mkdir $bin_path_xml_field
fi

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

echo "Creating OEMiROT image" > $current_log_file 2>&1

# update xml file : input file
$python$applicfg xmlval -v $app_bin_xml_field --string -n "$fw_in_bin_xml_field" $code_image_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; fi

# update xml file : output file
$python$applicfg xmlval -v $app_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $code_image_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; fi

"$stm32tpccli" -pb "$code_image_file" >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

# update xml file : input file
$python$applicfg xmlval -v $app_bin_xml_field --string -n "$fw_in_bin_xml_field" $code_image_init_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; fi

# update xml file : output file
$python$applicfg xmlval -v $app_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $code_image_init_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; fi

"$stm32tpccli" -pb "$code_image_init_file" >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

echo "Creating OEMiROT data" >> $current_log_file 2>&1

# update xml file : output file
$python$applicfg xmlval -v $data_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $data_image_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then step_error; fi

"$stm32tpccli" -pb "$data_image_file" >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

$python$applicfg xmlval -v $data_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $data_image_init_file --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; fi

"$stm32tpccli" -pb "$data_image_init_file" >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

exit 0
