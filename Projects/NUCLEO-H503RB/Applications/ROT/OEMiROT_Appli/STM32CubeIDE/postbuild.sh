#!/bin/bash -
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

# Environment variable for AppliCfg
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo AppliCfg with windows executable
  python=""
else
  #line for python
  echo AppliCfg with python script
  applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  #determine/check python version command
  python="python3 "
fi

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
