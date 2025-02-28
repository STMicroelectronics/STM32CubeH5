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

::======================================================================================================
::image binary files
::======================================================================================================
set s_code_bin="%projectdir%\..\Binary\appli_s.bin"
set ns_code_bin="%projectdir%\..\Binary\appli_ns.bin"
set s_ns_code_bin="%projectdir%\..\Binary\appli.bin"

::======================================================================================================
::image xml configuration files
::======================================================================================================
set code_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Image.xml"
set data_xml="%provisioningdir%\STiROT\Image\STiRoT_Data_Image.xml"
set code_init_xml="%provisioningdir%\STiROT\Image\STiRoT_Code_Init_Image.xml"
set data_init_xml="%provisioningdir%\STiROT\Image\STiRoT_Data_Init_Image.xml"

::======================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\STiROT_Appli_TrustZone\Images)
:: relative path from ROT_Provisioning\STiROT_Appli_TrustZone\Images directory to retrieve binary files
::======================================================================================================
set bin_path_xml_field="..\..\..\Applications\ROT\STiROT_Appli_TrustZone\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set stirot_app_bin_xml_field="%bin_path_xml_field%\appli.bin"
set stirot_app_hex_xml_field="%bin_path_xml_field%\appli_enc_sign.hex"
set stirot_app_init_hex_xml_field="%bin_path_xml_field%\appli_init_sign.hex"
set stirot_data_hex_xml_field="%provisioningdir%\STiROT\Binary\data_enc_sign.hex"
set stirot_data_init_bin_xml_field="%provisioningdir%\STiROT\Binary\data_init_sign.bin"

::======================================================================================================
::Variables updated by update_appli_setup
::======================================================================================================
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
echo Postbuild STiROT image >> %current_log_file% 2>>&1

IF %signing% == "nonsecure" (

:: Create one image (assembly secure and non secure binaries)
echo Creating only one image >> %current_log_file% 2>>&1

%python%%applicfg% oneimage -fb %s_code_bin% -o %image_size% -sb %ns_code_bin% -i 0x0 -ob %s_ns_code_bin% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

:: Create final image with Trust Package Creator
echo Creating STiROT image  >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %stirot_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %stirot_app_hex_xml_field% --string -n %fw_out_bin_xml_field% %code_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %stirot_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %stirot_app_init_hex_xml_field% --string -n %fw_out_bin_xml_field% %code_init_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %code_init_xml% >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

echo Creating STiROT data  >> %current_log_file% 2>>&1

::update data xml file : output file
%python%%applicfg% xmlval -v %stirot_data_hex_xml_field% --string -n %fw_out_bin_xml_field% %data_xml% --vb >> %current_log_file% 2>>&1
if !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %stirot_data_init_bin_xml_field% --string -n %fw_out_bin_xml_field% %data_init_xml% --vb >> %current_log_file% 2>>&1
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
