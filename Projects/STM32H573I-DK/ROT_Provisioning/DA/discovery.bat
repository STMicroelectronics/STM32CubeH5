@echo off
call %~dp0../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

%stm32programmercli% -c port=SWD debugauth=2
IF !errorlevel! NEQ 0 goto :error

echo "discovery script success"
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo "discovery script failed"
IF [%1] NEQ [AUTO] cmd /k
exit 1
