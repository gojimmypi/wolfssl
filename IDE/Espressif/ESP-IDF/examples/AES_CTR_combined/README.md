# AES_CTR Example

This example is based on the "Hello World" example, with the sample [Arduino AES_CTR](./Arduino/AES_CTR_combined_b2.ino) file.

Building on the original app, the wolfSSL component library was added from the [template example](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/template),
see the [components/wolfssl](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/template/components/wolfssl) directory.

The wolfSSL component directory assumes there's already a clone of wolfSSL in a parent directory:

```
cd /mnt/c/workspace
git clone https://github.com/gojimmypi/wolfssl.git

# latest release
git checkout v5.7.6-stable

# my branch based on v5.7.6-stable
git checkout AES_CTR_Example
```

## User Settings

The key changes from the default `user_settings.h` was to add these two macros for AES CTR:

```
/* Added for AES_CTR example */
#define WOLFSSL_AES
#define WOLFSSL_AES_COUNTER
```

# Arduino

The enclosed [user_settings.h](./components/wolfssl/user_settings.h)
was copied to:

```
C:\Users\%USERNAME%\Documents\Arduino\libraries\wolfssl\src
```

With the following lines REMOVED:

```
/* The Espressif project config file. See also sdkconfig.defaults */
#include "sdkconfig.h"

#define WOLFSSL_ESPIDF
```



# Original "Hello World" README follows:

| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |

# Hello World Example

Starts a FreeRTOS task to print "Hello World".

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

Follow detailed instructions provided specifically for this example.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)


## Example folder contents

The project **hello_world** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── pytest_hello_world.py      Python script used for automated testing
├── main
│   ├── CMakeLists.txt
│   └── hello_world_main.c
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Technical support and feedback

Please use the following feedback channels:

* For technical queries, go to the [esp32.com](https://esp32.com/) forum
* For a feature request or bug report, create a [GitHub issue](https://github.com/espressif/esp-idf/issues)

We will get back to you as soon as possible.
