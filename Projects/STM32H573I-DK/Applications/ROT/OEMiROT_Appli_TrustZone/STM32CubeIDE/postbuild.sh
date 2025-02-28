#!/bin/bash -
# arg1 is the binary type (1 nonsecure, 2 secure)
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
app_image_number=2
image_s_size=0x6000
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
ns_code_bin="$project_dir/../Binary/rot_tz_ns_app.bin"
one_code_bin="$project_dir/../Binary/rot_tz_app.bin"

#=============================================================================================
#image xml configuration files
#=============================================================================================
s_code_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Image.xml"
ns_code_xml=$provisioningdir/$bootpath/Images/$project"_NS_Code_Image.xml"
s_code_init_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Init_Image.xml"
ns_code_init_xml=$provisioningdir/$bootpath/Images/$project"_NS_Code_Init_Image.xml"
s_data_xml=$provisioningdir/$bootpath/Images/$project"_S_Data_Image.xml"
ns_data_xml=$provisioningdir/$bootpath/Images/$project"_NS_Data_Image.xml"
s_data_init_xml=$provisioningdir/$bootpath/Images/$project"_S_Data_Init_Image.xml"
ns_data_init_xml=$provisioningdir/$bootpath/Images/$project"_NS_Data_Init_Image.xml"

#=============================================================================================
#Variables for image xml configuration(ROT_Provisioning/%bootpath%/Images)
#relative path from ROT_Provisioning/%bootpath%/Images directory to retrieve binary files
#=============================================================================================
bin_path_xml_field="../../../Applications/ROT/OEMiROT_Appli_TrustZone/Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
ns_app_bin_xml_field="$bin_path_xml_field/rot_tz_ns_app.bin"
s_app_bin_xml_field="$bin_path_xml_field/rot_tz_s_app.bin"
ns_app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_tz_ns_app_enc_sign.hex"
s_app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_tz_s_app_enc_sign.hex"
ns_app_init_sign_hex_xml_field="$bin_path_xml_field/rot_tz_ns_app_init_sign.hex"
s_app_init_sign_hex_xml_field="$bin_path_xml_field/rot_tz_s_app_init_sign.hex"
s_data_enc_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/s_data_enc_sign.hex"
ns_data_enc_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/ns_data_enc_sign.hex"
s_data_init_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/s_data_init_sign.hex"
ns_data_init_sign_hex_xml_field="$provisioningdir/$bootpath/Binary/ns_data_init_sign.hex"

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
echo "Postbuild $signing image" >> $current_log_file 2>&1

if  [ $app_image_number -eq 1 ] && [ $signing == "nonsecure" ]; then
    echo "Creating only one image" >> $current_log_file 2>&1
    $python$applicfg oneimage -fb $s_code_bin -o $image_s_size -sb $ns_code_bin -i 0x0 -ob $one_code_bin --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
    ns_app_enc_sign_hex_xml_field="$bin_path_xml_field/rot_tz_app_enc_sign.hex"
    ns_app_init_sign_hex_xml_field="$bin_path_xml_field/rot_tz_app_init_sign.hex"
    ns_app_bin_xml_field="$bin_path_xml_field/rot_tz_app.bin"
fi

if  [ $signing == "secure" ]; then

    echo "Creating secure image"  >> $current_log_file 2>&1

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
fi

if  [ $signing == "nonsecure" ]; then
    echo "Creating nonsecure image"  >> $current_log_file 2>&1

    # update xml file : input file
    $python$applicfg xmlval -v $ns_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $ns_code_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then error; fi

    # update xml file : output file
    $python$applicfg xmlval -v $ns_app_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $ns_code_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then error; fi

    "$stm32tpccli" -pb $ns_code_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    # update xml file : input file
    $python$applicfg xmlval -v $ns_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $ns_code_init_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then error; fi

    # update xml file : output file
    $python$applicfg xmlval -v $ns_app_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $ns_code_init_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then step_error; fi

    # update data xml file : output file
    $python$applicfg xmlval -v $ns_data_enc_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $ns_data_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then step_error; fi

    $python$applicfg xmlval -v $ns_data_init_sign_hex_xml_field --string -n "$fw_out_bin_xml_field" $ns_data_init_xml --vb >> $current_log_file 2>&1
    if [ $? != "0" ]; then step_error; fi

    "$stm32tpccli" -pb $ns_code_init_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
fi
exit 0
