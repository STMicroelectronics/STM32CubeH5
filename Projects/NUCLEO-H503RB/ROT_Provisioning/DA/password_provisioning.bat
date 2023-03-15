IF [%1] NEQ [AUTO] call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: CubeProgammer path and input files
set "projectdir=%~dp0"
set board_password="%projectdir%\board_password.bin"
set otp_data_soc_mask="%projectdir%\data_soc_mask.bin"

set address_password=0x8FFF000
set address_data_soc_mask=0x8FFF020

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: =========================================== Write data soc mask =========================================================================
set "action=Write data soc mask"
echo %action%

:: Write OTP data soc mask
%stm32programmercli% %connect_no_reset% -w %otp_data_soc_mask% %address_data_soc_mask%
IF !errorlevel! NEQ 0 goto :error

:: =============================================== Write password =========================================================================
set "action=Write Password"
echo %action%

:: Write password
%stm32programmercli% %connect_no_reset% -w %board_password% %address_password%
IF !errorlevel! NEQ 0 goto :error
%stm32programmercli% %connect_reset%

echo        Programming password success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1