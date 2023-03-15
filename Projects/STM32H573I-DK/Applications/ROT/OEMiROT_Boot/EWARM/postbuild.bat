@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set "env_script=%provisioningdir%\env.bat"
set "auto_rot_update=auto_rot_update.bat"
set "preprocess_bl2_file=%projectdir%\image_macros_preprocessed_bl2.c"

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

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b oemurot_enable -m  RE_OEMUROT_ENABLE --decimal %auto_rot_update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
call auto_rot_update.bat
set "appli_dir=%oemirot_boot_path_project%"
set "update=%provisioningdir%\%bootpath%\ob_flash_programming.bat"
set "map_properties=%projectdir%\..\map.properties"
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set ns_code_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set ns_data_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Image.xml"
set ns_main="%appli_dir%\NonSecure\Inc\main.h"
set s_main="%appli_dir%\Secure\Inc\main.h"
set appli_flash_layout="%appli_dir%\Secure_nsclib\appli_flash_layout.h"
set ns_ewp="%appli_dir%\EWARM\NonSecure\Project_ns.ewp"
set s_ewp="%appli_dir%\EWARM\Secure\Project_s.ewp"
set appli_postbuild="%appli_dir%\EWARM\postbuild.bat"

set auth_s="Authentication secure key"
set auth_ns="Authentication non secure key"

set xml_fw_app_item_name="Firmware binary input file"
set xml_fw_data_item_name="Data binary input file"
set xml_output_item_name="Image output file"
set xml_enc_item_name="Encryption key"

set s_code_bin="%appli_dir%\EWARM\Secure\STM32H573I-DK_S\Exe\Project.bin"
set ns_code_bin="%appli_dir%\EWARM\NonSecure\STM32H573I-DK_NS\Exe\Project.bin"
set one_code_bin="%appli_dir%\Binary\rot_app.bin"

set "s_data_bin=../Binary/s_data.bin"
set "ns_data_bin=../Binary/ns_data.bin"
set s_icf_file="%appli_dir%\EWARM\Secure\stm32h573xx_flash_s.icf"
set ns_icf_file="%appli_dir%\EWARM\NonSecure\stm32h573xx_flash_ns.icf"




set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_CODE_REGION_START -m  RE_ADDRESS_SECURE_START %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_CODE_REGION_SIZE -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_CODE_REGION_START -m  RE_ADDRESS_NON_SECURE_START %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_CODE_REGION_SIZE -m  RE_IMAGE_NON_SECURE_IMAGE_SIZE %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b S_DATA -m RE_S_DATA_IMAGE_NUMBER %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NS_DATA -m RE_NS_DATA_IMAGE_NUMBER  %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_IMAGE_ASSEMBLY -m RE_CODE_IMAGE_ASSEMBLY  %map_properties% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b bootob -m  RE_BL2_BOOT_ADDRESS  -d 0x100 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b bootaddress -m  RE_BL2_BOOT_ADDRESS %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec1_end -m  RE_BL2_SEC1_END -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec2_start -m  RE_BL2_SEC2_START -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec2_end -m  RE_BL2_SEC2_END -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x8000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp1_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m  RE_S_DATA_IMAGE_NUMBER --decimal %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m  RE_NS_DATA_IMAGE_NUMBER --decimal %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %appli_postbuild% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b image_s_size -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE %appli_postbuild% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %s_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %ns_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %s_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %ns_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE %ns_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE %ns_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_TRAILER_MAX_SIZE -n TRAILER_MAX_SIZE %s_icf_file% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlname --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n %auth_ns% -sn %auth_s% -v 1 -c k %ns_code_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_UPDATE -c x %s_code_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_UPDATE -sm RE_IMAGE_FLASH_SECURE_UPDATE -v 0 -c x %ns_code_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_SECURE_UPDATE -c x %s_data_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE -c x %ns_data_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S %s_code_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S %ns_code_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S %s_data_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S %ns_data_xml% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% iofile --layout %preprocess_bl2_file% -mi RE_APP_IMAGE_NUMBER -me RE_ENCRYPTION --xml %s_code_xml% -in %xml_fw_app_item_name% -i %s_code_bin% -on %xml_output_item_name% -en %xml_enc_item_name% -b oemirot_appli_secure %env_script% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% iofile --layout %preprocess_bl2_file% -mi RE_APP_IMAGE_NUMBER -me RE_ENCRYPTION --xml %ns_code_xml% -in %xml_fw_app_item_name% -i %ns_code_bin% -on %xml_output_item_name% -en %xml_enc_item_name% -b oemirot_appli_non_secure %env_script% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% iofile --layout %preprocess_bl2_file% -mi RE_APP_IMAGE_NUMBER -me RE_ENCRYPTION --xml %ns_code_xml% -in %xml_fw_app_item_name% -i %one_code_bin% -on %xml_output_item_name% -en %xml_enc_item_name% -b one_code_image %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% iofile --layout %preprocess_bl2_file% -mi RE_S_DATA_IMAGE_NUMBER -me RE_ENCRYPTION --xml %s_data_xml% -in %xml_fw_data_item_name% -i %s_data_bin% -on %xml_output_item_name% -en %xml_enc_item_name% -b s_data_image %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% iofile --layout %preprocess_bl2_file% -mi RE_NS_DATA_IMAGE_NUMBER -me RE_ENCRYPTION --xml %ns_data_xml% -in %xml_fw_data_item_name% -i %ns_data_bin% -on %xml_output_item_name% -en %xml_enc_item_name% -b ns_data_image %update% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

if /i %oemirot_boot_path_project% == %oemirot_boot_path_project:Applications=% (goto :end)
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n S_CODE_OFFSET %s_main% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n S_CODE_SIZE %s_main% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n NS_CODE_SIZE %s_main% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n NS_DATA_IMAGE_EN -v 1 %ns_main% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 %appli_flash_layout% --vb  >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET %appli_flash_layout% --vb  >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_B_SIZE -n FLASH_B_SIZE %appli_flash_layout% --vb >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:end
exit 0

:error
echo "%command% : failed" >> %projectdir%\\output.txt
exit 1
