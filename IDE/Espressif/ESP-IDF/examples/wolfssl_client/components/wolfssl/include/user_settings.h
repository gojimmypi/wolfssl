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

/* This is the user_settings.h file for the wolfssl_client TLS example.
 * For application-specific settings, please see client-tls.h file */

// #define SHOW_SSID_AND_PASSWORD /* remove this to not show in startup log */

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
/* #define DEBUG_WOLFSSL */
/* #define WOLFSSL_ESP32_CRYPT_DEBUG */
/* #define WOLFSSL_ESP32_HW_LOCK_DEBUG */
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


// nothing defined, default used = "TLS13-AES128-GCM-SHA256"
// #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CBC-SM3" /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CBC-SM3:" /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-GCM-SHA256" /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-GCM-SHA256:"  /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-GCM-SHA256:DHE-PSK-AES128-GCM-SHA256"
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-GCM-SHA256:PSK-AES128-GCM-SHA256:"
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES128-GCM-SHA256:TLS13-AES256-GCM-SHA384:"
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-CHACHA20-POLY1305-SHA256" /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS_CHACHA20_POLY1305_SHA256" /* fails */
// #define WOLFSSL_ESP32_CIPHER_SUITE "TLS_SM4_CCM_SM3"  /* fails */

// "ECDHE-ECDSA-SM4-CBC-SM3"

#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)
    /* SM settings
TLS13-AES128-GCM-SHA256
TLS13-CHACHA20-POLY1305-SHA256
TLS13-SM4-GCM-SM3
TLS13-SM4-CCM-SM3
     **/

 // #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-GCM-SM3"
 // #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-CCM-SM3"
 // #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CBC-SM3"
 // #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-GCM-SM3"
 // #define WOLFSSL_ESP32_CIPHER_SUITE "ECDHE-ECDSA-SM4-CCM-SM3"
    #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-SM4-GCM-SM3:" \
                                       "TLS13-SM4-CCM-SM3:"

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


/* BEGIN EXPE#REIMENTAL OPTIONS.H */

    #define HAVE_POLY1305
    #define HAVE_CHACHA

    #undef  HAVE_AESGCM
    #define HAVE_AESGCM

    #undef  HAVE_ECC
    #define HAVE_ECC

    #undef  HAVE_SUPPORTED_CURVES
    #define HAVE_SUPPORTED_CURVES
    #undef  HAVE_ENCRYPT_THEN_MAC
    #define HAVE_ENCRYPT_THEN_MAC

#define NO_PSK

#define USE_WOLFSSL_IO
#define HAVE_AESGCM
#define WOLFSSL_TLS13
#define HAVE_HKDF
#define HAVE_FFDHE_4096
#define WC_RSA_PSS
//#define WOLFSSL_DTLS
//#define WOLFSSL_DTLS13
#define WOLFSSL_SEND_HRR_COOKIE
//#define WOLFSSL_DTLS_CID

/* added for TFM test */
#define USE_FAST_MATH
/* #define DEBUG_WOLFSSL */

#define WC_RSA_BLINDING
#define NO_MULTIBYTE_PRINT

#define HAVE_CRL
#define HAVE_CRL_MONITOR





#undef  HAVE_THREAD_LS
#define HAVE_THREAD_LS

#undef  NO_DO178
#define NO_DO178



#undef  ERROR_QUEUE_PER_THREAD
#define ERROR_QUEUE_PER_THREAD

#undef  TFM_TIMING_RESISTANT
#define TFM_TIMING_RESISTANT

#undef  ECC_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT

#undef  WC_RSA_BLINDING
#define WC_RSA_BLINDING

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

#undef  WOLFSSL_SM4
#define WOLFSSL_SM4

#undef  WOLFSSL_USE_ALIGN
#define WOLFSSL_USE_ALIGN

#undef  WOLFSSL_SHA224
#define WOLFSSL_SHA224

#undef  WOLFSSL_SHA512
#define WOLFSSL_SHA512

#undef  WOLFSSL_SHA384
#define WOLFSSL_SHA384

#undef  WOLFSSL_SM3
#define WOLFSSL_SM3

#undef  HAVE_HKDF
#define HAVE_HKDF

#undef  NO_DSA
#define NO_DSA

#undef  HAVE_ECC
#define HAVE_ECC

#undef  TFM_ECC256
#define TFM_ECC256

#undef  ECC_SHAMIR
#define ECC_SHAMIR

#undef  WOLFSSL_SM2
#define WOLFSSL_SM2

#undef  WOLFSSL_BASE16
#define WOLFSSL_BASE16

#undef  WC_RSA_PSS
#define WC_RSA_PSS

#undef  WOLFSSL_PSS_LONG_SALT
#define WOLFSSL_PSS_LONG_SALT

#undef  WOLFSSL_ASN_TEMPLATE
#define WOLFSSL_ASN_TEMPLATE

#undef  WOLFSSL_ASN_PRINT
#define WOLFSSL_ASN_PRINT

#undef  WOLFSSL_BASE64_ENCODE
#define WOLFSSL_BASE64_ENCODE

#undef  WOLFSSL_SHA3
#define WOLFSSL_SHA3

#undef  WOLFSSL_NO_SHAKE128
#define WOLFSSL_NO_SHAKE128

#undef  WOLFSSL_NO_SHAKE256
#define WOLFSSL_NO_SHAKE256

#undef  HAVE_POLY1305
#define HAVE_POLY1305

#undef  HAVE_CHACHA
#define HAVE_CHACHA

#undef  HAVE_HASHDRBG
#define HAVE_HASHDRBG

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SNI
#define HAVE_SNI

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  HAVE_FFDHE_2048
#define HAVE_FFDHE_2048

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  WOLFSSL_TLS13
#define WOLFSSL_TLS13

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_EXTENDED_MASTER
#define HAVE_EXTENDED_MASTER

#undef  NO_RC4
#define NO_RC4

#undef  HAVE_ENCRYPT_THEN_MAC
#define HAVE_ENCRYPT_THEN_MAC

#undef  NO_PSK
#define NO_PSK

#undef  NO_MD4
#define NO_MD4

#undef  WOLFSSL_SP_MATH_ALL
#define WOLFSSL_SP_MATH_ALL

#undef  WOLFSSL_SP_X86_64
#define WOLFSSL_SP_X86_64

#undef  WC_NO_ASYNC_THREADING
#define WC_NO_ASYNC_THREADING

#undef  HAVE_DH_DEFAULT_PARAMS
#define HAVE_DH_DEFAULT_PARAMS

#undef  WOLFSSL_SYS_CA_CERTS
#define WOLFSSL_SYS_CA_CERTS

#undef  NO_DES3
#define NO_DES3

#undef  GCM_TABLE_4BIT
#define GCM_TABLE_4BIT

#undef  HAVE_AESGCM
#define HAVE_AESGCM

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SERVER_RENEGOTIATION_INFO
#define HAVE_SERVER_RENEGOTIATION_INFO


#undef  HAVE_WC_INTROSPECTION
#define HAVE_WC_INTROSPECTION

/* END OPTIONS.H */

/* BEGIN Experimental WIN user_settings.h */
#define USE_WOLFSSL_IO
#define HAVE_AESGCM
#define WOLFSSL_TLS13
#define HAVE_HKDF
#define HAVE_FFDHE_4096
#define WC_RSA_PSS
//#define WOLFSSL_DTLS
//#define WOLFSSL_DTLS13
#define WOLFSSL_SEND_HRR_COOKIE
//#define WOLFSSL_DTLS_CID

/* added for TFM test */
#define USE_FAST_MATH
/* #define DEBUG_WOLFSSL */

#define WOLFSSL_LIB /* used below */
/* Configurations */
#if defined(HAVE_FIPS)
    /* FIPS */
    #define OPENSSL_EXTRA
    #define HAVE_THREAD_LS
    #define WOLFSSL_KEY_GEN
    #define HAVE_HASHDRBG
    #define WOLFSSL_SHA384
    #define WOLFSSL_SHA512
    #define NO_PSK
    #define NO_RC4
    #define NO_DSA
    #define NO_MD4

    #define GCM_NONCE_MID_SZ 12
#else
    /* Enables blinding mode, to prevent timing attacks */
    #define WC_RSA_BLINDING
    #define NO_MULTIBYTE_PRINT

    #define HAVE_CRL
    #define HAVE_CRL_MONITOR

    #if defined(WOLFSSL_LIB)
        /* The lib */
        #define OPENSSL_EXTRA
        #define WOLFSSL_RIPEMD
        #define NO_PSK
        #define HAVE_EXTENDED_MASTER
        #define WOLFSSL_SNIFFER
        #define HAVE_SECURE_RENEGOTIATION

        #define HAVE_AESGCM
        #define WOLFSSL_AESGCM_STREAM
        #define WOLFSSL_SHA384
        #define WOLFSSL_SHA512

        #define HAVE_SUPPORTED_CURVES
        #define HAVE_TLS_EXTENSIONS

        #define HAVE_ECC
        #define ECC_SHAMIR
        #define ECC_TIMING_RESISTANT

        #define WOLFSSL_SP_X86_64
        #define SP_INT_BITS  4096

        /* Optional Performance Speedups */
        #if 0
            /* AESNI on x64 */
            #ifdef _WIN64
                #define HAVE_INTEL_RDSEED
                #define WOLFSSL_AESNI
                #define HAVE_INTEL_AVX1
                #if 0
                    #define HAVE_INTEL_AVX2
                #endif
            #endif

            /* Single Precision Support for RSA/DH 1024/2048/3072 and
             * ECC P-256/P-384 */
            #define WOLFSSL_SP
            #define WOLFSSL_HAVE_SP_ECC
            #define WOLFSSL_HAVE_SP_DH
            #define WOLFSSL_HAVE_SP_RSA

            #ifdef _WIN64
                /* Old versions of MASM compiler do not recognize newer
                 * instructions. */
                #if 0
                    #define NO_AVX2_SUPPORT
                    #define NO_MOVBE_SUPPORT
                #endif
                #define WOLFSSL_SP_ASM
                #define WOLFSSL_SP_X86_64_ASM
            #endif
        #endif
    #else
        /* The servers and clients */
        #define OPENSSL_EXTRA
        #define NO_PSK
    #endif
#endif /* HAVE_FIPS */

/* end experimental WIN user_seetings.h */


/* SM Cipher */
#define WOLFSSL_SM2
#define WOLFSSL_SM3
#define WOLFSSL_SM4

#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)

    #define WOLFSSL_BASE16 /* required for WOLFSSL_SM2 */
    #ifndef _WIN32
        #define NO_FILESYSTEM
    #endif

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

    #undef  HAVE_ECC
    #define HAVE_ECC

    #undef  HAVE_SUPPORTED_CURVES
    #define HAVE_SUPPORTED_CURVES

    /* may be needed */
    //#undef  HAVE_ENCRYPT_THEN_MAC
    //#define HAVE_ENCRYPT_THEN_MAC
#else
    #define USE_CERT_BUFFERS_2048
#endif /* SM check */
