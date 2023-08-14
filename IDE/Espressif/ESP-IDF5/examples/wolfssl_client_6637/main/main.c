/* main.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

/* ESP specific */
#include <nvs_flash.h>

/* wolfSSL */
#include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>

/* this project */
#include "wifi_connect.h"
#include "client-tls.h"
#include "time_helper.h"

#ifdef WOLFSSL_TRACK_MEMORY
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

static const char* const TAG = "TLS Client";

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

    for(i=0;i<ATECC_MAX_SLOT; i++) {
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
            for(i=0;i<ATECC_MAX_SLOT;i++){
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

/* for FreeRTOS */
void app_main(void)
{
    ESP_LOGI(TAG, "---------------- wolfSSL TLS Client Example ------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
#ifdef HAVE_VERSION_EXTENDED_INFO
    esp_ShowExtendedSystemInfo();
#endif

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize WiFi */
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    /* set time for cert validation */
    set_time();

    ESP_LOGI(TAG, "Initial Stack Used (before wolfSSL Server): %d bytes",
                   CONFIG_ESP_MAIN_TASK_STACK_SIZE
                   - uxTaskGetStackHighWaterMark(NULL)
            );
    ESP_LOGI(TAG, "Starting TLS Client task ...\n");

#if defined(SINGLE_THREADED)
    /* just call the task */
    tls_smp_client_task((void*)NULL);
#else
    tls_args args[1];
    /* start a thread with the task */
    args[0].loops = 1;
    args[0].port = 11111;
    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
//    tls_smp_client_init(args);
#endif
    while (1) {
        ESP_LOGV(TAG, "\n\nLoop...\n\n");
        ESP_LOGV(TAG, "Stack used: %d", CONFIG_ESP_MAIN_TASK_STACK_SIZE
                                        - uxTaskGetStackHighWaterMark(NULL));
#if defined(SINGLE_THREADED)
        ESP_LOGV(TAG, "\n\nDone!\n\n");
        while (1);
#else
        vTaskDelay(60000);
#endif
    }
}
