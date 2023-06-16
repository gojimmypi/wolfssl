#!/bin/bash
THIS_ERROR_CODE=0
# export PATH=/mnt/s3linux/crosstool-NG/builds/xtensa-esp32s3-linux-uclibcfdpic/bin:$PATH
echo "sudo check"
sudo echo "ok"

cd /mnt/s3linux
export XTENSA_GNU_CONFIG=`pwd`/xtensa-dynconfig/esp32s3.so
echo $XTENSA_GNU_CONFIG

cd /mnt/c/test/wolfssl-xtensa

echo "Building wolfSSL ..."
make

THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Failed to build wolfSSL."
    exit 1
else
    echo ""
    echo "Build wolfSSL completed successfully!"
    echo ""
fi

make install
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Failed to install wolfSSL."
    exit 1
else
    echo ""
    echo "Install wolfSSL completed successfully!"
    echo ""
fi

cp ./wolfcrypt/test/testwolfcrypt  /mnt/s3linux/build-xtensa-2023.02-fdpic-esp32s3/target/opt/testwolfcrypt
cp ./wolfcrypt/benchmark/benchmark /mnt/s3linux/build-xtensa-2023.02-fdpic-esp32s3/target/opt/benchmark


cd /mnt/s3linux

echo "Creating build-xtensa-2023.02-fdpic-esp32s3/images/rootfs.cramfs ..."
nice make -C buildroot O=`pwd`/build-xtensa-2023.02-fdpic-esp32s3
THIS_ERROR_CODE=$?

if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Failed to create rootfs.cramfs."
    exit 1
else
    echo ""
    echo "Successfully created rootfs.cramfs"
    echo ""
fi

ls build-xtensa-2023.02-fdpic-esp32s3/images/rootfs.cramfs -al

sudo chmod 666 /dev/ttyUSB0

# check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo "Please follow the instruction of ESP-IDF installation and set IDF_PATH."
    exit 1
fi

# make sure it actually exists
if [ ! -d "$IDF_PATH" ]; then
    echo "ESP-IDF Development Framework doesn't exist.: $IDF_PATH"
    exit 1
fi

# is export.sh in the IDF path?
if [ ! -e "$IDF_PATH/export.sh" ]; then
    echo "ESP-IDF export.sh: $IDF_PATH/export.sh"
    exit 1
fi

# needs ESP-IDF:
parttool.py write_partition --partition-name rootfs --input build-xtensa-2023.02-fdpic-esp32s3/images/rootfs.cramfs

