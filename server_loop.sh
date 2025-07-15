#!/bin/bash

count=1

while true; do
    echo ""
    echo "waiting..."
    ./examples/server/server -v 4 -l TLS_AES_128_GCM_SHA256 -p 12345
    ((count++))
    echo "Loop iteration: $count"
done
