IF [%1] NEQ [AUTO] call ../env.bat



:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

:: =============================================== Remove protections and initialize Option Bytes ===========================================
set remove_protect_init=-ob WRPSGn1=0xff WRPSGn2=0xff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 NSBOOT_LOCK=0xC3 SRAM2_RST=0 SRAM2_ECC=0 SRAM1_RST=0 SRAM1_ECC=0 NSBOOTADD=0x80000 SWAP_BANK=0
:: =============================================== Erase the user flash =====================================================================
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect_init% %erase_all%
IF %errorlevel% NEQ 0 goto :error


echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1

