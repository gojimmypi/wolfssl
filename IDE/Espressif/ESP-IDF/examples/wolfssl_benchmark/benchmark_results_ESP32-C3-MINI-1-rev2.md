ESP-ROM:esp32c3-20200918
Build:Sep 18 2020
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:2
load:0x3fcd6100,len:0x16c8
load:0x403ce000,len:0x930
load:0x403d0000,len:0x2d28
entry 0x403ce000
I (32) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (32) boot: compile time 08:23:35
I (32) boot: chip revision: 2
I (35) boot_comm: chip revision: 2, min. bootloader chip revision: 0
I (43) boot.esp32c3: SPI Speed      : 40MHz
I (47) boot.esp32c3: SPI Mode       : DIO
I (52) boot.esp32c3: SPI Flash Size : 2MB
I (57) boot: Enabling RNG early entropy source...
I (62) boot: Partition Table:
I (66) boot: ## Label            Usage          Type ST Offset   Length
I (73) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (81) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (88) boot:  2 factory          factory app      00 00 00010000 00100000
I (95) boot: End of partition table
I (100) boot_comm: chip revision: 2, min. application chip revision: 0
I (107) esp_image: segment 0: paddr=00010020 vaddr=3c040020 size=13f30h ( 81712) map
I (133) esp_image: segment 1: paddr=00023f58 vaddr=3fc8a400 size=0142ch (  5164) load
I (135) esp_image: segment 2: paddr=0002538c vaddr=40380000 size=0a2a0h ( 41632) load
I (149) esp_image: segment 3: paddr=0002f634 vaddr=50000010 size=00010h (    16) load
I (149) esp_image: segment 4: paddr=0002f64c vaddr=00000000 size=009cch (  2508)
I (156) esp_image: segment 5: paddr=00030020 vaddr=42000020 size=36760h (223072) map
I (215) boot: Loaded app from partition at offset 0x10000
I (215) boot: Disabling RNG early entropy source...
I (227) cpu_start: Pro cpu up.
I (236) cpu_start: Pro cpu start user code
I (236) cpu_start: cpu freq: 160000000
I (236) cpu_start: Application information:
I (239) cpu_start: Project name:     wolfssl_benchmark
I (244) cpu_start: App version:      v5.5.3-stable-111-g07ceb86ca-di
I (251) cpu_start: Compile time:     Nov 22 2022 08:23:29
I (257) cpu_start: ELF file SHA256:  f31d71891f425018...
I (263) cpu_start: ESP-IDF:          v4.4.2-1-g0aba20e63d-dirty
I (270) heap_init: Initializing. RAM available for dynamic allocation:
I (277) heap_init: At 3FC8D790 len 00032870 (202 KiB): DRAM
I (283) heap_init: At 3FCC0000 len 0001F260 (124 KiB): STACK/DRAM
I (290) heap_init: At 50000020 len 00001FE0 (7 KiB): RTCRAM
I (297) spi_flash: detected chip: generic
I (301) spi_flash: flash io: dio
W (305) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (319) cpu_start: Starting scheduler.
I (323) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (329) wolfssl_benchmark: construct_argv arg:-lng 0

------------------------------------------------------------------------------
 wolfSSL version 5.5.3
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        650 KiB took 1.006 seconds,  646.123 KiB/s
AES-128-CBC-enc              1 MiB took 1.007 seconds,    1.091 MiB/s
AES-128-CBC-dec              1 MiB took 1.001 seconds,    1.000 MiB/s
AES-192-CBC-enc              1 MiB took 1.020 seconds,    0.981 MiB/s
AES-192-CBC-dec            950 KiB took 1.022 seconds,  929.550 KiB/s
AES-256-CBC-enc            925 KiB took 1.010 seconds,  915.842 KiB/s
AES-256-CBC-dec            875 KiB took 1.026 seconds,  852.827 KiB/s
AES-128-GCM-enc            275 KiB took 1.060 seconds,  259.434 KiB/s
AES-128-GCM-dec            275 KiB took 1.060 seconds,  259.434 KiB/s
AES-192-GCM-enc            275 KiB took 1.087 seconds,  252.990 KiB/s
AES-192-GCM-dec            275 KiB took 1.087 seconds,  252.990 KiB/s
AES-256-GCM-enc            250 KiB took 1.013 seconds,  246.792 KiB/s
AES-256-GCM-dec            250 KiB took 1.013 seconds,  246.792 KiB/s
GMAC Default               341 KiB took 1.001 seconds,  340.659 KiB/s
3DES                       325 KiB took 1.007 seconds,  322.741 KiB/s
MD5                         10 MiB took 1.001 seconds,   10.195 MiB/s
SHA                          5 MiB took 1.001 seconds,    4.854 MiB/s
SHA-256                      1 MiB took 1.000 seconds,    1.489 MiB/s
SHA-512                      1 MiB took 1.002 seconds,    1.267 MiB/s
HMAC-MD5                    10 MiB took 1.000 seconds,   10.083 MiB/s
HMAC-SHA                     5 MiB took 1.000 seconds,    4.810 MiB/s
HMAC-SHA256                  1 MiB took 1.008 seconds,    1.477 MiB/s
HMAC-SHA512                  1 MiB took 1.017 seconds,    1.248 MiB/s
PBKDF2                     192 bytes took 1.029 seconds,  186.589 bytes/s
RSA     2048   public        40 ops took 1.024 sec, avg 25.600 ms, 39.062 ops/sec
RSA     2048  private         2 ops took 3.248 sec, avg 1624.000 ms, 0.616 ops/sec
ECC   [      SECP256R1]   256  key gen         6 ops took 1.258 sec, avg 209.667 ms, 4.769 ops/sec
ECDHE [      SECP256R1]   256    agree         6 ops took 1.254 sec, avg 209.000 ms, 4.785 ops/sec
ECDSA [      SECP256R1]   256     sign         6 ops took 1.277 sec, avg 212.833 ms, 4.699 ops/sec
ECDSA [      SECP256R1]   256   verify         4 ops took 1.617 sec, avg 404.250 ms, 2.474 ops/sec
CURVE  25519  key gen         3 ops took 1.471 sec, avg 490.333 ms, 2.039 ops/sec
CURVE  25519    agree         4 ops took 1.957 sec, avg 489.250 ms, 2.044 ops/sec
ED     25519  key gen        36 ops took 1.005 sec, avg 27.917 ms, 35.821 ops/sec
ED     25519     sign        32 ops took 1.016 sec, avg 31.750 ms, 31.496 ops/sec
ED     25519   verify        24 ops took 1.045 sec, avg 43.542 ms, 22.967 ops/sec
Benchmark complete
I (42506) wolfssl_benchmark: wolf_test_task complete success result code = 0

