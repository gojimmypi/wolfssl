/* version.c
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
#include <wolfssl/version.h>


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_ESPIDF)
    #include <esp_log.h>
    #include "sdkconfig.h"
    const char* TAG = "Version";

int ShowExtendedVersionInfo() {
    ESP_LOGI(TAG, "Extended Version and Platform Information");

#if defined(WOLFSSL_MULTI_INSTALL_WARNING)
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "WARNING: Multiple wolfSSL installs found.");
    ESP_LOGI(TAG, "Check ESP-IDF and local project [components] directory.");
    ESP_LOGI(TAG, "");
#endif

    ESP_LOGI(TAG, "CONFIG_IDF_TARGET = %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_STRING = %s", LIBWOLFSSL_VERSION_STRING);

#if defined(LIBWOLFSSL_VERSION_GIT_HASH)
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_HASH = %s",
                   LIBWOLFSSL_VERSION_GIT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_SHORT_HASH )
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_SHORT_HASH = %s",
                   LIBWOLFSSL_VERSION_GIT_SHORT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_HASH_DATE)
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_HASH_DATE = %s",
                  LIBWOLFSSL_VERSION_GIT_HASH_DATE);
#endif

    /* some interesting settings are target specific (ESP32, -C3, -S3, etc */
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    /* not available for C3 at this time */
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    ESP_LOGI(TAG, "CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ = %u MHz",
                   CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
             );
    ESP_LOGI(TAG, "Xthal_have_ccount = %u", Xthal_have_ccount);
#else
    ESP_LOGI(TAG, "CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ = %u MHz",
                   CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
            );
    ESP_LOGI(TAG, "Xthal_have_ccount = %u", Xthal_have_ccount);
#endif

    /* all platforms: stack high water mark check */
#if defined(SINGLE_THREADED)
    /* need stack monitor for single thread */
#else
    ESP_LOGI(TAG, "Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
#endif

    /* check to see if we are using hardware encryption */
#if defined(NO_ESP32WROOM32_CRYPT)
    ESP_LOGI(TAG, "NO_ESP32WROOM32_CRYPT defined! HW acceleration DISABLED.");
#else
    #if defined(CONFIG_IDF_TARGET_ESP32C3)
        #error "ESP32WROOM32_CRYPT not yet supported on ESP32-C3"
    #elif defined(CONFIG_IDF_TARGET_ESP32S2)
        #error "ESP32WROOM32_CRYPT not yet supported on ESP32-S2"
    #elif defined(CONFIG_IDF_TARGET_ESP32S3)
        #error "ESP32WROOM32_CRYPT not yet supported on ESP32-S3"
    #else
        ESP_LOGI(TAG, "ESP32WROOM32_CRYPT is enabled.");
    #endif

    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH)
        ESP_LOGI(TAG, "NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH is defined!"
                      "(disabled HW SHA).");
    #endif

    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_AES)
        ESP_LOGI(TAG, "NO_WOLFSSL_ESP32WROOM32_CRYPT_AES is defined!"
                      "(disabled HW AES).");
    #endif

    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI)
        ESP_LOGI(TAG, "NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI defined!"
                      "(disabled HW RSA)");
    #endif
#endif
    return 0;
}

#else
    printf("show_extended_version_info() not implemented for this platform.")
#endif


