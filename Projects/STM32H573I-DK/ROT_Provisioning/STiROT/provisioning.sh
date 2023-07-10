#!/bin/bash -
# Getting the CubeProgammer_cli path 
source ../env.sh

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX
full_secure=$isFullSecure
if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`

# Environment variable used to know if the firmware image is full secure or not
stirot_config="./Config/STiRoT_Config.xml"
is_fw_sec="Is the firmware full secure"

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
AppliCfg=$python$applicfg

# External scripts
ob_flash_programming="ob_flash_programming.sh"
ob_key_provisioning="obkey_provisioning.sh"
update_ob_setup="update_ob_setup.sh"
update_appli_setup="update_appli_setup.sh"

# Logs files
ob_update_ob_log="update_ob_setup.log"
ob_update_appli_log="update_appli_setup.log"
ob_key_provisioning_log="obkey_provisioning.log"
provisioning_log="provisioning.log"
ob_flash_log="ob_flash_programming.log"

# Initial configuration
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_under_reset="-c port=SWD speed=fast ap=1 mode=UR"

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
    # Reset SECBOOT_LOCK option bit to 0xC3 (unlock boot address & unique boot entry) to handle Open product state
    echo "Resetting SECBOOT_LOCK to 0xC3 value" > $provisioning_log
    "$stm32programmercli" $connect_under_reset -ob SECBOOT_LOCK=0xC3 >> $provisioning_log
    if [ $? -ne 0 ]; then step_error; fi
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
  final_execution
}

# ============================================================= End functions ==============================================================
# All the steps to the STM32H5 product were executed correctly
final_execution()
{
  echo "====="
  echo "===== The board is correctly configured."
  echo "===== Connect UART console (115200 baudrate) to get application menu."
  echo "===== Power off/on the board to start the application."
  echo "====="
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
  echo "====="
  echo "===== Error while executing "$action"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}

# Define principal log file 
current_log_file=$provisioning_log
# Parse the project name chosen in env.sh
project_name=$(basename $oemirot_boot_path_project)

echo "====="
echo "===== Provisioning of STiRoT boot path"
echo "===== Application selected through env.sh: $stirot_boot_path_project"
echo "===== Product state must be Open. Execute  /ROT_Provisioning/DA/regression.sh if not the case."
echo "====="
echo 

# ============================================================== Script start  =============================================================
# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  # Verify the project name configuration
  if [[ ! $stirot_boot_path_project =~ "STiROT_Appli" ]]; then
    echo "====="
    echo "===== Wrong Boot path: $stirot_boot_path_project"
    echo "===== please modify the env.sh to set the right path"
    step_error
  fi
  # ====================================================== STM32H5 product preparation ======================================================
  echo "Step 1 : Configuration management"
  echo "   * STiRoT_Config.obk generation:"
  echo "       From TrustedPackageCreator (tab H5-OBkey)"
  echo "       Select STiRoT_Config.xml(Default path is /ROT_Provisioning/STiROT/Config/STiRoT_Config.xml)"
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

  # ====================================== Step to update full secure value in updateAppliSetup.sh ========================================
  echo
  # bypass this step when Generated By CubeMX
  if [ "$isGeneratedByCubeMX" == "false" ]; then
    echo "   * updateAppliSetup script update ..."
    $AppliCfg flash -xml $stirot_config -nxml "$is_fw_sec" -b "Full_secure" -sr "fullsecure.txt" $update_appli_setup --decimal
    if [ $? -ne 0 ]; then step_error; fi
    echo "       Full_secure variable successfully update according to STiRoT_Config.xml"
    # Step to get the value stored in fullsecure.txt file ======
    full_secure=`cat fullsecure.txt`
    rm fullsecure.txt
  fi

  # Check if project name is relevant with STiROT configuration xml file
  if [ "$full_secure" == "1" ]; then
    action="Check path project to STiROT_Appli"
    if [ "$project_name" == "STiROT_Appli" ]; then
      echo "STiROT_Appli_TrustZone project name selected is different from full secure configuration set into STiROT_Config.xml file" > $current_log_file 
      step_error
    fi
  fi
  if [ "$full_secure" == "0" ]; then
    action="Check path project to STiROT_Appli_TrustZone"
    if [ "$project_name" == "STiROT_Appli_TrustZone" ]; then
      echo "STiROT_Appli project name selected is different from fw full secure configuration set into STiROT_Config.xml" > $current_log_file
      step_error
    fi
  fi

fi
# ======================================================= Updating the Option bytes =======================================================
echo
current_log_file=$ob_update_ob_log
action="ob_flash_programming script update ..."
command="source $update_ob_setup AUTO"
echo "   * $action"
$command > $ob_update_ob_log
ob_update_ob_error=$?
if [ $ob_update_ob_error -ne 0 ]; then step_error; fi

echo "       Option bytes successfully updated according to STiRoT_Config.xml"
echo "       (see $ob_update_ob_log for details)"

# ================================================ Updating test Application files ========================================================
echo
current_log_file=$ob_update_appli_log
command="source $update_appli_setup AUTO"
# must be bypassed when generated by CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  if [ "$full_secure" == "1" ]; then
    action="STiROT_Appli project files (main.h, .icf) update ..."
  else
    action="STiROT_Appli_TrustZone project files (main.h, .icf) update ..."
  fi
  echo "   * $action"

  $command > $ob_update_appli_log
  if [ $? -ne 0 ]; then step_error; fi

  if [ "$full_secure" == "1" ]; then
    echo "       stm32h573xx_flash.icf and main.h successfully updated according to STiRoT_Config.xml"
  fi
  if [ "$full_secure" == "0" ]; then
    echo "       stm32h573xx_flash_s.icf and stm32h573xx_flash_ns.icf successfully updated according to STiRoT_Config.xml"
  fi

  echo "       (see $ob_update_appli_log for details)"
 
fi
# ========================================================= Images generation steps ========================================================  
echo
echo "Step 2 : Images generation"

# bypass this step when Generated By CubeMX
if [ "$isGeneratedByCubeMX" == "false" ]; then
  echo "   * Code firmware image generation:"

  if [ "$full_secure" == "1" ]; then
    echo "       Open the STiROT_Appli project with your preferred toolchain"
  fi
  if [ "$full_secure" == "0" ]; then
    echo "        Open the STiROT_Appli_TrustZone project with your preferred toolchain"
  fi
  echo "       Rebuild all files. The appli_enc_sign.hex file is generated with the postbuild command"
  echo "       Press any key to continue.."
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  echo
fi
echo "   * Data generation (if Data image is enabled):"
echo "       Select STiRoT_Data_Image.xml(Default path is /ROT_Provisioning/STiROT/Image/STiRoT_Data_Image.xml)"
echo "       Generate the data_enc_sign.hex image"
echo "       Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
echo

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
