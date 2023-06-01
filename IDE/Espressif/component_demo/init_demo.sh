# . /mnt/c/SysGCC/esp32/esp-idf/v5.0/export.sh


rm -rf ./build
rm -rf ./managed_components/
rm ./main/idf_component.yml
rm ./sdkconfig
rm ./dependencies.lock

idf.py add-dependency "wolfssl/wolfssl^1.0.4-dev"

idf.py build

idf.py -b 115200 -p /dev/ttyS23 flash

idf.py -b 115200 -p /dev/ttyS23 monitor
