@frankencode 

Hi Frank - 

First, regarding websockets, check out https://github.com/wolfssl/osp, in particular [wolfSSL/osp/websocketpp](https://github.com/wolfSSL/osp/tree/master/websocketpp).

I do not personally have any experience with that, but let me know if you are interested in using something other than the `esp-tls` layer in your project.

Regarding the esp-tls.... 

My example [wolfssl_client](https://github.com/gojimmypi/wolfssl/tree/ED25519_SHA2_fix/IDE/Espressif/ESP-IDF/examples/wolfssl_client) has been updated with a [Kconfig](https://github.com/gojimmypi/wolfssl/blob/ED25519_SHA2_fix/IDE/Espressif/ESP-IDF/examples/wolfssl_client/components/wolfssl/Kconfig) that has the esp-tls `TLS_STACK_WOLFSSL` capability with wolfSSL in the Espressif components and not a local component.

I would have used your ESP-IDF  fork at [frankencode/esp-idf](https://github.com/frankencode/esp-idf/tree/master) but I got into quite a tangle with the ESP-IDF v5.2 vs the v5.3 in `master`.

I do have the `esp-13.2.0_20240305` but it is not found looking in `esp-13.2.0_20230928`:

```
-- Compiler supported targets: xtensa-esp-elf
CMake Error at C:/SysGCC/esp32/esp-idf/v5.2-gojimmypi/tools/cmake/tool_version_check.cmake:36 (message):
  
  Tool doesn't match supported version from list ['esp-13.2.0_20240305']:
  C:/SysGCC/esp32/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin/xtensa-esp32-elf-gcc.exe
```

As such, for now I've made some in-place edits in my ESP-IDF v5.2 and commented below.

Here are the steps to use:

### 1. Rename the local project `components` directory to `components.bak`

This will disable wolfSSL as a local project component.

### 2. Copy the wolfssl directory and contents to ESP-IDF components.

My ESP-IDF components are located in `C:\SysGCC\esp32\esp-idf\v5.2\components`

```
C:\SysGCC\esp32\esp-idf\v5.2\components\wolfssl>dir /s

04/17/2024  09:16 AM            36,129 CMakeLists.txt
04/15/2024  05:09 PM            11,320 component.mk
04/17/2024  11:01 AM    <DIR>          include
04/16/2024  02:43 PM             1,048 Kconfig


 Directory of C:\SysGCC\esp32\esp-idf\v5.2\components\wolfssl\include

04/01/2024  10:01 AM               859 config.h
04/17/2024  11:01 AM            20,940 user_settings.h
```


### 3. Edit the wolfssl CMakeLists.txt in the new directory

My file is in `C:\SysGCC\esp32\esp-idf\v5.2\components\wolfssl`

On or about line 37 should be a comment to manually set the source for wolfSSL.

My wolfSSL source is my clone located in `C:\workspace\wolfssl-gojimmypi-pr`

```
# Optionally set your source to wolfSSL in your project CMakeLists.txt like this:
set(WOLFSSL_ROOT "C:/workspace/wolfssl-gojimmypi-pr" )
```

### 4. Edit esp-tls component CMakeLists.txt

Mine is in `C:\SysGCC\esp32\esp-idf\v5.2\components\esp-tls`

On or about line 26 should be a `if(CONFIG_ESP_TLS_USING_WOLFSSL)`.

Replace the `esp-wolfssl` with `wolfssl`. The old line with `esp-wolfssl` is commented out:

```
if(CONFIG_ESP_TLS_USING_WOLFSSL)
#   idf_component_get_property(wolfssl esp-wolfssl COMPONENT_LIB)
    idf_component_get_property(wolfssl wolfssl COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PUBLIC ${wolfssl})
endif()
```

### 5. Edit esp_tls_wolfssl.c

Mine is in `C:\SysGCC\esp32\esp-idf\v5.2\components\esp-tls`

Add these lines at the beginning of the file. I added mine after the netdb include.

```
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#define OPENSSL_EXTRA
#include <wolfssl/openssl/x509.h>
```

### 6. Edit esp_tls_crypto.c

Mine is in `C:\SysGCC\esp32\esp-idf\v5.2\components\esp-tls\esp-tls-crypto`

Include the wolfssl includes after the `#elif  CONFIG_ESP_TLS_USING_WOLFSSL`

```
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    #include "mbedtls/sha1.h"
    #include "mbedtls/base64.h"
    #define _esp_crypto_sha1 esp_crypto_sha1_mbedtls
    #define _esp_crypto_base64_encode esp_crypto_bas64_encode_mbedtls
#elif CONFIG_ESP_TLS_USING_WOLFSSL
    #define OPENSSL_EXTRA
    #include "wolfssl/wolfcrypt/settings.h"
    #include "wolfssl/ssl.h" /* SHA functions are listed in wolfssl/ssl.h */
    #include "wolfssl/openssl/sha.h" /* old SHA functions only available with OpenSSL */
    #include "wolfssl/wolfcrypt/coding.h"
    #define _esp_crypto_sha1 esp_crypto_sha1_wolfSSL
    #define _esp_crypto_base64_encode esp_crypto_base64_encode_woflSSL
#endif
```

### 7. Special notes

It appears that the `wolfSSL_SHA1` used in `components/esp-tls/esp-tls-crypto/esp_tls_crypto.c:47:26` has been deprecated and is now only defined when OpenSSL compatibility is enabled. (e.g. #define OPENSSL_EXTRA)

The `#include <wolfssl/wolfcrypt/settings.h>` should be included everywhere wolfSSL is used and before any other wolfSSL include. Never include `user_settings.h` directly.

It appears there may be some oddity with the wolfSSL `settings.h` file when referenced from some of the ESP-IDF components as you may have noticed that I have an explicit #define for `OPENSSL_EXTRA`.

This is not a websockets demo yet, just something that compiles with the Kconfig file to enable wolfSSL in the esp-tls layer.

Please let me know if this works as desired.

Cheers
