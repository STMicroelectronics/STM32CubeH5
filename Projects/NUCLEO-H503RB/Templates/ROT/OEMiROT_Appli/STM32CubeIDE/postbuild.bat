@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
:: arg1 is the config type (Debug, Release)
set "projectdir=%~dp0"
set "config=%1"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Copy application build in Binary location
copy %projectdir%\%config%\NUCLEO-H503RB_Templates.bin %projectdir%\..\Binary\rot_app.bin

set code_xml="%provisioningdir%\OEMiROT\Images\OEMiRoT_Code_Image.xml"
set data_xml="%provisioningdir%\OEMiROT\Images\OEMiRoT_Data_Image.xml"

echo Creating OEMiROT image  > %projectdir%\output.txt 2>&1
%stm32tpccli% -pb %code_xml% >> %projectdir%\output.txt 2>&1
if !errorlevel! neq 0 goto :error

echo Creating OEMiROT data  >> %projectdir%\output.txt 2>&1
%stm32tpccli% -pb %data_xml% >> %projectdir%\output.txt 2>&1
if !errorlevel! neq 0 goto :error


exit 0

:error
exit 1