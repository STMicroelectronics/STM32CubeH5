call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Data updated with the postbuild of OEMiROT-Boot
set wrpgrp1=0x3
set wrpgrp2=0xFE
set hdp1_start=0
set hdp1_end=0x7
set hdp2_start=0x0
set hdp2_end=0x0
set bootob=0x80044
set bootaddress=0x8004000
set app_image_number=1
set data_image_number=0
set code_image=%oemirot_appli%
set data_image=data_enc_sign.hex

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

:: =============================================== Remove protections and initialize Option Bytes  ==========================================
set remove_protect_init=-ob WRPSGn1=0xff WRPSGn2=0xff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 NSBOOT_LOCK=0xC3 SRAM2_RST=0 SRAM2_ECC=0 SRAM1_RST=0 SRAM1_ECC=0 NSBOOTADD=%bootob% SWAP_BANK=0
:: =============================================== Erase the user flash =====================================================================
set erase_all=-e all

:: ================================================ hardening ===============================================================================
set hide_protect=HDP1_STRT=%hdp1_start% HDP1_END=%hdp1_end% HDP2_STRT=%hdp2_start% HDP2_END=%hdp2_end%
set write_protect=WRPSGn1=%wrpgrp1% WRPSGn2=%wrpgrp2%
set ns_boot_lock=NSBOOT_LOCK=0xB4

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect_init% %erase_all%
IF !errorlevel! NEQ 0 goto :error

:: ==================================================== Download images ====================================================================
set "action=Application images programming in primary slots"
echo %action%

if  "%app_image_number%" == "1" (
set "action=Write OEMiROT_Appli Code"
echo %action%
%stm32programmercli% %connect_no_reset% -d ../../%oemirot_boot_path_project%/Binary/%code_image% -v
IF !errorlevel! NEQ 0 goto :error

echo "TZ Appli Written"
)

if  "%data_image_number%" == "1" (
set "action=Write OEMiROT_Appli Data"
echo %action%
IF not exist %rot_provisioning_path%\OEMiROT\Binary\%data_image% (
@echo [31mError: %data_image% does not exist! use TPC to generate it[0m  
goto :error
)
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\OEMiROT\Binary\%data_image% -v
IF !errorlevel! NEQ 0 goto :error
)

set "action=Write OEMiROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\NUCLEO-H503RB\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin %bootaddress% -v
IF !errorlevel! NEQ 0 goto :error

set "action=OEMiROT_Boot Written"
echo %action%

echo "Configure option Bytes: Write Protection, Hide Protection and boot lock"
%stm32programmercli% %connect_no_reset% -ob %write_protect% %hide_protect% %ns_boot_lock%
IF !errorlevel! NEQ 0 goto :error

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1

