# ESP32-C3 Benchmark: 160MHz FASTMATH 

```
I (355) wolf ESP-ROM:esp32c3-api1-20210207
Build:Feb  7 2021
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fcd5820,len:0x1738
load:0x403cc710,len:0xae8
load:0x403ce710,len:0x2e70
entry 0x403cc71a
I (30) boot: ESP-IDF v5.2-dev-1805-g9a1cc59338 2nd stage bootloader
I (30) boot: compile time Jul 26 2023 13:15:26
I (31) boot: chip revision: v0.4
I (35) boot.esp32c3: SPI Speed      : 80MHz
I (40) boot.esp32c3: SPI Mode       : DIO
I (44) boot.esp32c3: SPI Flash Size : 2MB
I (49) boot: Enabling RNG early entropy source...
I (54) boot: Partition Table:
I (58) boot: ## Label            Usage          Type ST Offset   Length
I (65) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (73) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (80) boot:  2 factory          factory app      00 00 00010000 00177000
I (88) boot: End of partition table
I (92) esp_image: segment 0: paddr=00010020 vaddr=3c040020 size=15ca8h ( 89256) map
I (115) esp_image: segment 1: paddr=00025cd0 vaddr=3fc8a400 size=010e0h (  4320) load
I (116) esp_image: segment 2: paddr=00026db8 vaddr=40380000 size=09260h ( 37472) load
I (128) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=3f878h (260216) map
I (170) esp_image: segment 4: paddr=0006f8a0 vaddr=40389260 size=0111ch (  4380) load
I (175) boot: Loaded app from partition at offset 0x10000
I (175) boot: Disabling RNG early entropy source...
I (190) cpu_start: Unicore app
I (191) cpu_start: Pro cpu up.
I (199) cpu_start: Pro cpu start user code
I (199) cpu_start: cpu freq: 160000000 Hz
I (199) cpu_start: Application information:
I (202) cpu_start: Project name:     wolfssl_benchmark
I (208) cpu_start: App version:      v5.6.3-stable-1171-ga2dfecda1-d
I (215) cpu_start: Compile time:     Jul 26 2023 13:14:56
I (221) cpu_start: ELF file SHA256:  fadaf1585...
I (226) cpu_start: ESP-IDF:          v5.2-dev-1805-g9a1cc59338
I (233) cpu_start: Min chip rev:     v0.3
I (237) cpu_start: Max chip rev:     v0.99
I (242) cpu_start: Chip rev:         v0.4
I (247) heap_init: Initializing. RAM available for dynamic allocation:
I (254) heap_init: At 3FC8C420 len 000502F0 (320 KiB): DRAM
I (260) heap_init: At 3FCDC710 len 00002950 (10 KiB): STACK/DRAM
I (267) heap_init: At 50000010 len 00001FD8 (7 KiB): RTCRAM
I (274) spi_flash: detected chip: generic
I (278) spi_flash: flash io: dio
W (282) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (295) sleep: Configure to isolate all GPIO pins in sleep state
I (302) sleep: Enable automatic switching of GPIO sleep configuration
I (309) app_start: Starting scheduler on CPU0
I (314) main_task: Started on CPU0
I (318) main_task: Calling app_main()
I (322) wolfssl_benchmark: --------------------------------------------------------
I (330) wolfssl_benchmark: --------------------------------------------------------
I (339) wolfssl_benchmark: ---------------------- BEGIN MAIN ----------------------
I (347) wolfssl_benchmark: --------------------------------------------------------
I (355) wolfssl_benchmark: --------------------------------------------------------
I (364) esp32_util: SSID and plain text WiFi password not displayed in startup logs.Define SHOW_SSID_AND_PASSWORD to enable.
I (375) esp32_util: Extended Version and Platform Information.
I (382) esp32_util: LIBWOLFSSL_VERSION_STRING = 5.6.3
I (388) esp32_util: LIBWOLFSSL_VERSION_HEX = 5006003
I (393) esp32_util: RSA_LOW_MEM is enabled
I (398) esp32_util: Using wolfSSL user_settings.h in tbd
I (404) esp32_util: Stack HWM: 54372

I (409) esp32_util:
I (411) esp32_util:
I (414) esp32_util: LIBWOLFSSL_VERSION_GIT_ORIGIN = https://github.com/gojimmypi/wolfssl.git
I (424) esp32_util: LIBWOLFSSL_VERSION_GIT_BRANCH = ED25519_SHA2_fix
I (431) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH = a2dfecda1ee3ea5dcf6413b5e355103ea07c495f
I (439) esp32_util: LIBWOLFSSL_VERSION_GIT_SHORT_HASH = a2dfecda1
I (446) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH_DATE = 'Wed Jul 26 09:38:54 2023 -0700'
I (455) esp32_util: CONFIG_IDF_TARGET = esp32c3
I (460) esp32_util: NO_ESP32_CRYPT defined! HW acceleration DISABLED.
I (467) esp32_util: SINGLE_THREADED
I (471) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (480) wolfssl_benchmark: Stack HWM: 54372

I (483) wolfssl_benchmark: USE_FAST_MATH
I (488) wolfssl_benchmark: construct_argv arg:-lng 0

I (493) wolfssl_benchmark: Enable ESP32-C3 timer
------------------------------------------------------------------------------
 wolfSSL version 5.6.3
------------------------------------------------------------------------------
Math:   Multi-Precision: Fast max-bits=4096 tfm.c
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        725 KiB took 1.026 seconds,  706.628 KiB/s Cycles per byte =  55.24
AES-128-CBC-enc             50 KiB took 1.082 seconds,   46.211 KiB/s Cycles per byte = 845.33
AES-128-CBC-dec             50 KiB took 1.089 seconds,   45.914 KiB/s Cycles per byte = 850.03
AES-192-CBC-enc             50 KiB took 1.297 seconds,   38.551 KiB/s Cycles per byte = 1013.13
AES-192-CBC-dec             50 KiB took 1.303 seconds,   38.373 KiB/s Cycles per byte = 1017.77
AES-256-CBC-enc             50 KiB took 1.511 seconds,   33.091 KiB/s Cycles per byte = 1180.73
AES-256-CBC-dec             50 KiB took 1.517 seconds,   32.960 KiB/s Cycles per byte = 1185.42
AES-128-GCM-enc             50 KiB took 1.187 seconds,   42.123 KiB/s Cycles per byte = 927.09
AES-128-GCM-dec             50 KiB took 1.186 seconds,   42.159 KiB/s Cycles per byte = 927.12
AES-192-GCM-enc             50 KiB took 1.405 seconds,   35.587 KiB/s Cycles per byte = 1097.49
AES-192-GCM-dec             50 KiB took 1.405 seconds,   35.587 KiB/s Cycles per byte = 1097.53
AES-256-GCM-enc             50 KiB took 1.623 seconds,   30.807 KiB/s Cycles per byte = 1267.70
AES-256-GCM-dec             50 KiB took 1.623 seconds,   30.807 KiB/s Cycles per byte = 1267.73
GMAC Default               478 KiB took 1.000 seconds,  478.000 KiB/s Cycles per byte =  81.71
3DES                       375 KiB took 1.003 seconds,  373.878 KiB/s Cycles per byte = 104.48
MD5                      10650 KiB took 1.001 seconds, 10639.361 KiB/s Cycles per byte =   3.67
SHA                       5425 KiB took 1.004 seconds, 5403.386 KiB/s Cycles per byte =   7.23
SHA-224                   1700 KiB took 1.013 seconds, 1678.184 KiB/s Cycles per byte =  23.27
SHA-256                   1700 KiB took 1.013 seconds, 1678.184 KiB/s Cycles per byte =  23.27
SHA-384                   1250 KiB took 1.002 seconds, 1247.505 KiB/s Cycles per byte =  31.33
SHA-512                   1250 KiB took 1.003 seconds, 1246.261 KiB/s Cycles per byte =  31.32
SHA-512/224               1250 KiB took 1.002 seconds, 1247.505 KiB/s Cycles per byte =  31.33
SHA-512/256               1250 KiB took 1.003 seconds, 1246.261 KiB/s Cycles per byte =  31.33
SHA3-224                   925 KiB took 1.007 seconds,  918.570 KiB/s Cycles per byte =  42.54
SHA3-256                   875 KiB took 1.010 seconds,  866.337 KiB/s Cycles per byte =  45.10
SHA3-384                   675 KiB took 1.012 seconds,  666.996 KiB/s Cycles per byte =  58.54
SHA3-512                   475 KiB took 1.019 seconds,  466.143 KiB/s Cycles per byte =  83.86
SHAKE128                  1075 KiB took 1.012 seconds, 1062.253 KiB/s Cycles per byte =  36.76
SHAKE256                   875 KiB took 1.010 seconds,  866.337 KiB/s Cycles per byte =  45.10
RIPEMD                    4325 KiB took 1.004 seconds, 4307.769 KiB/s Cycles per byte =   9.07
HMAC-MD5                 10525 KiB took 1.000 seconds, 10525.000 KiB/s Cycles per byte =   3.71
HMAC-SHA                  5375 KiB took 1.004 seconds, 5353.586 KiB/s Cycles per byte =   7.29
HMAC-SHA224               1675 KiB took 1.006 seconds, 1665.010 KiB/s Cycles per byte =  23.46
HMAC-SHA256               1675 KiB took 1.006 seconds, 1665.010 KiB/s Cycles per byte =  23.46
HMAC-SHA384               1250 KiB took 1.018 seconds, 1227.898 KiB/s Cycles per byte =  31.81
HMAC-SHA512               1250 KiB took 1.018 seconds, 1227.898 KiB/s Cycles per byte =  31.81
PBKDF2                       0 KiB took 1.088 seconds,    0.201 KiB/s Cycles per byte = 194354.12
RSA     2048   public        46 ops took 1.041 sec, avg 22.630 ms, 44.188 ops/sec
RSA     2048  private         2 ops took 9.073 sec, avg 4536.500 ms, 0.220 ops/sec
ECC   [      SECP256R1]   256  key gen         4 ops took 1.205 sec, avg 301.250 ms, 3.320 ops/sec
ECDHE [      SECP256R1]   256    agree         4 ops took 1.209 sec, avg 302.250 ms, 3.309 ops/sec
ECDSA [      SECP256R1]   256     sign         4 ops took 1.217 sec, avg 304.250 ms, 3.287 ops/sec
ECDSA [      SECP256R1]   256   verify         2 ops took 1.157 sec, avg 578.500 ms, 1.729 ops/sec
CURVE  25519  key gen         3 ops took 1.136 sec, avg 378.667 ms, 2.641 ops/sec
CURVE  25519    agree         4 ops took 1.513 sec, avg 378.250 ms, 2.644 ops/sec
ED     25519  key gen        73 ops took 1.003 sec, avg 13.740 ms, 72.782 ops/sec
ED     25519     sign        62 ops took 1.008 sec, avg 16.258 ms, 61.508 ops/sec
ED     25519   verify        40 ops took 1.038 sec, avg 25.950 ms, 38.536 ops/sec
Benchmark complete
I (64073) wolfssl_benchmark: Stack used: 1616
```
