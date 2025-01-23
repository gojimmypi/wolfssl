/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

static int test_wc_AesCtrEncryptDecrypt(void) {
  //EXPECT_DECLS;
  Aes aesEnc;
  Aes aesDec;
  /*
  byte key32[] = {
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
      0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
      0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66};

  byte vector[] = {
      0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
      0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
      0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20 };
    // 'Now is the time for all' (without null terminator)
  byte iv[] = "1234567890abcdef";
  */

  // Key 128 bits = 16 bytes
  byte key16[] = {0x55, 0x24, 0xcc, 0x11, 0xd9, 0x51, 0xb6, 0x37,
                  0x96, 0xd0, 0x5a, 0x9a, 0xcc, 0xe7, 0x31, 0x7 };

  // encrypted data [from "RAW fiddle (c).txt"]
  byte vector[] = {0xF8, 0x74, 0x60, 0x1C, 0x3F, 0x86, 0x9B, 0x6A,
                   0x19, 0x62, 0x58, 0xEB, 0x22, 0xED, 0xD3, 0x14};

  // initialisation vector [from "RAW fiddle (c).txt"] Also called or "salt" or "Nonce/Counter"
  byte iv[] = {0x5D, 0x97};

  byte enc[AES_BLOCK_SIZE * 2];
  byte dec[AES_BLOCK_SIZE * 2];

  // Init stack variables
  XMEMSET(&aesEnc, 0, sizeof(Aes));
  XMEMSET(&aesDec, 0, sizeof(Aes));
  XMEMSET(enc, 0, AES_BLOCK_SIZE * 2);
  XMEMSET(dec, 0, AES_BLOCK_SIZE * 2);

  ExpectIntEQ(wc_AesInit(&aesEnc, NULL, INVALID_DEVID), 0);
  ExpectIntEQ(wc_AesInit(&aesDec, NULL, INVALID_DEVID), 0);

  ExpectIntEQ(wc_AesSetKey(&aesEnc, key16, AES_BLOCK_SIZE * 2, iv,
      AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesEnc, enc, vector,
      sizeof(vector)/sizeof(byte)), 0);
  // Decrypt with wc_AesCtrEncrypt()
  ExpectIntEQ(wc_AesSetKey(&aesDec, key16, AES_BLOCK_SIZE * 2, iv,
      AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, enc, sizeof(enc)/sizeof(byte)),
      0);
  ExpectIntEQ(XMEMCMP(vector, dec, sizeof(vector)), 0);

  // Test bad args
  ExpectIntEQ(wc_AesCtrEncrypt(NULL, dec, enc, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, NULL, enc, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, NULL, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));

  wc_AesFree(&aesEnc);
  wc_AesFree(&aesDec);
  //return EXPECT_RESULT();
}

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    test_wc_AesCtrEncryptDecrypt();

}
