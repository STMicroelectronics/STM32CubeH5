call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set sec1_start=0
set sec1_end=0xE
set sec2_start=0x7F
set sec2_end=0x0
set wrpgrp1=0xFFFFFFF8
set wrpgrp2=0xFFFFFFFF
set hdp1_start=0
set hdp1_end=0xB
set hdp2_start=0x7F
set hdp2_end=0x0
set boot_lck=0xB4
set bootaddress=0xC000000
set bootob=0xC0000
set app_image_number=2
set s_data_image_number=0
set ns_data_image_number=0

set s_code_image=%oemirot_appli_secure%
set ns_code_image=%oemirot_appli_non_secure%
set one_code_image=%oemirot_appli_assembly_sign%
set s_data_image=s_data_enc_sign.hex
set ns_data_image=ns_data_enc_sign.hex

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

if "%isGeneratedByCubeMX%" == "true" (
set appli_dir=%oemirot_boot_path_project%
) else (
set appli_dir=../../%oemirot_boot_path_project%
)

:: =============================================== Remove protections and initialize Option Bytes ==========================================
set remove_protect_init=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3 SECBOOTADD=%bootob% SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 BOOT_UBE=0xB4

:: =============================================== Erase the user flash =====================================================================
set erase_all=-e all

:: =============================================== Hardening ===============================================================================
set hide_protect=HDP1_STRT=%hdp1_start% HDP1_END=%hdp1_end% HDP2_STRT=%hdp2_start% HDP2_END=%hdp2_end%
set write_protect=WRPSGn1=%wrpgrp1% WRPSGn2=%wrpgrp2%
set sec_water_mark=SECWM1_STRT=%sec1_start% SECWM1_END=%sec1_end% SECWM2_STRT=%sec2_start% SECWM2_END=%sec2_end%
set boot_lock=-ob SECBOOT_LOCK=%boot_lck%

:: =============================================== Configure Option Bytes ====================================================================
set "action=Set TZEN = 1"
echo %action%
:: Trust zone enabled is mandatory in order to execute OEM-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=0xB4
IF !errorlevel! NEQ 0 goto :error

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_reset% %remove_protect_init% %erase_all%
IF !errorlevel! NEQ 0 goto :error

set "action=Configure Secure Water Mark"
echo %action%
%stm32programmercli% %connect_no_reset% -ob %sec_water_mark%
IF !errorlevel! NEQ 0 goto :error
:: ==================================================== Download images ====================================================================

echo "Application images programming in download slots"

if  "%app_image_number%" == "2" (
set "action=Write Appli Secure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%s_code_image% -v
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli Secure Written"
set "action=Write Appli NonSecure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%ns_code_image% -v
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli NonSecure Written"
)

if  "%app_image_number%" == "1" (
set "action=Write One image Appli"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%one_code_image% -v
IF !errorlevel! NEQ 0 goto :error

echo "TZ Appli Written"
)

if  "%s_data_image_number%" == "1" (
set "action=Write Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\OEMiROT\Binary\%s_data_image% (
@echo [31mError: s_data_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error
)
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\OEMiROT\Binary\%s_data_image% -v
IF !errorlevel! NEQ 0 goto :error
)


if  "%ns_data_image_number%" == "1" (
set "action=Write non Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\OEMiROT\Binary\%ns_data_image% (
@echo [31mError: ns_data_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error
)
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\OEMiROT\Binary\%ns_data_image% -v
IF !errorlevel! NEQ 0 goto :error
)

set "action=Write OEMiROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\NUCLEO-H563ZI\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin %bootaddress% -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Boot Written"

:: ======================================================= Extra board protections =========================================================
set "action=Configure Option Bytes"
echo %action%
echo "Configure Secure option Bytes: Write Protection, Hide Protection and boot lock"
%stm32programmercli% %connect_no_reset% -ob %write_protect% %hide_protect% %boot_lock%
IF !errorlevel! NEQ 0 goto :error

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
echo.
IF [%1] NEQ [AUTO] cmd /k
exit 1
