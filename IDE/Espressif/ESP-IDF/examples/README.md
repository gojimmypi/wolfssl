# Espressif ESP-IDF wolfSSL Examples

# wolfSSL template

The [template](./template/README.md) project is a bare-bones project with all the includes, but nothing more
than a Hello World from the ESP-IDF and wolfSSL libraries. This is best if you want to get started and you know
where you are going.

# wolfSSL Benchmark

The [wolfssl_bencmark](./wolfssl_benchmark/README.md) is a sample application that includes the [wolfcrypt benchmark source code](https://github.com/wolfSSL/wolfssl/tree/master/wolfcrypt/benchmark)
in a wrapper for the ESP-IDF environment. See the wolfssl component [CMakeLists.txt](./wolfssl_benchmark/components/wolfssl/CMakeLists.txt) for `COMPONENT_SRCDIRS`:


```
set(COMPONENT_SRCDIRS # ...other sources...
                        "${WOLFSSL_ROOT}/wolfcrypt/benchmark/"
                        )
```

# wolfSSL Test

The [wolfssl_test](./wolfssl_test/README.md) is a sample application that includes the [wolfcrypt test source code](https://github.com/wolfSSL/wolfssl/tree/master/wolfcrypt/test)
in a wrapper for the ESP-IDF environment. See the wolfssl component [CMakeLists.txt](./wolfssl_test/components/wolfssl/CMakeLists.txt) for `COMPONENT_SRCDIRS`:


```
set(COMPONENT_SRCDIRS # ...other sources...
                        "${WOLFSSL_ROOT}/wolfcrypt/test/"
                        )
```

# wolfSSL Client

The [wolfssl_client](./wolfssl_client/README.md) project is a sample TLS client (packet sender).

# wolfSSL Server

The [wolfssl_server](./wolfssl_server/README.md) project is a sample TLS server (packet listener).

# wolfssl monitor

The [wolfssl_monitor.py](../../wolfssl_monitor.py) script is similar to the ESP-IDF `idf.py monitor`
command, but listens for keywords from the serial port to automatically exit monitor mode upon completion
of the embedded application. 
