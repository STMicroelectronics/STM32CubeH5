@echo off
call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Command with both key/certificate (if TZEN enabled) and password (if TZEN disabled)
%stm32programmercli% -c port=SWD key=.\Keys\key_3_leaf.pem cert=.\Certificates\cert_leaf_chain.b64 pwd=.\Binary\password.bin debugauth=1
IF !errorlevel! NEQ 0 goto :error

echo "dbg_auth script success"
cmd /k
exit 0

:error
echo "dbg_auth script failed"
cmd /k
exit 1
