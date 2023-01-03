/* benchmark main.c
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
/* ESP-IDF */
#include <esp_log.h>
#include "sdkconfig.h"
#ifdef HAVE_LIBOQS
#include <oqs/oqs.h>
#endif
/* wolfSSL */
#include <wolfssl/version.h>
#include <user_settings.h>
#ifndef WOLFSSL_ESPIDF
#warning "problem with wolfSSL user_settings. Check components/wolfssl/include"
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfcrypt/benchmark/benchmark.h>

/* check BENCH_ARGV in sdkconfig to determine need to set WOLFSSL_BENCH_ARGV */
#ifdef CONFIG_BENCH_ARGV
#define WOLFSSL_BENCH_ARGV CONFIG_BENCH_ARGV
#define WOLFSSL_BENCH_ARGV_MAX_ARGUMENTS 22 /* arbitrary number of max args */
#endif

/*
** the wolfssl component can be installed in either:
**
**   - the ESP-IDF component directory
**
**       ** OR **
**
**   - the local project component directory
**
** it is not recommended to install in both.
**
*/

#include "main.h"

static const char* const TAG = "wolfssl_benchmark";

#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)

#include "wolfssl/wolfcrypt/port/atmel/atmel.h"

/* when you need to use a custom slot allocation, */
/* enable the definition CUSTOM_SLOT_ALLOCAION.   */
#if defined(CUSTOM_SLOT_ALLOCATION)

static byte mSlotList[ATECC_MAX_SLOT];

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
    int i, slot = ATECC_INVALID_SLOT;

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

/* the following are needed by benchmark.c with args */
#ifdef WOLFSSL_BENCH_ARGV
char* __argv[WOLFSSL_BENCH_ARGV_MAX_ARGUMENTS];

int construct_argv()
{
    int cnt = 0;
    int i = 0;
    int len = 0;
    char *_argv; /* buffer for copying the string    */
    char *ch; /* char pointer to trace the string */
    char buff[16] = { 0 }; /* buffer for a argument copy       */

    ESP_LOGI(TAG, "construct_argv arg:%s\n", CONFIG_BENCH_ARGV);
    len = strlen(CONFIG_BENCH_ARGV);
    _argv = (char*)malloc(len + 1);
    if (!_argv) {
        return -1;
    }
    memset(_argv, 0, len + 1);
    memcpy(_argv, CONFIG_BENCH_ARGV, len);
    _argv[len] = '\0';
    ch = _argv;

    __argv[cnt] = malloc(10);
    sprintf(__argv[cnt], "benchmark");
    __argv[cnt][9] = '\0';
    cnt = 1;

    while (*ch != '\0') {
        /* check that we don't overflow manual arg assembly */
        if (cnt >= (WOLFSSL_BENCH_ARGV_MAX_ARGUMENTS)) {
            ESP_LOGE(TAG,
                     "Abort construct_argv;"
                     "Reached maximum defined arguments = %d",
                     WOLFSSL_BENCH_ARGV_MAX_ARGUMENTS);
            break;
        }

        /* skip white-space */
        while (*ch == ' ') { ++ch; }

        memset(buff, 0, sizeof(buff));
        /* copy each args into buffer */
        i = 0;
        while ((*ch != ' ') && (*ch != '\0') && (i < 16)) {
            buff[i] = *ch;
            ++i;
            ++ch;
        }
        /* copy the string into argv */
        __argv[cnt] = (char*)malloc(i + 1);
        memset(__argv[cnt], 0, i + 1);
        memcpy(__argv[cnt], buff, i + 1);
        /* next args */
        ++cnt;
    }

    free(_argv);

    return (cnt);
}
#endif

#define WOLFSSL_SHOW_ESPIDF_SETTING (a) { ESP_LOGI("TAG",a); };

/* entry point */
void app_main(void)
{
    int rc = 0;
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");


    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_STRING = %s", LIBWOLFSSL_VERSION_STRING);
    ESP_LOGI(TAG, "CONFIG_IDF_TARGET = %s", CONFIG_IDF_TARGET);

#if defined(CONFIG_IDF_TARGET_ESP32C3)
    /* not available for C3 at this time */
#else
   // ESP_LOGI(TAG, "CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ = %u MHz", CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);
    ESP_LOGI(TAG, "Xthal_have_ccount = %u", Xthal_have_ccount);
#endif
    ESP_LOGI(TAG, "Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));

#if defined(NO_ESP32WROOM32_CRYPT)
    ESP_LOGI(TAG, "NO_ESP32WROOM32_CRYPT defined! HW acceleration DISABLED.");
#else
    ESP_LOGI(TAG, "ESP32WROOM32_CRYPT is enabled.");
#endif

#if defined(GENERATE_MACHINE_PARSEABLE_REPORT)
    ESP_LOGI(TAG, "Report format enabled: GENERATE_MACHINE_PARSEABLE_REPORT");
#else
    ESP_LOGV(TAG, "Standard report format.");
#endif

    ESP_LOGI(TAG, "app_main CONFIG_BENCH_ARGV = %s", WOLFSSL_BENCH_ARGV);

/* when using atecc608a on esp32-wroom-32se */
#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)
#if defined(CUSTOM_SLOT_ALLOCATION)
    my_atmel_slotInit();
    /* to register the callback, it needs to be initialized. */
    if ((wolfCrypt_Init()) != 0) {
        ESP_LOGE(TAG, "wolfCrypt_Init failed");
        return;
    }
    atmel_set_slot_allocator(my_atmel_alloc, my_atmel_free);
#endif /* CUSTOM_SLOT_ALLOCATION */
#endif /* WOLFSSL_ESPWROOM32SE, etc */

#ifdef NO_CRYPT_BENCHMARK
    ESP_LOGI(TAG, "NO_CRYPT_BENCHMARK defined, skipping wolf_benchmark_task")
#else

    /* although wolfCrypt_Init() may be explicitly called above,
    ** not it is still always called in wolf_benchmark_task.
    */
    rc = wolf_benchmark_task(NULL);
    if (rc == 0) {
        ESP_LOGI(TAG, "wolf_test_task complete success result code = %d", rc);
    }
    else {
        ESP_LOGE(TAG, "wolf_test_task FAIL result code = %d", rc);
        /* see wolfssl/wolfcrypt/error-crypt.h */
    }
    ESP_LOGI(TAG, "Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));

    /* after the test, we'll just wait */
    while (1) {
        /* nothing */
    }

#endif /* NO_CRYPT_BENCHMARK */
} /* main */
