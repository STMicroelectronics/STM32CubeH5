call ../env.bat

:: Define TruseZone Configuration
if "%~1"=="" (set /p "tzen_state= The TrustZone feature is enabled ? %USERREG% [ y | n ]:") else (set tzen_state=%1%)
if /i "%tzen_state%" == "y" (set tzen_ob=0xB4) else ( set tzen_ob=0xC3)

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR

:: Initial configuration
set sec_water_mark=-ob SECWM1_STRT=0x0 SECWM1_END=0x7F SECWM2_STRT=0x0 SECWM2_END=0x7F

:: =============================================== Remove protections and initialize Option Bytes ===========================================
set remove_protect_init_non_secure=-ob WRPSGn1=0xffffffff WRPSGn2=0xffffffff HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0
set remove_protect_init_secure=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3 SWAP_BANK=0 SRAM2_RST=0 SRAM2_ECC=0 NSBOOTADD=0x80000 SECBOOTADD=0xC0000 BOOT_UBE=0xB4


:: =============================================== Erase the user flash =====================================================================
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================
:: Configure TrustZone Option Bytes
set "action=Set TZEN configuration"
echo %action%
%stm32programmercli% %connect_no_reset% -ob TZEN=%tzen_ob%
if %errorlevel% neq 0 goto :error

set "action=Configure Initial OB"
echo %action%
if /i "%tzen_state%" == "n" (
%stm32programmercli% %connect_no_reset% %remove_protect_init_non_secure% %erase_all%
set "action=Configure non secure OB"
if %errorlevel% neq 0 goto :error
)
if /i "%tzen_state%" == "y" (
%stm32programmercli% %connect_no_reset% %remove_protect_init_secure% %erase_all%
set "action=Configure secure OB"
if %errorlevel% neq 0 goto :error
)

:: Configure WaterMark Option Bytes (only when TZEN=1)
if /i "%tzen_state%" == "n" goto :end_programming
set "action=Configure Secure Water Mark"
echo %action%
%stm32programmercli% %connect_reset% %sec_water_mark%
if %errorlevel% neq 0 goto :error

:end_programming
echo Programming success
if [%2] neq [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Programming aborted >CON
if [%2] neq [AUTO] cmd /k
exit 1

