# wolfSSL Espressif IDE

This directory contains documentation and examples for the Espressif SoC devices.

Although wolfSSL _should_ work on any Espressif device, there's explicit support for these:

- esp32
- esp32c2
- esp32c3
- esp32c6
- esp32s2
- esp32s3
- esp32h2

## Special Installation

This is a special example of the `esp_http_client_example`.

The v5.2.2 (`my_522`) branch of the [gojimmypi ESP-IDF Fork](https://github.com/gojimmypi/esp-idf/tree/my_522) is needed.
This branch contains changes intended for the ESP-IDF but needed before fully tested and integrated with the Espressif ESP-IDF.

Find the root of your ESP-IDF install. In the case of VisualGDB:

```bash
# WSL
cd /mnt/c/SysGCC/esp32/esp-idf
```

or

```dos
cd C:\SysGCC\esp32-master\esp-idf\
```

Clone the `gojimmypi` fork of the ESP-IDF:

```
git clone --recursive https://github.com/gojimmypi/esp-idf.git v5.2-my_522
cd v5.2-my_522
git remote add upstream https://github.com/espressif/esp-idf.git
git fetch --all --tags

# git checkout tags/v5.2.2
git checkout my_522
git submodule update --init --recursive

./install.sh
. ./export.sh

# install wolfSSL as a component to Espressif ESP-IDF
git clone https://github.com/gojimmypi/wolfssl.git wolfssl-gojimmypi-my_522
cd wolfssl-gojimmypi-my_522
git checkout ED25519_SHA2_fix
cd ./IDE/Espressif/ESP-IDF
./setup.sh

# Build the example
cd ./examples/esp_http_client_example
idf.py build
```

## Getting Started

If you are new to wolfSSL on the Espressif ESP32, [this video](https://www.youtube.com/watch?v=CzwA3ZBZBZ8)
can help to get started:

[![Video Preview](https://img.youtube.com/vi/CzwA3ZBZBZ8/0.jpg)](https://www.youtube.com/watch?v=CzwA3ZBZBZ8)

Additional ESP-IDF specifics can be found in [Espressif/ESP-IDF](./ESP-IDF/README.md). The [wolfSSL Manual](https://www.wolfssl.com/documentation/manuals/wolfssl/index.html) is also a useful
resource.

## Requirements

The wolfSSL Espressif code requires the ESP-IDF to be installed for
[Windows](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html)
or [Linux / MacOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html).

See the [Espressif Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/).

Any editor can be used. See also the [Espressif Third-Party Tools](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/resources.html)
for a list of feature-rich Integrated Development Environments.
The [wolfSSL examples](./ESP-IDF/examples/README.md) all include a `./VisualGDB` directory with SoC-specific configurations
to help get started quickly.

Although not required, a [JTAG Adapter](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/index.html)
can be helpful for development. When not using a built-in JTAG from Espressif, the examples typically
use the open source [Tigard board](https://github.com/tigard-tools/tigard#readme).

## Environments

The preferred usage of wolfSSL for the Espressif ESP32 is with the Espressif ESP-IDF SDK. There are also libraries available
for use with PlatformIO and Arduino.

There are different installation methods depending on the environment used and project requirements.

| Environment        | no esp-tls          | esp-tls            |
| ------------------ | ------------------- |------------------- |
| VisualGDB          | [Managed Component](https://components.espressif.com/components/wolfssl/wolfssl) | [Managed Component](https://components.espressif.com/components/wolfssl/wolfssl), needs WIP ESP-IDF |
| Espressif ESP-IDF  | [Managed Component](https://components.espressif.com/components/wolfssl/wolfssl) | [Managed Component](https://components.espressif.com/components/wolfssl/wolfssl), needs WIP ESP-IDF |
| PlatformIO ESP-IDF | [PlatformIO Library](https://registry.platformio.org/libraries/wolfssl/wolfssl)  | [setup.sh](https://github.com/wolfSSL/wolfssl/blob/master/IDE/Espressif/ESP-IDF/setup.sh), [setup_win.bat](https://github.com/wolfSSL/wolfssl/blob/master/IDE/Espressif/ESP-IDF/setup_win.bat)  |
| PlatformIO Arduino | [PlatformIO Library](https://registry.platformio.org/libraries/wolfssl/Arduino-wolfSSL)  | Unsupported at this time |
| Arduino            | [Arduino Library](https://www.arduino.cc/reference/en/libraries/wolfssl/)                | Unsupported at this time  |

Note that all of the above options _install a copy_ of wolfSSL. This copy is static and disconnected from source control.

For the ESP-IDF, wolfssl can either be installed in the local project `./components` (default) or manually installed to the respective ESP-IDF [components directory](https://github.com/espressif/esp-idf/tree/master/components).

VisualGDB:

```
C:\SysGCC\esp32\esp-idf\v5.2\components
```


Espressif ESP-IDF:

```
C:\Espressif\frameworks\esp-idf-v5.2\components
```

PlatformIO:

```
C:\Users\%USERNAME%\.platformio\packages\framework-espidf\components\
```

## Examples:

There are a variety of examples to help get started:

* [ESP-IDF Examples](./ESP-IDF/README.md)

## Managed Component

The wolfSSL library can be installed as a managed component:

* [Espressif Managed Component Registry](https://components.espressif.com/components/wolfssl/wolfssl)




## Building wolfSSL for use with other libraries

```
# cd /your-wolfssl-repo
./autogen.h # as necessary
./configure --prefix=/mnt/c/workspace/my_wolfssl_bin --enable-all
make install

# then for some other library such as wolfTPM:

# cd /your-wolftpm-repo
./configure --enable-swtpm --with-wolfcrypt=/mnt/c/workspace/my_wolfssl_bin
```

## Notes:

VisualGDB.

There are registry entries with toolchain paths such as `SysGCC-xtensa-esp32-elf-11.2.0`.

Consider adding new entries for testing with a variety of toolchain versions.

```
Computer\HKEY_USERS\[user GUID]\Software\Free Software Foundation
Computer\HKEY_USERS\[user GUID]\Software\Sysprogs\GNUToolchains
```

| --------- | --------------------- |
| (Default) |                       |
| BINUTILS  | c:\SysGCC\esp32.12.2  |
| G++       | c:\SysGCC\esp32.12.2  |
| GCC       | c:\SysGCC\esp32.12.2  |

WSL environment:

Contents of `/etc/wsl.conf`:
```text
[automount]
options = "metadata"
```

To ignore changes in file attributes, see https://github.com/microsoft/WSL/issues/936#issuecomment-1751469229

```
git config core.filemode false
```


Quick start
```

WORKSPACE=/mnt/c/workspace
WRK_IDF_PATH=/mnt/c/SysGCC/esp32/esp-idf/v5.2
WRK_WOLFSSL_PATH=${WORKSPACE}/wolfssl-$USER
WRK_PROJECT_DIR=${WRK_WOLFSSL_PATH}/IDE/Espressif/ESP-IDF/examples/wolfssl_test

echo "Run export.sh from ${WRK_IDF_PATH}"
. ${WRK_IDF_PATH}/export.sh

echo "Build and flash project in ${WRK_PROJECT_DIR}"
cd ${WRK_PROJECT_DIR}
idf.py build flash -p /dev/ttyS9 -b 115200 monitor
```

Bad chip version:

```
ESP-ROM:esp32c3-20200918
Build:Sep 18 2020
rst:0x3 (RTC_SW_SYS_RST),boot:0xc (SPI_FAST_FLASH_BOOT)
Saved PC:0x403d151e
SPIWP:0xee
mode:DIO, clock div:2
load:0x3fcd6100,len:0x16c8
load:0x403ce000,len:0x930
load:0x403d0000,len:0x2d28
entry 0x403ce000
I (34) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (35) boot: compile time 08:29:06
I (35) boot: chip revision: 2
E (38) boot_comm: This chip is revision 2 but the application is configured for minimum revision 3. Can't run.
```

If you've encountered a chip version earlier than that confirmed to be working
at wolfSSL, try adjusting the settings in `menuconfig`.

#### A fatal error occurred: This chip is esp[X] not esp[Y]

```
A fatal error occurred: This chip is ESP32-S3 not ESP32-C3. Wrong --chip argument?
CMake Error at run_serial_tool.cmake:56 (message):
  /home/gojimmypi/.espressif/python_env/idf4.4_py3.8_env/bin/python
  /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/components/esptool_py/esptool/esptool.py
  --chip esp32c3 failed
```

Delete the `./build` and rename/delete your `sdkconfig` file, then run
`idf.py set-target`, in this example setting to `esp32c3`:

```bash
idf.py set-target esp32c3
```

#### Cmake Cache Warning

```
Executing action: clean
Project sdkconfig was generated for target 'esp32s3', but CMakeCache.txt contains 'esp32c3'. To keep the setting in sdkconfig (esp32s3) and re-generate CMakeCache.txt, run 'idf.py fullclean'. To re-generate sdkconfig for 'esp32c3' target, run 'idf.py set-target esp32c3'.
```

As indicated, run `idf.py set-target` and/or delete the `./build` directory.

#### Connecting, but fails to connect.

Some devices, particularly 3rd party, non-Espressif dev boards may not have implemented
the reset-program hardware properly, causing devices to not be programmed with the
`idf.py flash` command:

```
Connecting......................................

A fatal error occurred: Failed to connect to ESP32: Wrong boot mode detected (0x13)! The chip needs to be in download mode.
For troubleshooting steps visit: https://docs.espressif.com/projects/esptool/en/latest/troubleshooting.html
CMake Error at run_serial_tool.cmake:56 (message):
  /home/gojimmypi/.espressif/python_env/idf4.4_py3.8_env/bin/python
  /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/components/esptool_py/esptool/esptool.py
  --chip esp32 failed
```

Solution:

Press and hold `EN` button, press and release `IO0` button, then release `EN` button.

### Unknown CMake command "esptool_py_flash_project_args".

This unintuitive error was observed when including an unneeded `set(COMPONENTS` in the project-level CMakeLists.txt
and attempting to build with an older toolchain, such as the RTOS SDK 3.4 for the ESP8266.

### PermissionError: [Errno 13] Permission denied could not open port {}

This error, other than the obvious permissions, also occurs when the port is in use by another application:

```text
Traceback (most recent call last):
  File "/home/gojimmypi/.espressif/python_env/rtos3.4_py3.10_env/lib/python3.10/site-packages/serial/serialposix.py", line 322, in open
    self.fd = os.open(self.portstr, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
PermissionError: [Errno 13] Permission denied: '/dev/ttyS55'

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
    [... snip ...]
raise SerialException(msg.errno, "could not open port {}: {}".format(self._port, msg))
serial.serialutil.SerialException: [Errno 13] could not open port /dev/ttyS55: [Errno 13] Permission denied: '/dev/ttyS55'
```
### Panic Task watchdog got triggered.

Long-running code may trip the watchdog timer.

```
Task watchdog got triggered.

Guru Meditation Error: Core  0 panic'ed (unknown). Exception was unhandled.
```

The watchdog needs to be [fed](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/system/wdts.html?highlight=watchdog#_CPPv418esp_task_wdt_resetv) on a regular basis
with `void esp_task_wdt_reset(void)` from `esp8266/include/esp_task_wdt.h`.

Try turning off the WDT in menuconfig, or for Makefiles:

```
EXTRA_CFLAGS += -DNO_WATCHDOG
```

#### Other Solutions

See also [this ESP-FAQ Handbook](https://docs.espressif.com/projects/esp-faq/en/latest/esp-faq-en-master.pdf)
