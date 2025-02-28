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

::=============================================================================================
::Variables updated by OEMiROT_Boot postbuild
::=============================================================================================
set app_image_number=2
set image_s_size=0x6000
:: flag to switch between OEMiROT and OEMuROT
set oemurot_enable=0

if %oemurot_enable% == 1 (
set project=OEMuROT
set bootpath=STiROT_OEMuROT)

if %oemurot_enable% == 0 (
set project=OEMiROT
set bootpath=OEMiROT)

::=============================================================================================
::image binary files
::=============================================================================================
set s_code_bin="%projectdir%\..\Binary\rot_tz_s_app.bin"
set ns_code_bin="%projectdir%\..\Binary\rot_tz_ns_app.bin"
set one_code_bin="%projectdir%\..\Binary\rot_tz_app.bin"

::=============================================================================================
::image xml configuration files
::=============================================================================================
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set ns_code_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Image.xml"
set s_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Init_Image.xml"
set ns_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Init_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set ns_data_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Image.xml"
set s_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Init_Image.xml"
set ns_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Init_Image.xml"

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
:: relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
::=================================================================================================
set bin_path_xml_field="..\..\..\Applications\ROT\OEMiROT_Appli_TrustZone\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set s_app_bin_xml_field="%bin_path_xml_field%\rot_tz_s_app.bin"
set ns_app_bin_xml_field="%bin_path_xml_field%\rot_tz_ns_app.bin"
set s_app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_s_app_enc_sign.hex"
set ns_app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_ns_app_enc_sign.hex"
set s_app_init_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_s_app_init_sign.hex"
set ns_app_init_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_ns_app_init_sign.hex"
set s_data_enc_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\s_data_enc_sign.hex"
set ns_data_enc_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\ns_data_enc_sign.hex"
set s_data_init_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\s_data_init_sign.hex"
set ns_data_init_sign_hex_xml_field="%provisioningdir%\%bootpath%\Binary\ns_data_init_sign.hex"

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
echo Postbuild %signing% image >> %current_log_file% 2>>&1

if "%app_image_number%" == "2" (
goto :continue
)
IF %signing% == "nonsecure" (
echo Creating only one image >> %current_log_file% 2>>&1
%python%%applicfg% oneimage -fb %s_code_bin% -o %image_s_size% -sb %ns_code_bin% -i 0x0 -ob %one_code_bin% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error
set ns_app_enc_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_app_enc_sign.hex"
set ns_app_init_sign_hex_xml_field="%bin_path_xml_field%\rot_tz_app_init_sign.hex"
set ns_app_bin_xml_field="%bin_path_xml_field%\rot_tz_app.bin"
)

:continue
IF %signing% == "secure" (

echo Creating secure image  >> %current_log_file% 2>>&1

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
)

IF %signing% == "nonsecure" (
echo Creating nonsecure image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %ns_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %ns_code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %ns_app_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %ns_code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %ns_code_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %ns_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %ns_code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %ns_app_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %ns_code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update data xml file : output file
%python%%applicfg% xmlval -v %ns_data_enc_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %ns_data_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %ns_data_init_sign_hex_xml_field% --string -n %fw_out_bin_xml_field% %ns_data_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %ns_code_init_xml% >> %current_log_file% 2>>&1
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
