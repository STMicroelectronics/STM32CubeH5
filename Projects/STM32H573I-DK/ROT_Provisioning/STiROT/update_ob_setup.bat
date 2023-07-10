@ECHO OFF
call ../env.bat
:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Path and input files
set ob_flash_programming="ob_flash_programming.bat"
set obk_cfg_file="./Config/STiRoT_Config.xml"
              
set "projectdir=%~dp0"
:: xml item nem contents
:: This will allow to recover information from xml file
set code_size="Firmware area size"
set code_offset="Firmware execution area offset"
set data_image_en="Number of images managed"
set secure_area="Size of the secure area inside the firmware execution area"
set sram2_rst="SRAM2 erasing in case of reset"
set sram2_ecc="SRAM2 ECC management activation"
set code_offset="Firmware execution area offset"
set full_sec="Is the firmware full secure"

:start
goto exe:
goto py:
:exe
::line for Windows executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run AppliCfg with Windows executable
goto update
)
:py
::line for Python
echo run AppliCfg with Python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:update
set "AppliCfg=%python%%applicfg%"
:: ======================================================= Updating the Option bytes =======================================================
echo Updating the ob_flash_programming script ...
set "action=Updating Secure WaterMarks values"
%AppliCfg% obscript --xml %obk_cfg_file% -ob SECWM -sp %secure_area% -cs %code_size% -co %code_offset% -fs %full_sec% --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error

set "action=Updating SRAM2_RST value"
%AppliCfg% obscript --xml %obk_cfg_file% -ob SRAM2_RST -sp %sram2_rst% --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error

set "action=Updating SRAM2_ECC value"
%AppliCfg% obscript --xml %obk_cfg_file% -ob SRAM2_ECC -sp %sram2_ecc% --vb %ob_flash_programming%
if !errorlevel! neq 0 goto :error

set "action=Updating image number value"
%AppliCfg% flash -xml %obk_cfg_file% --name %data_image_en% -b image_number --decimal %ob_flash_programming% --vb
if !errorlevel! neq 0 goto :error

echo Script successfully updated according to STiRoT_Config.obk
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Update script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
