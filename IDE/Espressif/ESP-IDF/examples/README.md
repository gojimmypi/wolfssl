# Espressif Examples for wolfSSL

These are the core examples for wolfSSL:

- [Benchmark](./wolfssl_benchmark/README.md)

- [Test](./wolfssl_test/README.md)

- [TLS Client](./wolfssl_client/README.md)

- [TLS Server](./wolfssl_server/README.md)

## Other Espressif wolfSSL Examples

- [wolfssl-examples](https://github.com/wolfSSL/wolfssl-examples/tree/master/ESP32)

- [wolfssh](https://github.com/wolfSSL/wolfssh/tree/master/ide/Espressif)

- [wolfssh-examples](https://github.com/wolfSSL/wolfssh-examples/tree/main/Espressif)

## Installing wolfSSL for Espressif projects

[Core examples](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples) 
have a local `components/wolfssl` directory with a special CMakeFile.txt that does not require 
wolfSSL to be installed.

If you want to install wolfSSL, see the setup for [wolfSSL](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF#setup-for-linux) 
and [wolfSSH](https://github.com/wolfSSL/wolfssh/tree/master/ide/Espressif#setup-for-linux).

## VisualGDB

Users of VisualGDB can find project files in each respective example `.\VisualGDB` directory.
For convenience, there are separate project for various target chip sets and ESP-IDF version.

For devices without a built-in JTAG, the projects are configured with the open source [Tigard](https://www.crowdsupply.com/securinghw/tigard)
and using port `COM20`.

For devices _with_ a built-in JTAG, the projects are using `COM9`

Edit the COM port for your project:

- ESP-IDF Project; Bootloader COM Port.
- Raw Terminal; COM Port


## Throubleshooting

If unusual errors occur, exit Visual Studio and manually delete these directories to start over:

- `.\build`
- `.\VisualGDB\.visualgdb`
- `.\VisualGDB\.vs`






