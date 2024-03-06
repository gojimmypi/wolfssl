#!/bin/bash
#
# Syntax:
#   ./testMonitor.sh <example_name> <target> <keyword>
#
# Example:
#
#   ./testMonitor.sh wolfssl_test esp32c6 WIP
#
# Define ESPIDF_PUTTY_MONITOR to a non-blank value to call putty
# instead of using `idf.py monitor`
#========================================================================================

# Run shell check to ensure this a good script.
shellcheck "$0"

THIS_HOME_DIR=
export THIS_HOME_DIR="$(pwd)"
# export WOLFSSL_ESPIDF="/mnt/c/workspace/wolfssl-master/IDE/Espressif/ESP-IDF/examples"

# the first parameter is expected to be a project name in the WOLFSSL_ESPIDF directory.
if [ $# -lt 3 ]; then
    echo "Usage: $0 <example_name> <target> <keyword>"
    exit 1
else
    THIS_EXAMPLE="$1"
#    pushd "${WOLFSSL_ESPIDF}" || exit 1
#    pushd "./${THIS_EXAMPLE}" || exit 1

    THIS_TARGET="$2"
    THIS_KEYWORD="$3"
fi

echo "testMonitor current path:"
pwd

#ESP32c2 monitor is 78800

esp32_PORT="/dev/ttyS9"
esp32c2_PORT="/dev/ttyS71"
esp32c3_PORT="/dev/ttyS35"
esp32c6_PORT="/dev/ttyS36"
esp32h2_PORT="/dev/ttyS31"
esp32s2_PORT="/dev/ttyS30"
esp32s3_PORT="/dev/ttyS24"

esp8684_PORT="/dev/ttyS49"
# esp32c2_PORT="/dev/ttyS49" #8684

esp32_PUTTY="COM9"
esp32c2_PUTTY="COM71 - ESP32-C2 74880"
esp32c3_PUTTY="COM35"
esp32c6_PUTTY="COM36"
esp32h2_PUTTY="COM31"
esp32s2_PUTTY="COM30"
esp32s3_PUTTY="COM24"

echo "esp32_PORT:   $esp32_PORT"
echo "esp32c2_PORT: $esp32c2_PORT"
echo "esp32c3_PORT: $esp32c3_PORT"
echo "esp32c6_PORT: $esp32c6_PORT"
echo "esp32s2_PORT: $esp32s2_PORT"
echo "esp32s3_PORT: $esp32s3_PORT"
echo "esp32h2_PORT: $esp32h2_PORT"
echo "esp8684_PORT: $esp8684_PORT"

# given a THIS_TARGET, assign THIS_TARGET_PORT to the respective port.
THIS_TARGET_PORT="${THIS_TARGET}_PORT"

# Check that THIS_TARGET_PORT is defined.
if [ -z "$THIS_TARGET_PORT" ]; then
    echo "Error: No port defined for ${THIS_TARGET}"
    exit 1
else
    echo "THIS_TARGET_PORT=${THIS_TARGET_PORT}"
fi

THIS_TARGET_PORT="${!THIS_TARGET_PORT}"
echo THIS_TARGET_PORT="${THIS_TARGET_PORT}"


# The use of putty is optional
THIS_TARGET_PUTTY="${THIS_TARGET}_PUTTY"

if [ -z "$ESPIDF_PUTTY_MONITOR" ]; then
    echo "Using ESP-IDF monitor"
else
    # Check that THIS_TARGET_PUTTY is defined.
    echo "esp32_PUTTY:   $esp32_PUTTY"
    echo "esp32c2_PUTTY: $esp32c2_PUTTY"
    echo "esp32c3_PUTTY: $esp32c3_PUTTY"
    echo "esp32c6_PUTTY: $esp32c6_PUTTY"
    echo "esp32s2_PUTTY: $esp32s2_PUTTY"
    echo "esp32s3_PUTTY: $esp32s3_PUTTY"
    echo "esp32h2_PUTTY: $esp32h2_PUTTY"
    echo "esp8684_PUTTY: $esp8684_PUTTY"
    echo ""

    if [ -z "$THIS_TARGET_PUTTY" ]; then
        echo "Error: No putty profile defined for ${THIS_TARGET}"
        exit 1
    else
        echo "THIS_TARGET_PUTTY=${THIS_TARGET_PUTTY}"
    fi

    THIS_TARGET_PUTTY="${!THIS_TARGET_PUTTY}"
    echo THIS_TARGET_PUTTY="${THIS_TARGET_PUTTY}"
fi

if [[ "$THIS_TARGET" == "esp8684" ]]; then
    THIS_TARGET=esp32c2
fi



# Assemble some log file names.
echo ""
BUILD_LOG="${THIS_HOME_DIR}/logs/${THIS_EXAMPLE}_build_IDF_v5.1_${THIS_TARGET}_${THIS_KEYWORD}.txt"
FLASH_LOG="${THIS_HOME_DIR}/logs/${THIS_EXAMPLE}_flash_IDF_v5.1_${THIS_TARGET}_${THIS_KEYWORD}.txt"
THIS_LOG="${THIS_HOME_DIR}/logs/${THIS_EXAMPLE}_output_IDF_v5.1_${THIS_TARGET}_${THIS_KEYWORD}.txt"
THIS_CFG="${THIS_HOME_DIR}/logs/${THIS_EXAMPLE}_user_settings_IDF_v5.1_${THIS_TARGET}_${THIS_KEYWORD}.txt"

# cp ./components/wolfssl/include/user_settings.h "${THIS_CFG}"

echo  "BUILD_LOG = ${BUILD_LOG}"
echo  "FLASH_LOG = ${FLASH_LOG}"
echo  "THIS_LOG  = ${THIS_LOG}"
echo  "THIS_CFG  = ${THIS_CFG}"


idf.py --version                                > "${BUILD_LOG}" 2>&1
echo "Full clean for $THIS_TARGET..."

#---------------------------------------------------------------------
idf.py fullclean                                >> "${BUILD_LOG}" 2>&1
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Error during fullclean. Deleting build directory."
    rm -rf ./build
fi

#---------------------------------------------------------------------
echo "set-target $THIS_TARGET..."
idf.py "set-target" "$THIS_TARGET"              >> "${BUILD_LOG}" 2>&1
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Error during set-target"
    exit 1
fi

#---------------------------------------------------------------------
echo "Build $THIS_TARGET..."
idf.py build                                    >> "${BUILD_LOG}" 2>&1
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Error during build"
    exit 1
fi

#---------------------------------------------------------------------
echo "Flash $THIS_TARGET..."
idf.py flash -p "${THIS_TARGET_PORT}" -b 115200 >> "${FLASH_LOG}" 2>&1
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Error during flash"
    exit 1
fi

# popd || exit 1
# popd || exit 1

# Note both of the options spawn a separate process:
if [ -z "$ESPIDF_PUTTY_MONITOR" ]; then
    echo "Monitor..."
    echo  ./wolfssl_monitor.py --port "${THIS_TARGET_PORT}" --baudrate 115200 --logfile "${THIS_LOG}"

    ./wolfssl_monitor.py --port "${THIS_TARGET_PORT}" --baudrate 115200 --logfile "${THIS_LOG}" &
else
    echo "Calling putty..."
    echo "/mnt/c/tools/putty.exe -load \"$THIS_TARGET_PUTTY\""
    /mnt/c/tools/putty.exe -load "$THIS_TARGET_PUTTY" &
fi
