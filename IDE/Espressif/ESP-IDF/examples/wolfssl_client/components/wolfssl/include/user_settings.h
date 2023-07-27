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

#define SHOW_SSID_AND_PASSWORD /* remove this to not show in startup log */

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
#include <sdkconfig.h> /* essemtial for target chip detection */

#define WOLFSSL_ESPIDF

/*
 * choose ONE of these Espressif chips to define:
 *
 * WOLFSSL_ESP32
 * WOLFSSL_ESPWROOM32SE
 * WOLFSSL_ESP8266
 */
#define WOLFSSL_ESP32

/* #define DEBUG_WOLFSSL_VERBOSE */

#define BENCH_EMBEDDED

/* TLS 1.3                                 */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define WC_RSA_PSS
#define HAVE_HKDF
#define HAVE_AEAD
#define HAVE_SUPPORTED_CURVES

/* when you want to use SINGLE THREAD */
/* #define SINGLE_THREADED */
#define NO_FILESYSTEM

#define HAVE_AESGCM

/* when you want to use SHA384 */
/* #define WOLFSSL_SHA384 */

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

/* RSA primitive specific definition */
#if defined(WOLFSSL_ESP32) || defined(WOLFSSL_ESPWROOM32SE)
    /* Define USE_FAST_MATH and SMALL_STACK                        */
    #define ESP32_USE_RSA_PRIMITIVE
    /* threshold for performance adjustment for HW primitive use   */
    /* X bits of G^X mod P greater than                            */
    #define EPS_RSA_EXPT_XBTIS           36
    /* X and Y of X * Y mod P greater than                         */
    #define ESP_RSA_MULM_BITS            2000
#endif

/* debug options */
#define DEBUG_WOLFSSL
/* #define WOLFSSL_ESP32_CRYPT_DEBUG */
/* #define WOLFSSL_ATECC508A_DEBUG          */

/* date/time                               */
/* if it cannot adjust time in the device, */
/* enable macro below                      */
/* #define NO_ASN_TIME */
/* #define XTIME time */

/* when you want not to use HW acceleration */
/* #define NO_ESP32_CRYPT */
/* #define NO_WOLFSSL_ESP32_CRYPT_HASH*/
/* #define NO_WOLFSSL_ESP32_CRYPT_AES */
/* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */

/* adjust wait-timeout count if you see timeout in RSA HW acceleration */
#define ESP_RSA_TIMEOUT_CNT    0x249F00

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !defined(NO_WOLFSSL_ESP32_CRYPT_AES)
    /* AES192 is not supported on the ESP32-S3 HW at this time */
    #define NO_AES_192
    /* TODO remove upon merge of https://github.com/wolfSSL/wolfssl/pull/6624 */
#endif

#if defined(CONFIG_IDF_TARGET_ESP32)
    /* HW Enabled by default for ESP32. To disable: */
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    /* HW Disabled by default for ESP32-S2.   */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    /* HW Enabled by default for ESP32. To disable: */
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    /* HW Disabled by default for ESP32-C3.   */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    /* HW Disabled by default for ESP32-C6.   */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#elif defined(CONFIG_IDF_TARGET_ESP32H2)
    /* HW Disabled by default for ESP32-H2.   */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#else
    /* HW Disabled by default for all other ESP32-[?].  */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#endif

/* see esp_ShowExtendedSystemInfo in esp32-crypt.h for startup log info */
#define HAVE_VERSION_EXTENDED_INFO

/* optional SM4 Ciphers. See https://github.com/wolfSSL/wolfsm */
#define WOLFSSL_SM2
#define WOLFSSL_SM3
#define WOLFSSL_SM4

#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)
    /* SM settings */
//    #define WOLFSSL_BASE16 /* required for WOLFSSL_SM2 */
//
//    #undef  WOLFSSL_SM4_ECB
//    #define WOLFSSL_SM4_ECB
//
//    #undef  WOLFSSL_SM4_CBC
//    #define WOLFSSL_SM4_CBC
//
//    #undef  WOLFSSL_SM4_CTR
//    #define WOLFSSL_SM4_CTR
//
//    #undef  WOLFSSL_SM4_GCM
//    #define WOLFSSL_SM4_GCM
//
//    #undef  WOLFSSL_SM4_CCM
//    #define WOLFSSL_SM4_CCM
//
//    #define HAVE_POLY1305
//    #define HAVE_CHACHA
//
//    #undef  HAVE_AESGCM
//    #define HAVE_AESGCM
#else
    /* default settings */
    #define USE_CERT_BUFFERS_2048
#endif
