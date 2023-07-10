#!/bin/bash -
# Getting the CubeProgammer_cli path 
source ../env.sh


# Environment variable for log file
current_log_file="provisioning.log"
echo > $current_log_file

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX

if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

# Environment variable for AppliCfg
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`


# External scripts
ob_flash_programming="ob_flash_programming.sh"
ob_key_provisioning="obkey_provisioning.sh"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
else
   appli_dir="../../../$oemirot_boot_path_project"
fi

## Get config updated by OEMiROT_Boot
tmp_file=$cube_fw_path/Projects/STM32H573I-DK/ROT_Provisioning/img_config.sh

fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
ns_app_bin="$appli_dir/Binary/rot_tz_ns_app.bin"
s_app_bin="$appli_dir/Binary/rot_tz_s_app.bin"
s_code_image_file="$project_dir/Images/OEMuROT_S_Code_Image.xml"
ns_code_image_file="$project_dir/Images/OEMuROT_NS_Code_Image.xml"
s_data_xml="$project_dir/Images/OEMuROT_S_Data_Image.xml"
ns_data_xml="$project_dir/Images/OEMuROT_NS_Data_Image.xml"
ns_app_enc_sign_hex="$appli_dir/Binary/rot_tz_ns_app_enc_sign.hex"
s_app_enc_sign_hex="$appli_dir/Binary/rot_tz_s_app_enc_sign.hex"

#provisioning
ob_key_provisioning_log="obkey_provisioning.log"
provisioning_log="provisioning.log"
ob_flash_log="ob_flash_programming.log"

#Initial configuration
product_state=OPEN
connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug

flash_layout="$cube_fw_path/Projects/STM32H573I-DK/Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"

# Environment variable used to know if the firmware image is full secure or not
stirot_config="./Config/STiRoT_Config.xml"

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
  python="python "
fi

error_config()
{
  echo
  echo "====="
  echo "===== Error occurred."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}
# ========================================================= Script functions ===============================================================
# ================================================ Final product state selection ===========================================================
product_state_choice()
{
  action="Define product state value"
  echo "   * $action"
  read -p "       [ OPEN | PROVISIONED | TZ-CLOSED | CLOSED | LOCKED ]: " product_state
  product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')

  if [ "$product_state" == "OPEN" ]; then
    echo 
    ps_value=0xED
    connect_boot0
  fi

  if [ "$product_state" == "PROVISIONED" ]; then
    echo 
    ps_value=0x2E
    set_provisionning_ps
  fi

  if [ "$product_state" == "TZ-CLOSED" ]; then
    echo 
    ps_value=0xC6
    set_provisionning_ps
  fi

  if [ "$product_state" == "CLOSED" ]; then
    echo 
    ps_value=0x72
    set_provisionning_ps
  fi

  if [ "$product_state" == "LOCKED" ]; then
    echo 
    ps_value=0x5C
    set_provisionning_ps
  fi

  echo "       WRONG product state selected"
  current_log_file="./*.log files "
  echo
  product_state_choice
}
# ================================================ Option Bytes and flash programming ======================================================
# Connect BOOT0 pin to VDD
connect_boot0()
{

  echo "   * BOOT0 pin should be connected to VDD"
  echo "       (STM32H573I-DK: set SW1 to position 1)"
  echo "       Press any key to continue..."
  echo 
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  provisioning_step
}

# Provisioning execution 
set_provisionning_ps()
{
  action="Setting the product state PROVISIONING"
  current_log_file=$provisioning_log
  echo "   * $action"
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=0x17 > $provisioning_log
  if [ $? -ne 0 ]; then step_error; fi
  echo 
  provisioning_step
}

# Set the final product state of the STM32H5 product
set_final_ps()
{
  
  action="Setting the final product state $product_state "
  current_log_file=$provisioning_log
  echo "   * $action"
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=$ps_value >> $provisioning_log
  echo 
  if [ $? -ne 0 ]; then step_error; fi
  final_execution
}
# Provisioning the obk files step
provisioning_step()
{
  action="Provisionning the .obk files ..."
  current_log_file=$ob_key_provisioning_log
  echo "   * $action"
  command="source $ob_key_provisioning AUTO $product_state"
  $command > $ob_key_provisioning_log
  obkey_prog_error=$?
  if [ $obkey_prog_error -ne 0 ]; then step_error; fi
  echo "       Successful obk provisioning"
  echo "       (see $ob_key_provisioning_log for details)"
  echo 
  if [ "$product_state" != "OPEN" ]; then set_final_ps; fi
  set_final_ps
}

# ============================================================= End functions ==============================================================
# All the steps to the STM32H5 product were executed correctly
final_execution()
{
  echo "====="
  echo "===== The board is correctly configured."
  if [ $isGeneratedByCubeMX != "true" ]; then
    echo "===== Connect UART console (115200 baudrate) to get application menu."
  fi

  echo ===== Power off/on the board to start the application.
  echo =====

  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 0
}
# Error when external script is executed
step_error()
{ 
 #log error file
  if [ -e "error" ]; then
    cat error
    rm error
  fi
  echo 
  echo "====="
  echo "===== Error while executing "$action"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}
# ============================================================== Script start  =============================================================


echo "====="
echo "===== Provisioning of STiRoT_OEMuROT boot path"
echo "===== Application selected through env.sh:"
echo "=====   $oemirot_boot_path_project"
echo "===== Product state must be Open. Execute  /ROT_Provisioning/DA/regression.sh if not the case."
echo "====="
echo ""
if [ $isGeneratedByCubeMX != "true" ]; then
  if [[ ! $oemirot_boot_path_project =~ "OEMiROT_Appli_TrustZone" ]]; then
    echo "====="
    echo "===== Wrong Boot path: $oemirot_boot_path_project"
    echo "===== please modify the env.sh to the right path"
    step_error
  fi

  # =============================================== Steps to create the STiRoT_Config.obk file ==============================================
  echo "Step 1 : Configuration management"
  echo "   * STiRoT_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)"
  echo "       Select STiRoT_Config.xml(Default path is /ROT_Provisioning/STiROT_OEMuROT/Config/STiRoT_Config.xml)"
  echo "       Update the configuration (if/as needed) then generate STiRoT_Config.obk file"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  # =============================================== Steps to create the DA_Config.obk file ==============================================
  echo
  echo "   * DA_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)"
  echo "       Select DA_Config.xml(Default path is /ROT_Provisioning/DA/Config/DA_Config.xml)"
  echo "       Update the configuration (if/as needed) then generate DA_Config.obk file"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  # =============================================== Steps to create the OEMuRoT_Config.obk file ==============================================
  echo
  echo "   * OEMuRoT_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)"
  echo "       Select OEMuRoT_Config.xml(Default path is /ROT_Provisioning/STiROT_OEMuROT/Config/OEMuRoT_Config.xml)"
  echo "       Update the configuration (if/as needed) then generate OEMuRoT_Config.obk file"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  "$stm32tpccli" -obk Config/OEMuRoT_Config_Keys.xml >> $current_log_file
  if [ $? != "0" ]; then error_config; fi
fi

"$stm32tpccli" -pb ST/OEMuRoT_ST_Settings_1.xml >> $current_log_file
if [ $? != "0" ]; then error_config; fi
"$stm32tpccli" -obk ST/OEMuRoT_ST_Settings_2.xml >> $current_log_file
if [ $? != "0" ]; then error_config; fi
echo "       Successful OEMuRoT_Config.obk file generation"

if [ $isGeneratedByCubeMX != "true" ]; then
  #uncomment OEMUROT_ENABLE flag
  $python$applicfg setdefine -a uncomment -n OEMUROT_ENABLE -v 1 $flash_layout
  if [ $? != "0" ]; then error_config; fi

fi
# ========================================================= Images generation steps ========================================================  
echo
echo "Step 2 : Images generation"
echo "   * Boot firmware image generation"
echo "       Open the OEMiROT_Boot project with preferred toolchain and rebuild all files."
echo "       At this step the project is configured for STiROT_OEMuROT boot path"
echo "       Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
echo
#update xml file
if [ $isGeneratedByCubeMX != "true" ]; then
  source $tmp_file
  if [ "$app_image_number" != "2" ]; then
    ns_app_enc_sign_hex="$appli_dir/Binary/rot_tz_app_enc_sign.hex"
    ns_app_bin="$appli_dir/Binary/rot_tz_app.bin"
  fi
  $python$applicfg xmlval -v $s_app_bin --string -n "$fw_in_bin" $s_code_image_file --vb >> $current_log_file
  if [ $? != "0" ]; then error_config; fi
  $python$applicfg xmlval -v $ns_app_bin --string -n "$fw_in_bin" $ns_code_image_file --vb >> $current_log_file
  if [ $? != "0" ]; then error_config; fi
  $python$applicfg xmlval -v $s_app_enc_sign_hex --string -n "$fw_out_bin" $s_code_image_file --vb >> $current_log_file
  if [ $? != "0" ]; then error_config; fi
  $python$applicfg xmlval -v $ns_app_enc_sign_hex --string -n "$fw_out_bin" $ns_code_image_file --vb >> $current_log_file
  if [ $? != "0" ]; then error_config; fi

  echo "   * Code firmware image generation"
  echo "       Open the OEMxROT_Appli project with preferred toolchain."
  echo "       Rebuild all files. The appli_enc_sign.hex file is generated with the postbuild command."
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  
  if [ "$s_data_image_number" != "0" ]; then
    "$stm32tpccli" -pb $s_data_xml >> $provisioning_log
    if [ $? != "0" ]; then error_config; fi
  fi
  
  echo
  echo "   * Data generation (if Data image is enabled):"
  echo "       Select STiRoT_Data_Image.xml(Default path is /ROT_Provisioning/STiROT_OEMuROT/Image/STiRoT_Data_Image.xml)"
  echo "       Generate the data_enc_sign.hex image"
  echo "       Press any key to continue..."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  echo

  if [ "$ns_data_image_number" != "0" ]; then
    "$stm32tpccli" -pb $ns_data_xml >> $provisioning_log
    if [ $? != "0" ]; then error_config; fi
  fi
fi

# ========================================================= Board provisioning steps =======================================================  
echo "Step 3 : Provisioning"
echo "   * BOOT0 pin should be disconnected from VDD:"
echo "       (STM32H573I-DK: set SW1 to position 0)"
echo "       Press any key to continue..."
echo
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

# ================================================ Option Bytes and flash programming ====================================================
current_log_file=$ob_flash_log
action="Programming the option bytes and flashing the images ..."
command="source $ob_flash_programming AUTO"
echo "   * $action"
$command > $ob_flash_log
ob_flash_error=$?
if [ $ob_flash_error -ne 0 ]; then step_error; fi

echo "       Successful option bytes programming and images flashing"
echo "       (see $ob_flash_log for details)"
echo

# ============================================ Provisioning and product state modification =================================================
product_state_choice
