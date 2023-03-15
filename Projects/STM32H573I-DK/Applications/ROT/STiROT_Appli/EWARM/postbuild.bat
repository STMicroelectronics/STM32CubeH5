@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path 
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Create final image with Trust Package Creator
echo Creating STiROT image  > %projectdir%\output.txt 2>&1
set code_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Image.xml"
%stm32tpccli% -pb %code_xml% >> %projectdir%\output.txt 2>&1
if !errorlevel! neq 0 goto :error
exit 0

:error
exit 1