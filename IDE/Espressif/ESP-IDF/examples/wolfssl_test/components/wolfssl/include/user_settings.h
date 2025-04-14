/* wolfssl-component include/user_settings.h
 *
 * Copyright (C) 2006-2024 wolfSSL Inc.
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
#define WOLFSSL_ESPIDF_COMPONENT_VERSION 0x01

/* The Espressif project config file. See also sdkconfig.defaults */
#include "sdkconfig.h"

/* This user_settings.h is for Espressif ESP-IDF
 *
 * Standardized wolfSSL Espressif ESP32 + ESP8266 user_settings.h V5.7.0-1
 *
 * Do not include any wolfssl headers here.
 *
 * When editing this file:
 * ensure all examples match. The template example is the reference.
 */

/* Naming convention: (see also esp32-crypt.h for the reference source).
 *
 * CONFIG_
 *      This prefix indicates the setting came from the sdkconfig / Kconfig.
 *
 *      May or may not be related to wolfSSL.
 *
 *      The name after this prefix must exactly match that in the Kconfig file.
 *
 * WOLFSSL_
 *      Typical of many, but not all wolfSSL macro names.
 *
 *      Applies to all wolfSSL products such as wolfSSH, wolfMQTT, etc.
 *
 *      May or may not have a corresponding sdkconfig / Kconfig control.
 *
 * ESP_WOLFSSL_
 *      These are NOT valid wolfSSL macro names. These are names only used in
 *      the ESP-IDF Kconfig files. When parsed, they will have a "CONFIG_"
 *      suffix added. See next section.
 *
 * CONFIG_ESP_WOLFSSL_
 *      This is a wolfSSL-specific macro that has been defined in the ESP-IDF
 *      via the sdkconfig / menuconfig. Any text after this prefix should
 *      exactly match an existing wolfSSL macro name.
 *
 *      Applies to all wolfSSL products such as wolfSSH, wolfMQTT, etc.
 *
 *      These macros may also be specific to only the project or environment,
 *      and possibly not used anywhere else in the wolfSSL libraries.
 */

/* The Espressif sdkconfig will have chipset info.
**
** Some possible values:
**
**   CONFIG_IDF_TARGET_ESP32
**   CONFIG_IDF_TARGET_ESP32S2
**   CONFIG_IDF_TARGET_ESP32S3
**   CONFIG_IDF_TARGET_ESP32C3
**   CONFIG_IDF_TARGET_ESP32C6
*/

#undef  WOLFSSL_ESPIDF
#define WOLFSSL_ESPIDF

/* Test various user_settings between applications by selecting example apps
 * in `idf.py menuconfig` for Example wolfSSL Configuration settings: */

/* Turn on messages that are useful to see only in examples. */
//#define WOLFSSL_EXAMPLE_VERBOSITY

/* Paths can be long, ensure the entire value printed during debug */
#define WOLFSSL_MAX_ERROR_SZ 500


/* wolfSSL Examples: set macros used in example applications.
 *
 * These Settings NOT available in ESP-IDF (e.g. esp-tls)
 *
 * Any settings needed by ESP-IDF components should be explicitly set,
 * and not by these example-specific settings via CONFIG_WOLFSSL_EXAMPLE_n
 *
 * ESP-IDF settings should be Kconfig "CONFIG_[name]" values when possible. */


/* Other applications detected by cmake */


/* #warning "App config undetected" */
//#define FP_MAX_BITS (8192 * 2)
//#define FP_MAX_BITS MIN_FFDHE_FP_MAX_BITS
#define FP_MAX_BITS (4096 * 2)
//#define HAVE_ALPN
#define HAVE_SNI
//#define OPENSSL_EXTRA_X509_SMALL
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
//#define OPENSSL_EXTRA
#ifndef WOLFSSL_ALWAYS_VERIFY_CB
    #define WOLFSSL_ALWAYS_VERIFY_CB
#endif
#ifndef WOLFSSL_VERIFY_CB_ALL_CERTS
    #define WOLFSSL_VERIFY_CB_ALL_CERTS
#endif
#ifndef KEEP_PEER_CERT
    #define KEEP_PEER_CERT
#endif


/* Experimental Kyber */
#if 0
//#ifdef CONFIG_WOLFSSL_ENABLE_KYBER
    /* Kyber typically needs a minimum 10K stack */
    #define WOLFSSL_EXPERIMENTAL_SETTINGS
    #define WOLFSSL_HAVE_KYBER
    #define WOLFSSL_WC_KYBER
    #define WOLFSSL_SHA3
    #if defined(CONFIG_IDF_TARGET_ESP8266)
        /* With limited RAM, we'll disable some of the Kyber sizes: */
        #define WOLFSSL_NO_KYBER1024
        #define WOLFSSL_NO_KYBER768
        #define NO_SESSION_CACHE
    #endif
#endif

/* Pick a cert buffer size: */

#define USE_CERT_BUFFERS_2048
/* #define USE_CERT_BUFFERS_1024 */

/* The Espressif sdkconfig will have chipset info.
**
** Some possible values:
**
**   CONFIG_IDF_TARGET_ESP32
**   CONFIG_IDF_TARGET_ESP32S2
**   CONFIG_IDF_TARGET_ESP32S3
**   CONFIG_IDF_TARGET_ESP32C3
**   CONFIG_IDF_TARGET_ESP32C6
*/

/* Optionally enable Apple HomeKit from compiler directive or Kconfig setting */
#if defined(WOLFSSL_APPLE_HOMEKIT) || defined(CONFIG_WOLFSSL_APPLE_HOMEKIT)
     /* SRP is known to need 8K; slow on some devices */
     #define FP_MAX_BITS (8192 * 2)
     #define WOLFCRYPT_HAVE_SRP
     #define HAVE_CHACHA
     #define HAVE_POLY1305
     #define WOLFSSL_BASE64_ENCODE
 #endif /* Apple HomeKit settings */

/* Used by ESP-IDF components: */
//#if defined(CONFIG_ESP_TLS_USING_WOLFSSL)
#if 0
    /* The ESP-TLS */
    #ifndef FP_MAX_BITS
        #if defined(CONFIG_IDF_TARGET_ESP32C2) || \
            defined(CONFIG_IDF_TARGET_ESP8684) || \
            defined(CONFIG_IDF_TARGET_ESP8266)
            /* Optionally set smaller size here */
            #define FP_MAX_BITS MIN_FFDHE_FP_MAX_BITS
        #else
            #define FP_MAX_BITS (4096 * 2)
        #endif
    #endif
    #define HAVE_ALPN
    #ifndef CONFIG_IDF_TARGET_ESP8266
        /* Unless installed in the ESP8266 RTOS SDK locally, the wolfSSL
         * API for SNI will not be seen in the components/esp-tls layer.
         * Only enable SNI for non-ESP8266 targets by default: */
        #define HAVE_SNI
    #endif
    #define OPENSSL_EXTRA_X509_SMALL

    #define HAVE_TLS_EXTENSIONS
    #define HAVE_SUPPORTED_CURVES
#endif

/* Optionally enable some wolfSSH settings */
#if defined(ESP_ENABLE_WOLFSSH) || defined(CONFIG_ESP_ENABLE_WOLFSSH)
    /* The default SSH Windows size is massive for an embedded target.
     * Limit it: */
    #define DEFAULT_WINDOW_SZ 2000

    /* These may be defined in cmake for other examples: */
    #undef  WOLFSSH_TERM
    #define WOLFSSH_TERM

    /* optional debug */
    /* #undef  DEBUG_WOLFSSH */
    /* #define DEBUG_WOLFSSH */

    #undef  WOLFSSL_KEY_GEN
    #define WOLFSSL_KEY_GEN

    #undef  WOLFSSL_PTHREADS
    #define WOLFSSL_PTHREADS

    #define WOLFSSH_TEST_SERVER
    #define WOLFSSH_TEST_THREADING
#endif /* ESP_ENABLE_WOLFSSH */


/* Not yet using WiFi lib, so don't compile in the esp-sdk-lib WiFi helpers: */
/* #define USE_WOLFSSL_ESP_SDK_WIFI */

/*
 * ONE of these Espressif chip families will be detected from sdkconfig:
 *
 * WOLFSSL_ESP32
 * WOLFSSL_ESPWROOM32SE
 * WOLFSSL_ESP8266
 *
 * following ifdef detection only for syntax highlighting:
 */
#ifdef WOLFSSL_ESPWROOM32SE
    #undef WOLFSSL_ESPWROOM32SE
#endif
#ifdef WOLFSSL_ESP8266
    #undef WOLFSSL_ESP8266
#endif
#ifdef WOLFSSL_ESP32
    #undef WOLFSSL_ESP32
#endif
/* See below for chipset detection from sdkconfig.h */

/* when you want to use SINGLE THREAD. Note Default ESP-IDF is FreeRTOS */
#define SINGLE_THREADED

/* Small session cache saves a lot of RAM for ClientCache and SessionCache.
 * Memory requirement is about 5KB, otherwise 20K is needed when not specified.
 * If extra small footprint is needed, try MICRO_SESSION_CACHE (< 1K)
 * When really desperate or no TLS used, try NO_SESSION_CACHE.  */
//#define NO_SESSION_CACHE

/* Small Stack uses more heap. */
#define WOLFSSL_SMALL_STACK

/* Full debugging turned off, but show malloc failure detail */
#define DEBUG_WOLFSSL
/* #define DEBUG_WOLFSSL_MALLOC*/

/* See test.c that sets cert buffers; we'll set them here: */
//#define USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_2048

/* RSA_LOW_MEM: Half as much memory but twice as slow. */
//#define RSA_LOW_MEM


//#define XMALLOC
/* optionally turn off SHA512/224 SHA512/256 */
/* #define WOLFSSL_NOSHA512_224 */
/* #define WOLFSSL_NOSHA512_256 */

/* when you want to use SINGLE THREAD. Note Default ESP-IDF is FreeRTOS */
/* #define SINGLE_THREADED */

/* When you don't want to use the old SHA */
/* #define NO_SHA */
/* #define NO_OLD_TLS */

#define BENCH_EMBEDDED

/* TLS 1.3                                 */
#ifdef CONFIG_WOLFSSL_ALLOW_TLS13
    #define WOLFSSL_TLS13
    #define HAVE_TLS_EXTENSIONS
    #define HAVE_HKDF

    /* May be required */
    #ifndef HAVE_AEAD
    #endif

    /* Required for ECC */
    //#define HAVE_SUPPORTED_CURVES

    /* Required for RSA */
    #define WC_RSA_PSS

    /* TLS 1.3 normally requires HAVE_FFDHE */
    #if defined(HAVE_FFDHE_2048) || \
        defined(HAVE_FFDHE_3072) || \
        defined(HAVE_FFDHE_4096) || \
        defined(HAVE_FFDHE_6144) || \
        defined(HAVE_FFDHE_8192)
    #else
        #define HAVE_FFDHE_2048
        /* #error "TLS 1.3 requires HAVE_FFDHE_[nnnn]" */
    #endif
#endif

// #if defined(CONFIG_IDF_TARGET_ESP32C2) || defined(CONFIG_IDF_TARGET_ESP8684)
//
//     /* Optionally set smaller size here */
//     #define HAVE_FFDHE_4096
// #else
//     #define HAVE_FFDHE_4096
// #endif


#define NO_FILESYSTEM

#define NO_OLD_TLS

#define HAVE_AESGCM

/* Optional RIPEMD: RACE Integrity Primitives Evaluation Message Digest */
/* #define WOLFSSL_RIPEMD */

/* when you want to use SHA224 */
//#define WOLFSSL_SHA224

/* when you want to use SHA384 */
#define WOLFSSL_SHA384

/* Some features not enabled for ESP8266: */
#if defined(CONFIG_IDF_TARGET_ESP8266) || \
    defined(CONFIG_IDF_TARGET_ESP32C2)
    /* Some known low-memory devices have features not enabled by default. */
    /* TODO determine low memory configuration for ECC. */
#else
    /* when you want to use SHA512 */
    //#define WOLFSSL_SHA512

    /* when you want to use SHA3 */
    /* #define WOLFSSL_SHA3 */

    /* ED25519 requires SHA512 */
    //#define HAVE_ED25519
#endif

#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(CONFIG_IDF_TARGET_ESP32C2)
    #define MY_USE_ECC 0
    #define MY_USE_RSA 1
#else
    #define MY_USE_ECC 1
    #define MY_USE_RSA 0
#endif

/* We can use either or both ECC and RSA, but must use at least one. */
#if MY_USE_ECC || MY_USE_RSA
    #if MY_USE_ECC
        /* ---- ECDSA / ECC ---- */
        #define HAVE_ECC
        // #define HAVE_CURVE25519
        // #define HAVE_ED25519
        //#define WOLFSSL_SHA512

        #define HAVE_ECC384
        #define CURVE25519_SMALL

    #else
        #define WOLFSSH_NO_ECC
        /* WOLFSSH_NO_ECDSA is typically defined automatically,
         * here for clarity: */
        #define WOLFSSH_NO_ECDSA
    #endif

    #if MY_USE_RSA
        /* ---- RSA ----- */
        /* #define RSA_LOW_MEM */

        /* DH disabled by default, needed if ECDSA/ECC also turned off */
        //#define HAVE_DH
    #else
        #define WOLFSSH_NO_RSA
    #endif
#else
    #error "Either RSA or ECC must be enabled"
#endif

/* Optional OpenSSL compatibility */
/* #define OPENSSL_EXTRA */

/* #Optional HAVE_PKCS7 */
/* #define HAVE_PKCS7 */

#if defined(HAVE_PKCS7)
    /* HAVE_PKCS7 may enable HAVE_PBKDF2 see settings.h */
    #define NO_PBKDF2

    #define HAVE_AES_KEYWRAP
    #define HAVE_X963_KDF
    #define WOLFSSL_AES_DIRECT
#endif

/* when you want to use AES counter mode */
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

/* WC_NO_CACHE_RESISTANT: slower but more secure */
/* #define WC_NO_CACHE_RESISTANT */

/* TFM_TIMING_RESISTANT: slower but more secure */
//#define TFM_TIMING_RESISTANT

/* #define WOLFSSL_ATECC508A_DEBUG         */

/* date/time                               */
/* if it cannot adjust time in the device, */
/* enable macro below                      */
/* #define NO_ASN_TIME */
/* #define XTIME time */


/* Adjust wait-timeout count if you see timeout in RSA HW acceleration.
 * Set to very large number and enable WOLFSSL_HW_METRICS to determine max. */
#ifndef ESP_RSA_TIMEOUT_CNT
	#define ESP_RSA_TIMEOUT_CNT 0xFF0000
#endif

/* hash limit for test.c */
//#define HASH_SIZE_LIMIT

/* USE_FAST_MATH is default */
#define USE_FAST_MATH


/* Just syntax highlighting to check math libraries: */
#if defined(SP_MATH)               || \
    defined(USE_INTEGER_HEAP_MATH) || \
    defined(USE_INTEGER_HEAP_MATH) || \
    defined(USE_FAST_MATH)         || \
    defined(WOLFSSL_SP_MATH_ALL)   || \
    defined(WOLFSSL_SP_RISCV32)
#endif


//#define HAVE_VERSION_EXTENDED_INFO
/* #define HAVE_WC_INTROSPECTION */
#define MICRO_SESSION_CACHE
#ifndef NO_SESSION_CACHE
    #define  HAVE_SESSION_TICKET
#endif

/* #define HAVE_HASHDRBG */



#define WOLFSSL_ASN_TEMPLATE



/* optional SM4 Ciphers. See https://github.com/wolfSSL/wolfsm */
/*
#define WOLFSSL_SM2
#define WOLFSSL_SM3
#define WOLFSSL_SM4
*/

#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)
    /* SM settings, possible cipher suites:

        TLS13-AES128-GCM-SHA256
        TLS13-CHACHA20-POLY1305-SHA256
        TLS13-SM4-GCM-SM3
        TLS13-SM4-CCM-SM3

    #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-GCM-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-CCM-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CBC-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-GCM-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CCM-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-GCM-SM3:" \
                                       "TLS13-SM4-CCM-SM3:"
    */

    #undef  WOLFSSL_BASE16
    #define WOLFSSL_BASE16 /* required for WOLFSSL_SM2 */

    #undef  WOLFSSL_SM4_ECB
    #define WOLFSSL_SM4_ECB

    #undef  WOLFSSL_SM4_CBC
    #define WOLFSSL_SM4_CBC

    #undef  WOLFSSL_SM4_CTR
    #define WOLFSSL_SM4_CTR

    #undef  WOLFSSL_SM4_GCM
    #define WOLFSSL_SM4_GCM

    #undef  WOLFSSL_SM4_CCM
    #define WOLFSSL_SM4_CCM

    #define HAVE_POLY1305
    #define HAVE_CHACHA

    #undef  HAVE_AESGCM
    #define HAVE_AESGCM
#else
    /* default settings */
    #define USE_CERT_BUFFERS_2048
#endif

/* Chipset detection from sdkconfig.h
 * Default is HW enabled unless turned off.
 * Uncomment lines to force SW instead of HW acceleration */
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(WOLFSSL_ESPWROOM32SE)
    #define WOLFSSL_ESP32
    /*  Alternatively, if there's an ECC Secure Element present: */
    /* #define WOLFSSL_ESPWROOM32SE */

    /* wolfSSL HW Acceleration supported on ESP32. Uncomment to disable: */
    /*  #define NO_ESP32_CRYPT                 */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_HASH    */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_AES     */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MP_MUL  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_MULMOD  */
    /*  #define NO_WOLFSSL_ESP32_CRYPT_RSA_PRI_EXPTMOD */

    /*  These are defined automatically in esp32-crypt.h, here for clarity:  */
    #define NO_WOLFSSL_ESP32_CRYPT_HASH_SHA224 /* no SHA224 HW on ESP32  */

    #undef  ESP_RSA_MULM_BITS
    #define ESP_RSA_MULM_BITS 16 /* TODO add compile-time warning */
    /***** END CONFIG_IDF_TARGET_ESP32 *****/
#endif

/* RSA primitive specific definition, listed AFTER the Chipset detection */
#if defined(WOLFSSL_ESP32) || defined(WOLFSSL_ESPWROOM32SE)
    /* Consider USE_FAST_MATH and SMALL_STACK                        */

    #ifndef NO_RSA
        #define ESP32_USE_RSA_PRIMITIVE

        #if defined(CONFIG_IDF_TARGET_ESP32)
            #ifdef CONFIG_ESP_MAIN_TASK_STACK_SIZE
                #if CONFIG_ESP_MAIN_TASK_STACK_SIZE < 10500
                    #warning "RSA may be difficult with less than 10KB Stack "/
                #endif
            #endif

            /* NOTE HW unreliable for small values! */
            /* threshold for performance adjustment for HW primitive use   */
            /* X bits of G^X mod P greater than                            */
            #undef  ESP_RSA_EXPT_XBITS
            #define ESP_RSA_EXPT_XBITS 32

            /* X and Y of X * Y mod P greater than                         */
            #undef  ESP_RSA_MULM_BITS
            #define ESP_RSA_MULM_BITS  16
        #endif
    #endif
#endif




/* Conditional macros used in wolfSSL TLS client and server examples */
#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)
    #include <wolfssl/certs_test_sm.h>
    #define CTX_CA_CERT          root_sm2
    #define CTX_CA_CERT_SIZE     sizeof_root_sm2
    #define CTX_CA_CERT_TYPE     WOLFSSL_FILETYPE_PEM
    #define CTX_SERVER_CERT      server_sm2
    #define CTX_SERVER_CERT_SIZE sizeof_server_sm2
    #define CTX_SERVER_CERT_TYPE WOLFSSL_FILETYPE_PEM
    #define CTX_SERVER_KEY       server_sm2_priv
    #define CTX_SERVER_KEY_SIZE  sizeof_server_sm2_priv
    #define CTX_SERVER_KEY_TYPE  WOLFSSL_FILETYPE_PEM

    #undef  WOLFSSL_BASE16
    #define WOLFSSL_BASE16
#else
    #if defined(USE_CERT_BUFFERS_2048)
        #define USE_CERT_BUFFERS_256
        /* Be sure to include in app when using example certs: */
        /* #include <wolfssl/certs_test.h>                     */
        #define CTX_CA_CERT          ca_cert_der_2048
        #define CTX_CA_CERT_SIZE     sizeof_ca_cert_der_2048
        #define CTX_CA_CERT_TYPE     WOLFSSL_FILETYPE_ASN1

        #define CTX_SERVER_CERT      server_cert_der_2048
        #define CTX_SERVER_CERT_SIZE sizeof_server_cert_der_2048
        #define CTX_SERVER_CERT_TYPE WOLFSSL_FILETYPE_ASN1
        #define CTX_SERVER_KEY       server_key_der_2048
        #define CTX_SERVER_KEY_SIZE  sizeof_server_key_der_2048
        #define CTX_SERVER_KEY_TYPE  WOLFSSL_FILETYPE_ASN1

        #define CTX_CLIENT_CERT      client_cert_der_2048
        #define CTX_CLIENT_CERT_SIZE sizeof_client_cert_der_2048
        #define CTX_CLIENT_CERT_TYPE WOLFSSL_FILETYPE_ASN1
        #define CTX_CLIENT_KEY       client_key_der_2048
        #define CTX_CLIENT_KEY_SIZE  sizeof_client_key_der_2048
        #define CTX_CLIENT_KEY_TYPE  WOLFSSL_FILETYPE_ASN1

    #elif defined(USE_CERT_BUFFERS_1024)
        #define USE_CERT_BUFFERS_256
        /* Be sure to include in app when using example certs: */
        /* #include <wolfssl/certs_test.h>                     */
        #define CTX_CA_CERT          ca_cert_der_1024
        #define CTX_CA_CERT_SIZE     sizeof_ca_cert_der_1024
        #define CTX_CA_CERT_TYPE     WOLFSSL_FILETYPE_ASN1

        #define CTX_CLIENT_CERT      client_cert_der_1024
        #define CTX_CLIENT_CERT_SIZE sizeof_client_cert_der_1024
        #define CTX_CLIENT_CERT_TYPE WOLFSSL_FILETYPE_ASN1
        #define CTX_CLIENT_KEY       client_key_der_1024
        #define CTX_CLIENT_KEY_SIZE  sizeof_client_key_der_1024
        #define CTX_CLIENT_KEY_TYPE  WOLFSSL_FILETYPE_ASN1

        #define CTX_SERVER_CERT      server_cert_der_1024
        #define CTX_SERVER_CERT_SIZE sizeof_server_cert_der_1024
        #define CTX_SERVER_CERT_TYPE WOLFSSL_FILETYPE_ASN1
        #define CTX_SERVER_KEY       server_key_der_1024
        #define CTX_SERVER_KEY_SIZE  sizeof_server_key_der_1024
        #define CTX_SERVER_KEY_TYPE  WOLFSSL_FILETYPE_ASN1
    #else
        /* Optionally define custom cert arrays, sizes, and types here */
        #error "Must define USE_CERT_BUFFERS_2048 or USE_CERT_BUFFERS_1024"
    #endif
#endif /* Conditional key and cert constant names */

/******************************************************************************
** Sanity Checks
******************************************************************************/
#if defined(CONFIG_ESP_MAIN_TASK_STACK_SIZE)
    #if defined(WOLFCRYPT_HAVE_SRP)
        #if defined(FP_MAX_BITS)
            #if FP_MAX_BITS <  (8192 * 2)
                #define ESP_SRP_MINIMUM_STACK_8K (24 * 1024)
            #else
                #define ESP_SRP_MINIMUM_STACK_8K (28 * 1024)
            #endif
        #else
            #error "Please define FP_MAX_BITS when using WOLFCRYPT_HAVE_SRP."
        #endif

        #if (CONFIG_ESP_MAIN_TASK_STACK_SIZE < ESP_SRP_MINIMUM_STACK)
            #warning "WOLFCRYPT_HAVE_SRP enabled with small stack size"
        #endif
    #endif
#else
    #warning "CONFIG_ESP_MAIN_TASK_STACK_SIZE not defined!"
#endif




/* Memory callbacks */

#undef  USE_WOLFSSL_MEMORY
#define USE_WOLFSSL_MEMORY



#define DEBUG_WOLFSSL
//#define WOLFSSL_CERT_EXT



#define NO_WOLFSSL_SERVER
#define NO_DES3
#define NO_DSA
#define NO_MD4
#define NO_MD5
#define ECC_TIMING_RESISTANT
//#define ALT_ECC_SIZE
#define GCM_SMALL
#define CURVED25519_SMALL
#define CURVE25519_SMALL
#define WOLFSSL_SHA3_SMALL
//#define USE_SLOW_SHA512
#define NO_TLS_DH

#define WC_NO_CACHE_RESISTANT

#define NO_DSA
#define NO_RC4
#define NO_MD4
#define NO_PWDBASED
#define WOLFSSL_SMALL_CERT_VERIFY
#define ED25519_SMALL
#define WOLFSSL_SP_FAST_MODEXP
//#define NO_PSK
//#define NO_DH

#define NO_WOLFSSL_CLIENT
#define NO_WOLFSSL_CLIENT

//#define HAVE_FIPS
//#define WOLFSSL_FIPS_READY
//
//#undef  HAVE_FIPS_VERSION
//#define HAVE_FIPS_VERSION 5
//
//#undef HAVE_FIPS_VERSION_MINOR
//#define HAVE_FIPS_VERSION_MINOR 3

#undef WOLFSSL_WOLFSSH
#define WOLFSSL_WOLFSSH

#undef WC_RNG_SEED_CB
#define WC_RNG_SEED_CB


