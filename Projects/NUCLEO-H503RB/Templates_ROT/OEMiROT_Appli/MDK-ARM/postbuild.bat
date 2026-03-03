@ECHO OFF
:: Getting the Trusted Package Creator CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"
:: Enable delayed expansion
setlocal EnableDelayedExpansion

::

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

::===========================================================================================
::image xml configuration files
::===========================================================================================
set code_image_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Code_Image.xml"
set code_image_init_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Code_Init_Image.xml"
set data_image_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Data_Image.xml"
set data_image_init_file="%provisioningdir%\OEMiROT\Images\OEMiROT_Data_Init_Image.xml"

::===========================================================================================
:: Variables for image xml configuration(ROT_Provisioning\OEMiROT\Images)
:: relative path from ROT_Provisioning\OEMiROT\Images directory to retrieve binary files
::===========================================================================================
set bin_path_xml_field="..\..\..\Templates_ROT\OEMiROT_Appli\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set app_bin_xml_field="%bin_path_xml_field%\rot_app.bin"
set app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_app_enc_sign.hex"
set app_init_sign_hex_xml_field="%bin_path_xml_field%\rot_app_init_sign.hex"
set data_enc_sign_hex_xml_field="%provisioningdir%\OEMiROT\Binary\data_enc_sign.hex"
set data_init_sign_hex_xml_field="%provisioningdir%\OEMiROT\Binary\data_init_sign.hex"

::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%bin_path_xml_field%" (
mkdir "%bin_path_xml_field%"
)

:start
::=================================================================================================
:: Check if Python V3 is installed
::-------------------------------------------------------------------------------------------------
python --version >nul 2>&1
if %errorlevel% neq 0 (
  echo.
  echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
  echo.
  set "command=Python installation"
  goto :error
)
set "python=python "
:: If found, capture version string removing "Python "
for /f "tokens=2 delims= " %%A in ('python --version 2^>^&1') do (
    set "full_version=%%A"
)
:: extract version details
for /F "tokens=1,2,3 delims=." %%A in ("!full_version!") do (
  set MAJOR_VER=%%A
  set MINOR_VER=%%B
  set PATCH_VER=%%C
)
:: is v3
if not "%MAJOR_VER%" == "3" (
  python3 --version >nul 2>&1
  if !errorlevel! neq 0 (
    echo.
    echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
    echo.
    set "command=Python installation"
    goto :error
  )
  set "python=python3 "
)
::=================================================================================================

:: Environment variable for AppliCfg
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"

:postbuild
echo Creating OEMiROT image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %app_bin_xml_field% --string -n %fw_in_bin_xml_field% %code_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %app_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %code_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_image_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %app_bin_xml_field% --string -n %fw_in_bin_xml_field% %code_image_init_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %app_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %code_image_init_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_image_init_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

echo Creating OEMiROT data  >> %current_log_file% 2>>&1

::update xml file : output file
%python%%applicfg% xmlval -v %data_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %data_image_file% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %data_image_file% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %data_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %data_image_init_file% --vb >> %current_log_file% 2>>&1
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
