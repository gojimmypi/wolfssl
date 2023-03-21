/* ******************************************************************
*  user_settings.h
*  Build configuration for WolfSSL library.
*  Organized into the following sections:
*    o Debugging
*    o Target specific
*    o Project configuration.
*  ****************************************************************** */

/* BuildConfiguration.h missing  */
/* #include "../../../main/BuildConfiguration.h" */
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

/* ==================================================================
 * Debugging
 **/

// Enable debugging and logging support
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter08.html
#define DEBUG_WOLFSSL

// Undocumented, but exposes a lot of extra debug information. Helpful
// to see encryption/decryption steps output to console.
//#define DEBUG_WOLFSSL_VERBOSE

// Enable additional debugging during a TLS connection
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_debug_tls
//#define WOLFSSL_DEBUG_TLS

// Undocumented ESP32 specific debugging. Not sure if this does anything useful.
//#define WOLFSSL_ESP32WROOM32_CRYPT_DEBUG

// DEBUG ONLY!
// Enable things needed to decode SSL messages with wireshark, writing SSL
// secrets into a file. This file can be used for (Pre)-Master-Secret log
// filename in Wireshark->Edit->Preferences->Protocol->TLS.
// https ://www.wolfssl.com/forums/topic1580-creating-a-nss-key-log-file.html
// https ://github.com/wolfSSL/wolfssl-examples/blob/master/tls/client-tls13.c
//#define HAVE_SECRET_CALLBACK

#if 0
// Enable memory diagnostics.
// https://github.com/wolfSSL/wolfssl/blob/master/wolfssl/wolfcrypt/mem_track.h
#define USE_WOLFSSL_MEMORY
#define WOLFSSL_TRACK_MEMORY
#endif

#if 0
// Undocumented. Disable hardware acceleration of cryptography functions including RSA
#define NO_ESP32WROOM32_CRYPT

// Undocumented. Disable hardware acceleration of RSA primitives?
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI

// Undocumented. Disable hardware acceleration of SHA (e.g., SHA-256, SHA-512)
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH

// Undocumented. Disable hardware acceleration of AES
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_AES

#endif


/* ==================================================================
 * Target specific
 **/

// Minimum stack size required for any task that creates TLS sessions.
// Todo: figure out a suitable value.
#define TLS_MIN_STACK_SIZE 0 // bytes

// Enable ESP32-WROOM-32 hardware crypto and RNG support. Applies to
// customized support for ESP32s3 as well.
// See https://www.wolfssl.com/documentation/manuals/wolfssl/chapter04.html#esp32-wroom-32
// Options are: WOLFSSL_ESPWROOM32, WOLFSSL_ESPWROOM32SE, WOLFSSL_ESP8266
#define WOLFSSL_ESPWROOM32

// Exclude error strings (reduces code-size)
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#no_error_strings
// Saves about 11k from DATA_FLASH and 7k from INSTR_FLASH.
//#define NO_ERROR_STRINGS

// Disable use of CRT (Chinese remainder therom), which slows down RSA calculations
// but uses less "memory". Not sure if that's heap or stack or both. Untested.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#rsa_low_mem
// https://en.wikipedia.org/wiki/RSA_(cryptosystem)#Using_the_Chinese_remainder_algorithm
// Note: defining this significantly increases connection time, by more than 4s (from
// about 2.6s to 7s with defined too).
//#define RSA_LOW_MEM

// Uses a slower implementation for curve25519 that requires less memory.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#curve25519_small
#define CURVE25519_SMALL

// Increases use of dynamic memory in `wolfcrypt/src/integer.c` but can lead to slower performance
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_small_stack
// Apparently stack variables > 100 bytes are allocated from the heap.
#if !defined(_WIN32) && (defined(NO_ESP32WROOM32_CRYPT) || defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI) || defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH) || defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_AES))
#define WOLFSSL_SMALL_STACK
#endif


// Undocumented
// Apparently reduce peak TLS memory use.
//#define WOLFSSL_SMALL_STACK_CACHE

// Use smaller versions of the code and avoid large stack variables.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_sp_small
//#define WOLFSSL_SP_SMALL

// Enable SP math and algorithms.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_sp_math_all
//#define WOLFSSL_SP_MATH_ALL

// Verify certificate signature without using DecodedCert. Increases code size but
// reduces peak heap use.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_small_cert_verify
#define WOLFSSL_SMALL_CERT_VERIFY

// Reduces the size of the session cache
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#no_session_cache
#define SMALL_SESSION_CACHE

/* ==================================================================
 * Project configuration.
 **/


#define WOLFSSL_KEY_GEN

/* Shared configuration in same directory */
#include "Wolf_Features.h"
