# ESP32-C3 Benchmark: 160MHz WOLFSSL_SP_MATH_ALL + WOLFSSL_SP_RISCV32 

```
ESP-ROM:esp32c3-api1-20210207
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
I (92) esp_image: segment 0: paddr=00010020 vaddr=3c050020 size=15c98h ( 89240) map
I (115) esp_image: segment 1: paddr=00025cc0 vaddr=3fc8a400 size=010e0h (  4320) load
I (116) esp_image: segment 2: paddr=00026da8 vaddr=40380000 size=09270h ( 37488) load
I (128) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=42a58h (272984) map
I (172) esp_image: segment 4: paddr=00072a80 vaddr=40389270 size=0110ch (  4364) load
I (177) boot: Loaded app from partition at offset 0x10000
I (177) boot: Disabling RNG early entropy source...
I (192) cpu_start: Unicore app
I (193) cpu_start: Pro cpu up.
I (201) cpu_start: Pro cpu start user code
I (201) cpu_start: cpu freq: 160000000 Hz
I (201) cpu_start: Application information:
I (204) cpu_start: Project name:     wolfssl_benchmark
I (210) cpu_start: App version:      v5.6.3-stable-1171-ga2dfecda1-d
I (217) cpu_start: Compile time:     Jul 26 2023 13:14:56
I (223) cpu_start: ELF file SHA256:  70a158d49...
I (228) cpu_start: ESP-IDF:          v5.2-dev-1805-g9a1cc59338
I (235) cpu_start: Min chip rev:     v0.3
I (240) cpu_start: Max chip rev:     v0.99
I (244) cpu_start: Chip rev:         v0.4
I (249) heap_init: Initializing. RAM available for dynamic allocation:
I (256) heap_init: At 3FC8C420 len 000502F0 (320 KiB): DRAM
I (263) heap_init: At 3FCDC710 len 00002950 (10 KiB): STACK/DRAM
I (269) heap_init: At 50000010 len 00001FD8 (7 KiB): RTCRAM
I (276) spi_flash: detected chip: generic
I (280) spi_flash: flash io: dio
W (284) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (297) sleep: Configure to isolate all GPIO pins in sleep state
I (304) sleep: Enable automatic switching of GPIO sleep configuration
I (311) app_start: Starting scheduler on CPU0
I (316) main_task: Started on CPU0
I (320) main_task: Calling app_main()
I (324) wolfssl_benchmark: --------------------------------------------------------
I (332) wolfssl_benchmark: --------------------------------------------------------
I (341) wolfssl_benchmark: ---------------------- BEGIN MAIN ----------------------
I (349) wolfssl_benchmark: --------------------------------------------------------
I (357) wolfssl_benchmark: --------------------------------------------------------
I (366) esp32_util: SSID and plain text WiFi password not displayed in startup logs.Define SHOW_SSID_AND_PASSWORD to enable.
I (377) esp32_util: Extended Version and Platform Information.
I (384) esp32_util: LIBWOLFSSL_VERSION_STRING = 5.6.3
I (390) esp32_util: LIBWOLFSSL_VERSION_HEX = 5006003
I (395) esp32_util: RSA_LOW_MEM is enabled
I (400) esp32_util: Using wolfSSL user_settings.h in tbd
I (406) esp32_util: Stack HWM: 54372

I (411) esp32_util:
I (413) esp32_util:
I (416) esp32_util: LIBWOLFSSL_VERSION_GIT_ORIGIN = https://github.com/gojimmypi/wolfssl.git
I (426) esp32_util: LIBWOLFSSL_VERSION_GIT_BRANCH = ED25519_SHA2_fix
I (433) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH = a2dfecda1ee3ea5dcf6413b5e355103ea07c495f
I (441) esp32_util: LIBWOLFSSL_VERSION_GIT_SHORT_HASH = a2dfecda1
I (448) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH_DATE = 'Wed Jul 26 09:38:54 2023 -0700'
I (457) esp32_util: CONFIG_IDF_TARGET = esp32c3
I (462) esp32_util: NO_ESP32_CRYPT defined! HW acceleration DISABLED.
I (469) esp32_util: SINGLE_THREADED
I (473) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (482) wolfssl_benchmark: Stack HWM: 54372

I (485) wolfssl_benchmark: WOLFSSL_SP_MATH_ALL + WOLFSSL_SP_RISCV32
I (492) wolfssl_benchmark: construct_argv arg:-lng 0

I (498) wolfssl_benchmark: Enable ESP32-C3 timer
------------------------------------------------------------------------------
 wolfSSL version 5.6.3
------------------------------------------------------------------------------
Math:   Multi-Precision: Wolf(SP) word-size=32 bits=3072 sp_int.c
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        725 KiB took 1.027 seconds,  705.940 KiB/s Cycles per byte =  55.33
AES-128-CBC-enc             50 KiB took 1.082 seconds,   46.211 KiB/s Cycles per byte = 845.33
AES-128-CBC-dec             50 KiB took 1.088 seconds,   45.956 KiB/s Cycles per byte = 850.03
AES-192-CBC-enc             50 KiB took 1.297 seconds,   38.551 KiB/s Cycles per byte = 1013.13
AES-192-CBC-dec             50 KiB took 1.303 seconds,   38.373 KiB/s Cycles per byte = 1017.77
AES-256-CBC-enc             50 KiB took 1.511 seconds,   33.091 KiB/s Cycles per byte = 1180.73
AES-256-CBC-dec             50 KiB took 1.517 seconds,   32.960 KiB/s Cycles per byte = 1185.42
AES-128-GCM-enc             50 KiB took 1.187 seconds,   42.123 KiB/s Cycles per byte = 927.09
AES-128-GCM-dec             50 KiB took 1.186 seconds,   42.159 KiB/s Cycles per byte = 927.11
AES-192-GCM-enc             50 KiB took 1.405 seconds,   35.587 KiB/s Cycles per byte = 1097.49
AES-192-GCM-dec             50 KiB took 1.405 seconds,   35.587 KiB/s Cycles per byte = 1097.53
AES-256-GCM-enc             50 KiB took 1.623 seconds,   30.807 KiB/s Cycles per byte = 1267.69
AES-256-GCM-dec             50 KiB took 1.623 seconds,   30.807 KiB/s Cycles per byte = 1267.73
GMAC Default               478 KiB took 1.000 seconds,  478.000 KiB/s Cycles per byte =  81.71
3DES                       375 KiB took 1.003 seconds,  373.878 KiB/s Cycles per byte = 104.48
MD5                      10625 KiB took 1.000 seconds, 10625.000 KiB/s Cycles per byte =   3.67
SHA                       5400 KiB took 1.000 seconds, 5400.000 KiB/s Cycles per byte =   7.23
SHA-224                   1700 KiB took 1.014 seconds, 1676.529 KiB/s Cycles per byte =  23.30
SHA-256                   1700 KiB took 1.014 seconds, 1676.529 KiB/s Cycles per byte =  23.30
SHA-384                   1275 KiB took 1.009 seconds, 1263.627 KiB/s Cycles per byte =  30.91
SHA-512                   1275 KiB took 1.009 seconds, 1263.627 KiB/s Cycles per byte =  30.91
SHA-512/224               1275 KiB took 1.009 seconds, 1263.627 KiB/s Cycles per byte =  30.91
SHA-512/256               1275 KiB took 1.009 seconds, 1263.627 KiB/s Cycles per byte =  30.91
SHA3-224                   925 KiB took 1.006 seconds,  919.483 KiB/s Cycles per byte =  42.47
SHA3-256                   875 KiB took 1.008 seconds,  868.056 KiB/s Cycles per byte =  45.02
SHA3-384                   675 KiB took 1.010 seconds,  668.317 KiB/s Cycles per byte =  58.47
SHA3-512                   475 KiB took 1.019 seconds,  466.143 KiB/s Cycles per byte =  83.77
SHAKE128                  1075 KiB took 1.009 seconds, 1065.411 KiB/s Cycles per byte =  36.68
SHAKE256                   875 KiB took 1.008 seconds,  868.056 KiB/s Cycles per byte =  45.02
RIPEMD                    4325 KiB took 1.005 seconds, 4303.483 KiB/s Cycles per byte =   9.07
HMAC-MD5                 10525 KiB took 1.000 seconds, 10525.000 KiB/s Cycles per byte =   3.71
HMAC-SHA                  5375 KiB took 1.004 seconds, 5353.586 KiB/s Cycles per byte =   7.30
HMAC-SHA224               1675 KiB took 1.007 seconds, 1663.357 KiB/s Cycles per byte =  23.48
HMAC-SHA256               1675 KiB took 1.006 seconds, 1665.010 KiB/s Cycles per byte =  23.48
HMAC-SHA384               1250 KiB took 1.004 seconds, 1245.020 KiB/s Cycles per byte =  31.38
HMAC-SHA512               1250 KiB took 1.004 seconds, 1245.020 KiB/s Cycles per byte =  31.38
PBKDF2                       0 KiB took 1.092 seconds,    0.200 KiB/s Cycles per byte = 194936.64
RSA     2048   public        48 ops took 1.001 sec, avg 20.854 ms, 47.952 ops/sec
RSA     2048  private         2 ops took 9.438 sec, avg 4719.000 ms, 0.212 ops/sec
ECC   [      SECP256R1]   256  key gen         8 ops took 1.159 sec, avg 144.875 ms, 6.903 ops/sec
ECDHE [      SECP256R1]   256    agree         8 ops took 1.154 sec, avg 144.250 ms, 6.932 ops/sec
ECDSA [      SECP256R1]   256     sign         8 ops took 1.176 sec, avg 147.000 ms, 6.803 ops/sec
ECDSA [      SECP256R1]   256   verify         4 ops took 1.119 sec, avg 279.750 ms, 3.575 ops/sec
CURVE  25519  key gen         3 ops took 1.136 sec, avg 378.667 ms, 2.641 ops/sec
CURVE  25519    agree         4 ops took 1.512 sec, avg 378.000 ms, 2.646 ops/sec
ED     25519  key gen        73 ops took 1.004 sec, avg 13.753 ms, 72.709 ops/sec
ED     25519     sign        62 ops took 1.007 sec, avg 16.242 ms, 61.569 ops/sec
ED     25519   verify        40 ops took 1.033 sec, avg 25.825 ms, 38.722 ops/sec
Benchmark complete
I (63896) wolfssl_benchmark: Stack used: 1580
```
