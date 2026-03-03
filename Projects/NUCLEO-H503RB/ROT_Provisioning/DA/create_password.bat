@ECHO OFF

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: CubeProgammer path and input files
set "projectdir=%~dp0"
:: Clear password
set user_password="%projectdir%\user_password.bin"
:: Clear password and DA heading
set da_password="%projectdir%\da_password.bin"
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
::=================================================================================================
:: Check if Python V3 is installed
::-------------------------------------------------------------------------------------------------
python --version >nul 2>&1
if %errorlevel% neq 0 (
  echo.
  echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
  echo.
  set "command=Python installation"
  goto :error
)
set "python=python "
:: If found, capture version string removing "Python "
for /f "tokens=2 delims= " %%A in ('python --version 2^>^&1') do (
    set "full_version=%%A"
)
:: extract version details
for /F "tokens=1,2,3 delims=." %%A in ("!full_version!") do (
  set MAJOR_VER=%%A
  set MINOR_VER=%%B
  set PATCH_VER=%%C
)
:: is v3
if not "%MAJOR_VER%" == "3" (
  python3 --version >nul 2>&1
  if !errorlevel! neq 0 (
    echo.
    echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
    echo.
    set "command=Python installation"
    goto :error
  )
  set "python=python3 "
)
::=================================================================================================

:: Environment variable for AppliCfg
echo run config Appli with python script
set "applicfg=%projectdir%\..\..\..\..\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"

:create
set "AppliCfg=%python% %applicfg%"
:: ======================================================= Create board password =========================================================
set "action=Create password file"
echo %action%
%AppliCfg% hashcontent -i %user_password% --vb %board_password% --password --da_bin_file=%da_password% -h 0x000000800c000000
IF !errorlevel! NEQ 0 goto :error

::py ../AppliCfg.py hashcontent -i input_tests/user_password.bin --vb sha.bin --password --da_bin_file=test.bin -h 0x000000800c000000

echo Password script creation success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo        Error when trying to "%action%" >CON
echo        Password creation script aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1
