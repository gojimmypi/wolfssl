#!/bin/bash
#
# List all the ESP32 devices on all the ports
#

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

WRK_IDF_PATH_ESP32=/mnt/c/SysGCC/esp32/esp-idf/v5.2-master

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

# shell check should not follow into the ESP-IDF export.sh
# shellcheck disable=SC1090
# shellcheck disable=SC1091
. "$WRK_IDF_PATH_ESP32"/export.sh


# Find all /dev/ttyS[n] devices using ls and grep
# DEVICES=$(ls /dev/ttyS* 2>/dev/null | grep -o '/dev/ttyS[0-9]*')

DEVICES=()
for dev in /dev/ttyS*; do
    [[ -e "$dev" ]] && DEVICES+=("$dev")
done

echo "Here we go! A bit slow to query every device... ensure none are in use... please wait..."
# Iterate over the found devices
for DEVICE in "${DEVICES[@]}"; do
    # echo "Looking at $DEVICE"
    CHIP_TYPE=
    if [ -e "$DEVICE" ]; then
        # echo "Device $DEVICE exists.. checking..."

        THIS_ERROR_CODE=$( (echo "AT" > "$DEVICE"; echo $?) 2>/dev/null )
        # echo "THIS_ERROR_CODE=$THIS_ERROR_CODE for $DEVICE"

        if [ "$THIS_ERROR_CODE" -eq 0 ]; then
            # echo "Device $DEVICE accepted data without error, checking..."
            CHIP_TYPE=$(esptool.py -p "$DEVICE" -b 115200 chip_id | grep "Detecting chip type... ESP" | awk '{print $NF}')

            # esptool.py -p "$device" -b 115200 chip_id | grep "Detecting chip type... ESP"
            if [[ "$CHIP_TYPE" != "" ]]; then
                #echo "Found $DEVICE with $CHIP_TYPE"

                # code gen:
                # esp32_port="/dev/ttyS19"
                LOWERCASE_CHIP_TYPE="${CHIP_TYPE,,}"
                # echo $LOWERCASE_CHIP_TYPE
                CLEANED_LOWERCASE_CHIP_TYPE="${LOWERCASE_CHIP_TYPE//-}"
                echo "$CLEANED_LOWERCASE_CHIP_TYPE""_PORT=\"$DEVICE\""
            fi
        fi
    else
        echo "$DEVICE does not exist. No device connected."
    fi
done

