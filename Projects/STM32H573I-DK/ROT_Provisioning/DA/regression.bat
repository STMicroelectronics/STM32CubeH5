@ECHO OFF 
IF [%1] NEQ [AUTO] call ../env.bat

set connect_no_reset="-c port=SWD speed=fast ap=1 mode=Hotplug"
set connect_reset="-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst"

set /p "tzen_state= The TrustZone feature is enabled ? %USERREG% [ y | n ]: "
if /i "%tzen_state%" == "y" (
:: Configure OBKeys for Debug Authentication (in case of Provisioning Product state)
%stm32programmercli% %connect_reset% >nul
%stm32programmercli% %connect_no_reset% -sdp .\Binary\DA_Config.obk >nul

%stm32programmercli% -c port=SWD debugauth=2
%stm32programmercli% -c port=SWD speed=fast per=a key=.\Keys\key_3_leaf.pem cert=.\Certificates\cert_leaf_chain.b64 debugauth=1
if %errorlevel% neq 0 goto :error

) else (
    if /i "%tzen_state%" == "n" (
    :: Configure OBKeys for Debug Authentication (in case of Provisioning Product state)
    %stm32programmercli% %connect_reset% >nul
    %stm32programmercli% %connect_no_reset% -sdp .\DA\Binary\DA_ConfigWithPassword.obk >nul

    %stm32programmercli% -c port=SWD debugauth=2
    %stm32programmercli% -c port=SWD pwd=.\Binary\password.bin debugauth=1
    ) else (
    echo "Board not correctly configured"
    goto :error
    )
)

echo "regression script success"
cmd /k
exit 0

:error
echo "regression script failed"
cmd /k
exit 1
