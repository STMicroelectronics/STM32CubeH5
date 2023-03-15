IF [%1] NEQ [AUTO] call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set sec1_start=0
set sec1_end=0xB
set sec2_start=0x7F
set sec2_end=0x0
set wrpgrp1=0xFFFFFFFC
set wrpgrp2=0xFFFFFFFF
set hdp1_start=0
set hdp1_end=0x0
set hdp2_start=0x7F
set hdp2_end=0x0
set boot_lck=0xB4
set bootaddress=0xC000400
set bootob=0xC0004
set app_image_number=2
set s_data_image_number=1
set ns_data_image_number=1

set s_code_image=%oemirot_appli_secure%
set ns_code_image=%oemirot_appli_non_secure%
set one_code_image=rot_tz_app_enc_sign.hex
set s_data_image=s_data_enc_sign.hex
set ns_data_image=ns_data_enc_sign.hex
set oemurot_image=rot_enc_sign.hex

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

set image_number=2

:: =============================================== Remove protections and initialize Option Bytes ==========================================
set remove_protect=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3 SECBOOTADD=%bootob% SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 BOOT_UBE=0xB4
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

set "action=Set TZEN = 1"
echo %action%
:: Trust zone enabled is mandatory to execute ST-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=0xB4
IF !errorlevel! NEQ 0 goto :error

set "action=Remove Protection and erase All"
echo %action%
::%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%

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
%stm32programmercli% %connect_no_reset% -ob SECWM1_STRT=0x0 SECWM1_END=%sec1_end%
IF !errorlevel! NEQ 0 goto :error
%stm32programmercli% %connect_no_reset% -ob SECWM2_STRT=0x7F SECWM2_END=0x0
IF !errorlevel! NEQ 0 goto :error

:: ==================================================== Download images ====================================================================

echo "Application images programming in download slots"

if  "%app_image_number%" == "2" (
set "action=Write Appli Secure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %oemirot_boot_path_project%\Binary\%s_code_image% -v
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli Secure Written"
set "action=Write Appli NonSecure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %oemirot_boot_path_project%\Binary\%ns_code_image% -v
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli NonSecure Written"
)

if  "%app_image_number%" == "1" (
set "action=Write One image Appli"
echo %action%
%stm32programmercli% %connect_no_reset% -d %oemirot_boot_path_project%\Binary\%one_code_image% -v
IF !errorlevel! NEQ 0 goto :error

echo "TZ Appli Written"
)

if  "%s_data_image_number%" == "1" (
set "action=Write Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\STiROT_OEMuROT\Binary\s_data_enc_sign.hex (
@echo [31mError: s_data_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error
)
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\STiROT_OEMuROT\Binary\%s_data_image% -v
IF !errorlevel! NEQ 0 goto :error
)


if  "%ns_data_image_number%" == "1" (
set "action=Write non Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\STiROT_OEMuROT\Binary\ns_data_enc_sign.hex (
@echo [31mError: ns_data_enc_sign.hex does not exist! use TPC to generate it[0m
goto :error
)
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\STiROT_OEMuROT\Binary\%ns_data_image% -v
IF !errorlevel! NEQ 0 goto :error
)

set "action=Write OEMuROT HDPL2 data"
echo %action%
::%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\STiROT_OEMuROT\Binary\OEMuRoT_Config.hex -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT Data Written"

set "action=Write OEMuROT_Boot"
echo %action%
%stm32tpccli% -pb %rot_provisioning_path%\STiROT_OEMuROT\Images\STiRoT_Code_Image.xml
%stm32programmercli% %connect_no_reset% -d %~dp0..\..\Applications\ROT\OEMiROT_Boot\Binary\rot_enc_sign.hex -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Boot Written"

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
echo.
IF [%1] NEQ [AUTO] cmd /k
exit 1
