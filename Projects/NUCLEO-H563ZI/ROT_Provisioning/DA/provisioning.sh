#!/bin/bash -
# Getting the CubeProgammer_cli path 
source ../env.sh

if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi
# CubeProgammer path and input files
ob_programming="ob_programming.sh"
obk_provisioning="obk_provisioning.sh"
# Log Files
ob_programming_log="ob_programming.log"
obk_provisioning_log="obk_provisioning.log"
provisioning_log="provisioning.log"

# Initial configuration
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

# ========================================================= Script functions ===============================================================
# ================================================ Final product state selection ===========================================================
product_state_choice()
{
  action="Define product state value"
  echo "   * $action"
  read -p "       $ps_option: " product_state
  product_state=$(echo $product_state | tr '[:lower:]' '[:upper:]')

  if [ "$product_state" == "PROVISIONED" ]; then
    echo 
    ps_value=0x2E
    set_final_ps
  fi
  if [ $tzen_state == "y" ]; then 
    if [ "$product_state" == "TZ-CLOSED" ]; then
      echo 
      ps_value=0xC6
      set_final_ps
    fi
  fi
  if [ "$product_state" == "CLOSED" ]; then
    echo 
    ps_value=0x72
    set_final_ps
  fi

  if [ "$product_state" == "LOCKED" ]; then
    echo 
    ps_value=0x5C
    set_final_ps
  fi

  echo "       WRONG product state selected"
  current_log_file="./*.log files "
  echo
  product_state_choice
}
# ================================================ Option Bytes and flash programming ======================================================
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
  echo ""
  #In the final product state, the connection with the board is lost and the return value of the command cannot be verified
  final_execution
}
# Provisioning the obk files step
provisioning_step()
{
  action="Provisionning the .obk files ..."
  current_log_file=$obk_provisioning_log
  echo "   * $action"
  command="source $obk_provisioning $tzen_state AUTO"
  $command > $obk_provisioning_log
  obkey_prog_error=$?
  if [ $obkey_prog_error -ne 0 ]; then step_error; fi
  echo "       Successful obk provisioning"
  echo "       (see $obk_provisioning_log for details)"
  echo 
  product_state_choice
}

# ============================================================= End functions ==============================================================
# All the steps to the STM32H5 product were executed correctly
final_execution()
{
  echo "====="
  echo "===== The board is correctly configured."
  echo "====="
  $SHELL;
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
  $SHELL
  exit 1
}
# ============================================================== Script start  =============================================================

echo ""
echo "====="
echo "===== Provisioning of DA"
echo "====="
echo ""

# ======================================================== Define board configuration ======================================================
echo "Step 1 : Configuration management"
action="   * The TrustZone feature is enabled ?"
read -p "$action [ y | n ]: " tzen_state
tzen_state=$(echo $tzen_state | tr '[:upper:]' '[:lower:]')
echo

if [ $tzen_state == "y" ]; then 
  da_file=DA_Config
else
  da_file=DA_ConfigWithPassword
fi

if [ $tzen_state == "y" ]; then 
  ps_option="[ PROVISIONED | TZ-CLOSED | CLOSED | LOCKED ]"
else
  ps_option="[ PROVISIONED | CLOSED | LOCKED ]"
fi

# =============================================== Steps to create the OEMiROT_Config.obk file ==============================================
echo "   * $da_file.obk generation:"
echo "       From TrustedPackageCreator (tab H5-OBkey)."
echo "       Select $da_file.xml (Default path is /ROT_Provisioning/DA/Config/$da_file.xml)"
echo "       Update the configuration (if/as needed) then generate $da_file.obk file"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s

# ========================================================= Board provisioning steps =======================================================
# ====================================================== Option Bytes programming ==========================================================
echo "Step 2 : Initial Option Bytes programming"
action="Programming the option bytes  ..."
current_log_file=$ob_programming_log
command="source $ob_programming $tzen_state AUTO"
echo "   * $action"
$command > $ob_programming_log
if [ $? -ne 0 ]; then step_error; fi

echo "       Successful option bytes programming"
echo "       (see $ob_programming_log for details)"
echo

# ========================================================= Images generation steps ========================================================
echo "Step 3 : Images flashing"
echo "   * At this step, you have to flash your application with your preferred toolchain"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s

# ============================================ Provisioning and product state modification =================================================
set_provisionning_ps
