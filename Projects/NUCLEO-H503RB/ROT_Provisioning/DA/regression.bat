@ECHO OFF
IF [%1] NEQ [AUTO] call ../env.bat

set board_password=.\board_password.bin
set address_password=0x8FFF000

set connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"
set connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"

:: Configure OBKeys for Debug Authentication (in case of Provisioning Product state)
%stm32programmercli% %connect_reset% >nul
%stm32programmercli% %connect_no_reset% -w %board_password% %address_password% >nul

%stm32programmercli% -c port=SWD debugauth=2
%stm32programmercli% -c port=SWD pwd=password.bin debugauth=1
IF %errorlevel% NEQ 0 goto :error

echo "regression script done, press key"
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo "regression script failed"
echo "Please unplug, then plug your board and try again"
echo "press key"
IF [%1] NEQ [AUTO] pause
exit 1
