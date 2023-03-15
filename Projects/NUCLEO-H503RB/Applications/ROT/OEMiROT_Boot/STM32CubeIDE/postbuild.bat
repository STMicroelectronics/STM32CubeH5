set "projectdir=%~dp0"
set "config=%1"
set env_script="%projectdir%\..\..\..\..\ROT_Provisioning\env.bat"
call %env_script%

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:start
goto exe:
goto py:
:exe
::line for window executable
set "applicfg==%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto postbuild
)
:py
::line for python
echo run config Appli with python script
set "applicfg==%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python= "

:postbuild
echo Postbuild %config% > %projectdir%\output.txt 2>&1

:: copy OEMiROT binary to Binary location
set bin_dest_dir=%cube_fw_path%\Projects\NUCLEO-H503RB\Applications\ROT\OEMiROT_Boot\Binary
set bin_dest=%bin_dest_dir%\OEMiROT_Boot.bin
set bin_src=%cube_fw_path%\Projects\NUCLEO-H503RB\Applications\ROT\OEMiROT_Boot\STM32CubeIDE\%config%\OEMiROT_Boot.bin
copy %bin_src% %bin_dest%

set "appli_dir=%oemirot_boot_path_project%"
set "preprocess_bl2_file=%projectdir%\image_macros_preprocessed_bl2.c"
set "map_properties=%projectdir%\..\..\OEMiROT_Boot\map.properties"
set appli_flash_layout="%appli_dir%\Inc\appli_flash_layout.h"
set appli_linker_file="%appli_dir%\STM32CubeIDE\STM32H503RBTX_FLASH.ld"
set update="%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\ob_flash_programming.bat"
set code_xml="%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\Images\OEMiRoT_code_image.xml"
set data_xml="%projectdir%\..\..\..\..\ROT_Provisioning\OEMiROT\Images\OEMiRoT_Data_Image.xml"


set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_START -m  RE_AREA_0_OFFSET %map_properties% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b CODE_REGION_SIZE -m  RE_AREA_0_SIZE %map_properties% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b DATA -m RE_DATA_IMAGE_NUMBER  %map_properties% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Bypass configuration of application in case of cubemx
if "%isGeneratedByCubeMX%" == "true" goto :cubemx
set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n CODE_OFFSET %appli_linker_file% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% linker --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n CODE_SIZE %appli_linker_file% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: Bypass configuration of appli_flash_layout file if not present
if not exist %appli_flash_layout% goto :cubemx
set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_DATA_IMAGE_NUMBER -n MCUBOOT_DATA_IMAGE_NUMBER %appli_flash_layout% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS -n CODE_IMAGE_PRIMARY_PARTITION_ADDRESS %appli_flash_layout% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS_DATA -n DATA_IMAGE_PRIMARY_PARTITION_ADDRESS %appli_flash_layout% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:cubemx

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp1 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b wrpgrp2 -ms RE_BL2_WRP_START -me RE_BL2_WRP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp1_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_start -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% setob --layout %preprocess_bl2_file% -b hdp2_end -ms RE_BL2_HDP_START -me RE_BL2_HDP_END -msec RE_FLASH_PAGE_NBR -d 0x2000 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b bootob -m  RE_BL2_BOOT_ADDRESS  -d 0x100 %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b bootaddress -m  RE_BL2_PERSO_ADDRESS %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SIZE -c S %code_xml% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_ADDRESS -c x %code_xml% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_UPDATE -c x %data_xml% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_DATA_IMAGE_SIZE -c S %data_xml% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file%  -b data_image_number -m  RE_DATA_IMAGE_NUMBER --decimal %update% >> %projectdir%\output.txt 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

exit 0

:error
echo "%command% : failed" >> %projectdir%\\output.txt
exit 1

