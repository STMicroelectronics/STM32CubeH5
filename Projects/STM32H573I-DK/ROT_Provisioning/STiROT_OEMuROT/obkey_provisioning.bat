call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Getting the CubeProgammer_cli path 
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: Local variable to manage alone script execution
set "product_state=unknown"

:: Update local variable thanks to argument use with the command script executed
IF /i [%2] EQU [OPEN] set product_state=open

IF "!product_state!" == "open" (
set "action=Set SECBOOT_LOCK option byte to 0xC3 (unlock secure boot address)"
:: SECBOOT_LOCK reset to 0xC3 value is mandatory to change BOOT_UBE option byte (this is done only in OPEN product_state)
echo %action%
%stm32programmercli% %connect_no_reset% -ob SECBOOT_LOCK=0xC3
IF !errorlevel! NEQ 0 goto :error

set "action=Set UBE ==> OEM-iRoT (User Flash)"
echo %action%
:: Unique boot entry is set to OEM-iRoT (User Flash) to be able to configure OBKeys in product state Open
%stm32programmercli% %connect_no_reset% -ob BOOT_UBE=0xB4
IF !errorlevel! NEQ 0 goto :error
)

:: =============================================== Configure OB Keys =========================================================================
set "action=Configure OBKeys HDPL1-DA config area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ./../DA/Binary/DA_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-ST-iRoT config area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ./Binary/STiRoT_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-ST-iRoT data area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ./Binary/STiRoT_Data.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL2-OEMuRoT config area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ./Binary/OEMuRoT_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL2-OEMuRoT data area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ./Binary/OEMuRoT_Data.obk
IF !errorlevel! NEQ 0 goto :error

:: =============================================== Boot on ST-iRoT ==========================================================================
IF "!product_state!" == "open" (
set "action=Set UBE ==> ST-iRoT"
echo %action%
:: Unique boot entry is set to ST-iRoT to force ST-iRoT execution at each reset
%stm32programmercli% %connect_no_reset% -ob BOOT_UBE=0xC3
IF !errorlevel! NEQ 0 goto :error

set "action=Set SECBOOT_LOCK to 0xB4 (lock secure boot address)"
echo %action%
%stm32programmercli% %connect_no_reset% -ob SECBOOT_LOCK=0xB4
IF !errorlevel! NEQ 0 goto :error
)

echo Successful option bytes programming and images flashing
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Provisioning aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
