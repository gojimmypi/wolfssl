:: copy updated files from ESP-IDF components directory used during build, back to this local repo for checking to GitHub
echo copy disabled!
goto done

copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\sha512.c                         C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\sha512.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\sha256.c                         C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\sha256.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\sha.c                            C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\sha.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\ed25519.c                        C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\ed25519.c

copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\cmac.c                           C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\cmac.c

xcopy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\*.*                             C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\ /d /y

copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\port\Espressif\esp32_util.c      C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\port\Espressif\esp32_util.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\port\Espressif\esp32_sha.c       C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\port\Espressif\esp32_sha.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\port\Espressif\esp32_mp.c        C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\port\Espressif\esp32_mp.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\port\Espressif\esp32_aes.c       C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\port\Espressif\esp32_aes.c
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfssl\wolfcrypt\port\Espressif\esp32-crypt.h C:\workspace\wolfssl-gojimmypi\wolfssl\wolfcrypt\port\Espressif\esp32-crypt.h
copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\src\port\Espressif\esp32_aes.c       C:\workspace\wolfssl-gojimmypi\wolfcrypt\src\port\Espressif\esp32_aes.c

copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfcrypt\test\test.c                          C:\workspace\wolfssl-gojimmypi\wolfcrypt\test\test.c

copy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfssl\wolfcrypt\ed25519.h                    C:\workspace\wolfssl-gojimmypi\wolfssl\wolfcrypt\ed25519.h

xcopy C:\SysGCC\esp32-8.4\esp-idf\v4.4.1\components\wolfssl\wolfssl\wolfcrypt\*.*                         C:\workspace\wolfssl-gojimmypi\wolfssl\wolfcrypt\ /d /y

:done