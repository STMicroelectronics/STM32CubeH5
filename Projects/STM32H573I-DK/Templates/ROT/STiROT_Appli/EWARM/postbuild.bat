@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path 
:: arg1 is the binary type (1 nonsecure, 2 secure)
set "projectdir=%~dp0"
set "signing=%1"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

set code_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Image.xml"

echo Postbuild STiROT image > %projectdir%\output.txt 2>&1
if %signing% == "secure" (
:: Create final image with Trust Package Creator
echo Creating STiROT image  >> %projectdir%\output.txt 2>&1
%stm32tpccli% -pb %code_xml% >> %projectdir%\output.txt 2>&1
if %errorlevel% neq 0 goto :error
)
exit 0

:error
exit 1