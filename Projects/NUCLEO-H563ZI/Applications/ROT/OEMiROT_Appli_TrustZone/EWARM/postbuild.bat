@ECHO OFF
:: arg1 is the binary type (1 nonsecure, 2 secure)
set "signing=%1"

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

set s_code_xml="%provisioningdir%\OEMiROT\Images\OEMiROT_S_Code_Image.xml"
set ns_code_xml="%provisioningdir%\OEMiROT\Images\OEMiROT_NS_Code_Image.xml"
set s_code_bin="%projectdir%\..\Binary\rot_tz_s_app.bin"
set ns_code_bin="%projectdir%\..\Binary\rot_tz_ns_app.bin"
set one_code_bin="%projectdir%\..\Binary\rot_tz_app.bin"

::Variables updated by OEMiROT_Boot postbuild
set app_image_number=2
set image_s_size=0x6000

:start
goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
goto postbuild
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:postbuild
echo Postbuild %signing% image >> %current_log_file% 2>&1

if "%app_image_number%" == "2" (
goto :continue
)
IF %signing% == "nonsecure" (
echo Creating only one image >> %current_log_file% 2>&1
%python%%applicfg% oneimage -fb %s_code_bin% -o %image_s_size% -sb %ns_code_bin% -i 0x0 -ob %one_code_bin% --vb >> %current_log_file% 2>&1
if !errorlevel! neq 0 goto :error
)

:continue
IF %signing% == "secure" (
echo Creating secure image  >> %current_log_file% 2>&1
%stm32tpccli% -pb %s_code_xml% >> %current_log_file% 2>&1
if !errorlevel! neq 0 goto :error
)

IF %signing% == "nonsecure" (
echo Creating nonsecure image  >> %current_log_file% 2>&1
%stm32tpccli% -pb %ns_code_xml% >> %current_log_file% 2>&1
if !errorlevel! neq 0 goto :error
)
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
