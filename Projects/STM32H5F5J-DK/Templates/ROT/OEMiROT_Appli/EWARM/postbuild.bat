@ECHO OFF
:: arg1 is the binary type (nonsecure, secure)
:: When script is called from STM32CubeIDE : set signing="%1"
:: When script is called from IAR or KEIL  : set "signing=%1"
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

::=============================================================================================
::Variables updated by OEMiROT_Boot postbuild
::=============================================================================================
:: flag to switch between OEMiROT and OEMuROT
set oemurot_enable=0

if %oemurot_enable% == 1 (
set project=OEMuROT
set bootpath=STiROT_OEMuROT
)

if %oemurot_enable% == 0 (
set project=OEMiROT
set bootpath=OEMiROT
)

::=============================================================================================
::image binary files
::=============================================================================================
set s_code_bin="%projectdir%\..\Binary\rot_tz_s_app.bin"

::=============================================================================================
::image xml configuration files
::=============================================================================================
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set s_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Init_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set s_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Init_Image.xml"

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
:: relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
::=================================================================================================
set bin_path_xml_field="..\..\..\Templates\ROT\OEMiROT_Appli\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set s_app_bin_xml_field="%bin_path_xml_field%\rot_tz_s_app.bin"
set s_app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_s_app_enc_sign.hex"
set s_app_init_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_s_app_init_sign.hex"
set s_data_enc_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\s_data_enc_sign.hex"
set s_data_init_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\s_data_init_sign.hex"

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
echo Postbuild %signing% image >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %s_code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %s_code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %s_code_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %s_code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %s_code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update data xml file : output file
%python%%applicfg% xmlval -v %s_data_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %s_data_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %s_data_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %s_data_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %s_code_init_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
