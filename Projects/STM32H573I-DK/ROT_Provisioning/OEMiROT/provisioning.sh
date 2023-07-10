#!/bin/bash -
# Getting the CubeProgammer_cli path 
source ../env.sh

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX

# Environment variable for AppliCfg
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`

if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi
# CubeProgammer path and input files
ob_flash_programming="ob_flash_programming.sh"
obkey_programming="obkey_programming.sh"

ob_flash_log="ob_flash_programming.log"
obkey_programming_log="obkey_programming.log"
provisioning_log="provisioning.log"

# Initial configuration
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
else
   appli_dir="../../../$oemirot_boot_path_project"
fi

flash_layout="$cube_fw_path/Projects/STM32H573I-DK/Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"

## Get config updated by OEMiROT_Boot
tmp_file=$cube_fw_path/Projects/STM32H573I-DK/ROT_Provisioning/img_config.sh

fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
ns_app_bin="$appli_dir/Binary/rot_tz_ns_app.bin"
s_app_bin="$appli_dir/Binary/rot_tz_s_app.bin"

s_code_image_file=$project_dir"/Images/OEMiROT_S_Code_Image.xml"
ns_code_image_file=$project_dir"/Images/OEMiROT_NS_Code_Image.xml"
s_data_xml=$project_dir"/Images/OEMiROT_S_Data_Image.xml"
ns_data_xml=$project_dir"/Images/OEMiROT_NS_Data_Image.xml"
ns_app_enc_sign_hex=$appli_dir"/Binary/rot_tz_ns_app_enc_sign.hex"
s_app_enc_sign_hex=$appli_dir"/Binary/rot_tz_s_app_enc_sign.hex"

# Initial configuration
product_state=OPEN
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

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

# ========================================================= Script functions ===============================================================
# ================================================ Final product state selection ===========================================================
product_state_choice()
{
  action="Define product state value"
  echo "   * $action"
  if [ "$mode" != "AUTO" ]; then
    read -p "       [ OPEN | PROVISIONED | TZ-CLOSED | CLOSED | LOCKED ]: " product_state
    product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')
  else
    product_state=$2
    if [ "$product_state" == "OPEN" ]; then
      echo "       Product state OPEN not supported in AUTO mode"
      step_error
    fi
  fi

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

disconnect_boot0()
{
  echo "   * BOOT0 pin should be disconnected from VDD"
  echo "       (STM32H573I-DK: set SW1 to position 0)"
  echo "       Press any key to continue..."
  echo 
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
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
  #if [ $? -ne 0 ]; then step_error; fi
  final_execution
}
# Provisioning the obk files step
provisioning_step()
{
  action="Provisionning the .obk files ..."
  current_log_file=$obkey_programming_log
  echo "   * $action"
  command="source $obkey_programming AUTO"
  $command > $obkey_programming_log
  obkey_prog_error=$?
  if [ $obkey_prog_error -ne 0 ]; then step_error; fi
  echo "       Successful obk provisioning"
  echo "       (see $obkey_programming_log for details)"
  echo 
  if [ "$product_state" != "OPEN" ]; then set_final_ps; fi
  disconnect_boot0
  final_execution
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

# ========================================================== Initial instructions ==========================================================
echo "====="
echo "===== Provisioning of OEMiRoT boot path"
echo "===== Application selected through env.sh: $oemirot_boot_path_project"
echo "===== Product state must be Open. Execute  /ROT_Provisioning/DA/regression.sh if not the case."
echo "====="
echo 

# bypass this step when Generated By CubeMX
if [ $isGeneratedByCubeMX == "false" ]; then
  if [[ ! $oemirot_boot_path_project =~ "OEMiROT_Appli_TrustZone" ]]; then
    echo "====="
    echo "===== Wrong Boot path: $oemirot_boot_path_project"
    echo "===== please modify the env.sh to the right path"
    step_error
  fi

  # =============================================== Steps to create the OEMiROT_Config.obk file ==============================================
  echo "Step 1 : Configuration management"
  echo "   * OEMiROT_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)."
  echo "       Select OEMiROT_Config.xml(Default path is /ROT_Provisioning/OEMiROT/Config/OEMiROT_Config.xml)"
  echo "       Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys"
  echo "       Update the configuration (if/as needed) then generate OEMiROT_Config.obk file"
  echo "       Press any key to continue..."
  echo  
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

  # =============================================== Steps to create the DA_Config.obk file ===================================================
  echo "   * DA_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)."
  echo "       Select DA_Config.xml(Default path is /ROT_Provisioning/DA/DA_Config.xml)"
  echo "       Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys"
  echo "       Update the configuration (if/as needed) then generate DA_Config.obk file"
  echo "       Press any key to continue..."
  echo 
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

  $python$applicfg setdefine -a comment -n OEMUROT_ENABLE -v 1 $flash_layout
  if [ $? -ne 0 ]; then error; return 1; fi
fi
# ========================================================= Images generation steps ========================================================
echo "Step 2 : Images generation"
echo "   * Boot firmware image generation"
echo "       Open the OEMiROT_Boot project with preferred toolchain and rebuild all files."
echo "       At this step the project is configured for OEMiROT boot path."
echo "       Press any key to continue..."
echo
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

#update xml file
if [ $isGeneratedByCubeMX == "false" ]; then
    source $tmp_file
    if [ $app_image_number != "2" ]; then
        ns_app_enc_sign_hex=$appli_dir"/Binary/rot_tz_app_enc_sign.hex"
        ns_app_bin=$appli_dir"/Binary/rot_tz_app.bin"
    fi
    $python$applicfg xmlval -v $s_app_bin --string -n "$fw_in_bin" $s_code_image_file
    if [ $? != "0" ]; then step_error; fi
    $python$applicfg xmlval -v $ns_app_bin --string -n "$fw_in_bin" $ns_code_image_file
    if [ $? != "0" ]; then step_error; fi
    $python$applicfg xmlval -v $s_app_enc_sign_hex --string -n "$fw_out_bin" $s_code_image_file
    if [ $? != "0" ]; then step_error; fi
    $python$applicfg xmlval -v $ns_app_enc_sign_hex --string -n "$fw_out_bin" $ns_code_image_file
    if [ $? != "0" ]; then step_error; fi

    echo "   * Code firmware image generation"
    echo "       Open the OEMiROT_Appli_TrustZone project with preferred toolchain."
    echo "       Rebuild all files. The appli_enc_sign.hex file is generated with the postbuild command."
    echo "       Press any key to continue..."
    echo 
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

    echo "   * Data secure generation (if Data secure image is enabled)"
    echo "       Select OEMiRoT_S_Data_Image.xml(Default path is /ROT_Provisioning/OEMiROT/Images/OEMiRoT_S_Data_Image.xml)"
    echo "       Generate the data_enc_sign.hex image"
    echo "       Press any key to continue..."
    echo  
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
    if [ $s_data_image_number != "0" ]; then
        "$stm32tpccli" -pb $s_data_xml >> $provisioning_log
        if [ $? != "0" ]; then step_error; fi
    fi

    echo "   * Data non secure generation (if Data non secure image is enabled)"
    echo "       Select OEMiROT_NS_Data_Image.xml(Default path is /ROT_Provisioning/OEMiROT/Images/OEMiROT_NS_Data_Image.xml)"
    echo "       Generate the data_enc_sign.hex image"
    echo "       Press any key to continue..."
    echo 
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
    if [ $ns_data_image_number != "0" ]; then
      "$stm32tpccli" -pb $ns_data_xml >> $provisioning_log
      if [ $? != "0" ]; then step_error; fi
    fi
fi
# ========================================================= Board provisioning steps =======================================================
echo "Step 3 : Provisioning"
echo "   * BOOT0 pin should be disconnected from VDD"
echo "       (STM32H573I-DK: set SW1 to position 0)"
echo "       Press any key to continue..."
echo 
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

# ================================================ Option Bytes and flash programming ======================================================
action="Programming the option bytes and flashing the images ..."
current_log_file=$ob_flash_log
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
