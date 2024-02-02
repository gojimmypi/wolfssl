# wolfSSL TLS Client Example

This is the wolfSSL TLS Client demo, typically used with the [Espressif TLS Server](../wolfssl_server/README.md)
or the CLI [Server](https://github.com/wolfSSL/wolfssl/tree/master/examples/server).

When using the CLI, see the [example parameters](/IDE/Espressif/ESP-IDF/examples#interaction-with-wolfssl-cli).

For general information on [wolfSSL examples for Espressif](../README.md), see the
[README](https://github.com/wolfSSL/wolfssl/blob/master/IDE/Espressif/ESP-IDF/README.md) file.

## Troubleshooting

Wierd results, odd messages, unexpect compiler errors? Manually delete the Arduino build directory.
For Windows users, this is the set of hash-named directories located here:

```text
C:\Users\%USERNAME%\AppData\Local\Temp\arduino\sketches
```

## VisualGDB

Open the VisualGDB Visual Studio Project file in the VisualGDB directory and click the "Start" button.
No wolfSSL setup is needed. You may need to adjust your specific COM port. The default is `COM20`.

## ESP-IDF Commandline v5.x


1. `idf.py menuconfig` to config the project

      1-1. Example Configuration ->  

          Target host ip address : the host that you want to connect to.(default is 127.0.0.1)

     1-2. Example Connection Configuration ->
     
          WIFI SSID: your own WIFI, which is connected to the Internet.(default is "myssid")  
          WIFI Password: WIFI password, and default is "mypassword"
    
    
    Note: the example program uses 11111 port. If you want to use different port  
        , you need to modify DEFAULT_PORT definition in the code.

When you want to test the wolfSSL client

1. `idf.py -p <PORT> flash` and then `idf.py monitor` to load the firmware and see the context  
2. You can use <wolfssl>/examples/server/server program for test.  

         e.g. Launch ./examples/server/server -v 4 -b -i -d


## VisualGDB for ESP8266

Reminder that we build with `make` and not `cmake` in VisualGDB.

Build files will be created in `[project directory]\build`

## ESP-IDF CMake Commandline (version 3.5 or earlier for the ESP8266)

Build files will be created in `[project directory]\build\debug`

```
# Set your path to RTOS SDK, shown here for default from WSL vis VisualGDB
WRK_IDF_PATH=/mnt/c/SysGCC/esp8266/rtos-sdk/v3.4
. $WRK_IDF_PATH/export.sh

# install as needed / prompted
/mnt/c/SysGCC/esp8266/rtos-sdk/v3.4/install.sh

cd IDE/Espressif/ESP-IDF/examples/ESP8266

# adjust settings as desired
idf.py menuconfig

idf.py build flash -p /dev/ttyS55 -b 115200
```

## SM Ciphers

(TODO coming soon)

#### Working Linux Client to ESP32 Server

```
./examples/client/client -h 192.168.1.37 -p 11111 -v 3
```

```text
-c <file>   Certificate file,           default ./certs/client-cert.pem
-k <file>   Key file,                   default ./certs/client-key.pem
-A <file>   Certificate Authority file, default ./certs/ca-cert.pem
```

Example client, with default certs explicitly given:

```bash
./examples/client/client -h 192.168.1.37 -p 11111 -v 3 -c ./certs/client-cert.pem -k      ./certs/client-key.pem -A     ./certs/ca-cert.pem
```

Example client, with RSA 1024 certs explicitly given:

```
./examples/client/client -h 192.168.1.37 -p 11111 -v 3 -c ./certs/1024/client-cert.pem -k ./certs/1024/client-key.pem -A ./certs/1024/ca-cert.pem
```

Command:

```
cd /mnt/c/workspace/wolfssl-$USER/IDE/Espressif/ESP-IDF/examples/wolfssl_server
. /mnt/c/SysGCC/esp32/esp-idf/v5.1/export.sh
idf.py flash -p /dev/ttyS19 -b 115200 monitor
```

```
cd /mnt/c/workspace/wolfssl-$USER

./examples/client/client  -h 192.168.1.108 -v 4 -l TLS_SM4_GCM_SM3 -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem     -A ./certs/sm2/root-sm2.pem -C
```

Output:

```text
SSL version is TLSv1.3
SSL cipher suite is TLS_SM4_GCM_SM3
SSL curve name is SM2P256V1
I hear you fa shizzle!
```

#### Linux client to Linux server:

```
./examples/client/client  -h 127.0.0.1 -v 4 -l ECDHE-ECDSA-SM4-CBC-SM3     -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem     -A ./certs/sm2/root-sm2.pem -C

./examples/server/server                   -v 3 -l ECDHE-ECDSA-SM4-CBC-SM3     -c ./certs/sm2/server-sm2.pem -k ./certs/sm2/server-sm2-priv.pem     -A ./certs/sm2/client-sm2.pem -V
```

See the README.md file in the upper level 'examples' directory for [more information about examples](../README.md).
