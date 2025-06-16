Lastest Master:

```bash
cd /mnt/c/workspace/wolfssl-gojimmypi/IDE/Espressif/ESP-IDF/examples/esp_http_client_example

# working
WRK_IDF_PATH=/mnt/c/SysGCC/esp32-master/esp-idf/v5.3-master

# PR
WRK_IDF_PATH=/mnt/c/SysGCC/esp32-master/esp-idf/master-pr

pushd $WRK_IDF_PATH
# install if needed
# ./install.sh

. ./export.sh
popd

idf.py set-target esp32
idf.py erase-flash -p /dev/ttyS19 -b 230400
idf.py build flash -p /dev/ttyS19 -b 230400 monitor -b 115200

idf.py set-target esp32c6
idf.py build flash -p /dev/ttyS68 -b 230400 monitor -b 115200

```


How to build WSL ESP-IDF based project
=====================================

```bash
WRK_IDF_PATH=/mnt/c/SysGCC/esp32/esp-idf/v5.2-master

# Confirm 'origin/my_522' branch
cd $WRK_IDF_PATH
git status

echo "Run export.sh from ${WRK_IDF_PATH}"
. ${WRK_IDF_PATH}/export.sh

cd /mnt/c/workspace/wolfssl-gojimmypi/IDE/Espressif/ESP-IDF/examples/esp_http_client_example

idf.py set-target esp32c6

idf.py erase-flash -p /dev/ttyS68 -b 115200

idf.py build flash -p /dev/ttyS68 -b 115200 monitor

idf.py build flash -p /dev/ttyS68 -b 230400 monitor -b 115200
```

How to build PlatformIO based project
=====================================

gitrunner test #5

Modified for no local copy of wolfSSL

1. [Install PlatformIO Core](https://docs.platformio.org/page/core.html)
2. Download [development platform with examples](https://github.com/platformio/platform-espressif32/archive/develop.zip)
3. Extract ZIP archive
4. Run these commands:

```shell
# Setup PIO environment
set PATH=%PATH%;C:\Users\%USERNAME%\.platformio\penv\Scripts\

# The git-clone if wolfssl was from c:\workspace
# Change directory to this example:
cd C:\workspace\wolfssl-gojimmypi\IDE\Espressif\ESP-IDF\examples\esp_http_client_example

# The Espressif menuconfig
pio run -t menuconfig

# Clean
pio run -t clean

# Build project
pio run

# or verbose build:
pio run -v

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

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |
# ESP HTTP Client Example

See the README.md file in the upper level 'examples' directory for more information about examples.


## Requirements

Must use ESP-IDF Fork on wolfssl branch: https://github.com/gojimmypi/esp-idf/tree/wolfssl

See https://github.com/espressif/esp-idf/issues/13966

```
cd /mnt/c/workspace/wolfssl-gojimmypi/IDE/Espressif/ESP-IDF/examples/esp_http_client_example

. /mnt/c/SysGCC/esp32-master/esp-idf/v5.4-master/export.sh

idf.py add-dependency "wolfssl/wolfssl^5.7.1-preview2e"

# Managed Component not loaded until build
idf.py build
```


There are THREE possible wolfSSL libraries (not including staging instances):

```
idf_component_get_property(wolfssl wolfssl COMPONENT_LIB)

idf_component_get_property(wolfssl wolfssl__wolfssl COMPONENT_LIB)
idf_component_get_property(wolfssl esp-wolfssl COMPONENT_LIB)

```
