.W Pro cpu up.
SP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x1664
load:0x403c9700,len:0xb54
load:0x403cc700,len:0x2f78
entry 0x403c9938
I (24) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (25) boot: compile time 09:36:44
I (25) boot: chip revision: 0
I (28) boot.esp32s3: Boot SPI Speed : 80MHz
I (33) boot.esp32s3: SPI Mode       : DIO
I (38) boot.esp32s3: SPI Flash Size : 2MB
I (42) boot: Enabling RNG early entropy source...
I (48) boot: Partition Table:
I (51) boot: ## Label            Usage          Type ST Offset   Length
I (59) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (66) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (73) boot:  2 factory          factory app      00 00 00010000 00100000
I (81) boot: End of partition table
I (85) esp_image: segment 0: paddr=00010020 vaddr=3c050020 size=17fa4h ( 98212) map
I (111) esp_image: segment 1: paddr=00027fcc vaddr=3fc8fd70 size=01ff4h (  8180) load
I (113) esp_image: segment 2: paddr=00029fc8 vaddr=40374000 size=06050h ( 24656) load
I (122) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=420b8h (270520) map
I (174) esp_image: segment 4: paddr=000720e0 vaddr=4037a050 size=05d20h ( 23840) load
I (179) esp_image: segment 5: paddr=00077e08 vaddr=50000000 size=00010h (    16) load
I (184) boot: Loaded app from partition at offset 0x10000
I (186) boot: Disabling RNG early entropy source...
I (202) cpu_start: Pro cpu up.
I (203) cpu_start: Single core mode
I (211) cpu_start: Pro cpu start user code
I (211) cpu_start: cpu freq: 160000000
I (211) cpu_start: Application information:
I (214) cpu_start: Project name:     wolfssl_test
I (219) cpu_start: App version:      v5.5.3-stable-111-g07ceb86ca-di
I (226) cpu_start: Compile time:     Nov 22 2022 09:36:20
I (232) cpu_start: ELF file SHA256:  7637bec3fb20864f...
I (238) cpu_start: ESP-IDF:          v4.4.2-1-g0aba20e63d-dirty
I (245) heap_init: Initializing. RAM available for dynamic allocation:
I (252) heap_init: At 3FC92688 len 0004D978 (310 KiB): D/IRAM
I (258) heap_init: At 3FCE0000 len 0000EE34 (59 KiB): STACK/DRAM
I (265) heap_init: At 3FCF0000 len 00008000 (32 KiB): DRAM
I (271) heap_init: At 600FE000 len 00002000 (8 KiB): RTCRAM
I (278) spi_flash: detected chip: generic
I (282) spi_flash: flash io: dio
W (286) spi_flash: Detected size(8192k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (300) sleep: Configure to isolate all GPIO pins in sleep state
I (306) sleep: Enable automatic switching of GPIO sleep configuration
I (314) cpu_start: Starting scheduler on PRO CPU.
I (319) wolfssl_test: CONFIG_IDF_TARGET=esp32s3
I (319) wolfssl_test: MqttClient_NetConnect Task main, Stack HWM: 53600

I (329) wolfssl_test: LIBWOLFSSL_VERSION_STRING = 5.5.3
I (329) wolfssl_test: Stack HWM: 53588

------------------------------------------------------------------------------
 wolfSSL version 5.5.3
------------------------------------------------------------------------------
error    test passed!
MEMORY   test passed!
base64   test passed!
asn      test passed!
RANDOM   test passed!
MD5      test passed!
MD4      test passed!
SHA      test passed!
SHA-256  test passed!
SHA-512  test passed!
Hash     test passed!
HMAC-MD5 test passed!
HMAC-SHA test passed!
HMAC-SHA256 test passed!
HMAC-SHA512 test passed!
HMAC-KDF    test passed!
TLSv1.3 KDF test passed!
GMAC     test passed!
DES      test passed!
DES3     test passed!
AES      test passed!
AES192   test passed!
AES256   test passed!
AES-GCM  test passed!
RSA      test passed!
PWDBASED test passed!
ECC      test passed!
ECC buffer test passed!
CURVE25519 test passed!
ED25519  test passed!
logging  test passed!
time test passed!
mutex    test passed!
Test complete
I (122479) wolfcrypt_test: Exiting main with return code:  0

I (122479) wolfssl_test: Stack HWM: 5652

I (122489) wolfssl_test: wolf_test_task complete success result code = 0


