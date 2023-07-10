#!/bin/bash -
source ../env.sh

# CubeProgammer path and input files
ob_programming="ob_programming.sh"
password_programming="password_provisioning.sh"
create_password="create_password.sh"

ob_programming_log="ob_programming.log"
password_programming_log="password_provisioning.log"
create_password_log="create_password.log"
state_change_log="provisioning.log"

# Initial configuration
product_state="OPEN"
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

echo "projectdir = $projectdir"

# Error when external script is executed
step_error()
{
  echo 
  echo "====="
  echo "===== Error while executing "$action"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$1" != "AUTO" ]; then $SHELL; fi
#  exit 1
}

# ======================================================= Create board password =========================================================
create_password() {
  echo "   * Password board creation"
  echo "       Create a \"user_password.bin\" file.(Default path is \ROT_Provisioning\DA\user_password.bin)"
  echo "       WARNING: This step must be done at least one time to generate the password"
  echo "       If the content of user_password is correct the next setp could be executed"
  echo "       Press any key to continue..."
  echo
  read -p "" -n1 -s;
  action="Creating user password"
  current_log_file=$create_password_log
  command="source $create_password AUTO"
  echo "   * $action"
  $command > $create_password_log

  if [ $? -ne 0 ]; then step_error; fi

  echo "       User password correctly created"
  echo "       (see $create_password_log for details)"
  echo
  define_product_state
}
# ========================================= Product State configuration and Provisioning steps ==========================================

# ============================================================= End functions =============================================================
# All the steps to set the STM32H5 product were executed correctly
final_execution() {
  echo "====="
  echo "===== The board is correctly configured."
  echo "====="
  if [ "$1" != "AUTO" ]; then $SHELL; fi
#  exit 0
}

# Set the final product state of the STM32H5 product
set_final_ps() {

  action="Setting the final product state $product_state"
  current_log_file=$state_change_log
  echo "   * $action"
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=$ps_value >> $state_change_log
  #if [ $? -ne 0 ]; then step_error; fi
  echo
  final_execution
}

# Provisioning the obk files step
provisioning_step() {
  action="Provisionning the password ..."
  current_log_file=$password_programming_log
  echo "   * $action"
  command="source $password_programming AUTO"
  $command > $password_programming_log
  if [ $? -ne 0 ]; then step_error; fi

  echo "       Successful password provisioning"
  echo "       (see $password_programming_log for details)"
  echo

  if [ "$product_state" != "OPEN" ]; then set_final_ps; fi
  final_execution
}

# Provisioning execution
set_provisionning_ps() {
  action="Setting the product state PROVISIONING"
  current_log_file=$state_change_log
  echo "   * $action"
  "$stm32programmercli" $connect_reset > $state_change_log
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=0x17 >> $state_change_log
  if [ $? -ne 0 ]; then step_error; fi
  echo
  if [ "$password" == "y" ]; then 
    set_final_ps; 
  else
    provisioning_step
  fi
}
# ======================================================= define product state =========================================================
define_product_state() {
  action="Define final product state value"
  echo "   * $action"
  read -p "      $USERREG [ PROVISIONED | CLOSED | LOCKED ]: " product_state
  echo

  if [ "$product_state" == "PROVISIONED" ]; then
    ps_value=0x2E
    set_provisionning_ps
  fi

  if [ "$product_state" == "CLOSED" ]; then
    ps_value=0x72
    set_provisionning_ps
  fi

  if [ "$product_state" == "LOCKED" ]; then
    ps_value=0x5C
    set_provisionning_ps
  fi

  if [ "$product_state" != "PROVISIONED" ] && [ "$product_state" != "CLOSED" ] && [ "$product_state" != "LOCKED" ]; then
    echo "      Wrong product state $product_state"
    define_product_state
  fi
}


echo "====="
echo "===== Provisioning of DA"
echo "====="
echo

# ====================================================== Option Bytes programming ==========================================================
echo "Step 1 : Initial Option Bytes programming"
action="Programming the option bytes  ..."
current_log_file=$ob_programming_log
command="source $ob_programming AUTO"
echo "   * $action"
$command > $current_log_file
if [ $? -ne 0 ]; then step_error; fi

echo "       Successful option bytes programming"
echo "       (see $ob_programming_log for details)"
echo

# ========================================================= Images generation steps ========================================================
echo "Step 2 : Images flashing"
echo "   * At this step, you have to flash your application with your preferred toolchain"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s


# ================================================= Final product state selection ==========================================================
action="Password provisioning"
echo "   * $action"
echo "       WARNING: The password is definitively provisioned (in OTP), and cannot be changed even after regression"
echo "       Once provisioned, be sure not to change the password anymore (will not work)"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s
create_password