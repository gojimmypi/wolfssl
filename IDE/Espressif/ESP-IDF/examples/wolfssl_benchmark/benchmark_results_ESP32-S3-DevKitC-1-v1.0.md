) esp_image: segment ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:2
load:0x3fce3808,len:0x1664
load:0x403c9700,len:0xb54
load:0x403cc700,len:0x2f80
entry 0x403c9938
I (29) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (30) boot: compile time 17:24:46
I (30) boot: chip revision: 0
I (33) boot.esp32s3: Boot SPI Speed : 40MHz
I (38) boot.esp32s3: SPI Mode       : DIO
I (42) boot.esp32s3: SPI Flash Size : 2MB
I (47) boot: Enabling RNG early entropy source...
I (53) boot: Partition Table:
I (56) boot: ## Label            Usage          Type ST Offset   Length
I (63) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (71) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (78) boot:  2 factory          factory app      00 00 00010000 00100000
I (86) boot: End of partition table
I (90) esp_image: segment 0: paddr=00010020 vaddr=3c040020 size=13fcch ( 81868) map
I (119) esp_image: segment 1: paddr=00023ff4 vaddr=3fc8fcc0 size=01e68h (  7784) load
I (121) esp_image: segment 2: paddr=00025e64 vaddr=40374000 size=0a1b4h ( 41396) load
I (136) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=35964h (219492) map
I (191) esp_image: segment 4: paddr=0006598c vaddr=4037e1b4 size=01b00h (  6912) load
I (194) esp_image: segment 5: paddr=00067494 vaddr=50000000 size=00010h (    16) load
I (202) boot: Loaded app from partition at offset 0x10000
I (203) boot: Disabling RNG early entropy source...
I (220) cpu_start: Pro cpu up.
I (220) cpu_start: Single core mode
I (229) cpu_start: Pro cpu start user code
I (229) cpu_start: cpu freq: 160000000
I (229) cpu_start: Application information:
I (232) cpu_start: Project name:     wolfssl_benchmark
I (237) cpu_start: App version:      v5.5.3-stable-111-g07ceb86ca-di
I (245) cpu_start: Compile time:     Nov 22 2022 17:24:19
I (251) cpu_start: ELF file SHA256:  e068dd87c7db816b...
I (257) cpu_start: ESP-IDF:          v4.4.2-1-g0aba20e63d-dirty
I (263) heap_init: Initializing. RAM available for dynamic allocation:
I (270) heap_init: At 3FC934F0 len 0004CB10 (306 KiB): D/IRAM
I (277) heap_init: At 3FCE0000 len 0000EE34 (59 KiB): STACK/DRAM
I (284) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (290) heap_init: At 600FE000 len 00002000 (8 KiB): RTCRAM
I (297) spi_flash: detected chip: generic
I (301) spi_flash: flash io: dio
W (305) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (318) cpu_start: Starting scheduler on PRO CPU.
I (323) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (329) wolfssl_benchmark: construct_argv arg:-lng 0

------------------------------------------------------------------------------
 wolfSSL version 5.5.3
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                        725 KiB took 1.009 seconds,  718.533 KiB/s
AES-128-CBC-enc            950 KiB took 1.003 seconds,  947.159 KiB/s
AES-128-CBC-dec            950 KiB took 1.025 seconds,  926.829 KiB/s
AES-192-CBC-enc            875 KiB took 1.005 seconds,  870.647 KiB/s
AES-192-CBC-dec            875 KiB took 1.025 seconds,  853.659 KiB/s
AES-256-CBC-enc            825 KiB took 1.022 seconds,  807.241 KiB/s
AES-256-CBC-dec            800 KiB took 1.011 seconds,  791.296 KiB/s
AES-128-GCM-enc            250 KiB took 1.107 seconds,  225.836 KiB/s
AES-128-GCM-dec            250 KiB took 1.107 seconds,  225.836 KiB/s
AES-192-GCM-enc            225 KiB took 1.016 seconds,  221.457 KiB/s
AES-192-GCM-dec            225 KiB took 1.017 seconds,  221.239 KiB/s
AES-256-GCM-enc            225 KiB took 1.038 seconds,  216.763 KiB/s
AES-256-GCM-dec            225 KiB took 1.039 seconds,  216.554 KiB/s
GMAC Default               303 KiB took 1.002 seconds,  302.395 KiB/s
3DES                       350 KiB took 1.009 seconds,  346.878 KiB/s
MD5                         10 MiB took 1.001 seconds,   10.317 MiB/s
SHA                          4 MiB took 1.003 seconds,    4.357 MiB/s
SHA-256                      2 MiB took 1.005 seconds,    1.700 MiB/s
SHA-512                    950 KiB took 1.004 seconds,  946.215 KiB/s
HMAC-MD5                    10 MiB took 1.001 seconds,   10.195 MiB/s
HMAC-SHA                     4 MiB took 1.001 seconds,    4.317 MiB/s
HMAC-SHA256                  2 MiB took 1.013 seconds,    1.687 MiB/s
HMAC-SHA512                950 KiB took 1.020 seconds,  931.373 KiB/s
PBKDF2                     224 bytes took 1.061 seconds,  211.122 bytes/s
RSA     2048   public        36 ops took 1.031 sec, avg 28.639 ms, 34.918 ops/sec
RSA     2048  private         2 ops took 3.569 sec, avg 1784.500 ms, 0.560 ops/sec
ECC   [      SECP256R1]   256  key gen         6 ops took 1.413 sec, avg 235.500 ms, 4.246 ops/sec
ECDHE [      SECP256R1]   256    agree         6 ops took 1.411 sec, avg 235.167 ms, 4.252 ops/sec
ECDSA [      SECP256R1]   256     sign         6 ops took 1.435 sec, avg 239.167 ms, 4.181 ops/sec
ECDSA [      SECP256R1]   256   verify         4 ops took 1.822 sec, avg 455.500 ms, 2.195 ops/sec
CURVE  25519  key gen         2 ops took 1.135 sec, avg 567.500 ms, 1.762 ops/sec
CURVE  25519    agree         2 ops took 1.134 sec, avg 567.000 ms, 1.764 ops/sec
ED     25519  key gen        55 ops took 1.005 sec, avg 18.273 ms, 54.726 ops/sec
ED     25519     sign        44 ops took 1.010 sec, avg 22.955 ms, 43.564 ops/sec
ED     25519   verify        14 ops took 1.101 sec, avg 78.643 ms, 12.716 ops/sec
Benchmark complete
I (42647) wolfssl_benchmark: wolf_test_task complete success result code = 0
