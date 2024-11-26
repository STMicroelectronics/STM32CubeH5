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

:: Variables for image xml configuration
set appli_dir="..\..\..\Applications\ROT\STiROT_Appli"
set code_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Image.xml"
set data_xml="%provisioningdir%\STiROT\Image\STiRoT_Data_Image.xml"
set code_init_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Init_Image.xml"
set data_init_xml="%provisioningdir%\STiROT\Image\STiRoT_Data_Init_Image.xml"
set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"
set stirot_app_bin="%appli_dir%\Binary\appli.bin"
set stirot_app_hex="%appli_dir%\Binary\appli_enc_sign.hex"
set stirot_app_init_hex="%appli_dir%\Binary\appli_init_sign.hex"
set stirot_data_hex="%provisioningdir%\STiROT\Binary\data_enc_sign.hex"
set stirot_data_init_bin="%provisioningdir%\STiROT\Binary\data_init_sign.bin"

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
echo Postbuild STiROT image >> %current_log_file% 2>>&1
IF %signing% == "secure" (
:: Create final image with Trust Package Creator
echo Creating STiROT image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %stirot_app_bin% --string -n %fw_in_bin% %code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %stirot_app_hex% --string -n %fw_out_bin% %code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %stirot_app_bin% --string -n %fw_in_bin% %code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %stirot_app_init_hex% --string -n %fw_out_bin% %code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_init_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

echo Creating STiROT data  >> %current_log_file% 2>>&1

::update data xml file : output file
%python%%applicfg% xmlval -v %stirot_data_hex% --string -n %fw_out_bin% %data_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %stirot_data_init_bin% --string -n %fw_out_bin% %data_init_xml% --vb >> %current_log_file% 2>>&1
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
