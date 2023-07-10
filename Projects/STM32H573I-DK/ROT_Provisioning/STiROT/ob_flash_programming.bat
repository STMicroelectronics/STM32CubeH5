call ../env.bat
:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

set image_number=1

if "%isGeneratedByCubeMX%" == "true" (
set appli_dir=%stirot_boot_path_project%
) else (
set appli_dir=../../%stirot_boot_path_project%
)
:: =============================================== Remove protections and erase the user flash ===============================================

set remove_protect=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

set "action=Set TZEN = 1"
echo %action%
:: Trust zone enabled is mandatory to execute ST-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=0xB4
IF !errorlevel! NEQ 0 goto :error

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%
IF !errorlevel! NEQ 0 goto :error

echo "Set SRAM 2 configuration"
:: Recommended configuration for secure boot is :
::   - SRAM2 erased in case of reset ==> SRAM2_RST=0
::   - SRAM2 ECC activated. Hack tentative detection enabled ==> SRAM2_ECC=0
%stm32programmercli% %connect_no_reset% -ob SRAM2_RST=0 SRAM2_ECC=0
IF !errorlevel! NEQ 0 goto :error 

set "action=Define secure area through watermarks"
echo %action%
:: This configuration depends on user mapping but the watermarks should cover at least the secure area part of the firmware execution slot.
:: The secure area can also be larger in order to include additional sectors. For example the secure firmware will have to manage user data.
%stm32programmercli% %connect_no_reset% -ob SECWM1_STRT=0x0 SECWM1_END=0xF
IF !errorlevel! NEQ 0 goto :error
%stm32programmercli% %connect_no_reset% -ob SECWM2_STRT=0x7F SECWM2_END=0x0
IF !errorlevel! NEQ 0 goto :error

:: ==================================================== Download images ====================================================================
set "action=Download the code image in the download slots"
echo %action%

IF not exist %appli_dir%\Binary\%stirot_appli% (
@echo [31mError: appli_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error)
%stm32programmercli% %connect_no_reset% -d  %appli_dir%\Binary\%stirot_appli%
IF !errorlevel! NEQ 0 goto :error

if  "%image_number%" == "2" (
set "action=Download the data image in the download slots"
echo %action%
IF not exist %~dp0.\Binary\data_enc_sign.hex (
@echo [31mError: data_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error
)
%stm32programmercli% %connect_no_reset% -d %~dp0.\Binary\data_enc_sign.hex
IF !errorlevel! NEQ 0 goto :error
)

set "action=Set UBE for ST-iRoT"
echo %action%
:: Unique boot entry is set to ST-iRoT to force ST-iRoT execution at each reset
%stm32programmercli% %connect_reset% -ob BOOT_UBE=0xC3
IF !errorlevel! NEQ 0 goto :error

set "action=Set SECBOOT_LOCK option byte"
echo %action%
:: SECBOOT_LOCK should be set to 0xB4 (locked) to be compliant with certification document
%stm32programmercli% %connect_no_reset% -ob SECBOOT_LOCK=0xB4
IF !errorlevel! NEQ 0 goto :error

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
