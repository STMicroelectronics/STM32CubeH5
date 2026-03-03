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

# arg1 is the binary type (nonsecure, secure)
signing=$1
# arg2 is the config type (Debug, Release)
config=$2
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

#=============================================================================================
#Variables updated by OEMiROT_Boot postbuild
#=============================================================================================
#flag to switch between OEMiROT and OEMuROT
oemurot_enable=0

if [ "$oemurot_enable" == "1" ]; then
    project=OEMuROT
    bootpath=STiROT_OEMuROT
fi

if [ "$oemurot_enable" == "0" ]; then
    project=OEMiROT
    bootpath=OEMiROT
fi

#=============================================================================================
#image binary files
#=============================================================================================
s_code_bin="$project_dir/../Binary/rot_tz_s_app.bin"

#=============================================================================================
#image xml configuration files
#=============================================================================================
s_code_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Image.xml"
s_code_init_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Init_Image.xml"
s_data_xml=$provisioningdir/$bootpath/Images/$project"_S_Data_Image.xml"
s_data_init_xml=$provisioningdir/$bootpath/Images/$project"_S_Data_Init_Image.xml"

#=============================================================================================
#Variables for image xml configuration(ROT_Provisioning/%bootpath%/Images)
#relative path from ROT_Provisioning/%bootpath%/Images directory to retrieve binary files
#=============================================================================================
bin_path_xml_field="../../../Applications/ROT/OEMiROT_Appli/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
s_app_bin_xml_field="$bin_path_xml_field/rot_tz_s_app.bin"
s_app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_tz_s_app_enc_sign.hex"
s_app_init_sign_hex_xml_field="$bin_path_xml_field/rot_tz_s_app_init_sign.hex"
s_data_enc_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/s_data_enc_sign.hex"
s_data_init_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/s_data_init_sign.hex"

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

#postbuild
echo "Postbuild $signing image" >> $current_log_file 2>&1

# update xml file : input file
$python$applicfg xmlval -v $s_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $s_code_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

# update xml file : output file
$python$applicfg xmlval -v $s_app_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $s_code_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

"$stm32tpccli" -pb $s_code_xml >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

# update xml file : input file
$python$applicfg xmlval -v $s_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $s_code_init_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

# update xml file : output file
$python$applicfg xmlval -v $s_app_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $s_code_init_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

# update data xml file : output file
$python$applicfg xmlval -v $s_data_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $s_data_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

$python$applicfg xmlval -v $s_data_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $s_data_init_xml --vb >> $current_log_file 2>&1
if [ $? != "0" ]; then error; fi

"$stm32tpccli" -pb $s_code_init_xml >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

exit 0
