# ESP32-C3 Benchmark: 160MHz WOLFSSL_SP_MATH_ALL + WOLFSSL_SP_RISCV32 

```
I (253) cpu_start: Chip rev ESP-ROM:esp32c6-20220919
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
I (85) esp_image: segment 0: paddr=00010020 vaddr=42038020 size=14908h ( 84232) map
I (111) esp_image: segment 1: paddr=00024930 vaddr=40800000 size=036e8h ( 14056) load
I (115) esp_image: segment 2: paddr=00028020 vaddr=42000020 size=37908h (227592) map
I (163) esp_image: segment 3: paddr=0005f930 vaddr=408036e8 size=05d84h ( 23940) load
I (169) esp_image: segment 4: paddr=000656bc vaddr=40809470 size=00f94h (  3988) load
I (173) boot: Loaded app from partition at offset 0x10000
I (175) boot: Disabling RNG early entropy source...
I (191) cpu_start: Unicore app
I (192) cpu_start: Pro cpu up.
W (203) clk: esp_perip_clk_init() has not been implemented yet
I (209) cpu_start: Pro cpu start user code
I (210) cpu_start: cpu freq: 160000000 Hz
I (210) cpu_start: Application information:
I (213) cpu_start: Project name:     wolfssl_benchmark
I (218) cpu_start: App version:      v5.6.3-stable-1171-ga2dfecda1-d
I (225) cpu_start: Compile time:     Jul 26 2023 13:38:40
I (231) cpu_start: ELF file SHA256:  dd54c768e...
I (237) cpu_start: ESP-IDF:          v5.2-dev-1805-g9a1cc59338
I (243) cpu_start: Min chip rev:     v0.0
I (248) cpu_start: Max chip rev:     v0.99
I (253) cpu_start: Chip rev:         v0.0
I (258) heap_init: Initializing. RAM available for dynamic allocation:
I (265) heap_init: At 4080B360 len 000712B0 (452 KiB): D/IRAM
I (271) heap_init: At 4087C610 len 00002F54 (11 KiB): STACK/DIRAM
I (278) heap_init: At 50000000 len 00003FE8 (15 KiB): RTCRAM
I (285) spi_flash: detected chip: generic
I (289) spi_flash: flash io: dio
W (293) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (306) sleep: Configure to isolate all GPIO pins in sleep state
I (313) sleep: Enable automatic switching of GPIO sleep configuration
I (320) app_start: Starting scheduler on CPU0
I (325) main_task: Started on CPU0
I (329) main_task: Calling app_main()
I (333) wolfssl_benchmark: --------------------------------------------------------
I (341) wolfssl_benchmark: --------------------------------------------------------
I (350) wolfssl_benchmark: ---------------------- BEGIN MAIN ----------------------
I (358) wolfssl_benchmark: --------------------------------------------------------
I (366) wolfssl_benchmark: --------------------------------------------------------
I (375) esp32_util: SSID and plain text WiFi password not displayed in startup logs.Define SHOW_SSID_AND_PASSWORD to enable.
I (386) esp32_util: Extended Version and Platform Information.
I (393) esp32_util: LIBWOLFSSL_VERSION_STRING = 5.6.3
I (399) esp32_util: LIBWOLFSSL_VERSION_HEX = 5006003
I (404) esp32_util: RSA_LOW_MEM is enabled
I (409) esp32_util: Using wolfSSL user_settings.h in tbd
I (415) esp32_util: CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ = 160 MHz
I (422) esp32_util: Stack HWM: 54368

I (426) esp32_util:
I (429) esp32_util:
I (432) esp32_util: LIBWOLFSSL_VERSION_GIT_ORIGIN = https://github.com/gojimmypi/wolfssl.git
I (441) esp32_util: LIBWOLFSSL_VERSION_GIT_BRANCH = ED25519_SHA2_fix
I (448) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH = a2dfecda1ee3ea5dcf6413b5e355103ea07c495f
I (457) esp32_util: LIBWOLFSSL_VERSION_GIT_SHORT_HASH = a2dfecda1
I (464) esp32_util: LIBWOLFSSL_VERSION_GIT_HASH_DATE = 'Wed Jul 26 09:38:54 2023 -0700'
I (472) esp32_util: CONFIG_IDF_TARGET = esp32c6
I (478) esp32_util: NO_ESP32_CRYPT defined! HW acceleration DISABLED.
I (485) esp32_util: SINGLE_THREADED
I (489) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (497) wolfssl_benchmark: Stack HWM: 54360

I (500) wolfssl_benchmark: WOLFSSL_SP_MATH_ALL + WOLFSSL_SP_RISCV32
I (507) wolfssl_benchmark: construct_argv arg:-lng 0

I (513) wolfssl_benchmark: Enable ESP32-C6 timer
------------------------------------------------------------------------------
 wolfSSL version 5.6.3
------------------------------------------------------------------------------
Math:   Multi-Precision: Wolf(SP) word-size=32 bits=3072 sp_int.c
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        725 KiB took 1.011 seconds,  717.112 KiB/s Cycles per byte =  54.49
AES-128-CBC-enc             50 KiB took 1.036 seconds,   48.263 KiB/s Cycles per byte = 809.00
AES-128-CBC-dec             50 KiB took 1.042 seconds,   47.985 KiB/s Cycles per byte = 813.54
AES-192-CBC-enc             50 KiB took 1.241 seconds,   40.290 KiB/s Cycles per byte = 969.35
AES-192-CBC-dec             50 KiB took 1.247 seconds,   40.096 KiB/s Cycles per byte = 973.82
AES-256-CBC-enc             50 KiB took 1.446 seconds,   34.578 KiB/s Cycles per byte = 1129.71
AES-256-CBC-dec             50 KiB took 1.452 seconds,   34.435 KiB/s Cycles per byte = 1134.28
AES-128-GCM-enc             50 KiB took 1.135 seconds,   44.053 KiB/s Cycles per byte = 886.06
AES-128-GCM-dec             50 KiB took 1.135 seconds,   44.053 KiB/s Cycles per byte = 886.03
AES-192-GCM-enc             50 KiB took 1.343 seconds,   37.230 KiB/s Cycles per byte = 1048.98
AES-192-GCM-dec             50 KiB took 1.343 seconds,   37.230 KiB/s Cycles per byte = 1049.01
AES-256-GCM-enc             50 KiB took 1.551 seconds,   32.237 KiB/s Cycles per byte = 1211.88
AES-256-GCM-dec             50 KiB took 1.551 seconds,   32.237 KiB/s Cycles per byte = 1211.91
GMAC Default               506 KiB took 1.000 seconds,  506.000 KiB/s Cycles per byte =  77.17
3DES                       400 KiB took 1.059 seconds,  377.715 KiB/s Cycles per byte = 103.49
MD5                      10750 KiB took 1.000 seconds, 10750.000 KiB/s Cycles per byte =   3.63
SHA                       5525 KiB took 1.003 seconds, 5508.475 KiB/s Cycles per byte =   7.09
SHA-224                   1700 KiB took 1.001 seconds, 1698.302 KiB/s Cycles per byte =  23.00
SHA-256                   1700 KiB took 1.001 seconds, 1698.302 KiB/s Cycles per byte =  22.99
SHA-384                   1300 KiB took 1.011 seconds, 1285.856 KiB/s Cycles per byte =  30.35
SHA-512                   1300 KiB took 1.010 seconds, 1287.129 KiB/s Cycles per byte =  30.33
SHA-512/224               1300 KiB took 1.009 seconds, 1288.404 KiB/s Cycles per byte =  30.33
SHA-512/256               1300 KiB took 1.009 seconds, 1288.404 KiB/s Cycles per byte =  30.33
SHA3-224                   925 KiB took 1.004 seconds,  921.315 KiB/s Cycles per byte =  42.41
SHA3-256                   875 KiB took 1.007 seconds,  868.918 KiB/s Cycles per byte =  44.94
SHA3-384                   675 KiB took 1.009 seconds,  668.979 KiB/s Cycles per byte =  58.41
SHA3-512                   475 KiB took 1.019 seconds,  466.143 KiB/s Cycles per byte =  83.74
SHAKE128                  1075 KiB took 1.007 seconds, 1067.527 KiB/s Cycles per byte =  36.59
SHAKE256                   875 KiB took 1.007 seconds,  868.918 KiB/s Cycles per byte =  44.95
RIPEMD                    4325 KiB took 1.000 seconds, 4325.000 KiB/s Cycles per byte =   9.04
HMAC-MD5                 10650 KiB took 1.001 seconds, 10639.361 KiB/s Cycles per byte =   3.67
HMAC-SHA                  5475 KiB took 1.003 seconds, 5458.624 KiB/s Cycles per byte =   7.15
HMAC-SHA224               1700 KiB took 1.009 seconds, 1684.836 KiB/s Cycles per byte =  23.18
HMAC-SHA256               1700 KiB took 1.009 seconds, 1684.836 KiB/s Cycles per byte =  23.18
HMAC-SHA384               1275 KiB took 1.006 seconds, 1267.396 KiB/s Cycles per byte =  30.82
HMAC-SHA512               1275 KiB took 1.005 seconds, 1268.657 KiB/s Cycles per byte =  30.80
PBKDF2                       0 KiB took 1.076 seconds,    0.203 KiB/s Cycles per byte = 192085.19
RSA     2048   public        58 ops took 1.016 sec, avg 17.517 ms, 57.087 ops/sec
RSA     2048  private         2 ops took 7.834 sec, avg 3917.000 ms, 0.255 ops/sec
ECC   [      SECP256R1]   256  key gen         8 ops took 1.025 sec, avg 128.125 ms, 7.805 ops/sec
ECDHE [      SECP256R1]   256    agree         8 ops took 1.024 sec, avg 128.000 ms, 7.812 ops/sec
ECDSA [      SECP256R1]   256     sign         8 ops took 1.043 sec, avg 130.375 ms, 7.670 ops/sec
ECDSA [      SECP256R1]   256   verify         4 ops took 1.011 sec, avg 252.750 ms, 3.956 ops/sec
CURVE  25519  key gen         4 ops took 1.277 sec, avg 319.250 ms, 3.132 ops/sec
CURVE  25519    agree         4 ops took 1.276 sec, avg 319.000 ms, 3.135 ops/sec
ED     25519  key gen        88 ops took 1.009 sec, avg 11.466 ms, 87.215 ops/sec
ED     25519     sign        76 ops took 1.002 sec, avg 13.184 ms, 75.848 ops/sec
ED     25519   verify        52 ops took 1.015 sec, avg 19.519 ms, 51.232 ops/sec
Benchmark complete
I (60809) wolfssl_benchmark: Stack used: 1480
```
