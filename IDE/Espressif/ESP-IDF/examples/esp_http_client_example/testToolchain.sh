#!/usr/bin/env bash
#
testAll.sh http_test  /mnt/c/SysGCC/esp32/esp-idf/v5.2-master  /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
testAll.sh http_test  /mnt/c/SysGCC/esp32/esp-idf/v5.3-master  /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4

testAll.sh http_test  /mnt/c/SysGCC/esp32.12.2/esp-idf/v5.0    /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
testAll.sh http_test  /mnt/c/SysGCC/esp32.12.2/esp-idf/v5.1    /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4

testAll.sh http_test  /mnt/c/SysGCC/esp32/esp-idf/master       /mnt/c/SysGCC/esp8266/rtos-sdk/v3.4

