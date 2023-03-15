call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: =============================================== Configure Option Bytes Keys =============================================================
set "action=Configure OBKeys HDPL1-DA config area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ../DA/Binary/DA_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-OEM-iRoT config area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp Binary/OEMiRoT_Config.obk
IF !errorlevel! NEQ 0 goto :error

set "action=Configure OBKeys HDPL1-OEM-iRoT data area"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp Binary/OEMiRoT_Data.obk
IF !errorlevel! NEQ 0 goto :error

%stm32programmercli% %connect_reset%

echo Provisioning success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Provisioning aborted >CON
echo.
IF [%1] NEQ [AUTO] cmd /k
exit 1
