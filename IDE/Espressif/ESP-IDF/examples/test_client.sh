#!/bin/bash

# -h [host]
# -p [port]
# -v 3       # TLS 1.2
# -v 4       # TLS 1.3
# -N         # non blocking
# -d         # disable peer check

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

FAIL=0
cd "~/workspace/wolfssl" || exit 1

while true; do
    echo "Testing TLS 1.2 (only) on low memory devices ..."
    # TLS 1.2
    echo "esp8266"
    ./examples/client/client -h 192.168.1.47  -p 11111 -v 3 -d -A ./certs/1024/ca-cert.pem -c ./certs/1024/client-cert.pem -k ./certs/1024/client-key.pem || FAIL=$((FAIL + 1))

    echo "esp32c2"
    ./examples/client/client -h 192.168.1.48  -p 11111 -v 3 -d -A ./certs/1024/ca-cert.pem -c ./certs/1024/client-cert.pem -k ./certs/1024/client-key.pem || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.53  -p 11111 -v 3 -d -A ./certs/1024/ca-cert.pem -c ./certs/1024/client-cert.pem -k ./certs/1024/client-key.pem || FAIL=$((FAIL + 1))

    echo "Testing TLS 1.2 and TLS 1.3 on capable devices"

    echo "esp32"
    ./examples/client/client -h 192.168.1.27  -p 11111 -v 3 || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.27  -p 11111 -v 4 || FAIL=$((FAIL + 1))

    echo "esp32S3"
    ./examples/client/client -h 192.168.1.78  -p 11111 -v 3 || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.78  -p 11111 -v 4 || FAIL=$((FAIL + 1))

    echo "esp32S3"
    ./examples/client/client -h 192.168.1.236 -p 11111 -v 3 || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.236 -p 11111 -v 4 || FAIL=$((FAIL + 1))


    echo "esp32c3"
    ./examples/client/client -h 192.168.1.26  -p 11111 -v 3 || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.26  -p 11111 -v 4 || FAIL=$((FAIL + 1))

    echo "esp32c6"
    ./examples/client/client -h 192.168.1.127 -p 11111 -v 3 || FAIL=$((FAIL + 1))
    ./examples/client/client -h 192.168.1.127 -p 11111 -v 4 || FAIL=$((FAIL + 1))

    if [[ $FAIL -ne 0 ]]; then
        echo "❌ $FAIL clients failed"
        exit 1
    else
        echo "✅ All clients succeeded."
    fi
done # loop until an error is encountered
