# ESP32-C6 Benchmark: 160MHz FASTMATH 

```
_start: Max chip rev:     v0.99
IESP-ROM:esp32c6-20220919
Build:Sep 19 2022
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:2
load:0x40875730,len:0x1804
load:0x4086c410,len:0xd14
load:0x4086e610,len:0x2e8c
entry 0x4086c41a
I (23) boot: ESP-IDF v5.2-dev-1805-g9a1cc59338 2nd stage bootloader
I (24) boot: compile time Jul 26 2023 13:39:17
I (24) boot: chip revision: v0.0
I (28) boot.esp32c6: SPI Speed      : 80MHz
I (33) boot.esp32c6: SPI Mode       : DIO
I (38) boot.esp32c6: SPI Flash Size : 2MB
I (42) boot: Enabling RNG early entropy source...
I (48) boot: Partition Table:
I (51) boot: ## Label            Usage          Type ST Offset   Length
I (59) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (66) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (74) boot:  2 factory          factory app      00 00 00010000 00177000
I (81) boot: End of partition table
I (85) esp_image: segment 0: paddr=00010020 vaddr=42038020 size=14a20h ( 84512) map
I (111) esp_image: segment 1: paddr=00024a48 vaddr=40800000 size=035d0h ( 13776) load
I (115) esp_image: segment 2: paddr=00028020 vaddr=42000020 size=34728h (214824) map
I (160) esp_image: segment 3: paddr=0005c750 vaddr=408035d0 size=05e9ch ( 24220) load
I (167) esp_image: segment 4: paddr=000625f4 vaddr=40809470 size=00f94h (  3988) load
I (171) boot: Loaded app from partition at offset 0x10000
I (172) boot: Disabling RNG early entropy source...
I (189) cpu_start: Unicore app
I (189) cpu_start: Pro cpu up.
W (200) clk: esp_perip_clk_init() has not been implemented yet
I (207) cpu_start: Pro cpu start user code
I (207) cpu_start: cpu freq: 160000000 Hz
I (207) cpu_start: Application information:
I (210) cpu_start: Project name:     wolfssl_benchmark
I (216) cpu_start: App version:      v5.6.3-stable-1171-ga2dfecda1-d
I (223) cpu_start: Compile time:     Jul 26 2023 13:38:40
I (229) cpu_start: ELF file SHA256:  3388207ef...
I (234) cpu_start: ESP-IDF:          v5.2-dev-1805-g9a1cc59338
I (241) cpu_start: Min chip rev:     v0.0
I (246) cpu_start: Max chip rev:     v0.99
I (250) cpu_start: Chip rev:         v0.0
I (255) heap_init: Initializing. RAM available for dynamic allocation:
I (262) heap_init: At 4080B360 len 000712B0 (452 KiB): D/IRAM
I (269) heap_init: At 4087C610 len 00002F54 (11 KiB): STACK/DIRAM
I (276) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
I (283) spi_flash: detected chip: generic
I (287) spi_flash: flash io: dio
W (290) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (304) sleep: Configure to isolate all GPIO pins in sleep state
I (310) sleep: Enable automatic switching of GPIO sleep configuration
I (318) app_start: Starting scheduler on CPU0
I (322) main_task: Started on CPU0
I (326) main_task: Calling app_main()
I (330) wolfssl_benchmark: --------------------------------------------------------
I (338) wolfssl_benchmark: --------------------------------------------------------
I (347) wolfssl_benchmark: ---------------------- BEGIN MAIN ----------------------
I (355) wolfssl_benchmark: --------------------------------------------------------
I (363) wolfssl_benchmark: --------------------------------------------------------
I (372) esp32_util: SSID and plain text WiFi password not displayed in startup logs.Define SHOW_SSID_AND_PASSWORD to enable.
I (383) esp32_util: Extended Version and Platform Information.
I (390) esp32_util: LIBWOLFSSL_VERSION_STRING = 5.6.3
I (396) esp32_util: LIBWOLFSSL_VERSION_HEX = 5006003
I (401) esp32_util: RSA_LOW_MEM is enabled
I (406) esp32_util: Using wolfSSL user_settings.h in tbd
I (412) esp32_util: CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ = 160 MHz
I (419) esp32_util: Stack HWM: 54368

I (423) esp32_util:
I (426) esp32_util:
I (429) esp32_util: LIBWOLFSSL_VERSION_GIT_ORIGIN = https://github.com/gojimmypi/wolfssl.git
I (438) esp32_util: LIBWOLFSSL_VERSION_GIT_BRANCH = ED25519_SHA2_fix
I (445) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH = a2dfecda1ee3ea5dcf6413b5e355103ea07c495f
I (454) esp32_util: LIBWOLFSSL_VERSION_GIT_SHORT_HASH = a2dfecda1
I (461) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH_DATE = 'Wed Jul 26 09:38:54 2023 -0700'
I (469) esp32_util: CONFIG_IDF_TARGET = esp32c6
I (475) esp32_util: NO_ESP32_CRYPT defined! HW acceleration DISABLED.
I (482) esp32_util: SINGLE_THREADED
I (486) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (494) wolfssl_benchmark: Stack HWM: 54360

I (497) wolfssl_benchmark: USE_FAST_MATH
I (502) wolfssl_benchmark: construct_argv arg:-lng 0

I (508) wolfssl_benchmark: Enable ESP32-C6 timer
------------------------------------------------------------------------------
 wolfSSL version 5.6.3
------------------------------------------------------------------------------
Math:   Multi-Precision: Fast max-bits=4096 tfm.c
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        725 KiB took 1.010 seconds,  717.822 KiB/s Cycles per byte =  54.41
AES-128-CBC-enc             50 KiB took 1.036 seconds,   48.263 KiB/s Cycles per byte = 809.01
AES-128-CBC-dec             50 KiB took 1.042 seconds,   47.985 KiB/s Cycles per byte = 813.54
AES-192-CBC-enc             50 KiB took 1.241 seconds,   40.290 KiB/s Cycles per byte = 969.35
AES-192-CBC-dec             50 KiB took 1.247 seconds,   40.096 KiB/s Cycles per byte = 973.82
AES-256-CBC-enc             50 KiB took 1.446 seconds,   34.578 KiB/s Cycles per byte = 1129.70
AES-256-CBC-dec             50 KiB took 1.451 seconds,   34.459 KiB/s Cycles per byte = 1134.28
AES-128-GCM-enc             50 KiB took 1.134 seconds,   44.092 KiB/s Cycles per byte = 886.04
AES-128-GCM-dec             50 KiB took 1.134 seconds,   44.092 KiB/s Cycles per byte = 886.02
AES-192-GCM-enc             50 KiB took 1.343 seconds,   37.230 KiB/s Cycles per byte = 1048.98
AES-192-GCM-dec             50 KiB took 1.343 seconds,   37.230 KiB/s Cycles per byte = 1049.01
AES-256-GCM-enc             50 KiB took 1.551 seconds,   32.237 KiB/s Cycles per byte = 1211.88
AES-256-GCM-dec             50 KiB took 1.551 seconds,   32.237 KiB/s Cycles per byte = 1211.91
GMAC Default               507 KiB took 1.001 seconds,  506.494 KiB/s Cycles per byte =  77.17
3DES                       400 KiB took 1.060 seconds,  377.358 KiB/s Cycles per byte = 103.49
MD5                      10750 KiB took 1.000 seconds, 10750.000 KiB/s Cycles per byte =   3.63
SHA                       5525 KiB took 1.002 seconds, 5513.972 KiB/s Cycles per byte =   7.08
SHA-224                   1725 KiB took 1.014 seconds, 1701.183 KiB/s Cycles per byte =  22.96
SHA-256                   1725 KiB took 1.014 seconds, 1701.183 KiB/s Cycles per byte =  22.95
SHA-384                   1275 KiB took 1.005 seconds, 1268.657 KiB/s Cycles per byte =  30.77
SHA-512                   1275 KiB took 1.004 seconds, 1269.920 KiB/s Cycles per byte =  30.74
SHA-512/224               1275 KiB took 1.003 seconds, 1271.186 KiB/s Cycles per byte =  30.74
SHA-512/256               1275 KiB took 1.003 seconds, 1271.186 KiB/s Cycles per byte =  30.74
SHA3-224                   925 KiB took 1.006 seconds,  919.483 KiB/s Cycles per byte =  42.47
SHA3-256                   875 KiB took 1.008 seconds,  868.056 KiB/s Cycles per byte =  45.02
SHA3-384                   675 KiB took 1.010 seconds,  668.317 KiB/s Cycles per byte =  58.48
SHA3-512                   475 KiB took 1.019 seconds,  466.143 KiB/s Cycles per byte =  83.82
SHAKE128                  1075 KiB took 1.009 seconds, 1065.411 KiB/s Cycles per byte =  36.66
SHAKE256                   875 KiB took 1.008 seconds,  868.056 KiB/s Cycles per byte =  45.02
RIPEMD                    4325 KiB took 1.000 seconds, 4325.000 KiB/s Cycles per byte =   9.04
HMAC-MD5                 10650 KiB took 1.001 seconds, 10639.361 KiB/s Cycles per byte =   3.67
HMAC-SHA                  5475 KiB took 1.002 seconds, 5464.072 KiB/s Cycles per byte =   7.15
HMAC-SHA224               1700 KiB took 1.007 seconds, 1688.183 KiB/s Cycles per byte =  23.14
HMAC-SHA256               1700 KiB took 1.007 seconds, 1688.183 KiB/s Cycles per byte =  23.14
HMAC-SHA384               1250 KiB took 1.000 seconds, 1250.000 KiB/s Cycles per byte =  31.24
HMAC-SHA512               1275 KiB took 1.019 seconds, 1251.227 KiB/s Cycles per byte =  31.21
PBKDF2                       0 KiB took 1.074 seconds,    0.204 KiB/s Cycles per byte = 191768.42
RSA     2048   public        56 ops took 1.026 sec, avg 18.321 ms, 54.581 ops/sec
RSA     2048  private         2 ops took 7.226 sec, avg 3613.000 ms, 0.277 ops/sec
ECC   [      SECP256R1]   256  key gen         4 ops took 1.237 sec, avg 309.250 ms, 3.234 ops/sec
ECDHE [      SECP256R1]   256    agree         4 ops took 1.239 sec, avg 309.750 ms, 3.228 ops/sec
ECDSA [      SECP256R1]   256     sign         4 ops took 1.247 sec, avg 311.750 ms, 3.208 ops/sec
ECDSA [      SECP256R1]   256   verify         2 ops took 1.189 sec, avg 594.500 ms, 1.682 ops/sec
CURVE  25519  key gen         4 ops took 1.277 sec, avg 319.250 ms, 3.132 ops/sec
CURVE  25519    agree         4 ops took 1.276 sec, avg 319.000 ms, 3.135 ops/sec
ED     25519  key gen        87 ops took 1.001 sec, avg 11.506 ms, 86.913 ops/sec
ED     25519     sign        76 ops took 1.008 sec, avg 13.263 ms, 75.397 ops/sec
ED     25519   verify        52 ops took 1.016 sec, avg 19.538 ms, 51.181 ops/sec
Benchmark complete
I (61388) wolfssl_benchmark: Stack used: 1536
```
