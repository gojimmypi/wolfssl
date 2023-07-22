#ifndef _WIN_USER_SETTINGS_H_
#define _WIN_USER_SETTINGS_H_

/* Verify this is Windows */
#ifndef _WIN32
#error This user_settings.h header is only designed for Windows
#endif

#define USE_WOLFSSL_IO
#define HAVE_AESGCM
#define WOLFSSL_TLS13
#define HAVE_HKDF
#define HAVE_FFDHE_4096
#define WC_RSA_PSS
#define WOLFSSL_DTLS
#define WOLFSSL_DTLS13
#define WOLFSSL_SEND_HRR_COOKIE
#define WOLFSSL_DTLS_CID

/* added for TFM test */
#define USE_FAST_MATH
#define DEBUG_WOLFSSL

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

#define WOLFSSL_SM2
#define WOLFSSL_SM3
#define WOLFSSL_SM4

#if defined(WOLFSSL_SM2) || defined(WOLFSSL_SM3) || defined(WOLFSSL_SM4)

#define WOLFSSL_BASE16 /* required for WOLFSSL_SM2 */
    #define NO_FILESYSTEM

    #undef BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3
    #define BUILD_TLS_ECDHE_ECDSA_WITH_SM4_CBC_SM3

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
    #define USE_CERT_BUFFERS_2048
#endif

#endif /* _WIN_USER_SETTINGS_H_ */
