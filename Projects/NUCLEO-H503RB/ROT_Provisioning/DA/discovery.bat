@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

%stm32programmercli% -c port=SWD debugauth=2
IF !errorlevel! NEQ 0 goto :error

echo "discovery script success"
cmd /k
exit 0

:error
echo "discovery script failed"
cmd /k
exit 1
