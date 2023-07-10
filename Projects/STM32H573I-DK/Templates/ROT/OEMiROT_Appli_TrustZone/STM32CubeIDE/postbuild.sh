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


rot_update_script="$project_dir/../../../../Applications/ROT/OEMiROT_Boot/auto_rot_update.sh"
source $rot_update_script

s_code_xml=$provisioningdir/$bootpath/Images/$project"_S_Code_Image.xml"
ns_code_xml=$provisioningdir/$bootpath/Images/$project"_NS_Code_Image.xml"
s_code_bin="$project_dir/Secure/$config/STM32H573I-DK_OEMiROT_Appli_TrustZone_S_Templates.bin"
ns_code_bin="$project_dir/NonSecure/$config/STM32H573I-DK_OEMiROT_Appli_TrustZone_NS_Templates.bin"
bin_dest_dir="$project_dir/../Binary"
one_code_bin="$project_dir/../Binary/rot_tz_app.bin"

#Variables updated by OEMiROT_Boot postbuild
app_image_number=2
image_s_size=0x6000

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

echo "Postbuild $signing image" >> $current_log_file 2>&1

if  [ $app_image_number -eq 1 ] && [ $signing == "nonsecure" ]; then
    echo "Creating only one image" >> $current_log_file 2>&1
    $python$applicfg oneimage -fb $s_code_bin -o $image_s_size -sb $ns_code_bin -i 0x0 -ob $one_code_bin --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
fi

if  [ $signing == "secure" ]; then
    echo "Copy secure binary to Binary location" >> $current_log_file 2>&1
    cp $s_code_bin $bin_dest_dir/rot_tz_s_app.bin >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    echo "Creating secure image"  >> $current_log_file 2>&1
    "$stm32tpccli" -pb $s_code_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
fi

if  [ $signing == "nonsecure" ]; then
    echo "Copy nonsecure binary to Binary location" >> $current_log_file 2>&1
    cp $ns_code_bin $bin_dest_dir/rot_tz_ns_app.bin >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    echo "Creating nonsecure image"  >> $current_log_file 2>&1
    "$stm32tpccli" -pb $ns_code_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
fi
exit 0
