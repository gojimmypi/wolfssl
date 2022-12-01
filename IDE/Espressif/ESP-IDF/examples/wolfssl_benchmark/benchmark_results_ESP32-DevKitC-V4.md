ets Jun  8 2016 00:22:57

rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:6680
load:0x40078000,len:14848
load:0x40080400,len:3792
0x40080400: _init at ??:?

entry 0x40080694
I (27) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (27) boot: compile time 08:04:18
I (27) boot: chip revision: 1
I (32) boot_comm: chip revision: 1, min. bootloader chip revision: 0
I (39) boot.esp32: SPI Speed      : 40MHz
I (43) boot.esp32: SPI Mode       : DIO
I (48) boot.esp32: SPI Flash Size : 2MB
I (53) boot: Enabling RNG early entropy source...
I (58) boot: Partition Table:
I (62) boot: ## Label            Usage          Type ST Offset   Length
I (69) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (76) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (84) boot:  2 factory          factory app      00 00 00010000 00100000
I (91) boot: End of partition table
I (95) boot_comm: chip revision: 1, min. application chip revision: 0
I (102) esp_image: segment 0: paddr=00010020 vaddr=3f400020 size=147c4h ( 83908) map
I (141) esp_image: segment 1: paddr=000247ec vaddr=3ffb0000 size=02424h (  9252) load
I (145) esp_image: segment 2: paddr=00026c18 vaddr=40080000 size=09400h ( 37888) load
I (163) esp_image: segment 3: paddr=00030020 vaddr=400d0020 size=34620h (214560) map
I (241) esp_image: segment 4: paddr=00064648 vaddr=40089400 size=0210ch (  8460) load
I (245) esp_image: segment 5: paddr=0006675c vaddr=50000000 size=00010h (    16) load
I (252) boot: Loaded app from partition at offset 0x10000
I (253) boot: Disabling RNG early entropy source...
I (269) cpu_start: Pro cpu up.
I (270) cpu_start: Starting app cpu, entry point is 0x4008109c
0x4008109c: call_start_cpu1 at /mnt/c/SysGCC/esp32/esp-idf/v4.4.2/components/esp_system/port/cpu_start.c:160

I (0) cpu_start: App cpu up.
I (284) cpu_start: Pro cpu start user code
I (284) cpu_start: cpu freq: 160000000
I (284) cpu_start: Application information:
I (288) cpu_start: Project name:     wolfssl_benchmark
I (294) cpu_start: App version:      v5.5.3-stable-111-g07ceb86ca-di
I (301) cpu_start: Compile time:     Nov 23 2022 08:03:50
I (307) cpu_start: ELF file SHA256:  94b151e007f3319f...
I (313) cpu_start: ESP-IDF:          v4.4.2-1-g0aba20e63d-dirty
I (320) heap_init: Initializing. RAM available for dynamic allocation:
I (327) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (333) heap_init: At 3FFB3DE0 len 0002C220 (176 KiB): DRAM
I (339) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (346) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (352) heap_init: At 4008B50C len 00014AF4 (82 KiB): IRAM
I (360) spi_flash: detected chip: generic
I (363) spi_flash: flash io: dio
W (367) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (381) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (391) wolfssl_benchmark: app_main CONFIG_BENCH_ARGV = -lng 0
I (401) wolfssl_benchmark: construct_argv arg:-lng 0

------------------------------------------------------------------------------
 wolfSSL version 5.5.3
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                          6 MiB took 1.003 seconds,    6.377 MiB/s
AES-128-CBC-enc              8 MiB took 1.002 seconds,    7.602 MiB/s
AES-128-CBC-dec              7 MiB took 1.000 seconds,    7.178 MiB/s
AES-192-CBC-enc              7 MiB took 1.003 seconds,    6.962 MiB/s
AES-192-CBC-dec              7 MiB took 1.001 seconds,    6.610 MiB/s
AES-256-CBC-enc              6 MiB took 1.003 seconds,    6.426 MiB/s
AES-256-CBC-dec              6 MiB took 1.001 seconds,    6.122 MiB/s
AES-128-GCM-enc              2 MiB took 1.006 seconds,    1.917 MiB/s
AES-128-GCM-dec              2 MiB took 1.007 seconds,    1.915 MiB/s
AES-192-GCM-enc              2 MiB took 1.004 seconds,    1.872 MiB/s
AES-192-GCM-dec              2 MiB took 1.004 seconds,    1.872 MiB/s
AES-256-GCM-enc              2 MiB took 1.000 seconds,    1.831 MiB/s
AES-256-GCM-dec              2 MiB took 1.001 seconds,    1.829 MiB/s
GMAC Default                 3 MiB took 1.000 seconds,    2.630 MiB/s
3DES                         3 MiB took 1.006 seconds,    2.864 MiB/s
MD5                         92 MiB took 1.000 seconds,   92.163 MiB/s
SHA                         39 MiB took 1.000 seconds,   38.843 MiB/s
SHA-256                     16 MiB took 1.000 seconds,   15.503 MiB/s
SHA-512                      8 MiB took 1.000 seconds,    8.228 MiB/s
HMAC-MD5                    91 MiB took 1.000 seconds,   91.138 MiB/s
HMAC-SHA                    39 MiB took 1.000 seconds,   38.501 MiB/s
HMAC-SHA256                 15 MiB took 1.001 seconds,   15.365 MiB/s
HMAC-SHA512                  8 MiB took 1.000 seconds,    8.105 MiB/s
PBKDF2                       2 KiB took 1.015 seconds,    1.878 KiB/s
RSA     2048   public       314 ops took 1.001 sec, avg 3.188 ms, 313.686 ops/sec
RSA     2048  private         6 ops took 1.196 sec, avg 199.333 ms, 5.017 ops/sec
ECC   [      SECP256R1]   256  key gen        38 ops took 1.001 sec, avg 26.342 ms, 37.962 ops/sec
ECDHE [      SECP256R1]   256    agree        38 ops took 1.000 sec, avg 26.316 ms, 38.000 ops/sec
ECDSA [      SECP256R1]   256     sign        38 ops took 1.014 sec, avg 26.684 ms, 37.475 ops/sec
ECDSA [      SECP256R1]   256   verify        20 ops took 1.022 sec, avg 51.100 ms, 19.569 ops/sec
CURVE  25519  key gen        16 ops took 1.018 sec, avg 63.625 ms, 15.717 ops/sec
CURVE  25519    agree        16 ops took 1.017 sec, avg 63.563 ms, 15.733 ops/sec
ED     25519  key gen       344 ops took 1.002 sec, avg 2.913 ms, 343.313 ops/sec
ED     25519     sign       304 ops took 1.000 sec, avg 3.289 ms, 304.000 ops/sec
ED     25519   verify       160 ops took 1.011 sec, avg 6.319 ms, 158.259 ops/sec
Benchmark complete
I (355661) wolfssl_benchmark: wolf_test_task complete success result code = 0