@ECHO OFF 
IF [%1] NEQ [AUTO] call ../env.bat

set /p "tzen_state= The TrustZone feature is enabled ? %USERREG% [ y | n ]: "
if /i "%tzen_state%" == "y" (
%stm32programmercli% -c port=SWD debugauth=2
%stm32programmercli% -c port=SWD speed=fast key=.\Keys\key_3_leaf.pem cert=.\Certificates\cert_leaf_chain.b64 debugauth=1
IF %errorlevel% NEQ 0 goto :error

) else (
    if /i "%tzen_state%" == "n" (
    %stm32programmercli% -c port=SWD debugauth=2
    %stm32programmercli% -c port=SWD pwd=.\Binary\password.bin debugauth=1
    IF %errorlevel% NEQ 0 goto :error
    )
)

echo "regression script success"
cmd /k
exit 0

:error
echo "regression script failed"
cmd /k
exit 1
