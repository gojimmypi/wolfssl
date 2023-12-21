/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_log.h>

#ifdef WOLFSSL_USER_SETTINGS
    #include <wolfssl/wolfcrypt/settings.h>
    #include <wolfcrypt/benchmark/benchmark.h>
#else
    #error "Missing WOLFSSL_USER_SETTINGS in CMakeLists or Makefile:\
CFLAGS +=-DWOLFSSL_USER_SETTINGS"
#endif

const char * TAG = "main";

void app_main()
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "This is ESP8266 chip with %d CPU cores, WiFi, ",
                  chip_info.cores);

    ESP_LOGI(TAG, "silicon revision %d, ", chip_info.revision);

    ESP_LOGI(TAG, "%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
                 (chip_info.features & CHIP_FEATURE_EMB_FLASH)
                    ? "embedded"
                    : "external"
                 );

    fflush(stdout);
    while (1) {
        benchmark_test(NULL);
        ESP_LOGI("main", "loop");
    }
    esp_restart();
}
