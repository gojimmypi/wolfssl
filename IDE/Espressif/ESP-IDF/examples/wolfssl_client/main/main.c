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
#include "sdkconfig.h"
#include "main.h"

/* ESP specific */
#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_event.h>

/* wolfSSL */
/* Always includ wolfcrypt/settings.h before any other wolfSSL file.    */
/* Reminder: settings.h pulls in user_settings.h; don't include it here */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
#ifndef WOLFSSL_ESPIDF
    #warning "Problem with wolfSSL user_settings."
    #warning "Check components/wolfssl/include"
#endif

/* this project */
#include "client-tls.h"
#include "time_helper.h"

#ifndef CONFIG_IDF_TARGET_ESP32H2
    /* There's no WiFi on ESP32-H2.
     * For wired ethernet, see:
     * https://github.com/wolfSSL/wolfssl-examples/tree/master/ESP32/TLS13-ENC28J60-client */
    #include "wifi_connect.h"
    /*
     * Note ModBus TCP cannot be disabled on ESP8266 tos-sdk/v3.4
     * See https://github.com/espressif/esp-modbus/issues/2
     */
#endif

#ifdef WOLFSSL_TRACK_MEMORY
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

static const char* TAG = "TLS Client";

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


/* see
 * C:\SysGCC\esp8266\rtos-sdk\v3.4\components\esp8266\ld\esp8266.project.ld.in
 */
extern uintptr_t _data_start[];
extern uintptr_t _data_end[];
extern uintptr_t _rodata_start[];
extern uintptr_t _rodata_end[];
extern uintptr_t _bss_start[];
extern uintptr_t _bss_end[];
extern uintptr_t _rtc_data_start[];
extern uintptr_t _rtc_data_end[];
extern uintptr_t _rtc_bss_start[];
extern uintptr_t _rtc_bss_end[];
extern uintptr_t _iram_start[];
extern uintptr_t _iram_end[];
extern uintptr_t _init_start[];
extern uintptr_t _init_end[];
extern uintptr_t _iram_text_start[];
extern uintptr_t _iram_text_end[];
extern uintptr_t _iram_bss_start[];
extern uintptr_t _iram_bss_end[];
extern uintptr_t _noinit_start[];
extern uintptr_t _noinit_end[];
extern uintptr_t _text_start[];
extern uintptr_t _text_end[];
extern uintptr_t _heap_start[];
extern uintptr_t _heap_end[];
extern uintptr_t _rtc_data_start[];
extern uintptr_t _rtc_data_end[];
extern void* _thread_local_start;
extern void* _thread_local_end;

/* See https://github.com/esp8266/esp8266-wiki/wiki/Memory-Map */
#define MEM_MAP_IO_START  ((void*)(0x3FF00000))
#define MEM_MAP_IO_END    ((void*)(0x3FF0FFFF))
#define USER_DATA_START   ((void*)(0x3FFE8000))
#define USER_DATA_END     ((void*)(0x3FFE8000 + 0x14000))
#define ETS_SYS_START     ((void*)(0x3FFFC000))
#define ETS_SYS_END       ((void*)(0x3FFFC000 + 0x4000))
#define IRAM1_START       ((void*)(0x40100000))
#define IRAM1_END         ((void*)(0x40100000 + 0x8000))
#define IRAMF1_START      ((void*)(0x40108000))
#define IRAMF1_END        ((void*)(0x40108000 + 0x4000))
#define IRAMF2_START      ((void*)(0x4010C000))
#define IRAMF2_END        ((void*)(0x4010C000 + 0x4000))
//extern void *xPortSupervisorStackPointer;

enum sdk_memory_segment
{
    /* Ensure this list exactly matches order in sdk_memory_segment_text */
    mem_map_io = 0,
    thread_local,
    data,
    user_data_ram,
    bss,
    noinit,
    ets_system,
    iram1,
    iramf1,
    iramf2,
    iram,
    iram_text,
    iram_bss,
    init,
    text,
    rodata,
    rtc_data,
    SDK_MEMORY_SEGMENT_COUNT
};

static void*      sdk_memory_segment_start[SDK_MEMORY_SEGMENT_COUNT] = {};
static void*        sdk_memory_segment_end[SDK_MEMORY_SEGMENT_COUNT] = {};
static const char* sdk_memory_segment_text[SDK_MEMORY_SEGMENT_COUNT] = {
    "C memory map io ",
    "* thread_local  ",
    "C data          ",
    "* user data ram ",
    "* bss           ",
    "* noinit        ",
    "C ets system    ",
    "C iram1         ",
    "C iramf1        ",
    "C iramf2        ",
    "* iram          ",
    "* iram_text     ",
    "* iram_bss      ",
    "* init          ",
    "* text          ",
    "* rodata        ",
    "* rtc data      ",
};

/* Given a given memory segment [m]: assign text names, starting and ending
 * addresses. See also sdk_var_whereis() that requires this initialization. */
int sdk_log_meminfo(enum sdk_memory_segment m, void* start, void* end)
{
    const char* str;
    word32 len = 0;
    str = sdk_memory_segment_text[m];
    sdk_memory_segment_start[m] = start;
    sdk_memory_segment_end[m] = end;
    /* For ESP8266 See ./build/[Debug|Release]/esp8266/esp8266.project.ld */
    if (m == SDK_MEMORY_SEGMENT_COUNT) {
        ESP_LOGI(TAG, "                    Linker Memory Map");
        ESP_LOGI(TAG, "-----------------------------------------------------");
        ESP_LOGI(TAG, "                  Start         End          Length");
    }
    else {
        len = (word32)end - (word32)start;
        ESP_LOGI(TAG, "%s: %p ~ %p : 0x%05x (%d)", str, start, end, len, len );
    }
    return ESP_OK;
}

esp_err_t sdk_var_whereis(const char* v_name, void* v);
/* Show all known linker memory segment names, starting & ending addresses. */
int sdk_init_meminfo(void) {
    void* sample_heap_var;
    int sample_stack_var = 0;

    sdk_log_meminfo(SDK_MEMORY_SEGMENT_COUNT, NULL, NULL); /* print header */
    sdk_log_meminfo(mem_map_io,    MEM_MAP_IO_START,    MEM_MAP_IO_END);
    sdk_log_meminfo(thread_local,  _thread_local_start, _thread_local_end);
    sdk_log_meminfo(data,          _data_start,         _data_end);
    sdk_log_meminfo(user_data_ram, USER_DATA_START,     USER_DATA_END);
    sdk_log_meminfo(bss,           _bss_start,          _bss_end);
    sdk_log_meminfo(noinit,        _noinit_start,       _noinit_end);
    sdk_log_meminfo(ets_system,    ETS_SYS_START,       ETS_SYS_END);
    sdk_log_meminfo(rodata,        _rodata_start,       _rodata_end);
    sdk_log_meminfo(iram1,         IRAM1_START,         IRAM1_END);
    sdk_log_meminfo(iramf1,        IRAMF1_START,        IRAMF1_END);
    sdk_log_meminfo(iramf2,        IRAMF2_START,        IRAMF2_END);
    sdk_log_meminfo(iram,          _iram_start,         _iram_end);
    sdk_log_meminfo(iram_text,     _iram_text_start,    _iram_text_end);
    sdk_log_meminfo(iram_bss,      _iram_bss_start,     _iram_bss_end);
    sdk_log_meminfo(init,          _init_start,         _init_end);
    sdk_log_meminfo(text,          _text_start,         _text_end);
    sdk_log_meminfo(rtc_data,      _rtc_data_start,     _rtc_data_end);
    ESP_LOGI(TAG, "-----------------------------------------------------");
    sample_heap_var = malloc(1);
    if (sample_heap_var == NULL) {
        ESP_LOGE(TAG, "Unable to allocate heap memory in sdk_var_whereis().");
    }
    else {
        sdk_var_whereis("sample_stack_var", (void*)&sample_stack_var);
        sdk_var_whereis("sample_heap_var", sample_heap_var);
        free(sample_heap_var);
    }
    return ESP_OK;
}

/* Returns ESP_OK if found in known memory map, ESP_FAIL otherwise */
esp_err_t sdk_var_whereis(const char* v_name, void* v) {
    esp_err_t ret = ESP_FAIL;

    for (enum sdk_memory_segment m = 0 ;m < SDK_MEMORY_SEGMENT_COUNT; m++) {
        if (v >= sdk_memory_segment_start[m] &&
            v <= sdk_memory_segment_end[m]) {
                ret = ESP_OK;
                ESP_LOGI(TAG, "Variable [%s] found at %p in %s", v_name, v,
                              sdk_memory_segment_text[m]);
                if (m == user_data_ram) {

                }
            }
    }

    if (ret == ESP_FAIL) {
        ESP_LOGW(TAG, "%s not found in known memory map: %p", v_name, v);
    }
    return ret;
}

/* for FreeRTOS */
void app_main(void)
{
    int stack_start = 0;
    esp_err_t ret = 0;
    ESP_LOGI(TAG, "---------------- wolfSSL TLS Client Example ------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    sdk_init_meminfo();

#ifdef ESP_TASK_MAIN_STACK
    ESP_LOGI(TAG, "ESP_TASK_MAIN_STACK: %d", ESP_TASK_MAIN_STACK);
#endif
#ifdef TASK_EXTRA_STACK_SIZE
    ESP_LOGI(TAG, "TASK_EXTRA_STACK_SIZE: %d", TASK_EXTRA_STACK_SIZE);
#endif

#ifndef SINGLE_THREADED

#ifdef INCLUDE_uxTaskGetStackHighWaterMark
    ESP_LOGI(TAG, "CONFIG_ESP_MAIN_TASK_STACK_SIZE = %d bytes (%d words)",
                   CONFIG_ESP_MAIN_TASK_STACK_SIZE,
                   (int)(CONFIG_ESP_MAIN_TASK_STACK_SIZE / sizeof(void*)));

    /* Returns the high water mark of the stack associated with xTask. That is,
     * the minimum free stack space there has been (in bytes not words, unlike
     * vanilla FreeRTOS) since the task started. The smaller the returned
     * number the closer the task has come to overflowing its stack.
     * see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html
     */
    stack_start = uxTaskGetStackHighWaterMark(NULL);
    sdk_var_whereis("stack_start", &stack_start);
    ESP_LOGI(TAG, "Stack Start HWM: %d bytes", stack_start);
#endif
#endif

#ifdef HAVE_VERSION_EXTENDED_INFO
    esp_ShowExtendedSystemInfo();
#endif

    /* Set time for cert validation.
     * Some lwIP APIs, including SNTP functions, are not thread safe. */
    ret = set_time(); /* need to setup NTP before WiFi */

    /* Optionally erase flash */
    /* ESP_ERROR_CHECK(nvs_flash_erase()); */

#ifdef FOUND_PROTOCOL_EXAMPLES_DIR
    ESP_LOGI(TAG, "FOUND_PROTOCOL_EXAMPLES_DIR active, using example code.");
    ESP_ERROR_CHECK(nvs_flash_init());

    #if defined(CONFIG_IDF_TARGET_ESP32H2)
        ESP_LOGE(TAG, "There's no WiFi on ESP32-H2.");
    #else
        #ifdef CONFIG_EXAMPLE_WIFI_SSID
            if (XSTRCMP(CONFIG_EXAMPLE_WIFI_SSID, "myssid") == 0) {
                ESP_LOGW(TAG, "WARNING: CONFIG_EXAMPLE_WIFI_SSID is myssid.");
                ESP_LOGW(TAG, "  Do you have a WiFi AP called myssid, or ");
                ESP_LOGW(TAG, "  did you forget the ESP-IDF configuration?");
            }
        #else
            #define CONFIG_EXAMPLE_WIFI_SSID "myssid"
            ESP_LOGW(TAG, "WARNING: CONFIG_EXAMPLE_WIFI_SSID not defined.");
        #endif
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_ERROR_CHECK(example_connect());
    #endif
#else
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize NVS */
    ret = nvs_flash_init();
#if defined(CONFIG_IDF_TARGET_ESP8266)
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
#else
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
#endif
    ESP_ERROR_CHECK(ret);

    #if defined(CONFIG_IDF_TARGET_ESP32H2)
        ESP_LOGE(TAG, "There's no WiFi on ESP32-H2. ");
    #else
        /* Initialize WiFi */
        ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
        ret = wifi_init_sta();
        while (ret != 0) {
            ESP_LOGI(TAG, "Waiting...");
            vTaskDelay(60000 / portTICK_PERIOD_MS);
            ESP_LOGI(TAG, "Trying WiFi again...");
            ret = wifi_init_sta();
        }
    #endif
#endif

    /* Once we are connected to the network, start & wait for NTP time */
    ret = set_time_wait_for_ntp();

    if (ret < -1) {
        /* a value of -1 means there was no NTP server, so no need to wait */
        ESP_LOGI(TAG, "Waiting 10 more seconds for NTP to complete." );
        vTaskDelay(10000 / portTICK_PERIOD_MS); /* brute-force solution */
        esp_show_current_datetime();
    }

    /* HWM is maximum amount of stack space that has been unused, in bytes
     * not words (unlike vanilla freeRTOS). */
#ifndef SINGLE_THREADED

#endif

#ifndef SINGLE_THREADED
    ESP_LOGI(TAG, "Initial Stack Used (before wolfSSL Server): %d bytes",
                   CONFIG_ESP_MAIN_TASK_STACK_SIZE
                   - (uxTaskGetStackHighWaterMark(NULL))
            );
    ESP_LOGI(TAG, "Starting TLS Client task ...\n");
#endif

#if defined(SINGLE_THREADED)
    /* just call the task */
    tls_smp_client_task((void*)NULL);
#else
    tls_args args[1] = {0};
    /* start a thread with the task */
    args[0].loops = 10;
    args[0].port = 11111;

    int this_heap = esp_get_free_heap_size();
    ESP_LOGI(TAG, "main tls_smp_client_init heap @ %p = %d", &this_heap, this_heap);
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

#ifndef SINGLE_THREADED

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

#else
        ESP_LOGV(TAG, "\n\nDone!\n\n");
        while (1);
#endif

} /* app_main */
