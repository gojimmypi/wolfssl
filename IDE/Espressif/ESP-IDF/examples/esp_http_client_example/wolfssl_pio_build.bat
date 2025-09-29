::!/bin/dos

:: Delete the current sdfconfig to ensure defaults are loaded
:: rm sdkconfig.esp32dev

:: Remove the ensire prior .pio build directory to ensure a clean, fresh build
rmdir .pio /s /q

:: build
:: pio run
pio run --verbose > build.log 2>&1
if %ERRORLEVEL% neq 0 (
    @echo An error occurred during build. Exiting with error code %ERRORLEVEL%.
    exit /b %ERRORLEVEL%
)

:: upload and launch putty
pio run --target upload --upload-port COM19
if %ERRORLEVEL% neq 0 (
    @echo An error occurred during upload. Exiting with error code %ERRORLEVEL%.
    exit /b %ERRORLEVEL%
)

c:\tools\putty.exe -load COM19
