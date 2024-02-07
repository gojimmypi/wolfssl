/* esp_sdk_wifi_lib.c
 *
 * Copyright (C) 2006-2024 wolfSSL Inc.
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* Reminder: user_settings.h is needed and included from settings.h
 * Be sure to define WOLFSSL_USER_SETTINGS, typically in CMakeLists.txt */
#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_ESPIDF) /* Entire file is only for Espressif EDP-IDF */

/* Espressif */
#include "sdkconfig.h" /* programmatically generated from sdkconfig */
#include <esp_log.h>
#include <esp_err.h>

/* wolfSSL */
#include <wolfssl/wolfcrypt/port/Espressif/esp-sdk-lib.h>

#define ESP_SDK_WIFI_LIB_VERSION 1

static const char* TAG = "wifi lib";

esp_err_t esp_sdk_wifi_lib_init(void)
{
    int ret = ESP_OK;
    ESP_LOGI(TAG, "esp_sdk_wifi_lib_init Ver %d", ESP_SDK_WIFI_LIB_VERSION);
    return ret;
}

#endif
