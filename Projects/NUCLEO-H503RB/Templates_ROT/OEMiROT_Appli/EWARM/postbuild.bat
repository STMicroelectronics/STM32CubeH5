@ECHO OFF
:: Getting the Trusted Package Creator CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

:: Variables for image xml configuration
set appli_dir="..\..\..\Templates_ROT\OEMiROT_Appli"
set code_image_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Code_Image.xml"
set code_image_init_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Code_Init_Image.xml"
set data_image_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Data_Image.xml"
set data_image_init_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Data_Init_Image.xml"
set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"
set app_bin="%appli_dir%\Binary\rot_app.bin"
set app_enc_sign_hex="%appli_dir%\Binary\rot_app_enc_sign.hex"
set app_init_sign_hex="%appli_dir%\Binary\rot_app_init_sign.hex"
set data_enc_sign_hex="%provisioningdir%\OEMiROT\Binary\data_enc_sign.hex"
set data_init_sign_hex="%provisioningdir%\OEMiROT\Binary\data_init_sign.hex"

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
echo Creating OEMiROT image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %app_bin% --string -n %fw_in_bin% %code_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %app_enc_sign_hex% --string -n %fw_out_bin% %code_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_image_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %app_bin% --string -n %fw_in_bin% %code_image_init_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %app_init_sign_hex% --string -n %fw_out_bin% %code_image_init_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_image_init_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

echo Creating OEMiROT data  >> %current_log_file% 2>>&1

::update xml file : output file
%python%%applicfg% xmlval -v %data_enc_sign_hex% --string -n %fw_out_bin% %data_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %data_image_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %data_init_sign_hex% --string -n %fw_out_bin% %data_image_init_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %data_image_init_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error
exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
