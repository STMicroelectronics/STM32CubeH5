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
s_code_bin="$project_dir/Secure/$config/STM32H573I-DK_STiROT_Appli_TrustZone_S.bin"
ns_code_bin="$project_dir/NonSecure/$config/STM32H573I-DK_STiROT_Appli_TrustZone_NS.bin"
s_ns_code_bin="$project_dir/../Binary/appli.bin"

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
  python="python "
fi

echo "Postbuild STiROT image" >> $current_log_file 2>&1

if  [ $signing == "nonsecure" ]; then
	# Create one image (assembly secure and non secure binaries)
	echo "Creating only one image" >> $current_log_file 2>&1
	$python$applicfg oneimage -fb $s_code_bin -o $image_size -sb $ns_code_bin -i 0x0 -ob $s_ns_code_bin --vb >> $current_log_file 2>&1
	if [ $? != 0 ]; then error; fi

	# Create final image with Trust Package Creator
	echo "Creating STiROT image"  >> $current_log_file 2>&1
	"$stm32tpccli" -pb $code_xml >> $current_log_file 2>&1
	if [ $? != 0 ]; then error; fi
fi
exit 0