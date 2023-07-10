#!/bin/bash -
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source "$provisioningdir/env.sh"

# arg1 is the config type (Debug, Release)
config=$1
# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > $current_log_file

code_bin="$project_dir/$config/STM32H573I-DK_STiROT_Appli.bin"
bin_dest="$project_dir/../Binary/appli.bin"

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Create final image with Trust Package Creator
echo "Copy binary to Binary location" >> $current_log_file 2>&1
cp $code_bin $bin_dest >> $current_log_file 2>&1
if [ $? != 0 ]; then error; fi
echo "Creating STiROT image" > $current_log_file
"$stm32tpccli" -pb "$provisioningdir/STiROT/Image/STiROT_Code_Image.xml" >> $current_log_file
if [ $? != 0 ]; then error; fi
exit 0
