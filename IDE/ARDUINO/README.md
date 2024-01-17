### wolfSSL with Arduino

##### Reformatting wolfSSL as a compatible Arduino Library
This is a shell script that will re-organize the wolfSSL library to be 
compatible with Arduino projects that use Arduino IDE 1.5.0 or newer. 
The Arduino IDE requires a library's source files to be in the library's root 
directory with a header file in the name of the library. This script moves all 
src/ files to the `IDE/ARDUINO/wolfSSL/src` directory and creates a stub header
file called `wolfssl.h` inside that directory.

Step 1: To configure wolfSSL with Arduino, enter one of the following commands
from within the `wolfssl/IDE/ARDUINO` directory:

1. `./wolfssl-arduino.sh`
    - Creates an Arduino Library in `wolfSSL` directory
2 `./wolfssl-arduino.sh INSTALL`
     - Creates an Arduino Library in `wolfSSL` directory
     - Moves that directory to the Arduino library directory:
       - `$HOME/Arduino/libraries` for most bash environments
       - `/mnt/c/Users/$USER/Documents/Arduino/libraries` (for WSL)
3. `./wolfssl-arduino.sh INSTALL /path/to/repository`
     - Creates an Arduino Library in `wolfSSL` directory
     - Copies that directory contents to the specified `/path/to/repository`
4. `./wolfssl-arduino.sh INSTALL /path/to/any/other/directory`
     - Creates an Arduino Library in `wolfSSL` directory
     - Copies that directory contents to the specified `/path/to/repository`

Step 2: Edit `<arduino-libraries>/wolfSSL/src/user_settings.h`
If building for Intel Galileo platform add: `#define INTEL_GALILEO`.
Add any other custom settings, for a good start see the examples in wolfssl root
"/examples/configs/user_settings_*.h"

Step 3: If you experience any issues with custom user_settings.h see the wolfssl
porting guide here for more assistance: https://www.wolfssl.com/docs/porting-guide/

If you have any issues contact support@wolfssl.com for help.

##### Including wolfSSL in Arduino Libraries (for Arduino version 2.0 or greater)

1. In the Arduino IDE:

The wolfSSL library should automatically be detected when found in the `libraries`
directory.

    - In `Sketch -> Include Library` choose wolfSSL for new sketches.


##### Including wolfSSL in Arduino Libraries (for Arduino version 1.6.6)

1. In the Arduino IDE:
    - In `Sketch -> Include Library -> Add .ZIP Library...` and choose the
        `IDE/ARDUNIO/wolfSSL` folder.
    - In `Sketch -> Include Library` choose wolfSSL.

##### wolfSSL Examples

Open an example Arduino sketch for wolfSSL:

	- wolfSSL Client INO sketch: `sketches/wolfssl_client/wolfssl_client.ino`

	- wolfSSL Server INO sketch: `sketches/wolfssl_server/wolfssl_server.ino`
