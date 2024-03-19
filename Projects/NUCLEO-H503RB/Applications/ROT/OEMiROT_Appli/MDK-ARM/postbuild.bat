@ECHO OFF
:: Getting the Trusted Package Creator CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion
:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%


echo Creating OEMiROT image  >> %current_log_file% 2>&1
%stm32tpccli% -pb "%provisioningdir%\OEMiROT\Images\OEMiROT_Code_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error

echo Creating OEMiROT data  >> %current_log_file% 2>&1
%stm32tpccli% -pb "%provisioningdir%\OEMiROT\Images\OEMiROT_Data_Image.xml" >> %current_log_file% 2>&1
IF !errorlevel! NEQ 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
