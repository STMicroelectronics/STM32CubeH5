@ECHO OFF

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: CubeProgammer path and input files
set "projectdir=%~dp0"
:: Clear password
set user_password="%projectdir%\user_password.bin"
:: Clear password and DA heading
set da_password="%projectdir%\password.bin"
:: HASH of da_password
set board_password="%projectdir%\board_password.bin"

:start
goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%projectdir%\..\..\..\..\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto create
)
:py
::line for python
echo run config Appli with python script
set "applicfg=%projectdir%\..\..\..\..\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:create
set "AppliCfg=%python% %applicfg%"
:: ======================================================= Create board password =========================================================   
set "action=Create password file"
echo %action%
%AppliCfg% hashcontent -i %user_password% -d %da_password% -h 0x80000000 --create --vb %board_password%
IF !errorlevel! NEQ 0 goto :error

echo Password script creation success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Password creation script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
