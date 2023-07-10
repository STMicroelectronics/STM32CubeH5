#!/bin/bash -
# Getting the CubeProgammer_cli path 
if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

source ../env.sh NULL

# Environment variable for AppliCfg
SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`

isGeneratedByCubeMX=$PROJECT_GENERATED_BY_CUBEMX

# CubeProgammer path and input files
ob_flash_programming="ob_flash_programming.sh"
password_programming="../DA/password_provisioning.sh"
create_password="../DA/create_password.sh"

ob_flash_log="ob_flash_programming.log"
password_programming_log="../DA/password_provisioning.log"
create_password_log="../DA/create_password.log"

# Environment variable for log file
current_log_file="provisioning.log"
echo "" > $current_log_file



if [ $isGeneratedByCubeMX == "true" ]; then
   appli_dir=$oemirot_boot_path_project
else
   appli_dir="../../../$oemirot_boot_path_project"
fi

# Variables for image xml configuration
fw_in_bin="Firmware binary input file"
fw_out_bin="Image output file"
app_bin="$appli_dir/Binary/rot_app.bin"
app_enc_sign_hex="$appli_dir/Binary/rot_app_enc_sign.hex"
code_image_file="$projectdir/Images/OEMiROT_Code_Image.xml"


# Initial configuration
product_state=OPEN
connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
connect_reset="-c port=SWD speed=fast ap=1 mode=UR"

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

# ========================================================= Script functions ===============================================================
# ================================================ Final product state selection ===========================================================
define_product_state()
{
  action="Define final product state value"
  echo "   * $action"
  read -p "       [ OPEN | PROVISIONED | CLOSED | LOCKED ]: " product_state
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
  define_product_state
}
# ================================================ Option Bytes and flash programming ======================================================
# Connect BOOT0 pin to VDD
connect_boot0()
{
  echo "   * BOOT0 pin should be connected to VDD"
  echo "       (NUCLEO-H503RB: connect CN7/pin5 with CN7/pin7)"
  echo "       Press any key to continue..."
  echo 
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  provisioning_step
}

# Provisioning execution 
set_provisionning_ps()
{
  action="Setting the product state PROVISIONING"
  echo "   * $action"
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=0x17 >> $current_log_file
  if [ $? -ne 0 ]; then step_error; fi
  echo 
  provisioning_step
}

# Set the final product state of the STM32H5 product
set_final_ps()
{
  
  action="Setting the final product state $product_state "
  echo "   * $action"
  "$stm32programmercli" $connect_no_reset -ob PRODUCT_STATE=$ps_value >> $current_log_file
  echo 
  #if [ $? -ne 0 ]; then step_error; fi
  final_execution
}
# Provisioning the obk files step
provisioning_step()
{
  action="Provisionning the password ..."
  current_log_file=$password_programming_log
  echo "   * $action"
  command="source $password_programming AUTO"
  $command > $password_programming_log
  obkey_prog_error=$?
  if [ $obkey_prog_error -ne 0 ]; then step_error; fi
  echo "       Successful password provisioning"
  echo "       (see $password_programming_log for details)"
  echo 
  if [ "$product_state" != "OPEN" ]; then set_final_ps; fi
  disconnect_boot0
  final_execution
}
disconnect_boot0()
{
  echo "   * BOOT0 pin should be disconnected from VDD"
  echo "       (NUCLEO-H503RB: disconnect CN7/pin5 from CN7/pin7)"
  echo "       Press any key to continue..."
  echo ""
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
}
create_password ()
{
  echo "   * Password board creation"
  echo "       Create a "user_password.bin" file.(Default path is /ROT_Provisioning/DA/user_password.bin)"
  echo "       WARNING: This step must be done at least one time to generate the password"
  echo "       If the content of the user_password is correct the next step could be executed"
  echo "       Press any key to continue..."
  echo
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

  action="Creating user password"
  current_log_file=$create_password_log
  command="source $create_password AUTO"
  echo "   * $action"
  $command > $create_password_log
  create_password_error=$?
  if [ $create_password_error -ne 0 ]; then step_error; fi

  echo "       User password correctly created"
  echo "       (see $create_password_log for details)"
  echo
  define_product_state
}
# ============================================================= End functions ==============================================================
# All the steps to the STM32H5 product were executed correctly
final_execution()
{
  echo "====="
  echo "===== The board is correctly configured."
  if [ "$isGeneratedByCubeMX" == "true" ]; then no_menu; fi
  echo "====="
  echo "===== Connect UART console (115200 baudrate) to get application menu."
  no_menu
}

no_menu()
{
  echo "====="
  $SHELL
#  exit 0
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
#  exit 1
}
# ============================================================== Script start  =============================================================

echo "====="
echo "===== Provisioning of OEMiRoT boot path"
echo "===== Application selected through env.sh:"
echo "=====   $oemirot_boot_path_project"
echo "===== Product state must be Open. Execute  \ROT_Provisioning\DA\regression.sh if not the case."
echo "====="
echo ""

# bypass this step when Generated By CubeMX
if [ $isGeneratedByCubeMX == "true" ]; then
  if [[ ! $oemirot_boot_path_project =~ "OEMiROT_Appli" ]]; then
    echo "====="
    echo "===== Wrong Boot path: $oemirot_boot_path_project"
    echo "===== please modify the env.bat to set the right path"
    step_error
  fi
else

  # =============================================== Steps to create the OEMiROT_Config.obk file ==============================================
  echo "Step 1 : Configuration management"
  echo "   * OEM Keys generation:"
  echo "       Warning: Default keys must NOT be used in a product."
  echo "       Run Keygen script to randomly regenerate your own keys (Keys/*.pem and OEMiROT_Boot/Src/keys.c)"
  echo "       Press any key to continue..."
  echo ""
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi


fi
# ========================================================= Images generation steps ========================================================  
echo "Step 2 : Images generation"
echo "   * Boot firmware image generation"
echo "       Open the OEMiROT_Boot project with preferred toolchain and rebuild all files."
echo "       Press any key to continue..."
echo
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

#update xml file
if [ "$isGeneratedByCubeMX" != "true" ]; then

  $python$applicfg xmlval -v $app_bin --string -n "$fw_in_bin" $code_image_file --vb >> $current_log_file
  if [ $? -ne 0 ]; then step_error; fi
  $python$applicfg xmlval -v $app_enc_sign_hex --string -n "$fw_out_bin" $code_image_file --vb >> $current_log_file
  if [ $? -ne 0 ]; then step_error; fi

  echo "   * Code firmware image generation"
  echo "       Open the OEMiROT_Appli project with preferred toolchain."
  echo "       Rebuild all files. The rot_app_enc_sign.hex file is generated with the postbuild command."
  echo "       Press any key to continue..."
  echo
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
  echo "   * Data generation (if Data image is enabled)"
  echo "       Select OEMiROT_Data_Image.xml(Default path is ROT_Provisioning/OEMiROT/Images/OEMiROT_Data_Image.xml)"
  echo "       Generate the data_enc_sign.hex image"
  echo "       Press any key to continue..."
  echo
  if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
fi
# ========================================================= Board provisioning steps =======================================================  
echo "Step 2 : Provisioning password"
echo "   * BOOT0 pin should be disconnected from VDD"
echo "       (NUCLEO-H503RB: disconnect CN7/pin5 from CN7/pin7)"
echo "       Press any key to continue..."
echo
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi


# ================================================ Option Bytes and flash programming ======================================================
action="Programming the option bytes and flashing the images ..."
current_log_file=$ob_flash_log
command="source $ob_flash_programming AUTO"
echo "   * $action"
$command > $current_log_file
ob_flash_error=$?
if [ $ob_flash_error -ne 0 ]; then step_error; fi
echo "       Successful option bytes programming and images flashing"
echo "       (see $ob_flash_log for details)"
echo 
# ============================================= Define if password creation is necessary  ==================================================

action="Password provisioning"
echo "   * $action"
echo "       WARNING: The password is definitively provisioned (in OTP), and cannot be changed even after regression"
echo "       Once provisioned, be sure not to change the password anymore (will not work)"
echo "       Press any key to continue..."
echo
read -p "" -n1 -s
create_password
