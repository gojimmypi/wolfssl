#!/bin/bash
THIS_ERROR_CODE=0
# export PATH=/mnt/s3linux/crosstool-NG/builds/xtensa-esp32s3-linux-uclibcfdpic/bin:$PATH

echo "sudo check"
sudo echo "ok"

ls /dev/ttyUSB0

THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Failed to find /dev/ttyUSB0"
    exit 1
else
    echo ""
    echo "Found /dev/ttyUSB0"
    echo ""
fi

sudo chmod 666 /dev/ttyUSB0
THIS_ERROR_CODE=$?
if [ $THIS_ERROR_CODE -ne 0 ]; then
    echo ""
    echo "Failed assign permissions to /dev/ttyUSB0"
    exit 1
else
    echo ""
    echo "Assigned permissions to /dev/ttyUSB0"
    echo ""
fi

cd /mnt/s3linux
export XTENSA_GNU_CONFIG=`pwd`/xtensa-dynconfig/esp32s3.so
echo $XTENSA_GNU_CONFIG

cd /mnt/c/test/wolfssl-xtensa
pwd

# -mforce-l32
./configure \
  --host=xtensa-esp32s3-linux-uclibcfdpic \
  CC=xtensa-esp32s3-linux-uclibcfdpic-gcc \
  AR=xtensa-esp32s3-linux-uclibcfdpic-ar \
  STRIP=xtensa-esp32s3-linux-uclibcfdpic-strip \
  RANLIB=xtensa-esp32s3-linux-uclibcfdpic-ranlib \
  --prefix=/mnt/s3linux/build-xtensa-2023.02-fdpic-esp32s3/target/opt \
  CFLAGS="-mfdpic -mforce-l32  -DNO_WRITEV -DWOLFSSL_USER_SETTINGS -DDEBUG_WOLFSSL -DTFM_NO_ASM -DALT_ECC_SIZE" \
  --disable-filesystem --disable-all \
  --disable-shared --enable-psk --enable-sha --disable-fpecc --disable-ecc --disable-rsa --disable-sp-asm --disable-sha384 --disable-sha512 --disable-sha224 --disable-md5


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
pwd
parttool.py write_partition --partition-name rootfs --input build-xtensa-2023.02-fdpic-esp32s3/images/rootfs.cramfs
echo $(date) | tee ./build-xtensa-2023.02-fdpic-esp32s3/target/opt/timestamp.txt
ls ./build-xtensa-2023.02-fdpic-esp32s3/images/rootfs.cramfs -al

