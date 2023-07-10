@ECHO OFF

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Getting the CubeProgammer_cli path
call ../env.bat

::Keys files
set "projectdir=%~dp0"
pushd %projectdir%\Keys
set keys_pem_dir=%cd%
popd
pushd %projectdir%\..\..\Applications\ROT\OEMiROT_Boot\Src
set keys_c_dir=%cd%
popd
pushd %projectdir%\..\..\..\..\Middlewares\Third_Party\mcuboot
set mcuboot_dir=%cd%
popd
set "keys_c=%keys_c_dir%\keys.c"

:: Keys backup
for /f %%# in ('wmic os get localdatetime^|findstr .') do if "%%#" neq "" set date=%%#
set date=%date:~,4%_%date:~4,2%_%date:~6,2%_%date:~8,2%_%date:~10,2%_%date:~12,2%
set "key_pem_backup_dir=%keys_pem_dir%\%date%_keybkp"
set "cmdcreatedirpem=mkdir %key_pem_backup_dir%
%cmdcreatedirpem%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "key_c_backup_dir=%keys_c_dir%\%date%_keybkp"
set "cmdcreatedirc=mkdir %key_c_backup_dir%
%cmdcreatedirc%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "cmdcpypem=copy %keys_pem_dir%\*.pem %key_pem_backup_dir%"
%cmdcpypem%
IF %ERRORLEVEL% NEQ 0 goto :error_key
set "cmdcpyc=copy %keys_c% %key_c_backup_dir%"
%cmdcpyc%
IF %ERRORLEVEL% NEQ 0 goto :error_key

:start
goto exe:
goto py:
:exe
::line for window executable
set "imgtool=%mcuboot_dir%\scripts\dist\imgtool\imgtool.exe"
set "python="
if exist %imgtool% (
echo Keygen with windows executable
goto cont
)
:py
::line for python
echo Keygen with python script
set "imgtool=%mcuboot_dir%\scripts\imgtool.py"
set "python=python "

:cont
:keygen
::ecc 256 auth key
set "key_ecc=%keys_pem_dir%\OEMiRoT_Authentication.pem"
set "command_key=%python%%imgtool% keygen -k %key_ecc% -t ecdsa-p256"
%command_key%
IF !errorlevel! NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpub -k %key_ecc%  > %keys_c%"
%command_key%
IF !errorlevel! NEQ 0 goto :error_key

:: priv key to encode images
set "key_ecc_enc_priv=%keys_pem_dir%\OEMiRoT_Encryption.pem"
set "key_ecc_enc_pub=%keys_pem_dir%\OEMiRoT_Encryption_Pub.pem"
set "command_key=%python%%imgtool% keygen -k %key_ecc_enc_priv% -t ecdsa-p256 -e %key_ecc_enc_pub%"
%command_key%
IF !errorlevel! NEQ 0 goto :error_key
set "command_key=%python%%imgtool% getpriv  --minimal -k %key_ecc_enc_priv%  >> %keys_c%"
%command_key%
IF !errorlevel! NEQ 0 goto :error_key
echo Script success!
cmd /k
exit 0

:error_key
echo "%command_key% : failed"
echo Script failure
cmd /k
exit 1
