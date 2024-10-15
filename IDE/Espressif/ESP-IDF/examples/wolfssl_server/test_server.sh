#!/bin/bash
#
# peek_openssl.sh
#

LOOPS=10000

# Run shell check to ensure this a good script.
# Specify the executable shell checker you want to use:
MY_SHELLCHECK="shellcheck"

# Check if the executable is available in the PATH
if command -v "$MY_SHELLCHECK" >/dev/null 2>&1; then
    # Run your command here
    shellcheck "$0" || exit 1
else
    echo "$MY_SHELLCHECK is not installed. Please install it if changes to this script have been made."
    exit 1
fi

failed=0
count=1

echo "Running clients to connect to servers..."
pushd /mnt/c/workspace/wolfssl-gojimmypi-pr || exit 1


tls_test() {
    THIS_DEVICE_NAME="$1"
    THIS_IP=$2
    echo "Connect to $THIS_DEVICE_NAME at $THIS_IP"

    if ! ./examples/client/client  -h "$THIS_IP" -v 4; then
        echo "Failed with exit code $?."
        failed=1
    fi
}

until [ $count -gt "$LOOPS" ] || [ $failed -gt 0 ]; do
    echo "Count: $count"

    # COM9 SP32-H2, no WiFi

    tls_test "COM11 ESP8266"  "192.168.1.47"

    tls_test "COM10 ESP8684 (C2)" "192.168.1.48"

    tls_test "COM12 ESP32-C2" "192.168.1.53"

    tls_test "COM6 ESP32-C3" "192.168.1.26"

    tls_test "COM8 ESP32-C6" "192.168.1.127"

    tls_test "COM7 ESP32"    "192.168.1.27"

    tls_test "COM5 ESP32-S2" "192.168.1.78"

    tls_test "COM4 ESP32-S3" "192.168.1.236"

    count=$((count + 1))
done

popd || exit 1

echo "done"
