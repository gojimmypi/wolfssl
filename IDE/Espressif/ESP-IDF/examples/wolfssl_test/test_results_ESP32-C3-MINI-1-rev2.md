app from ESP-ROM:esp32c3-20200918
Build:Sep 18 2020
rst:0x1 (POWERON),boot:0xc (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fcd6100,len:0x16c8
load:0x403ce000,len:0x930
load:0x403d0000,len:0x2d28
entry 0x403ce000
I (32) boot: ESP-IDF v4.4.2-1-g0aba20e63d-dirty 2nd stage bootloader
I (32) boot: compile time 08:36:34
I (32) boot: chip revision: 2
I (35) boot_comm: chip revision: 2, min. bootloader chip revision: 0
I (43) boot.esp32c3: SPI Speed      : 80MHz
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
I (107) esp_image: segment 0: paddr=00010020 vaddr=3c050020 size=17e48h ( 97864) map
I (131) esp_image: segment 1: paddr=00027e70 vaddr=3fc8a400 size=01590h (  5520) load
I (132) esp_image: segment 2: paddr=00029408 vaddr=40380000 size=06c10h ( 27664) load
I (142) esp_image: segment 3: paddr=00030020 vaddr=42000020 size=43c34h (277556) map
I (188) esp_image: segment 4: paddr=00073c5c vaddr=40386c10 size=0375ch ( 14172) load
I (191) esp_image: segment 5: paddr=000773c0 vaddr=50000010 size=00010h (    16) load
I (197) boot: Loaded app from partition at offset 0x10000
I (200) boot: Disabling RNG early entropy source...
I (217) cpu_start: Pro cpu up.
I (225) cpu_start: Pro cpu start user code
I (226) cpu_start: cpu freq: 160000000
I (226) cpu_start: Application information:
I (228) cpu_start: Project name:     wolfssl_test
I (234) cpu_start: App version:      v5.5.3-stable-111-g07ceb86ca-di
I (241) cpu_start: Compile time:     Nov 22 2022 08:36:26
I (247) cpu_start: ELF file SHA256:  39578af7ed3b4b7f...
I (253) cpu_start: ESP-IDF:          v4.4.2-1-g0aba20e63d-dirty
I (259) heap_init: Initializing. RAM available for dynamic allocation:
I (267) heap_init: At 3FC8C840 len 000337C0 (205 KiB): DRAM
I (273) heap_init: At 3FCC0000 len 0001F260 (124 KiB): STACK/DRAM
I (280) heap_init: At 50000020 len 00001FE0 (7 KiB): RTCRAM
I (286) spi_flash: detected chip: generic
I (291) spi_flash: flash io: dio
W (295) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (308) sleep: Configure to isolate all GPIO pins in sleep state
I (315) sleep: Enable automatic switching of GPIO sleep configuration
I (322) cpu_start: Starting scheduler.
I (326) wolfssl_test: CONFIG_IDF_TARGET=esp32c3
I (326) wolfssl_test: MqttClient_NetConnect Task main, Stack HWM: 53784

I (336) wolfssl_test: LIBWOLFSSL_VERSION_STRING = 5.5.3
I (336) wolfssl_test: Stack HWM: 53784

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
I (110066) wolfcrypt_test: Exiting main with return code:  0

I (110066) wolfssl_test: Stack HWM: 6204

I (110066) wolfssl_test: wolf_test_task complete success result code = 0






