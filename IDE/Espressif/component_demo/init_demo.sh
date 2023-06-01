. /mnt/c/SysGCC/esp32/esp-idf/v5.0/export.sh


rm -rf ./build
rm -rf ./managed_components/
rm ./main/idf_component.yml
rm ./sdkconfig
rm ./dependencies.lock

idf.py add-dependency "wolfssl/wolfssl^1.0.3-dev"

idf.py build
