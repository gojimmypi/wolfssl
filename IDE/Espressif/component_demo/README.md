


Unfortunately, there is no way to change the build-system name of a dependency installed by the component manager. It's always `namespace__component`.

```
. ~/esp/esp-idf/export.sh
idf.py create-project-from-example "wolfssl/wolfssl^1.0.13-dev:wolfssl_benchmark"
cd wolfssl_benchmark
idf.py -b 115200 flash monitor
```

```
. /mnt/c/SysGCC/esp32/esp-idf/v5.0/export.sh
idf.py create-project-from-example "wolfssl/wolfssl^1.0.13-dev:wolfssl_benchmark"
cd wolfssl_benchmark
idf.py -b 115200 flash monitor
```

To not scan every serial port looking for an ESP32, use the `-p` parameter:

```
. /mnt/c/SysGCC/esp32/esp-idf/v5.0/export.sh
idf.py create-project-from-example "wolfssl/wolfssl^1.0.13-dev:wolfssl_benchmark"
cd wolfssl_benchmark
idf.py -p /dev/ttyS23 -b 115200 flash monitor
```
