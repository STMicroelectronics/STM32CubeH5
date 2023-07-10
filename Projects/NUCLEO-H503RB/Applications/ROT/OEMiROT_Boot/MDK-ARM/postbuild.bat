@ECHO OFF
:: Getting the Trusted Package Creator and STM32CubeProgammer CLI path
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
set "preprocess_bl2_file=%projectdir%\image_macros_preprocessed_bl2.c"
set "appli_dir=../../../../%oemirot_boot_path_project%"
set "update=%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\ob_flash_programming.bat"

:: Environment variable for AppliCfg
set isGeneratedByCubeMX=%PROJECT_GENERATED_BY_CUBEMX%
set appli_flash_layout="%appli_dir%\Inc\appli_flash_layout.h"
set appli_linker_file="%appli_dir%\MDK-ARM\stm32h5xx_app.sct"
set "map_properties=%projectdir%\..\..\OEMiROT_Boot\map.properties"
set code_xml="%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\Images\OEMiRoT_code_image.xml"
set data_xml="%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\Images\OEMiRoT_Data_Image.xml"

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_START -m  RE_IMAGE_FLASH_ADDRESS %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_SIZE -m  RE_IMAGE_FLASH_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DATA -m RE_DATA_IMAGE_NUMBER  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DATA_REGION_START -m  RE_IMAGE_FLASH_ADDRESS_DATA %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DATA_REGION_SIZE -m  RE_IMAGE_FLASH_DATA_IMAGE_SIZE %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_START -m RE_FLASH_AREA_BL2_OFFSET  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ROT_REGION_SIZE -m RE_FLASH_AREA_BL2_SIZE  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b HASH_REF_AREA_START -m RE_FLASH_HASH_REF_AREA_OFFSET  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b HASH_REF_AREA_SIZE -m RE_FLASH_HASH_REF_AREA_SIZE  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NVCNT_AREA_START -m RE_FLASH_BL2_NVCNT_AREA_OFFSET  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b NVCNT_AREA_SIZE -m RE_FLASH_BL2_NVCNT_AREA_SIZE  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b AREA_PERSO_START -m RE_FLASH_AREA_PERSO_OFFSET  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b AREA_PERSO_SIZE -m RE_FLASH_AREA_PERSO_SIZE  %map_properties% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Bypass configuration of application in case of cubemx
if "%isGeneratedByCubeMX%" == "true" goto :cubemx
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n CODE_OFFSET %appli_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n CODE_SIZE %appli_linker_file% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Bypass configuration of appli_flash_layout file if not present
if not exist %appli_flash_layout% goto :cubemx
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_IMAGE_NUMBER -n MCUBOOT_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS -n CODE_IMAGE_PRIMARY_PARTITION_ADDRESS %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS_DATA -n DATA_IMAGE_PRIMARY_PARTITION_ADDRESS %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:cubemx

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% --vb >> %current_log_file% 2>&1"
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

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b bootob -m  RE_BL2_BOOT_ADDRESS  -d 0x100 %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b bootaddress -m  RE_BL2_PERSO_ADDRESS %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SIZE -c S %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS -c x %code_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_UPDATE -c x %data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_IMAGE_SIZE -c S %data_xml% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b data_image_number -m  RE_DATA_IMAGE_NUMBER --decimal %update% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1

