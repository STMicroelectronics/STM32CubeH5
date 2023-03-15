@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path 
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

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