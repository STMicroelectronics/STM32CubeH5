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

#=========================================================================
#image xml configuration files
#=========================================================================
code_xml="$provisioningdir/STiROT/Image/STiRoT_Code_Image.xml"
data_xml="$provisioningdir/STiROT/Image/STiRoT_Data_Image.xml"
code_init_xml="$provisioningdir/STiROT/Image/STiRoT_Code_Init_Image.xml"
data_init_xml="$provisioningdir/STiROT/Image/STiRoT_Data_Init_Image.xml"

#=====================================================================================
#Variables for image xml configuration(ROT_Provisioning/STiROT/Images)
#relative path from ROT_Provisioning/STiROT/Images directory to retrieve binary files
#=====================================================================================
bin_path_xml_field="../../../Applications/ROT/STiROT_Appli/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
stirot_app_bin_xml_field="$bin_path_xml_field/appli.bin"
stirot_app_hex_xml_field="$bin_path_xml_field/appli_enc_sign.hex"
stirot_app_init_hex_xml_field="$bin_path_xml_field/appli_init_sign.hex"
stirot_data_hex_xml_field="$provisioningdir/STiROT/Binary/data_enc_sign.hex"
stirot_data_init_bin_xml_field="$provisioningdir/STiROT/Binary/data_init_sign.bin"

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

#postbuild
echo Postbuild STiROT image >> $current_log_file 2>&1
# Create final image with Trust Package Creator
echo "Creating STiROT image" > $current_log_file

# update xml file : input file
$python$applicfg xmlval -v $stirot_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $code_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi

# update xml file : output file
$python$applicfg xmlval -v $stirot_app_hex_xml_field --string -n "$fw_out_bin_xml_field" $code_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi

"$stm32tpccli" -pb "$code_xml" >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi

# update xml file : input file
$python$applicfg xmlval -v $stirot_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $code_init_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi

# update xml file : output file
$python$applicfg xmlval -v $stirot_app_init_hex_xml_field --string -n "$fw_out_bin_xml_field" $code_init_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi

"$stm32tpccli" -pb "$code_init_xml" >> $current_log_file
if [ $? != 0 ]; then error; fi

echo "Creating STiROT data" > $current_log_file

# update data xml file : output file
$python$applicfg xmlval -v $stirot_data_hex_xml_field --string -n "$fw_out_bin_xml_field" $data_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi

$python$applicfg xmlval -v $stirot_data_init_bin_xml_field --string -n "$fw_out_bin_xml_field" $data_init_xml --vb >> $current_log_file 2>&1
if [ $? -ne 0 ]; then error; return 1; fi
exit 0
