#!/bin/bash

export THIS_TARGET=esp32c6

export esp32c3_PORT=/dev/ttyS33
export esp32c6_PORT=/dev/ttyS36

echo ${THIS_TARGET}_PORT

export THIS_APP=wolfssl_benchmark

export BUILD_LOG="./${THIS_APP}/logs/${THIS_APP}_build_IDF_v5_auto${THIS_TARGET}.txt"
export  THIS_LOG="./${THIS_APP}/logs/${THIS_APP}_IDF_v5_auto${THIS_TARGET}.txt"

echo  "BUILD_LOG = ${BUILD_LOG}"
echo  "THIS_LOG  = ${THIS_LOG}"

pushd ${THIS_APP}

idf.py --version                                     > "${BUILD_LOG}" 2>&1
echo "Building..."
idf.py build flash -p ${THIS_TARGET}_PORT -b 115200 >> "${BUILD_LOG}" 2>&1

popd

./wolfssl_monitor.py --port ${THIS_TARGET}_PORT --baudrate 115200 --logfile "${THIS_LOG}"



