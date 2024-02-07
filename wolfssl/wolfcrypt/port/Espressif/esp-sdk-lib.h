/* esp-sdk-lib.h
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
#ifndef __ESP_SDK_LIB_H__

#define __ESP_SDK_LIB_H__

#include <wolfssl/wolfcrypt/settings.h> /* references user_settings.h */

#if defined(WOLFSSL_ESPIDF) /* Entire file is only for Espressif EDP-IDF */

#ifndef WOLFSSL_USER_SETTINGS
    #error  "WOLFSSL_USER_SETTINGS must be defined for Espressif targts"
#endif

#include "sdkconfig.h" /* ensure ESP-IDF settings are available everywhere */

WOLFSSL_LOCAL esp_err_t esp_sdk_time_mem_init(void);
WOLFSSL_LOCAL esp_err_t esp_sdk_time_lib_init(void);
WOLFSSL_LOCAL esp_err_t esp_sdk_wifi_lib_init(void);

/* Check for traps */
#if !defined(NO_SESSION_CACHE) && !defined(MICRO_SESSION_CACHE) && \
    !defined(SMALL_SESSION_CACHE)
    #warning "Limited DRAM/IRAM on ESP8266. Check session cache settings"
#endif

#endif /* WOLFSSL_ESPIDF */

#endif /* __ESP32_CRYPT_H__ */
