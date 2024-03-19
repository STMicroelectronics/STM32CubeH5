call ../env.bat

:: Define TrustZone Configuration
if "%~1"=="" (set /p "tzen_state= The TrustZone configuration is enable ? %USERREG% [ y | n ]:") else (set tzen_state=%1%)
if /i "%tzen_state%" == "y" (set da_file=DA_Config) else (set da_file=DA_ConfigWithPassword)

:: Define the obk file to provisionned
set "da_obk_file=%da_file%.obk"
:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=Hotplug -hardRst

:: =============================================== Configure OB Keys ========================================================
:provisioning
:: Verify if obk file exists
if not exist %~dp0\Binary\%da_obk_file% (
@echo [31m     Error: %da_obk_file% does not exist! use TPC to generate it[0m >CON
goto :error
)

:: Provisioning the obk file 
set "action=Provisioning the obk file"
echo %action%
%stm32programmercli% %connect_reset%
%stm32programmercli% %connect_no_reset% -sdp .\Binary\%da_obk_file%
if %errorlevel% neq 0 (goto :error)

:: Reset the board after provisioning
set "action=Reset the board after provisioning"
echo %action%
%stm32programmercli% %connect_reset%
if %errorlevel% neq 0 goto :error 

echo Provisioning success
if [%2] neq [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Provisioning aborted >CON
if [%2] neq [AUTO] cmd /k
exit 1
