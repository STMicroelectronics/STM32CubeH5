@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: CubeProgammer path and input files
set ob_programming="ob_programming.bat"
set obk_provisioning="obk_provisioning.bat"
:: Log Files
set ob_programming_log="ob_programming.log"
set obk_provisioning_log="obk_provisioning.log"
set provisioning="provisioning.log"
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

echo.
echo =====
echo ===== Provisioning of DA
echo =====
echo.
::Define a BS variable containing a backspace (0x08) character
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"
:: ======================================================== Define board configuration ======================================================
echo Step 1 : Configuration management
set "action= * The TrustZone feature is enabled ?"
set /p "tzen_state=%BS%  %action%  [ y | n ]: "
echo.
if /i "%tzen_state%" == "y" (set da_file=DA_Config) else (set da_file=DA_ConfigWithPassword)
if /i "%tzen_state%" == "y" (set "ps_option=[ PROVISIONED | TZ-CLOSED | CLOSED | LOCKED ]") else (set "ps_option=[ PROVISIONED | CLOSED | LOCKED ]")

:: =============================================== Steps to create the OEMiROT_Config.obk file ==============================================
echo    * %da_file%.obk generation:
echo        From TrustedPackageCreator (tab H5-OBkey).
echo        Select %da_file%.xml (Default path is \ROT_Provisioning\DA\Config\%da_file%.xml)
echo        Update the configuration (if/as needed) then generate %da_file%.obk file
echo        Press any key to continue...
echo.
pause >nul

:: ========================================================= Board provisioning steps =======================================================
:: ====================================================== Option Bytes programming ==========================================================
echo Step 2 : Initial Option Bytes programming
set "action=Programming the option bytes  ..."
set current_log_file=%ob_programming_log%
set "command=start /w /b call %ob_programming% %tzen_state% AUTO"
echo    * %action%
%command% > %ob_programming_log%
if !errorlevel! neq 0 goto :step_error

echo        Successful option bytes programming
echo        (see %ob_programming_log% for details)
echo.

:: ========================================================= Images generation steps ========================================================
echo Step 3 : Images flashing
echo    * At this step, you have to flash your application with your preferred toolchain
echo        Press any key to continue...
echo.
pause >nul

:: ================================================== Provisioning the obk file ===========================================================
:: Set the product state "Provisioning". This will allow to execute the provisioning step
:set_provisioning_ps
set "action=Setting the product state PROVISIONING"
set current_log_file=%provisioning%
echo    * %action%
echo.

set "command=%stm32programmercli% %connect_no_reset% -ob PRODUCT_STATE=0x17"
echo %command% > %provisioning%
%command% >> %provisioning%
if !errorlevel! neq 0 goto :step_error
goto provisioning_step


:: Provisioning the obk file step
:provisioning_step
set "action=Provisioning the .obk file ..."
set current_log_file=%obk_provisioning_log%
set "command=start /w /b call %obk_provisioning% %tzen_state% AUTO"
echo    * %action%
%command% > %obk_provisioning_log%
if !errorlevel! neq 0 goto :step_error

echo        Successful obk provisioning
echo        (see %obk_provisioning_log% for details)
echo.

:provisioning
:: ================================================ Final product state selection =========================================================
:product_state_choice
for /f %%A in ('"prompt $H & echo on & for %%B in (1) do rem"') do set "BS=%%A"
set "action=Define product state value"
echo    * %action%
set /p "product_state=%BS%       %ps_option%: "

if /i "%product_state%" == "PROVISIONED" (
echo.
set ps_value=0x2E
goto set_final_ps
)

if /i "%tzen_state%" == "y" (
if /i "%product_state%" == "TZ-CLOSED" (
echo.
set ps_value=0xC6
goto set_final_ps
)
)

if /i "%product_state%" == "CLOSED" (
echo.
set ps_value=0x72
goto set_final_ps
)

if /i "%product_state%" == "LOCKED" (
echo.
set ps_value=0x5C
goto set_final_ps
)

echo        WRONG product state selected
set current_log_file="./*.log files "
echo;
goto product_state_choice

: Set the final product state of the STM32H5 product
:set_final_ps
set "action=Setting the final product state %product_state% "
set current_log_file=%provisioning%
echo    * %action%
set "command=%stm32programmercli% %connect_no_reset% -ob PRODUCT_STATE=%ps_value%"
echo %command% >> %provisioning%
%command% >> %provisioning%
echo.
:: In the final product state, the connection with the board is lost and the return value of the command cannot be verified
goto final_execution

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
