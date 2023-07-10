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
set hdp1_end=0x0
set hdp2_start=0x7F
set hdp2_end=0x0
set boot_lck=0xB4
set bootaddress=0xC000400
set bootob=0xC0004
set app_image_number=2
set s_data_image_number=0
set ns_data_image_number=0

set s_code_image=%oemirot_appli_secure%
set ns_code_image=%oemirot_appli_non_secure%
set one_code_image=%oemirot_appli_assembly_sign%
set s_data_image=s_data_enc_sign.hex
set ns_data_image=ns_data_enc_sign.hex
set oemurot_image=rot_enc_sign.hex

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

set image_number=2

:: =============================================== Remove protections and initialize Option Bytes ==========================================
set remove_protect=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 BOOT_UBE=0xB4
set erase_all=-e all

if "%isGeneratedByCubeMX%" == "true" (
set appli_dir=%oemirot_boot_path_project%
) else (
set appli_dir=../../%oemirot_boot_path_project%
)

:: =============================================== Configure Option Bytes ====================================================================

set "action=Set TZEN = 1"
echo %action%
:: Trust zone enabled is mandatory to execute ST-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=0xB4
IF !errorlevel! NEQ 0 goto :error

set "action=Set SECBOOT_LOCK option byte to 0xC3 (unlock secure boot address by default)"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_reset% -ob SECBOOT_LOCK=0xC3
IF !errorlevel! NEQ 0 goto :error

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%
IF !errorlevel! NEQ 0 goto :error

set "action=Set SecureBoot address"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_reset% -ob SECBOOTADD=%bootob%
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
%stm32programmercli% %connect_no_reset% -ob SECWM1_STRT=0x0 SECWM1_END=%sec1_end%
IF !errorlevel! NEQ 0 goto :error
%stm32programmercli% %connect_no_reset% -ob SECWM2_STRT=0x7F SECWM2_END=0x0
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

set "action=Write OEMuROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\STM32H573I-DK\Applications\ROT\OEMiROT_Boot\Binary\rot_enc_sign.hex -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Boot Written"

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
echo.
IF [%1] NEQ [AUTO] cmd /k
exit 1
