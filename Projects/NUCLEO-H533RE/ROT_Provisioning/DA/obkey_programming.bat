set stm32programmercli="C:\Users\boudinar\Downloads\H512K_test\bin\STM32_Programmer_CLI.exe"
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

echo "Set UBE ==> OEMiRoT (User Flash)"
:: Unique boot entry is set to OEMiRoT (User Flash) to be able to configure OBKeys in product state Open
%stm32programmercli% %connect_no_reset% -ob BOOT_UBE=0xB4
IF %errorlevel% NEQ 0 goto :error

:: =============================================== Configure OB Keys =========================================================================
echo "Configure OBKeys HDPL1-DA config area"
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp ../DA/DA_Config.obk
IF %errorlevel% NEQ 0 goto :error

echo "Provisioning success"
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo "Provisioning aborted"
IF [%1] NEQ [AUTO] cmd /k
exit 1