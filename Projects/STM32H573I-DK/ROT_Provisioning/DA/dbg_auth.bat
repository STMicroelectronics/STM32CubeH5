@echo off
call %~dp0../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Select Key/certificate (if TZEN enabled) and password (if TZEN disabled)
set key=Keys/key_3_leaf.pem
set cert=Certificates/cert_leaf_chain.b64
set pass=Binary/password.bin

:: Command with both key/certificate (if TZEN enabled) and password (if TZEN disabled)
set permission=
IF [%1] NEQ [] set permission="per=%1"

%stm32programmercli% -c port=SWD %permission% key=%key% cert=%cert% pwd=%pass% debugauth=1
IF !errorlevel! NEQ 0 goto :error

echo "dbg_auth script success"
IF [%2] NEQ [AUTO] cmd /k
exit 0

:error
echo "dbg_auth script failed"
IF [%2] NEQ [AUTO] cmd /k
exit 1
