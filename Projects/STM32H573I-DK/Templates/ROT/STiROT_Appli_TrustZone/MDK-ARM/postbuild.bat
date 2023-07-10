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

:: arg1 is the binary type (1 nonsecure, 2 secure)
set "signing=%1"

set code_xml="%projectdir%\..\..\..\..\ROT_Provisioning\STiROT\Image\STiRoT_Code_Image.xml"
set s_code_bin="%projectdir%\Secure\STM32H573I-DK_S\Exe\Project.bin"
set ns_code_bin="%projectdir%\NonSecure\STM32H573I-DK_NS\Exe\Project.bin"
set s_ns_code_bin="%projectdir%\..\Binary\appli.bin"

::Variables updated by update_appli_setup
set image_size=0x00006000

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
echo Postbuild STiROT image >> %current_log_file% 2>&1
if %signing% == "nonsecure" (
:: Create one image (assembly secure and non secure binaries)
echo Creating only one image >> %current_log_file% 2>&1
%python%%applicfg% oneimage -fb %s_code_bin% -o %image_size% -sb %ns_code_bin% -i 0x0 -ob %s_ns_code_bin% --vb >> %current_log_file% 2>&1
if !errorlevel! neq 0 goto :error

:: Create final image with Trust Package Creator
echo Creating STiROT image  >> %current_log_file% 2>&1
%stm32tpccli% -pb %code_xml% >> %current_log_file% 2>&1
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
