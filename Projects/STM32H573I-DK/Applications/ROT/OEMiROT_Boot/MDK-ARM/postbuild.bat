@ECHO OFF
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

:start
goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto postbuild
)
:py
::line for python
echo run config Appli with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python= "

:postbuild
set "auto_rot_update=%projectdir%\..\auto_rot_update.bat"
set "map_properties=%projectdir%\..\map.properties"
set "preprocess_bl2_file=%projectdir%\image_macros_preprocessed_bl2.c"
set "appli_dir=../../../../%oemirot_boot_path_project%"
set "update=%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\ob_flash_programming.bat"


set "provisioning=%projectdir%\..\..\..\..\ROT_Provisioning\img_config.bat"
set ns_main="%appli_dir%\NonSecure\Inc\main.h"
set s_main="%appli_dir%\Secure\Inc\main.h"
set appli_flash_layout="%appli_dir%\Secure_nsclib\appli_flash_layout.h"
set appli_postbuild="%appli_dir%\MDK-ARM\postbuild.bat"
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b oemurot_enable -m  RE_OEMUROT_ENABLE --decimal %auto_rot_update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
call %auto_rot_update%
set "update=%provisioningdir%\%bootpath%\ob_flash_programming.bat"
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set ns_code_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set ns_data_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Image.xml"
set auth_s="Authentication secure key"
set auth_ns="Authentication non secure key"
set xml_fw_app_item_name="Firmware binary input file"
set xml_fw_data_item_name="Data binary input file"
set xml_output_item_name="Image output file"
set xml_enc_item_name="Encryption key"
set s_sct_file="%appli_dir%\MDK-ARM\Secure\stm32h573xx_s.sct"
set ns_sct_file="%appli_dir%\MDK-ARM\NonSecure\stm32h573xx_ns.sct"

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_CODE_REGION_START -m  RE_ADDRESS_SECURE_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_CODE_REGION_SIZE -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_CODE_REGION_START -m  RE_ADDRESS_NON_SECURE_START %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_CODE_REGION_SIZE -m  RE_IMAGE_NON_SECURE_IMAGE_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% modifyfilevalue --variable bootPath --delimiter = --value %project% %map_properties% --str --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_DATA -m RE_S_DATA_IMAGE_NUMBER %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_DATA -m RE_NS_DATA_IMAGE_NUMBER %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_IMAGE_ASSEMBLY -m RE_CODE_IMAGE_ASSEMBLY %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b MCUBOOT_OVERWRITE_ONLY -m RE_OVER_WRITE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b TRAILER_SIZE -m RE_TRAILER_MAX_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_DATA_REGION_START -m  RE_AREA_4_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_DATA_REGION_SIZE -m  RE_AREA_4_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_DATA_REGION_START -m  RE_AREA_5_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_DATA_REGION_SIZE -m  RE_AREA_5_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_START -m  RE_FLASH_AREA_BL2_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_SIZE -m  RE_FLASH_AREA_BL2_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b SCRATCH_REGION_START -m  RE_FLASH_AREA_SCRATCH_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b SCRATCH_REGION_SIZE -m  RE_FLASH_AREA_SCRATCH_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_S_CODE_REGION_START -m  RE_AREA_2_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_NS_CODE_REGION_START -m  RE_AREA_3_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_S_DATA_REGION_START -m  RE_AREA_6_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DOWNLOAD_NS_DATA_REGION_START -m  RE_AREA_7_OFFSET %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b bootob -m  RE_BL2_BOOT_ADDRESS  -d 0x100 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b bootaddress -m  RE_BL2_BOOT_ADDRESS %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec1_end -m  RE_BL2_SEC1_END -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec2_start -m  RE_BL2_SEC2_START -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec2_end -m  RE_BL2_SEC2_END -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp1_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m  RE_S_DATA_IMAGE_NUMBER --decimal %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m  RE_NS_DATA_IMAGE_NUMBER --decimal %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %appli_postbuild% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m  RE_S_DATA_IMAGE_NUMBER --decimal %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m  RE_NS_DATA_IMAGE_NUMBER --decimal %provisioning% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b image_s_size -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE %appli_postbuild% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %s_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %ns_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %s_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %ns_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE %ns_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE %ns_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE %s_sct_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlname --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n %auth_ns% -sn %auth_s% -v 1 -c k %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_UPDATE -c x %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_UPDATE -sm RE_IMAGE_FLASH_SECURE_UPDATE -v 0 -c x %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_SECURE_UPDATE -c x %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE -c x %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %s_main% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %s_main% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE %s_main% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if /i %oemirot_boot_path_project% == %oemirot_boot_path_project:Applications=% (goto :end)
set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n NS_DATA_IMAGE_EN -v 1 %ns_main% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_B_SIZE -n FLASH_B_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if %oemurot_enable% == 1 (
	%stm32tpccli% -pb %rot_provisioning_path%\STiROT_OEMuROT\Images\STiRoT_Code_Image.xml
)

:end

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1

