@echo off

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Getting the CubeProgammer_cli path
call ../env.bat

:: CubeProgammer path and input files
set ob_programming="ob_programming.bat"
set password_programming="password_provisioning.bat"
set create_password="create_password.bat"

set ob_programming_log="ob_programming.log"
set password_programming_log="password_provisioning.log"
set create_password_log="create_password.log"
set state_change_log="provisioning.log"

:: Initial configuration
set product_state=OPEN
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

echo =====
echo ===== Provisioning of DA
echo =====
echo.

:: ====================================================== Option Bytes programming ==========================================================
echo Step 1 : Initial Option Bytes programming
set "action=Programming the option bytes  ..."
set current_log_file=%ob_programming_log%
set "command=start /w /b call %ob_programming% AUTO"
echo    * %action%
%command% > %ob_programming_log%
if !errorlevel! neq 0 goto :step_error

echo        Successful option bytes programming
echo        (see %ob_programming_log% for details)
echo.

:: ========================================================= Images generation steps ========================================================
echo Step 2 : Images flashing
echo    * At this step, you have to flash your application with your preferred toolchain
echo        Press any key to continue...
echo.
pause >nul


:: ================================================= Final product state selection ==========================================================
set "action=Password provisioning"
echo    * %action%
echo        WARNING: The password is definitively provisioned (in OTP), and cannot be changed even after regression
echo        Once provisioned, be sure not to change the password anymore (will not work)
echo        Press any key to continue...
echo.
pause >nul
goto create_password


:define_product_state
set "action=Define final product state value"
echo    * %action%
set /p "product_state=      %USERREG% [ PROVISIONED | CLOSED | LOCKED ]: "
echo.

if /i "%product_state%" == "PROVISIONED" (
set ps_value=0x2E
goto set_provisionning_ps
)

if /i "%product_state%" == "CLOSED" (
set ps_value=0x72
goto set_provisionning_ps
)

if /i "%product_state%" == "LOCKED" (
set ps_value=0x5C
goto set_provisionning_ps
)

echo       Wrong product state %product_state%
goto step_error
:: ======================================================= Create board password =========================================================
:create_password
echo    * Password board creation
echo        Create a "user_password.bin" file.(Default path is \ROT_Provisioning\DA\user_password.bin)
echo        WARNING: This step must be done at least one time to generate the password
echo        If the content of user_password is correct the next setp could be executed
echo        Press any key to continue...
echo.
pause >nul

set "action=Creating user password"
set current_log_file=%create_password_log%
set "command=start /w /b call %create_password% AUTO"
echo    * %action%
%command% > %create_password_log%

if !errorlevel! neq 0 goto :step_error

echo        User password correctly created
echo        (see %create_password_log% for details)
echo.
goto define_product_state

:: ========================================= Product State configuration and Provisioning steps ==========================================

:: Provisioning execution
:set_provisionning_ps
set "action=Setting the product state PROVISIONING"
set current_log_file=%state_change_log%
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -ob PRODUCT_STATE=0x17"
%stm32programmercli% %connect_reset% > %state_change_log%
echo %command% >> %state_change_log%
%command% >> %state_change_log%
if !errorlevel! neq 0 goto :step_error
echo.
if /i "%password%" == "y" goto :set_final_ps
goto provisioning_step

:: Set the final product state of the STM32H5 product
:set_final_ps
set "action=Setting the final product state %product_state%"
set current_log_file=%state_change_log%
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -ob PRODUCT_STATE=%ps_value%"
echo %command% >> %state_change_log%
%command% >> %state_change_log%
echo.
:: In the final product state, the connection with the board is lost and the return value of the command cannot be verified
goto final_execution

:: Provisioning the obk files step
:provisioning_step
set "action=Provisionning the password ..."
set current_log_file=%password_programming_log%
echo    * %action%
set "command=start /w /b call %password_programming% AUTO"
%command% > %password_programming_log%
if !errorlevel! neq 0 goto :step_error

echo        Successful password provisioning
echo        (see %password_programming_log% for details)
echo.
if /i "%product_state%" NEQ "OPEN" goto :set_final_ps


:: ============================================================= End functions =============================================================
:: All the steps to set the STM32H5 product were executed correctly
:final_execution
echo =====
echo ===== The board is correctly configured.
echo =====
cmd /k
exit 0

:: Error when external script is executed
:step_error
echo.
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
cmd /k
exit 1

