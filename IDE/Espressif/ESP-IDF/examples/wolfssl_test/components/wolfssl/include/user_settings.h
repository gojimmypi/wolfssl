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
#include "sdkconfig.h"

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
#include "sdkconfig.h"

/* when you want to use SINGLE THREAD. Note Default ESP-IDF is FreeRTOS */
/* #define SINGLE_THREADED */

/* #define ESP_DISABLE_HW_TASK_LOCK */

#define WOLFSSL_ESPIDF

/*
 * choose ONE of these Espressif chips to define:
 *
 * WOLFSSL_ESP32
 * WOLFSSL_ESPWROOM32SE
 * WOLFSSL_ESP8266
 */

#define WOLFSSL_ESP32
/* #define WOLFSSL_NOSHA512_224 */
/* #define WOLFSSL_NOSHA512_256 */

/* When you don't want to use the old SHA */
/* #define NO_SHA */
/* #define NO_OLD_TLS */

#define BENCH_EMBEDDED

/* TLS 1.3                                 */
#define WOLFSSL_TLS13
#define HAVE_TLS_EXTENSIONS
#define WC_RSA_PSS
#define HAVE_HKDF
#define HAVE_AEAD
#define HAVE_SUPPORTED_CURVES

#define WOLFSSL_BENCHMARK_FIXED_UNITS_KB

#define NO_FILESYSTEM

 #define HAVE_AESGCM

#define WOLFSSL_RIPEMD
/* when you want to use SHA224 */
#define WOLFSSL_SHA224

/* when you want to use SHA384 */
#define WOLFSSL_SHA3

#ifdef CONFIG_IDF_TARGET_ESP32C3
    /* TODO there's no 384/512 HW, but allow fallback to SW */
    #define WOLFSSL_SHA384
    #define WOLFSSL_SHA512
    #define HAVE_ED25519 /* ED25519 requires SHA512 */
#else
    #define WOLFSSL_SHA384
    #define WOLFSSL_SHA512
    #define HAVE_ED25519
#endif

#define HAVE_ECC
#define HAVE_CURVE25519
#define CURVE25519_SMALL

 #define OPENSSL_EXTRA
/* when you want to use pkcs7 */

#define HAVE_PKCS7

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
    #define EPS_RSA_EXPT_XBTIS           0 /* NOTE HW unreliable for small values! */
    /* X and Y of X * Y mod P greater than                         */
    #define ESP_RSA_MULM_BITS            0
#endif


/* #define WOLFSSL_ATECC508A_DEBUG          */

/* date/time                               */
/* if it cannot adjust time in the device, */
/* enable macro below                      */
/* #define NO_ASN_TIME */
/* #define XTIME time */


/* adjust wait-timeout count if you see timeout in RSA HW acceleration */
#define ESP_RSA_TIMEOUT_CNT    0x249F00

/* #define HASH_SIZE_LIMIT */ /* for test.c */

#define USE_FAST_MATH
// #define SP_MATH

#define WOLFSSL_SMALL_STACK


#define HAVE_VERSION_EXTENDED_INFO
/* #define HAVE_WC_INTROSPECTION */

#define  HAVE_SESSION_TICKET

/* #define HAVE_HASHDRBG */

/* Shared configuration in same directory */
/* #include "Wolf_Features.h" */

#define WOLFSSL_KEY_GEN
#define WOLFSSL_CERT_REQ
#define WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_EXT
#define WOLFSSL_SYS_CA_CERTS


#define WOLFSSL_CERT_TEXT

#define WOLFSSL_ASN_TEMPLATE

/*
#undef  WOLFSSL_KEY_GEN

#undef  WOLFSSL_CERT_REQ

#undef  WOLFSSL_CERT_GEN

#undef  WOLFSSL_CERT_EXT

#undef  WOLFSSL_SYS_CA_CERTS
*/

/*
--enable-keygen
--enable-certgen
--enable-certreq
--enable-certext
--enable-asn-template
*/



/* Default is HW enabled unless turned off.
** Uncomment these lines for SW: */

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !defined(NO_WOLFSSL_ESP32_CRYPT_AES)
    /* AES192 is not supported on the ESP32-S3 HW at this time */
   // #define NO_AES_192
#endif

#if defined(CONFIG_IDF_TARGET_ESP32)
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
    // #define DEBUG_WOLFSSL_SHA_MUTEX
    // #define WOLFSSL_TEST_STRAY 1

    /*  These are defined automatically in esp32-crypt.h, here for clarity:  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA224 /* no SHA223 HW on ESP32  */
    /* end CONFIG_IDF_TARGET_ESP32C3 */
    #undef  ESP_RSA_MULM_BITS
    #define ESP_RSA_MULM_BITS 16
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
    /* end CONFIG_IDF_TARGET_ESP32S2 */

#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    /* #define NO_ESP32_CRYPT                 */
    /* #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /* #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
    /* end CONFIG_IDF_TARGET_ESP32S3 */

#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    /*  wolfSSL Hardware Acceleration is enabled unless otherwise turned off:*/
    /*  #define NO_ESP32_CRYPT                 */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_HASH    */ /* to disable all SHA HW   */
    /*  These are defined automatically in esp32-crypt.h, here for clarity:  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA384    /* no SHA384 HW on C6  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA512    /* no SHA512 HW on C6  */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_AES             */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI         */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MP_MUL  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MULMOD  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_EXPTMOD */
    /* end CONFIG_IDF_TARGET_ESP32C3 */

#elif defined(CONFIG_IDF_TARGET_ESP32C6)
    /*  wolfSSL Hardware Acceleration is enabled unless otherwise turned off: */

    /*  #define NO_ESP32_CRYPT                 */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /*  These are defined automatically in esp32-crypt.h, here for clarity:  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA384    /* no SHA384 HW on C6  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA512    /* no SHA512 HW on C6  */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI         */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MP_MUL  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MULMOD  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_EXPTMOD */

    /*  #define NO_WOLFSSL_ESP32_CRYPT_AES             */
    /* end CONFIG_IDF_TARGET_ESP32C6 */

#elif defined(CONFIG_IDF_TARGET_ESP32H2)
/*  wolfSSL Hardware Acceleration not yet implemented */
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI

    /* end CONFIG_IDF_TARGET_ESP32H2 */
#else
    #define NO_ESP32_CRYPT
    #define NO_WOLFSSL_ESP32_CRYPT_HASH
    #define NO_WOLFSSL_ESP32_CRYPT_AES
    #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI
#endif

/* debug options */
#define WOLFSSL_ESPIDF_ERROR_PAUSE /* Pause in a loop rather than exit. */
//#define ESP_VERIFY_MEMBLOCK
#define WOLFSSL_HW_METRICS
//#define DEBUG_WOLFSSL_VERBOSE
//#define DEBUG_WOLFSSL
//#define WOLFSSL_ESP32_CRYPT_DEBUG
//#define NO_RECOVER_SOFTWARE_CALC

/* optionally turn off individual math HW acceleration features */

/* Turn off Large Number Multiplication:
** [Z = X * Y] in esp_mp_mul()                                  */
/* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MP_MUL         */

/* Turn off Large Number Modular Exponentiation:
** [Z = X^Y mod M] in esp_mp_exptmod()                          */
/* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_EXPTMOD        */

/* Turn off Large Number Modular Multiplication
** [Z = X × Y mod M] in esp_mp_mulmod()                         */
/* #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MULMOD         */


/* #define HONOR_MATH_USED_LENGTH     */ /* this is known to fail in TFM */
/* #define CHECK_MP_READ_UNSIGNED_BIN */ /* this is known to fail in TFM */
#define WOLFSSL_PUBLIC_MP /* used by benchmark */
#define USE_CERT_BUFFERS_2048

/* Optionally include alternate HW test library: alt_hw_test.h */
/* When enabling, the ./components/wolfssl/CMakeLists.txt file
 * will need the name of the library in the idf_component_register
 * for the PRIV_REQUIRES list. */
/* #define INCLUDE_ALT_HW_TEST */

/* #define NO_HW_MATH_TEST */


/* when turning on ECC508 / ECC608 support
#define WOLFSSL_ESPWROOM32SE
#define HAVE_PK_CALLBACKS
#define WOLFSSL_ATECC508A
#define ATCA_WOLFSSL
*/

/* USE_FAST_MATH is default */

/* use SP_MATH */
// #undef USE_FAST_MATH
// #define WOLFSSL_SP_MATH_ALL

/* use integer heap math */
//#undef USE_FAST_MATH
//#define USE_INTEGER_HEAP_MATH

// #define USE_ESP_DPORT_ACCESS_READ_BUFFER


// #define WOLFSSL_BASE16 /* required for WOLFSSL_SM2 */
// #define WOLFSSL_SM2
// #define WOLFSSL_SM3
// #define WOLFSSL_SM4
// #define WOLFSSL_ESP32_HW_LOCK_DEBUG
