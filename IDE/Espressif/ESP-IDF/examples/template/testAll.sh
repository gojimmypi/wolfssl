#!/usr/bin/env bash
#
# testAll.sh [keyword suffix] [ESP32 toolchain dir] [ESP8266 toolchain dir]
#
# Example:
#
#   testAll.sh http_test  /mnt/c/SysGCC/esp32/esp-idf/v5.2-master  /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
#
# Build and compile the wolfssl_test for all platforms.
#
# Supply optional keyword suffix value for log file names.
#
# See testMonitor.sh for USB port settings.
#
# Define ESPIDF_PUTTY_MONITOR to a non-blank value to call putty.
# instead of using `idf.py monitor`
#==============================================================================

# Run shell check to ensure this a good script.
# Specify the executable shell checker you want to use:
MY_SHELLCHECK="shellcheck"

# Check if the executable is available in the PATH
if command -v "$MY_SHELLCHECK" > /dev/null 2>&1; then
    # Run your command here
    shellcheck "$0" || exit 1
else
    echo "$MY_SHELLCHECK is not installed. Please install it if changes to this script have been made."
    exit 1
fi

# Run shell check to ensure this a good script.
shellcheck "$0"

if [[ "$PATH" == *"rtos-sdk"* ]]; then
    echo "Error. Detected rtos-sdk in path."
    echo "Need to start with clean path (no prior idf.py setup) "
    exit 1
fi

# ESP32 Path for ESP-IDF: fixed value or param #2
WRK_IDF_PATH_ESP32=$2
if [[ "$WRK_IDF_PATH_ESP32" == "" ]]; then
    #WRK_IDF_PATH_ESP32=/mnt/c/SysGCC/esp32/esp-idf/v5.2-master
    WRK_IDF_PATH_ESP32=/mnt/c/SysGCC/esp32-master/esp-idf/master
fi

if [[ -d "$WRK_IDF_PATH_ESP32" ]]; then
    echo "Using IDF Path for ESP32: $WRK_IDF_PATH_ESP32"
else
    echo "Path not found for ESP32: $WRK_IDF_PATH_ESP32"
    exit 1
fi

# ESP32 Path for ESP-IDF: fixed value or param #3
WRK_IDF_PATH_ESP8266=$3
if [[ "$WRK_IDF_PATH_ESP8266" == "" ]]; then
    WRK_IDF_PATH_ESP8266=/mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
fi

if [[ -d "$WRK_IDF_PATH_ESP8266" ]]; then
    echo "Using IDF Path for ESP8266: $WRK_IDF_PATH_ESP8266"
else
    echo "Path not found for ESP8266: $WRK_IDF_PATH_ESP8266"
    exit 1
fi

if [[ "$CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG" == "1" ]]; then
    echo "Using CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG"
else
    echo "NOT using CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG"
    echo "Unless you have a SSID called myssid, examples will not connect with default password"
    echo "export CONFIG_WOLFSSL_USE_MY_PRIVATE_CONFIG=1 to use private config file"
    exit 1
fi

#******************************************************************************
# Kill all currently running instances of putty.exe
# If there are no running instances, taskkill exits with non-zero error code.
#******************************************************************************
echo "Closing any open putty sessions"
taskkill.exe /IM putty.exe /F  > /dev/null 2>&1;

# Abort on any future errors
set -e

# Save the current PATH to a temporary variable
ORIGINAL_PATH="$PATH"
echo "ORIGINAL_PATH=$PATH"

export ESPIDF_PUTTY_MONITOR="TRUE"

THIS_SUFFIX="$1"


#******************************************************************************
# ESP8266 uses rtos-sdk/v3.4 toolchain. Test this first, as it is slowest.
# WRK_IDF_PATH=/mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
#******************************************************************************

# Clear ESP-IDF environment variables to ensure clean start for export.sh
unset ESP_IDF_VERSION
unset ESP_ROM_ELF_DIR
unset IDF_DEACTIVATE_FILE_PATH
unset IDF_PATH
unset IDF_PYTHON_ENV_PATH
unset IDF_TOOLS_EXPORT_CMD
unset IDF_TOOLS_INSTALL_CMD
unset OPENOCD_SCRIPTS


#******************************************************************************
# Backup existing build and sdkconfig files
#******************************************************************************

THIS_BUILD_BACKUP="./build.test.bak"
THIS_CONFIG_BACKUP="./sdkconfig.bak"

if [ -d "$THIS_BUILD_BACKUP" ]; then
    # If prior backup exists, delete it.
    rm -rf "$THIS_BUILD_BACKUP" || exit 1
    echo "Directory $THIS_BUILD_BACKUP has been deleted."
fi

if [ -d "./build" ]; then
    # If we already have a build directory, save it.
    mv "./build" "$THIS_BUILD_BACKUP" || exit 1
    echo "Directory ./build has been moved to $THIS_BUILD_BACKUP"
fi

if [ -f "$THIS_CONFIG_BACKUP" ]; then
  # If a prior config backup exists, delete it.
  rm "$THIS_CONFIG_BACKUP" || exit 1
  echo "Config $THIS_CONFIG_BACKUP has been deleted."
fi

if [ -f "./sdkconfig" ]; then
    # If we already have a config, save it.
    mv "./sdkconfig" "$THIS_CONFIG_BACKUP" || exit 1
    echo "File ./sdkconfig has been moved to $THIS_CONFIG_BACKUP"
fi


#******************************************************************************
# Setup ESP8266 environment
#******************************************************************************

echo "Run ESP8266 export.sh from ${WRK_IDF_PATH_ESP8266}"
if [ -f "$WRK_IDF_PATH_ESP8266/export.sh" ]; then
    # shell check should not follow into the ESP-IDF export.sh
    # shellcheck disable=SC1090
    # shellcheck disable=SC1091
    . "$WRK_IDF_PATH_ESP8266"/export.sh
    THIS_ERROR_CODE=$?
    if [ $THIS_ERROR_CODE -ne 0 ]; then
        echo ""
        echo "Error during export.sh"
        exit $THIS_ERROR_CODE
    fi
else
    echo "File $WRK_IDF_PATH_ESP8266/export.sh not found"
    exit 1
fi


# Tensilica
./testMonitor.sh wolfssl_test esp8266 "$THIS_SUFFIX" || exit 1 # 2715073

#******************************************************************************
# ESP32[-N] uses esp-idf/v5.2 toolchain
# WRK_IDF_PATH=/mnt/c/SysGCC/esp32/esp-idf/v5.2
#******************************************************************************
# Restore the original PATH
export PATH="$ORIGINAL_PATH"

# Clear ESP-IDF environment variables to ensure clean start
unset ESP_IDF_VERSION
unset ESP_ROM_ELF_DIR
unset IDF_DEACTIVATE_FILE_PATH
unset IDF_PATH
unset IDF_PYTHON_ENV_PATH
unset IDF_TOOLS_EXPORT_CMD
unset IDF_TOOLS_INSTALL_CMD
unset OPENOCD_SCRIPTS

echo "Run ESP32 export.sh from ${WRK_IDF_PATH_ESP32}"

pushd "$WRK_IDF_PATH_ESP32"
# shell check should not follow into the ESP-IDF export.sh
# shellcheck disable=SC1090
# shellcheck disable=SC1091
. ./export.sh
popd

# Comment numeric values are recently observed runtime durations.
# Different tests may be enabled for each device.
# This list is not indicative of relative performance.

# Limited hardware acceleration, test slowest first:
./testMonitor.sh wolfssl_test esp32h2 "$THIS_SUFFIX" || exit 1 # 1424084 esp32h2 COM31" ok
./testMonitor.sh wolfssl_test esp8684 "$THIS_SUFFIX" || exit 1 # 1065290 esp8684 COM49" ok

# RISC-V
./testMonitor.sh wolfssl_test esp32c2 "$THIS_SUFFIX" || exit 1 # 1133856 esp32c2 COM79" ok
./testMonitor.sh wolfssl_test esp32c3 "$THIS_SUFFIX" || exit 1 # 344677  esp32c3 COM35"   NT
./testMonitor.sh wolfssl_test esp32c6 "$THIS_SUFFIX" || exit 1 # 346393  esp32c6 COM36" ok

# Xtensa
./testMonitor.sh wolfssl_test esp32   "$THIS_SUFFIX" || exit 1 # 259093  esp32   COM9"    NT
./testMonitor.sh wolfssl_test esp32s2 "$THIS_SUFFIX" || exit 1 # 305004  esp32s2 COM30"   NT
./testMonitor.sh wolfssl_test esp32s3 "$THIS_SUFFIX" || exit 1 # 267518  esp32s3 COM24"   NT

#******************************************************************************
# Restore prior build and sdkconfig files
#******************************************************************************

if [ -f "$THIS_CONFIG_BACKUP" ]; then
    echo "Restoring prior sdkconfig..."
    if [ -f "./sdkconfig" ]; then
        rm "./sdkconfig" || exit 1
    fi
    mv "$THIS_CONFIG_BACKUP" "./sdkconfig"  || exit 1
    echo "Config file ./sdkconfig has been restored from $THIS_CONFIG_BACKUP"
fi

if [ -d "$THIS_BUILD_BACKUP" ]; then
    if [ -d "./build" ]; then
        rm -rf "./build" || exit 1
    fi
    mv "$THIS_BUILD_BACKUP" "./build" || exit 1
    echo "Build directory has been restored from $THIS_BUILD_BACKUP"
fi

# Restore the original PATH
export PATH="$ORIGINAL_PATH"

echo "Done!"
