#!/bin/bash
#
# List all the ESP32 devices on all the ports
#

# Find all /dev/ttyS[n] devices using ls and grep
DEVICES=$(ls /dev/ttyS* 2>/dev/null | grep -o '/dev/ttyS[0-9]*')
echo "Here we go!"
# Iterate over the found devices
for DEVICE in $DEVICES; do
    # echo "Looking at $DEVICE"
    CHIP_TYPE=
    if [ -e "$DEVICE" ]; then
        # echo "Device $DEVICE exists.. checking..."

        THIS_ERROR_CODE=$( (echo "AT" > $DEVICE; echo $?) 2>/dev/null )
        # echo "THIS_ERROR_CODE=$THIS_ERROR_CODE for $DEVICE"

        if [ $THIS_ERROR_CODE -eq 0 ]; then
            # echo "Device $DEVICE accepted data without error, checking..."
            CHIP_TYPE=$(esptool.py -p $DEVICE -b 115200 chip_id | grep "Detecting chip type... ESP" | awk '{print $NF}')

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
        echo "$device does not exist. No device connected."
    fi
done

