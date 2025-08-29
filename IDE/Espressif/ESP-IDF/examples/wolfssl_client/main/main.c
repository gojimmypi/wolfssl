/* main.c
 *
 * Copyright (C) 2006-2025 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#include "sdkconfig.h"
#include "main.h"

/* ESP specific */
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_event.h>

/* wolfSSL */
/* Always include wolfcrypt/settings.h before any other wolfSSL file.    */
/* Reminder: settings.h pulls in user_settings.h; don't include it here. */
#ifdef WOLFSSL_USER_SETTINGS
    #include <wolfssl/wolfcrypt/settings.h>
    #ifndef WOLFSSL_ESPIDF
        #warning "Problem with wolfSSL user_settings."
        #warning "Check components/wolfssl/include"
    #endif
    /* This project not yet using the library */
    #undef USE_WOLFSSL_ESP_SDK_WIFI
    #include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
#else
    /* Define WOLFSSL_USER_SETTINGS project wide for settings.h to include   */
    /* wolfSSL user settings in ./components/wolfssl/include/user_settings.h */
    #error "Missing WOLFSSL_USER_SETTINGS in CMakeLists or Makefile:\
    CFLAGS +=-DWOLFSSL_USER_SETTINGS"
#endif

/* this project */
#include "client-tls.h"
#include "time_helper.h"

#ifdef CONFIG_IDF_TARGET_ESP32H2
    /* There's no WiFi on ESP32-H2.
     * For wired ethernet, see:
     * https://github.com/wolfSSL/wolfssl-examples/tree/master/ESP32/TLS13-ENC28J60-client */
#else
    #include "wifi_connect.h"
    /*
     * Note ModBus TCP cannot be disabled on ESP8266 tos-sdk/v3.4
     * See https://github.com/espressif/esp-modbus/issues/2
     */
#endif

#ifdef WOLFSSL_TRACK_MEMORY
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

#if defined(WOLFSSL_TEST_LOOP) && WOLFSSL_TEST_LOOP
    SemaphoreHandle_t task_semaphore;
#endif

static unsigned int stack_start = 0;
// static unsigned int stack_current = 0;


static const char* TAG = "main";

#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)

#include "wolfssl/wolfcrypt/port/atmel/atmel.h"

/* when you want to use a custom slot allocation */
/* enable the definition CUSTOM_SLOT_ALLOCATION. */

#if defined(CUSTOM_SLOT_ALLOCATION)

static byte mSlotList[ATECC_MAX_SLOT];

int atmel_set_slot_allocator(atmel_slot_alloc_cb alloc, atmel_slot_dealloc_cb dealloc);

/* initialize slot array */
void my_atmel_slotInit()
{
    int i;
    for(i = 0;i < ATECC_MAX_SLOT;i++) {
        mSlotList[i] = ATECC_INVALID_SLOT;
    }
}

/* allocate slot depending on slotType */
int my_atmel_alloc(int slotType)
{
    int i, slot = -1;

    switch(slotType){
        case ATMEL_SLOT_ENCKEY:
            slot = 4;
            break;
        case ATMEL_SLOT_DEVICE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE_ENC:
            slot = 4;
            break;
        case ATMEL_SLOT_ANY:
            for(i = 0;i < ATECC_MAX_SLOT;i++){
                if(mSlotList[i] == ATECC_INVALID_SLOT){
                    slot = i;
                    break;
                }
            }
    }

    return slot;
}

/* free slot array       */
void my_atmel_free(int slotId)
{
    if(slotId >= 0 && slotId < ATECC_MAX_SLOT){
        mSlotList[slotId] = ATECC_INVALID_SLOT;
    }
}
#endif /* CUSTOM_SLOT_ALLOCATION                                       */
#endif /* WOLFSSL_ESPWROOM32SE && HAVE_PK_CALLBACK && WOLFSSL_ATECC508A */

/* Entry for FreeRTOS */
void app_main(void)
{
#if WOLFSSL_TEST_LOOP
    task_semaphore =  xSemaphoreCreateBinary();
#endif
#if defined(ESP_IDF_VERSION) && (ESP_IDF_VERSION == ESP_IDF_VERSION_VAL(5, 2, 2))
    ESP_LOGI(TAG, "Found esp-IDF v5.2.2");
#else
    ESP_LOGW(TAG, "This example is intended for ESP-IDF version 5.2.2");
#endif
#ifdef CONFIG_ESP_TLS_USING_WOLFSSL
    stack_start = esp_sdk_stack_pointer();
    stack_current = esp_sdk_stack_pointer();
#endif
    ESP_LOGI(TAG, "------------------ wolfSSL Test Example ----------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "Stack Start: 0x%x", stack_start);
#ifdef WOLFSSL_ESP_NO_WATCHDOG
    ESP_LOGW(TAG, "Found WOLFSSL_ESP_NO_WATCHDOG, disabling...");
    esp_DisableWatchdog();
#endif

#if defined(HAVE_VERSION_EXTENDED_INFO)
    esp_ShowExtendedSystemInfo();
#else
    ESP_LOGW(TAG, "HAVE_VERSION_EXTENDED_INFO not defined");
#endif

#ifdef ESP_SDK_MEM_LIB_VERSION
    /* Set time for cert validation.
     * Some lwIP APIs, including SNTP functions, are not thread safe. */
    // set_time(); /* need to setup NTP before WiFi */
    set_time_from_string("Wed Jun 18 12:41:45 2025 -0700");
#endif

    ESP_LOGI(TAG, "nvs flash init..");
    esp_err_t ret = nvs_flash_init();

#if defined(ESP_ERR_NVS_NO_FREE_PAGES) && defined(ESP_ERR_NVS_NEW_VERSION_FOUND)
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "nvs flash erase..");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_LOGI(TAG, "nvs flash erase..");
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
#endif

    ESP_LOGI(TAG, "esp netif init...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "esp event loop create default...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

#if defined(CONFIG_IDF_TARGET_ESP32H2)
    ESP_LOGI(TAG, "There's no WiFi on the ESP32-H2");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#else
    ESP_LOGI(TAG, "example connect...");
    ESP_ERROR_CHECK(example_connect());
#endif

#ifdef USE_WOLFSSL_ESP_SDK_TIME
    /* Once we are connected to the network, start & wait for NTP time */
    ret = set_time_wait_for_ntp();
#endif

#if defined(SINGLE_THREADED)
    /* just call the task */
    tls_smp_client_task_2((void*)NULL);
#else
    tls_args args[1] = {0};
    /* start a thread with the task */
    args[0].loops = 10;
    args[0].port = 11111;

    /* HWM is maximum amount of stack space that has been unused, in bytes
     * not words (unlike vanilla freeRTOS). */
    this_heap = esp_get_free_heap_size();
    ESP_LOGI(TAG, "Initial Stack Used (before wolfSSL Server): %d bytes",
                   CONFIG_ESP_MAIN_TASK_STACK_SIZE
                   - (uxTaskGetStackHighWaterMark(NULL))
            );
    ESP_LOGI(TAG, "Starting TLS Client task ...\n");
    ESP_LOGI(TAG, "main tls_smp_client_init heap @ %p = %d",
                  &this_heap, this_heap);
    tls_smp_client_init(args);
/* optional additional client threads
    tls_smp_client_init(args);
    tls_smp_client_init(args);
    tls_smp_client_init(args);
    tls_smp_client_init(args);
    tls_smp_client_init(args);
    tls_smp_client_init(args);
    tls_smp_client_init(args);
*/
#endif

    /* Done */
#ifdef SINGLE_THREADED
    ESP_LOGV(TAG, "\n\nDone!\n\n");
    while (1);
#else
    ESP_LOGV(TAG, "\n\nvTaskDelete...\n\n");
    vTaskDelete(NULL);
    /* done */
    while (1) {
        ESP_LOGV(TAG, "\n\nLoop...\n\n");
    #ifdef INCLUDE_uxTaskGetStackHighWaterMark
        ESP_LOGI(TAG, "Stack HWM: %d", uxTaskGetStackHighWaterMark(NULL));

        ESP_LOGI(TAG, "Stack used: %d", CONFIG_ESP_MAIN_TASK_STACK_SIZE
                                        - (uxTaskGetStackHighWaterMark(NULL) ));
    #endif
        vTaskDelay(60000);
    } /* done while */
#endif /* else not SINGLE_THREADED */

} /* app_main */
