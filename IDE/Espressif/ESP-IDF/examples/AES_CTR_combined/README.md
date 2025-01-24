# AES_CTR Example

This example is based on the Espressif [Hello World Example](https://github.com/espressif/esp-idf/tree/release/v5.2/examples/get-started/hello_world), with the sample [Arduino AES_CTR](./Arduino/AES_CTR_combined_b2/AES_CTR_combined_b2.ino) file.

Building on the original app, the wolfSSL component library was [added](https://github.com/gojimmypi/wolfssl/commit/a13ba6a1fd814303e435fa4d3314b96f5e517f5e)
from the [template example](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/template),
see the [components/wolfssl](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples/template/components/wolfssl) directory.

You can see all my changes in the [commit history](https://github.com/gojimmypi/wolfssl/commits/AES_CTR_Example/IDE/Espressif/ESP-IDF/examples/AES_CTR_combined).

The wolfSSL component directory assumes there's already a clone of wolfSSL in a parent directory:

```
cd /mnt/c/workspace
git clone https://github.com/gojimmypi/wolfssl.git

# latest release
git checkout v5.7.6-stable

# my branch based on v5.7.6-stable
git checkout AES_CTR_Example
```

As shown above, this branch is based on the recent [wolfSSL v5.7.6-stable](https://github.com/wolfSSL/wolfssl/releases/tag/v5.7.6-stable) release.


## User Settings

The key changes from the default `user_settings.h` was to add these two macros for AES CTR:

```
/* Added for AES_CTR example */
#define WOLFSSL_AES
#define WOLFSSL_AES_COUNTER
```



## ExpectIntEQ

The `ExpectIntEQ` from other examples is found in [tests/unit.h](https://github.com/gojimmypi/wolfssl/blob/master/tests/unit.h).

Although it might be an interesting future exercise to integrate the test harness files with the Arduino library,
it was a bit more than desired for this exercise. Instead, there's a single
[new ExpectIntEQ macro](https://github.com/gojimmypi/wolfssl/blob/241b81d5d5a54ef4d6ce58a400b5dfcddc49d6d1/IDE/Espressif/ESP-IDF/examples/AES_CTR_combined/main/hello_world_main.c#L37)
like this for the `ExpectIntEQ`:

```
#define ExpectIntEQ(p1, p2) if (p1 == p2) {                   \
                                ESP_LOGI("test", "success");  \
                            }                                 \
                            else {                            \
                                ESP_LOGE("test", "failed");   \
                            }
```

# Espressif ESP-IDF

The ESP-IDF example is this root directory at [IDE/Espressif/ESP-IDF/examples/AES_CTR_combined](https://github.com/gojimmypi/wolfssl/tree/AES_CTR_Example/IDE/Espressif/ESP-IDF/examples/AES_CTR_combined).

The instructions for building, shown here using WSL:

```
WRK_IDF_PATH=/mnt/c/SysGCC/esp32/esp-idf/v5.2

echo "Run export.sh from ${WRK_IDF_PATH}"
. ${WRK_IDF_PATH}/export.sh

# build the example:
idf.py build

# optionally erase the flash
idf.py erase-flash -p /dev/ttyS19 -b 115200

# flash the code onto the serial device at /dev/ttyS19
idf.py flash -p /dev/ttyS19 -b 115200

# build, flash, and view UART output with one command:
idf.py flash -p /dev/ttyS19 -b 115200 monitor
```

There's also an enclosed `AES_CTR_combined.sln` isual Studio file that can be used with the
[VisualGDB Extension](https://visualgdb.com/).

# Arduino

The enclosed [user_settings.h](./components/wolfssl/include/user_settings.h)
was clightly modified and copied to the installed wolfssl Arduino library here, overwriting the file that was already there:

```
C:\Users\%USERNAME%\Documents\Arduino\libraries\wolfssl\src
```

With the following lines REMOVED for Arduino:

```
/* The Espressif project config file. See also sdkconfig.defaults */
#include "sdkconfig.h"

#define WOLFSSL_ESPIDF
```

Of particular interest is the [wolfSSL include file section](https://github.com/gojimmypi/wolfssl/blob/241b81d5d5a54ef4d6ce58a400b5dfcddc49d6d1/IDE/Espressif/ESP-IDF/examples/AES_CTR_combined/Arduino/AES_CTR_combined_b2/AES_CTR_combined_b2.ino#L30C1-L33C35)
in this exact order:

```
#include "wolfssl.h"
#include <wolfssl/wolfcrypt/settings.h> // Reminder: settings.h includes user_settings.h
                                        // For ALL settings, see ~\Arduino\libraries\wolfSSL\src\user_settings.h
#include <wolfssl/wolfcrypt/aes.h>
```

See also the [IDE/ARDUINO](https://github.com/gojimmypi/wolfssl/tree/AES_CTR_Example/IDE/ARDUINO) examples.



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
