@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
set connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

:: In case of Provisioning Product state, try to provision DA with password
set "command=start /b call password_provisioning.bat AUTO"
%command% >nul

:: Regression with password
%stm32programmercli% -c port=SWD pwd=password.bin debugauth=1
if !errorlevel! neq 0 goto :error

echo "regression script success"
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo "regression script failed"
IF [%1] NEQ [AUTO] cmd /k
exit 1
