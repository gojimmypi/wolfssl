/* ******************************************************************
*  Wolf_Features.h
*  Selects features from the WolfSSL library required for Lumos.
*  Organized into the following sections:
*    o Target specific
*    o Transport layer security features
*    o Key generation & certificate testing
*    o Investigate
*    o Optimisation
*    o Derived configuration
*  ****************************************************************** */

#pragma once

/* ==================================================================
 * Target specific
 **/

#if defined(WOLFSSL_ESPWROOM32)

 // Undocumented, but required to enable ESP32 support. Wraps random
 // number generator selection, logging behaviour etc, for example.
#define WOLFSSL_ESPIDF

// AES-192 is not supported on the ESP32s3; is available for ESP32
// and may be available for other devices.
// undocumented.
#define NO_AES_192

// Controls how long to wait for hardware acceleration operations to
// complete in, for example, esp32_mp. Units are loop cycles.
#define ESP_RSA_TIMEOUT_CNT    0x249F00

// Use the ESP32 RNG directly
// Bypass P-RNG and use ESP32 HW RNG directly
#define WC_NO_HASHDRBG

// Define custom random number generator
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#custom_rand_generate_block
#ifdef __cplusplus
// extern "C" int wolf_rng_gen(unsigned char* output, unsigned int sz);
#else
// extern int wolf_rng_gen(unsigned char* output, unsigned int sz);
#endif
// #define CUSTOM_RAND_GENERATE_BLOCK  wolf_rng_gen

// Enable RSA/RH 4096-bit support (normally disabled for ESP).
// See https://github.com/wolfSSL/wolfssl/issues/6108
// See https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_sp_4096
#define WOLFSSL_SP_4096

// Following https://github.com/wolfSSL/wolfssl/issues/6108
// Required when using WOLFSSL_SP_MATH_ALL
#define SP_INT_BITS (4096)

// Following https://github.com/wolfSSL/wolfssl/issues/6108
#define FP_MAX_BITS (8192)

// Customized WolfSSL source to get wolfSSL_X509_load_certificate_file & wolfSSL_X509_free
// Probably OPENSSL_EXTRA can do this once the https://github.com/wolfSSL/wolfssl/issues/6028
// issue is fixed.
#define BLS_CHECK_CERT_EXPIRY

#elif defined(_WIN32)

 // Undocumented. From default configuration file for Windows.
#define USE_WOLFSSL_IO


#endif


/* ==================================================================
 * Transport layer security features.
 **/

 // Undocumented. Presumably includes TLSv1.3 support.
#define WOLFSSL_TLS13

// Undocumented. Presumably includes TLSv1.2 support.
// Intention is not to have this as TLSv1.3 is more secure, simpler
// and what's required for SDC. Todo: confirm standard.
//#define WOLFSSL_TLSV1_2

 // Enable support for post-handshake authentication in TLS 1.3
 // Required for: https://www.wolfssl.com/documentation/manuals/wolfssl/group__Setup.html#function-wolfssl_ctx_allow_post_handshake_auth
 // .NET will request a post handshake authentication when certification is required and isn't specified when
 // the ssl certificate is registered with "netsh http add sslcert..." using the "clientcertnegotiation=enable" flag
#define WOLFSSL_POST_HANDSHAKE_AUTH

// Force callback set in wolfSSL_CTX_set_verify to be called every time (not just on failure)
// see: https://www.wolfssl.com/forums/post2349.html#p2349
#define WOLFSSL_ALWAYS_VERIFY_CB

// Support certificate extensions. If an unsupported or unknown extension that is marked as critical is found,
// then an error message is returned, otherwise unsupported or unknown extensions found are ignored.
// see: https://www.wolfssl.com/documentation/manuals/wolfssl/chapter07.html#supported-certificate-extensions
#define WOLFSSL_CERT_EXT

// Enable loading private keys with passwords. Supported, but missing documentation
// https://github.com/wolfSSL/wolfssl/issues/6008
// Maybe better to use device encryption as private key should be generated on the
// device and never leave it (once certificate signing requests are working).
// #define WOLFSSL_ENCRYPTED_KEYS

// ED25519 test fails on ESP32 & ESP32s3 (see https://github.com/wolfSSL/wolfssl/issues/5948).
// ED25519 is used for TLSv1.3 (https://en.wikipedia.org/wiki/Transport_Layer_Security#TLS_1.3)
#define HAVE_ED25519

// Enable elliptical curve cryptography support.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_ecc
// Used by TLSv1.3
#define HAVE_ECC

// Turns on curve25519 algorithm
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_curve25519
#define HAVE_CURVE25519

// Enable AES-GCM support
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_aesgcm
// Algorithm used to encrypt/decrypt sessions
#define HAVE_AESGCM

// Enable Diffie-Hellman. Used for TLS session encryption
// Undocumented. Algorithm used to encrypt/decrypt sessions.
#define HAVE_FFDHE_4096

// Undocumented. May be required for session resumption.
// David from WolfSSL:
// Session resumption in TLS v1.3 always uses session tickets.
// Server encrypts the session into a blob and sends to client to present on resumption.
// we do have a 0-RTT example. It is also know are “early data”.
// Note: even with this defined, session resumption doesn't work for
// the .net 4.8 discovery proxy. Without it, session resumption is
// working for the Discovery Wolf server though. This probably means
// it isn't needed and .NET doesn't support TLSv1.3 session resumption
// at this time.
//#define WOLFSSL_EARLY_DATA

// Enable support for TLS extenstions, required for most TLS builds.
// See https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_tls_extensions
// Could be required for TLS with ECDHE/ECDSA (Elliptic-curve Diffie-Hellman/ Elliptic-curve
// digital signature algorithm)
#define HAVE_TLS_EXTENSIONS

// Enables TLS supported curves and key share extensions used with TLS.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_supported_curves
#define HAVE_SUPPORTED_CURVES

// Undocumented. Something to do with RSA; possibly related to TLS.
// Could be probabilistic signature scheme (https://en.wikipedia.org/wiki/Probabilistic_signature_scheme)
// Included in default configuration.
#define WC_RSA_PSS

// Undocumented. Looks like support for a specific algorithm. Possibly
// https://en.wikipedia.org/wiki/HKDF, a key derivation function. Might
// be used in Diffie-Hellman. May be needed for TLSv1.3
// https://en.wikipedia.org/wiki/Transport_Layer_Security#TLS_1.3
// Included in default configuration.
#define HAVE_HKDF

// Undocumented. Looks like support for a specific algorithm. Possibly
// https://medium.com/asecuritysite-when-bob-met-alice/so-what-is-aead-and-why-is-it-so-important-for-encryption-8e2bf16eed6f
// and probably needed.
#define HAVE_AEAD

// enable SHA-512 support: https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_sha512
// Required for TLSv1.3
#define WOLFSSL_SHA512

// undocumented. Might disable checking of certificate expiry time.
//#define NO_ASN_TIME

// To get wolfSSL_X509_get_ext_d2i & CERT_POLICY_OID used to verify that a
// certificate contains the right policy.
// Currently we get this issue https://github.com/wolfSSL/wolfssl/issues/6028
// on Espressif so disabled for now. Todo: get this going once the Wolf issue is fixed.
#if defined(_WIN32)
#define OPENSSL_EXTRA
#endif

// To support TLSv1.3 session resumption.
// https://www.wolfssl.com/documentation/manuals/wolfssl/group__IO.html#function-wolfssl_usesessionticket
//#define HAVE_SESSION_TICKET

// Enables support for secure renegotiation
// https://www.wolfssl.com/documentation/manuals/wolfssl/group__IO.html#function-wolfssl_usesecurerenegotiation
// Note: this is not recommended (by Wolf). This explains why it is bad, at least when initiated by the client:
// https://crashtest-security.com/secure-client-initiated-ssl-renegotiation/
// .NET will use secure renegotiation when certificate authentication is required and isn't specified when
// the ssl certificate is registered with "netsh http add sslcert..." using the "clientcertnegotiation=enable" flag
#define HAVE_SECURE_RENEGOTIATION
#if !defined(WOLFSSL_TLSV1_2)
#undef HAVE_SECURE_RENEGOTIATION
#endif

// Enables blinding mode, to prevent timing attacks
// See https://www.wolfssl.com/documentation/manuals/wolfssl/chapter04.html#timing-resistance-in-wolfssl
// todo: there may be more to do here.
#define WC_RSA_BLINDING

// To prevent timing attacks
// See https://www.wolfssl.com/documentation/manuals/wolfssl/chapter04.html#timing-resistance-in-wolfssl
// todo: there may be more to do here.
#define ECC_TIMING_RESISTANT

/* ==================================================================
 * Key generation & certificate testing (e.g., check expiry date).
 **/

 // Enable key generation:
 // https://www.wolfssl.com/documentation/manuals/wolfssl/chapter07.html#rsa-key-generation
#define WOLFSSL_KEY_GEN

// Enable certificate generation:
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter07.html#certificate-generation
#define WOLFSSL_CERT_GEN

// Enable certificate request generation:
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter07.html#certificate-signing-request-csr-generation
#define WOLFSSL_CERT_REQ

/* ==================================================================
 * Investigate - Todo: not sure if these are needed.
 * **/

 // Undocumented feature to use alt names. Not sure if this is needed.
 //#define WOLFSSL_ALT_NAMES

 // undocumented. Something to do with Diffie-Hellman (an algorithm for two
 // parties to exchange keys to setup symmetric encryption, like TLS)
 // https://www.wolfssl.com/documentation/manuals/wolfssl/group__Diffie-Hellman.html#function-wc_dh_ffdhe4096_get
 // Not sure if this is required or optional for TLSv1.3.
 // Included in default Windows configuration, but not Espressif.
 //#define HAVE_FFDHE_4096

 // Support the PKCS#7 file format (https://en.wikipedia.org/wiki/PKCS_7)
 // https://www.wolfssl.com/documentation/manuals/wolfssl/chapter04.html#pkcs-7
 //#define HAVE_PKCS7

 /* ==================================================================
  * Optimisation
  **/

  // Algorithms that are probably not used or considered deprecated
#define NO_DSA
#define NO_MD4
#define NO_MD5
#define NO_RC4
#define NO_RABBIT
#define NO_PSK

#if !defined(WOLFSSL_ENCRYPTED_KEYS)
// Disables DES3 algorith. OpenSSL uses DES to encrypt
// private key with a password, so this is needed then.
#define NO_DES3
#endif

// Disables password based key derivation functions. Needed for
// certificates that are password protected.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#no_pwdbased
#if !defined(WOLFSSL_ENCRYPTED_KEYS)
#define NO_PWDBASED
#endif

// Remove special characters that embedded devices may have problems with.
// See ChangeLog.md in wolfssl root folder.
#define NO_MULTIBYTE_PRINT

// Turns off the pre-shared key extension
// See: https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#no_psk
#define NO_PSK

/* ==================================================================
 * Derived configuration.
 **/

#if defined(HAVE_PKCS7)
#define HAVE_AES_KEYWRAP
#define HAVE_X963_KDF
#define WOLFSSL_AES_DIRECT
#endif

// esp32-wroom-32se specific definition
#if defined(WOLFSSL_ESPWROOM32SE)
#define WOLFSSL_ATECC508A
#define HAVE_PK_CALLBACKS
#endif

// rsa primitive specific definitions for ESPWROOM32 & ESPWROOM32se
#if defined(WOLFSSL_ESPWROOM32) || defined(WOLFSSL_ESPWROOM32SE)

// Enable USE_FAST_MATH and SMALL_STACK
#define ESP32_USE_RSA_PRIMITIVE

// Set threshold for using hardware acceleration when doing
// large number modular exponentiation (i.e., Z = X^Y mod M,
// by esp_mp_exptmod)
#define EPS_RSA_EXPT_XBTIS 36 // bits

// X and Y of X * Y mod P greater than
// Set threshold for using hardware acceleration when doing
// large number modular multiplication (i.e., Z = X * Y mod M,
// by esp_mp_mulmod).
#define ESP_RSA_MULM_BITS 2000
#endif


#if WOLFSSL_TLSV1_2

// Removes support for non-AEAD algorithms (authenticated encryption with
// associated data). We may not need this support & this could save memory.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#wolfssl_aead_only
// It may be disabled automatically for TLSv1.3
#define WOLFSSL_AEAD_ONLY

// Enable extended master secret PRF (pseudo random function) for calculation
// of session keys used with TLSv1.2 and earlier.
// Required by HAVE_SECURE_RENEGOTIATION.
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#have_extended_master
// Extended master secrets thwart tripple handshake attacks by ensuring different
// sessions end up with different master secret values. [[rfc7267:§4]].
#if defined(HAVE_SECURE_RENEGOTIATION)
#define HAVE_EXTENDED_MASTER
#endif

#else

// Disable support for TLSv1.2
// https://www.wolfssl.com/documentation/manuals/wolfssl/chapter02.html#-disable-tlsv12
#define WOLFSSL_NO_TLS12
#define NO_OLD_TLS

#endif
