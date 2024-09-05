::!/bin/dos

:: Delete the current sdfconfig to ensure defaults are loaded
:: rm sdkconfig.esp32dev

:: Remove the ensire prior .pio build directory to ensure a clean, fresh build
rmdir .pio /s /q

:: build and upload
pio run --target upload --upload-port COM19

c:\tools\putty.exe -load COM19
