This is a supplementary suggestion to the [wolfssl forum question](https://www.wolfssl.com/forums/post7407.html)
regarding PlatformIO and the wolfSSL library.

Note that the https://registry.platformio.org/libraries/onelife/wolfssl is *NOT* an official wolfSSL source
and is not maintained by wolfSSL staff.

Still, given that it may be included in a project, here are some tips to get it working:

Given a VS Code `[project]` directory, these changes are needed:

## Edit `[project]\.pio\libdeps\esp32dev\wolfssl\src\wolfcrypt\src`

delete all the `*.i` files

delete these files:

- `sp_arm32.c`
- `sp_arm64.c`
- `sp_armthumb.c`
- `sp_c32.c`
- `sp_c64.c`
- `sp_cortexm.c`
- `sp_dsp32.c`
- `sp_x86_64.c`
- `sp_cortexm.c`

(do NOT delete `sp_int.c`)

## Edit directory: `[project]\.pio\libdeps\esp32dev\wolfssl\src\wolfcrypt\src\port`

Delete all of the directories EXCEPT `Atmel` and `Espressif`


## Edit directory:`[project]\.pio\libdeps\esp32dev\wolfssl\src\wolfcrypt\`

Delete `test` and `benchmark` directories


## Edit file: `[project]\.pio\libdeps\esp32dev\wolfssl\src\user_settings.h`

See the enclosed [user_settings.h](./user_settings.h) - copy it to:

`[project]\.pio\libdeps\esp32dev\wolfssl\src\user_settings.h`


## Edit file: `[project]\.pio\libdeps\esp32dev\wolfssl\src\wolfssl\wolfcrypt\wolf_crypt_settings.h`

Comment out the `#define FREERTOS` in the `#if defined(WOLFSSL_ESPIDF)` section, on or around line 333.

```c
#if defined(WOLFSSL_ESPIDF)
    /* #define FREERTOS */
```

