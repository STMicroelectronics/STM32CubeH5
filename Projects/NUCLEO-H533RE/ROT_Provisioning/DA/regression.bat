@echo off
call %~dp0../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
set connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

:: Select Key/certificate (if TZEN enabled) and password (if TZEN disabled)
set key=Keys/key_3_leaf.pem
set cert=Certificates/cert_leaf_chain.b64
set pass=Binary/password.bin

:: Close debug if opened
%stm32programmercli% %connect_no_reset% debugauth=3 >nul

:: In case of Provisioning Product state, try to disable TZEN and provision DA with password
%stm32programmercli% %connect_reset% -ob TZEN=0xC3 >nul
%stm32programmercli% %connect_reset% >nul
%stm32programmercli% %connect_no_reset% -sdp %~dp0Binary\DA_ConfigWithPassword.obk >nul

:: Command with both key/certificate (if TZEN enabled) and password (if TZEN disabled)
%stm32programmercli% -c port=SWD per=a key=%key% cert=%cert% pwd=%pass% debugauth=1
if !errorlevel! neq 0 goto :error

echo "regression script success"
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo "regression script failed"
IF [%1] NEQ [AUTO] cmd /k
exit 1
