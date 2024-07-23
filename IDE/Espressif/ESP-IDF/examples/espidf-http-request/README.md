How to build PlatformIO based project
=====================================

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-espressif32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Setup PIO environment
set PATH=%PATH%;C:\Users\%USERNAME%\.platformio\penv\Scripts\

# The git-clone if wolfssl was from c:\workspace
# Change directory to this example:
cd C:\workspace\wolfssl-gojimmypi\IDE\Espressif\ESP-IDF\examples\espidf-http-request

# The Espressif menuconfig
pio run -t menuconfig

# Clean
pio run -t clean

# Build project
pio run

## Other commands:

# Upload firmware
pio run --target upload

# Build specific environment
pio run -e quantum

# Upload firmware for the specific environment
pio run -e quantum --target upload

# Clean build files
pio run --target clean

# Upload to COM82
pio run --target upload --upload-port COM19

# monitoe serial port (ctrl-C to exit)
pio device monitor -p COM19
```
