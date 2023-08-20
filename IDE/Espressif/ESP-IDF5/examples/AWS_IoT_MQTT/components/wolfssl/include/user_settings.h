/* user_settings.h
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

#include <sdkconfig.h> /* essential to chip set detection */

/* ENABLE_MQTT_TLS is needed but may already be defined in CMake */
#ifndef ENABLE_MQTT_TLS
    #define ENABLE_MQTT_TLS
#endif

/* WOLFMQTT_USER_SETTINGS is needed may already be defined in CMake */
#ifndef WOLFMQTT_USER_SETTINGS
    #define WOLFMQTT_USER_SETTINGS
#endif

#undef WOLFSSL_ESPIDF
#undef WOLFSSL_ESP32
#undef WOLFSSL_ESPWROOM32SE
#undef WOLFSSL_ESP32
#undef WOLFSSL_ESP8266

/* The Espressif sdkconfig will have chipset info.
**
** Possible values:
**
**   CONFIG_IDF_TARGET_ESP32
**   CONFIG_IDF_TARGET_ESP32S2
**   CONFIG_IDF_TARGET_ESP32S3
**   CONFIG_IDF_TARGET_ESP32C3
**   CONFIG_IDF_TARGET_ESP32C6
*/

#define WOLFSSL_ESPIDF

/*
 * choose ONE of these Espressif chips to define:
 *
 * WOLFSSL_ESP32
 * WOLFSSL_ESPWROOM32SE
 * WOLFSSL_ESP8266
 */

#define WOLFSSL_ESP32

/* optionally turn off SHA512/224 SHA512/256 */
/* #define WOLFSSL_NOSHA512_224 */
/* #define WOLFSSL_NOSHA512_256 */

#define BENCH_EMBEDDED
#define USE_CERT_BUFFERS_2048

/* TLS 1.3                                 */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define WC_RSA_PSS
#define HAVE_HKDF
#define HAVE_AEAD
#define HAVE_SUPPORTED_CURVES

#define WOLFSSL_BENCHMARK_FIXED_UNITS_KB

/* when you want to use SINGLE THREAD; ESP-IDF is freeRTOS */
/* #define SINGLE_THREADED */

#define NO_FILESYSTEM

#define HAVE_AESGCM

#define WOLFSSL_RIPEMD
/* when you want to use SHA224 */
/* #define WOLFSSL_SHA224 */

#define NO_OLD_TLS
#define WOLFSSL_SHA3

/* when you want to use SHA384 */
/* #define WOLFSSL_SHA384 */

/* when you DOD NOT want to use SHA256 */
/* #define NO_SHA256 */

#define WOLFSSL_SHA512
#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL
#define HAVE_ED25519

/* when you want to use pkcs7 */
/* #define HAVE_PKCS7 */

#if defined(HAVE_PKCS7)
    #define HAVE_AES_KEYWRAP
    #define HAVE_X963_KDF
    #define WOLFSSL_AES_DIRECT
#endif

/* when you want to use aes counter mode */
/* #define WOLFSSL_AES_DIRECT */
/* #define WOLFSSL_AES_COUNTER */

/* esp32-wroom-32se specific definition */
#if defined(WOLFSSL_ESPWROOM32SE)
    #define WOLFSSL_ATECC508A
    #define HAVE_PK_CALLBACKS
    /* when you want to use a custom slot allocation for ATECC608A */
    /* unless your configuration is unusual, you can use default   */
    /* implementation.                                             */
    /* #define CUSTOM_SLOT_ALLOCATION                              */
#endif

/* rsa primitive specific definition */
#if defined(WOLFSSL_ESP32) || defined(WOLFSSL_ESPWROOM32SE)
    /* Define USE_FAST_MATH and SMALL_STACK                        */
    #define ESP32_USE_RSA_PRIMITIVE
    /* threshold for performance adjustment for HW primitive use   */
    /* X bits of G^X mod P greater than                            */
    #define EPS_RSA_EXPT_XBTIS           32
    /* X and Y of X * Y mod P greater than                         */
    #define ESP_RSA_MULM_BITS            9
#endif
#define RSA_LOW_MEM

/* debug options */
/* #define DEBUG_WOLFSSL */
/* #define WOLFSSL_ESP32_CRYPT_DEBUG */
/* #define WOLFSSL_ATECC508A_DEBUG          */

/* date/time                               */
/* if it cannot adjust time in the device, */
/* enable macro below                      */
/* #define NO_ASN_TIME */
/* #define XTIME time */

/* adjust wait-timeout count if you see timeout in RSA HW acceleration */
#define ESP_RSA_TIMEOUT_CNT    0x249F00

#define HASH_SIZE_LIMIT /* for test.c */

#define USE_FAST_MATH

/* optionally use SP_MATH */
/* #define SP_MATH */

#define WOLFSSL_SMALL_STACK

#define HAVE_VERSION_EXTENDED_INFO
#define HAVE_WC_INTROSPECTION

/* allows for all version info, even that suppressed with introspection */
#define ALLOW_BINARY_MISMATCH_INTROSPECTION

/* Default is HW enabled unless turned off.
** Uncomment these lines for SW: */
#if defined(CONFIG_IDF_TARGET_ESP32)
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#else
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#endif
/* optional SM4 Ciphers. See https://github.com/wolfSSL/wolfsm */
/*
#define WOLFSSL_SM2
#define WOLFSSL_SM3
#define WOLFSSL_SM4
*/

/* this is for example code */
#define NO_MAIN_DRIVER

/* VS include follows: */
#ifndef _WOLFMQTT_VS_SETTINGS_
#define _WOLFMQTT_VS_SETTINGS_

/* Don't include this if using autoconf cross-compile */
#if defined(HAVE_CONFIG_H)
    #warning  "Unexpected use of config.h"
#else
/* TLS Support */
#undef  ENABLE_MQTT_TLS
#define ENABLE_MQTT_TLS

/* MQTT-SN Support */
#undef  WOLFMQTT_SN
#define WOLFMQTT_SN

/* MQTT v5.0 support */
#undef  WOLFMQTT_V5
#define WOLFMQTT_V5

/* Enable property callback support */
#ifdef WOLFMQTT_V5
    #undef  WOLFMQTT_PROPERTY_CB
    #define WOLFMQTT_PROPERTY_CB
#endif

/* Non-blocking support */
#undef  WOLFMQTT_NONBLOCK
#define WOLFMQTT_NONBLOCK

/* Disable socket timeout code */
//#undef  WOLFMQTT_NO_TIMEOUT
//#define WOLFMQTT_NO_TIMEOUT

/* Disconnect callback support */
#undef  WOLFMQTT_DISCONNECT_CB
#define WOLFMQTT_DISCONNECT_CB

/* Multi-threading */
#undef  WOLFMQTT_MULTITHREAD
#define WOLFMQTT_MULTITHREAD

/* Debugging */
/*
#undef  DEBUG_WOLFMQTT
#define DEBUG_WOLFMQTT

#undef  WOLFMQTT_DEBUG_CLIENT
#define WOLFMQTT_DEBUG_CLIENT

#undef  WOLFMQTT_DEBUG_SOCKET
#define WOLFMQTT_DEBUG_SOCKET

#undef  WOLFMQTT_DEBUG_THREAD
#define WOLFMQTT_DEBUG_THREAD
*/

/* Disable error strings */
/*
#undef  WOLFMQTT_NO_ERROR_STRINGS
#define WOLFMQTT_NO_ERROR_STRINGS
*/

#endif /* !HAVE_CONFIG_H */

#endif /* _WOLFMQTT_VS_SETTINGS_ */

