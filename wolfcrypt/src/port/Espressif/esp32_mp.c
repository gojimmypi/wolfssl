/* esp32_mp.c
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

/*
 * See ESP32 Technical Reference Manual - RSA Accelerator Chapter
 *
 * esp_mp_exptmod()  Large Number Modular Exponentiation Z = X^Y mod M
 * esp_mp_mulmod()   Large Number Modular Multiplication Z = X × Y mod M
 * esp_mp_mul()      Large Number Multiplication         Z = X × Y
 *
 * The RSA Accelerator supports operand lengths of:
 * N ∈ {512,1024 , 1536, 2048, 2560, 3072, 3584, 4096} bits. The bit length
 * of arguments Z, X, Y , M, and r can be any one from the N set, but all
 * numbers in a calculation must be of the same length.
 *
 * The bit length of M′ is always 32.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>

#include "wolfssl/wolfcrypt/logging.h"

#if !defined(NO_RSA) || defined(HAVE_ECC)

#if defined(WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI) && \
   !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI)

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#include <wolfssl/wolfcrypt/wolfmath.h>

#ifndef SINGLE_THREADED
    /* Espressif freeRTOS */
    #include <freertos/semphr.h>
#endif

#define ESP_HW_RSAMAX_BIT           4096
#define ESP_HW_MULTI_RSAMAX_BITS    2048
#define ESP_HW_RSAMIN_BIT           512
#define BYTE_TO_WORDS(s)            (((s+3)>>2))         /* (s+(4-1))/ 4    */
#define BITS_TO_WORDS(s)            (((s+31)>>3)>>2)     /* (s+(32-1))/ 8/ 4*/
#define BITS_IN_ONE_WORD            32

#define MP_NG   -1

#define ESP_TIMEOUT(cnt)         (cnt >= ESP_RSA_TIMEOUT_CNT)

#ifdef DEBUG_WOLFSSL
    static int hw_validation = 0; /* are we validating HW and SW? (prevent call to HW) */
    #define SET_HW_VALIDATION {hw_validation = 1;}
    #define CLR_HW_VALIDATION {hw_validation = 0;}
    #define IS_HW_VALIDATION (hw_validation == 1)
    #undef WOLFSSL_HW_METRICS
    #define WOLFSSL_HW_METRICS /* usage metrics always on during debug */
#endif

/* For esp_mp_exptmod and esp_mp_mulmod we need a variety of calculated helper
** values to properly setup the hardware. see esp_mp_montgomery_init() */
struct esp_mp_helper
{
    word32 Xs;
    word32 Ys;
    word32 Ms;
    word32 Rs;
    word32 maxWords_sz;
    word32 hwWords_sz;
    MATH_INT_T r_inv;
    mp_digit mp;
    mp_digit mp2;
};

static const char* const TAG = "wolfssl_esp32_mp";

/* usage metrics can be turned on independently of debugging */
#ifdef WOLFSSL_HW_METRICS
    static uint32_t esp_mp_mul_usage_ct = 0;
    static uint32_t esp_mp_mulmod_usage_ct = 0;
    static uint32_t esp_mp_mulmod_fallback_ct = 0;
    static uint32_t esp_mp_mulmod_even_mod_ct = 0;
    static uint32_t esp_mp_mulmod_small_y_ct = 0;
    static uint32_t esp_mp_mul_error_ct = 0;
    static uint32_t esp_mp_mulmod_error_ct = 0;
#endif

/* mutex */
static wolfSSL_Mutex mp_mutex;
static int espmp_CryptHwMutexInit = 0;

#ifdef DEBUG_WOLFSSL
    /* when debugging, we'll double-check the mutex with call depth */
    #ifndef NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD
        static int esp_mp_exptmod_depth_counter = 0;
    #endif /* NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD */
#endif /* DEBUG_WOLFSSL */

/*
* check if the HW is ready before accessing it
*
* See 24.3.1 Initialization of ESP32 Technical Reference Manual
* https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
*
* The RSA Accelerator is activated by enabling the corresponding peripheral
* clock, and by clearing the DPORT_RSA_PD bit in the DPORT_RSA_PD_CTRL_REG
* register. This releases the RSA Accelerator from reset.
*
* See esp_mp_hw_lock().
*
* When the RSA Accelerator is released from reset, the register RSA_CLEAN_REG
* reads 0 and an initialization process begins. Hardware initializes the four
* memory blocks by setting them to 0. After initialization is complete,
* RSA_CLEAN_REG reads 1. For this reason, software should query RSA_CLEAN_REG
* after being released from reset, and before writing to any RSA Accelerator
* memory blocks or registers for the first time.
*/
static int esp_mp_hw_wait_clean(void)
{
    int ret = MP_OKAY;
    word32 timeout = 0;

#if CONFIG_IDF_TARGET_ESP32S3
    while (!ESP_TIMEOUT(++timeout) && DPORT_REG_READ(RSA_QUERY_CLEAN_REG) != 1)
    {
      /*  wait. expected delay 1 to 2 uS  */
    }
#else
    /* RSA_CLEAN_REG is now called RSA_QUERY_CLEAN_REG.
    ** hwcrypto_reg.h maintains RSA_CLEAN_REG for backwards compatibility:
    ** so this block _might_ not be needed. */
    ESP_EM__PRE_MP_HW_WAIT_CLEAN

    /* wait until ready,
    ** or timeout counter exceeds ESP_RSA_TIMEOUT_CNT in user_settings */
    while(!ESP_TIMEOUT(++timeout) && DPORT_REG_READ(RSA_CLEAN_REG) == 0) {
        /*  wait. expected delay 1 to 2 uS  */
        ESP_EM__MP_HW_WAIT_CLEAN
    }
#endif

    if (ESP_TIMEOUT(timeout)) {
        ESP_LOGE(TAG, "esp_mp_hw_wait_clean waiting HW ready timed out.");
        ret = MP_HW_BUSY;
    }
    return ret;
}

/*
* esp_mp_hw_lock()
*
* Lock HW engine.
* This should be called before using engine.
*
* Returns 0 if the HW lock was initialized and mutex lock.
*
* See Chapter 24:
*  https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
*
* The RSA Accelerator is activated by enabling the corresponding peripheral
* clock, and by clearing the DPORT_RSA_PD bit in the DPORT_RSA_PD_CTRL_REG
* register. This releases the RSA Accelerator from reset.
*
* When the RSA Accelerator is released from reset, the register RSA_CLEAN_REG
* reads 0 and an initialization process begins. Hardware initializes the four
* memory blocks by setting them to 0. After initialization is complete,
* RSA_CLEAN_REG reads 1. For this reason, software should query RSA_CLEAN_REG
* after being released from reset, and before writing to any RSA Accelerator
* memory blocks or registers for the first time.
*/
static int esp_mp_hw_lock()
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_mp_hw_lock");

    if (espmp_CryptHwMutexInit == 0) {
        ret = esp_CryptHwMutexInit(&mp_mutex);
        if (ret == 0) {
            /* flag esp mp as initialized */
            espmp_CryptHwMutexInit = 1;
        }
        else {
            ESP_LOGE(TAG, "mp mutex initialization failed.");
        }
    }
    else {
        /* mp_mutex has already been initialized */
    }

    /* Set our mutex to indicate the HW is in use */
    if (ret == 0) {
        /* lock hardware */
        ret = esp_CryptHwMutexLock(&mp_mutex, ESP_MP_HW_LOCK_MAX_DELAY);
        if (ret != 0) {
            ESP_LOGE(TAG, "mp engine lock failed.");
            ret = MP_HW_BUSY; /* caller is expected to fall back to SW */
        }
   }

#if CONFIG_IDF_TARGET_ESP32S3
    /* Activate the RSA accelerator. See 20.3 of ESP32-S3 technical manual.
     * periph_module_enable doesn't seem to be documented and in private folder
     * with v5 release. Maybe it will be deprecated? */
    if (ret == 0) {
        periph_module_enable(PERIPH_RSA_MODULE);

        /* clear bit to enable hardware operation; (set to disable) */
        DPORT_REG_CLR_BIT(SYSTEM_RSA_PD_CTRL_REG, SYSTEM_RSA_MEM_PD);
    }
#else
    /* Enable RSA hardware */
    if (ret == 0) {
        periph_module_enable(PERIPH_RSA_MODULE);

        /* clear bit to enable hardware operation; (set to disable) */
        DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
        ESP_EM__POST_SP_MP_HW_LOCK
    }
#endif

    /* reminder: wait until RSA_CLEAN_REG reads 1
    **   see esp_mp_hw_wait_clean() */

    ESP_LOGV(TAG, "leave esp_mp_hw_lock");
    return ret;
}

/*
**  Release HW engine
*/
static int esp_mp_hw_unlock( void )
{
    int ret = MP_OKAY;
#if CONFIG_IDF_TARGET_ESP32S3
    /* Deactivate the RSA accelerator. See 20.3 of ESP32-S3 technical manual.
     * periph_module_enable doesn't seem to be documented and in private folder
     * with v5 release. Maybe it will be deprecated? */
    DPORT_REG_SET_BIT(SYSTEM_RSA_PD_CTRL_REG, SYSTEM_RSA_MEM_PD);
    periph_module_disable(PERIPH_RSA_MODULE);

#else
    /* set bit to disabled hardware operation; (clear to enable) */
    DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    /* Disable RSA hardware */
    periph_module_disable(PERIPH_RSA_MODULE);
#endif

    /* unlock */
    esp_CryptHwMutexUnLock(&mp_mutex);

    ESP_LOGV(TAG, "unlock");
    return ret;
}

/*
** esp_mp_hw_islocked() - detect if we've locked the HW for use.
**
** WARNING: this does *not* detect separate calls to the
**          periph_module_disable() and periph_module_enable().
*/
static int esp_mp_hw_islocked(void)
{
    int ret = 0;
#ifdef SINGLE_THREADED
    if (mp_mutex == 0) {
        /* not in use */
        ESP_LOGV(TAG, "SINGLE_THREADED esp_mp_hw_islocked = false");
    } else {
        ESP_LOGV(TAG, "SINGLE_THREADED esp_mp_hw_islocked = true");
        ret = 1;
    }
#else
    TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder(mp_mutex);
    if (mutexHolder == NULL) {
        /* Mutex is not in use */
        ESP_LOGV(TAG, "multi-threaded esp_mp_hw_islocked = false");
    } else {
        ESP_LOGV(TAG, "multi-threaded esp_mp_hw_islocked = true");
        ret = 1;
    }
#endif
    return ret;
}

/* mulmod and mulexp_mod HW accelerator need mongomery math prep: M' */
#if !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD) \
      || \
    !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MULMOD)

static int esp_calc_Mdash(MATH_INT_T *M, word32 k, mp_digit* md)
{
    int ret = MP_OKAY;

    ESP_LOGV(TAG, "\nBegin esp_calc_Mdash \n");

#ifdef USE_ALT_MPRIME
    /* M' = M^(-1) mod b; b = 2^32 */

    /* Call Large Number Modular Exponentiation
     *
     *    Z = X^Y mod M
     *
     *    mp_exptmod notation: Y = (G ^ X) mod P
     *
     *    G is our parameter: M
     */
    MATH_INT_T X[1] = { }; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T P[1] = { }; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T Y[1] = { }; /* TODO WOLFSSL_SMALL_STACK */
    word32 Xs;
    mp_init(X);
    mp_init(P);
    mp_init(Y);

    /* MATH_INT_T value of (-1) */
    X->dp[0] = 1;
    X->sign = MP_NEG;
    X->used = 1;

    Xs = mp_count_bits(X);

    /* MATH_INT_T value of 2^32 */
    P->dp[1] = 1;
    P->used = 2;

    /* this fails due to even P number; ((b & 1) == 0) in fp_montgomery_setup()
     * called from _fp_exptmod_ct, called from fp_exptmod */
    ret = mp_exptmod(M, X, P, Y);

    *md = Y->dp[0];
    ESP_LOGI(TAG, "esp_calc_Mdash %u", *md);
#else
    /* this is based on an article by Cetin Kaya Koc,
     * A New Algorithm for Inversion: mod p^k, June 28 2017 */
    int i;
    int xi;
    int b0 = 1;
    int bi;
    word32  N = 0;
    word32  x;

    N = M->dp[0];
    bi = b0;
    x  = 0;

    for (i = 0; i < k; i++) {
        xi = bi % 2;
        if (xi < 0) {
            xi *= -1;
        }
        bi = (bi - N * xi) / 2;
        x |= (xi << i);
    }
    /* 2's complement */
    *md = ~x + 1;
#endif

    ESP_LOGV(TAG, "\nEnd esp_calc_Mdash \n");

    return ret;
}
#endif /* !NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_[MULMOD/EXPTMOD] for M' */

/* the result may need to have extra bytes zeroed or used length adjusted */
static int esp_clean_result(MATH_INT_T* Z, int used_padding)
{
    int ret = MP_OKAY;
    uint16_t this_extra;

/* TODO remove this section if MP_SIZE accepted into sp_int.h
** See https://github.com/wolfSSL/wolfssl/pull/6565 */
    uint16_t dp_length = 0; (void) dp_length;
#ifdef USE_FAST_MATH
    dp_length = FP_SIZE;
#else
    dp_length = SP_INT_DIGITS;
#endif
/* TODO end */

    this_extra = Z->used;
    if (this_extra > MP_SIZE) {
        ESP_LOGW(TAG, "Warning (Z->used: %d) > (MP_SIZE: %d); adjusting...",
                                Z->used,        MP_SIZE);
        this_extra = MP_SIZE;
    }

    while (Z->dp[this_extra] > 0 && (this_extra < MP_SIZE)) {
        ESP_LOGV(TAG, "Adjust! %d", this_extra);
        Z->dp[this_extra] = 0;
        this_extra++;
    }

    /* a result of 1 is interesting */
    if (Z->dp[0] == 1) {
        /*
         * When the exponent is 0: In this case, the result of the modular
         * exponentiation operation will always be 1, regardless of the value
         * of the base.
         *
         * When the base is 1: If the base is equal to 1, then the result of
         * the modular exponentiation operation will always be 1, regardless
         * of the value of the exponent.
         *
         * When the exponent is equal to the totient of the modulus: If the
         * exponent is equal to the totient of the modulus, and the base is
         * relatively prime to the modulus, then the result of the modular
         * exponentiation operation will be 1.
         */
        ESP_LOGV(TAG, "Z->dp[0] == 1");
    }

    /* trim any trailing zeros and adjust z.used size */
    if (Z->used > 1) {
        ESP_LOGV(TAG, "ZTrim: Z->used = %d", Z->used);
        for (size_t i = Z->used; i > 1; i--) {
            if (Z->dp[i - 1] == 0) {
                /* last element in zero based array */
                Z->used = i - 1;
            }
            else {
                break; /* if not zero, nothing else to do */
            }
        }
        ESP_LOGV(TAG, "New Z->used = %d", Z->used);
    }
    else {
        ESP_LOGV(TAG, "no z-trim needed");
    }

#if defined(WOLFSSL_SP_INT_NEGATIVE) || defined(USE_FAST_MATH)
    if (Z->sign != 0) {
        Z->sign = 1; /* any value other than zero is assumed negative */
    }
#endif
    return ret;
}

/* start HW process */
static void process_start(word32 reg)
{
    /* see 3.16 "software needs to always use the "volatile"
    ** attribute when accessing registers in these two address spaces. */
    DPORT_REG_WRITE(reg, 1);
    ESP_EM__POST_PROCESS_START
}

/* wait until done */
static int wait_until_done(volatile uint32_t reg)
{
    int ret = MP_OKAY;
    word32 timeout = 0;
    /* wait until done && not timeout */
    asm volatile("memw");
    while (!ESP_TIMEOUT(++timeout) &&
                DPORT_REG_READ(reg) != 1) {
        asm volatile("nop"); /* wait */
    }

    ESP_EM__DPORT_FIFO_READ

    /* clear interrupt */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);
    asm volatile("memw");

    if (ESP_TIMEOUT(timeout)) {
        ESP_LOGE(TAG, "rsa operation timed out.");
        ret = MP_HW_ERROR;
    }

    return ret;
}

/* read data from memory into mp_init          */
static int esp_memblock_to_mpint(volatile const uint32_t mem_address,
                                 volatile MATH_INT_T* mp,
                                 word32 numwords)
{
    int ret = MP_OKAY;
#ifdef USE_ESP_DPORT_ACCESS_READ_BUFFER
    esp_dport_access_read_buffer((uint32_t*)mp->dp, mem_address, numwords);
#else
    int try_ct = 18;

    __asm__ volatile ("memw");

    DPORT_INTERRUPT_DISABLE();
    for (volatile uint32_t i = 0;  i < numwords; ++i) {
        ESP_EM__3_16
        // DPORT_SEQUENCE_REG_READ(0x3FF40078);
        mp->dp[i] = DPORT_SEQUENCE_REG_READ(mem_address + i * 4);
    }
    DPORT_INTERRUPT_RESTORE();

    if (try_ct < 1) {
       // ESP_LOGW(TAG, "esp_memblock_to_mpint timeout exceeded during read");
    }
#endif
    mp->used = numwords;

#if defined(ESP_VERIFY_MEMBLOCK)
    ret = XMEMCMP((const void *)mem_address, (const void *)&mp->dp, numwords * sizeof(word32));
    if (ret != 0 ) {
        ESP_LOGW(TAG, "Validation Failure esp_memblock_to_mpint.\n"
                      "Reading %u Words at Address =  0x%08x",
                       (int)(numwords * sizeof(word32)),
                       (unsigned int)mem_address);
        ESP_LOGI(TAG, "Trying again... ");
        esp_dport_access_read_buffer((uint32_t*)mp->dp, mem_address, numwords);
        mp->used = numwords;
        if (0 != XMEMCMP((const void *)mem_address, (const void *)&mp->dp, numwords * sizeof(word32))) {
            ESP_LOGE(TAG, "Validation Failure esp_memblock_to_mpint "
                           "a second time. Giving up.");
            ret = MP_VAL;
        }
        else {
            ESP_LOGI(TAG, "Successfully re-read after Validation Failure.");
            ret = MP_VAL;
        }
    }
#endif
    return ret;
}

/* Write 0x00 to [wordSz] of register memory starting at mem_address */
static int esp_zero_memblock(volatile const u_int32_t* mem_address,
                            int wordSz)
{
    int ret = MP_OKAY;
    for (int i=0; i < wordSz; i++) {
        __asm__ volatile ("memw");
        DPORT_REG_WRITE((volatile void *)mem_address + (i * sizeof(word32)), 0);
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
    }
    return ret;
}

/* write MATH_INT_T mp value (dp[]) into memory block
 */
static int esp_mpint_to_memblock(volatile u_int32_t mem_address,
                                 const MATH_INT_T* mp,
                                 const word32 bits,
                                 const word32 hwords)
{
    int ret = MP_OKAY;

    /* init */
    word32 i;
    word32 len = (bits / 8 + ((bits & 7) != 0 ? 1 : 0));

    len = (len + sizeof(word32)-1) / sizeof(word32);

    for (i=0; i < hwords; i++) {
        if (i < len) {
            ESP_LOGV(TAG, "Write i = %d value.", i);
            __asm__ volatile ("memw");
            DPORT_REG_WRITE((volatile void *)mem_address + (i * sizeof(word32)), mp->dp[i]);
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
        }
        else {
            if (i == 0) {
                ESP_LOGV(TAG, "esp_mpint_to_memblock zero?");
            }
            ESP_LOGV(TAG, "Write i = %d value = zero.", i);
            __asm__ volatile ("memw");
            /* TODO we may be able to skip zero in certain circumstances */
            DPORT_REG_WRITE(mem_address + (i * sizeof(word32)), 0);
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
            __asm__ volatile ("nop");
        }
    }

#if defined(ESP_VERIFY_MEMBLOCK)
    len = XMEMCMP((const void *)mem_address, (const void*)mp->dp, (int)hwords);
    if (len != 0) {
        ESP_LOGE(TAG, "esp_mpint_to_memblock compare fails at %d", len);
        ret = MP_VAL;
    }
#endif
    return ret;
}

/* return needed HW words.
 * supported words length
 *  words    : { 16,   32,  48,    64,   80,   96,  112,  128}
 *  bits     : {512, 1024, 1536, 2048, 2560, 3072, 3584, 4096}
 */
static word32 words2hwords(word32 wd)
{
    const word32 bit_shift  = 4;

    return (((wd + 0xf) >> bit_shift) << bit_shift);
}

/* count the number of words is needed for bits */
static word32 bits2words(word32 bits)
{
    /* 32 bits */
    const word32 d = sizeof(word32) * WOLFSSL_BIT_SIZE;

    return ((bits + (d - 1)) / d);
}

#if      !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD) \
      || !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MULMOD)
/* rinv and M' only used for mulmod and mulexp_mod */

/* get rinv */
static int esp_get_rinv(MATH_INT_T *rinv, MATH_INT_T *M, word32 exp)
{
    /* todo */
//     return mp_montgomery_calc_normalization(rinv, M);
    ESP_LOGV(TAG, "\nBegin esp_get_rinv \n");

    int ret = 0;
#ifdef DEBUG_WOLFSSL
    MATH_INT_T rinv2[1]; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T M2[1];    /* TODO WOLFSSL_SMALL_STACK */
    mp_copy(M, M2); /* copy (src = M) to (dst = M2) */
    mp_copy(rinv, rinv2); /* copy (src = M) to (dst = M2) */
#endif

    /* 2^(exp)
     *
     * rinv will have all zeros with a 1 in last word.
     * e.g. exp=2048 will have a 1 in dp[0x40] = dp[64]
     * this is the 65'th element (zero based)
     * Value for used = 0x41 = 65
     **/
    if ((ret = mp_2expt(rinv, exp)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate mp_2expt()");
        return ret;
    }

    /* r_inv = R^2 mod M(=P) */
    if (ret == 0 && (ret = mp_mod(rinv, M, rinv)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate mp_mod()");
        return ret;
    }

#ifdef DEBUG_WOLFSSL
    int ret2; (void)ret2;
    /* computes a = B**n mod b without division or multiplication useful for
    * normalizing numbers in a Montgomery system. */
    ret2 = mp_montgomery_calc_normalization(rinv2, M2);

#endif

    ESP_LOGV(TAG, "\nEnd esp_get_rinv \n");
    return ret;
}
#endif /* ! xEXPTMOD || ! xMULMOD for rinv */

/* during debug, we'll compare HW to SW results */
int esp_hw_validation_active(void)
{
#ifdef DEBUG_WOLFSSL
    return IS_HW_VALIDATION;
#else
    return 0; /* we're never validating when not debugging */
#endif
}

#ifndef NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MP_MUL
/* Large Number Multiplication
 *
 * See 24.3.3 of the ESP32 Technical Reference Manual
 *
 * Z = X * Y;  */
int esp_mp_mul(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* Z)
{
    int ret;
    ret = MP_OKAY; /* assume success until proven wrong */

    word32 Xs;
    word32 Ys;
    word32 Zs;
    word32 maxWords_sz;
    word32 hwWords_sz;

    /* if either operand is zero, there's nothing to do.
     * Y checked first, as it was observed to be zero during
     * wolfcrypt tests more often than X */
    if (mp_iszero(Y) || mp_iszero(X)) {
        mp_forcezero(Z);
        return MP_OKAY;
    }

/* During debug, we may be validating against SW result. */
#ifdef DEBUG_WOLFSSL
    MATH_INT_T X2[1]; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T Y2[1]; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T Z2[1]; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T PEEK[1]; /* TODO WOLFSSL_SMALL_STACK */

    /* The caller should have checked if the call was for a SW validation.
     * During debug, we'll return an error. */
    if (esp_hw_validation_active()) {
        return MP_HW_VALIDATION_ACTIVE;
    }
    mp_init(X2);
    mp_init(Y2);
    mp_init(Z2);

    mp_copy(X, X2); /* copy (src = X) to (dst = X2) */
    mp_copy(Y, Y2); /* copy (src = Y) to (dst = Y2) */
    mp_copy(Z, Z2); /* copy (src = Z) to (dst = Z2) */

#endif

//    ESP_LOGV(TAG, "\nBegin esp_mp_mul \n");
//    if (X == Z) {
//        ESP_LOGW(TAG, "mp_mul X == Z");
//    }
//    if (Y == Z) {
//        ESP_LOGW(TAG, "mp_mul Y == Z");
//    }

/* if we are supporting negative numbers, check that first since operands
 * may be later modified (e.g. Z = Z * X) */
#if defined(WOLFSSL_SP_INT_NEGATIVE) || defined(USE_FAST_MATH)
    /* neg check: X*Y becomes negative */
    int neg;

    /* aka (X->sign == Y->sign) ? MP_ZPOS : MP_NEG; , but with mp_isneg(): */
    neg = (mp_isneg(X) == mp_isneg(Y)) ? MP_ZPOS : MP_NEG;
    if (neg) {
        /* Negative numbers are relatively infrequent.
         * May be interesting during verbose debugging: */
        ESP_LOGV(TAG, "mp_isneg(X) = %d; mp_isneg(Y) = %d; neg = %d ",
                       mp_isneg(X),      mp_isneg(Y),           neg);
    }
#endif


#ifdef DEBUG_WOLFSSL
    if (IS_HW_VALIDATION) {
        ESP_LOGE(TAG, "Caller must not try HW when validation active."); /* TODO handle with semaphore  */
    }
    else {
        SET_HW_VALIDATION;
        mp_mul(X2, Y2, Z2);
        CLR_HW_VALIDATION;
    }
#endif /* DEBUG_WOLFSSL */

#if CONFIG_IDF_TARGET_ESP32S3

    int BitsInX = mp_count_bits(X);
    int BitsInY = mp_count_bits(Y);

    /* X & Y must be represented by the same number of bits. Must be
     * enough to represent the larger one. */
    int MinXYBits = max(BitsInX, BitsInY);

    /* Figure out how many words we need to
     * represent each operand & the result. */
    int WordsForOperand = bits2words(MinXYBits);
    int WordsForResult = bits2words(BitsInX + BitsInY);

    /* Make sure we are within capabilities of hardware. */
    if ( (WordsForOperand * BITS_IN_ONE_WORD) > ESP_HW_MULTI_RSAMAX_BITS ) {
        ESP_LOGW(TAG, "exceeds max bit length(2048)");
        return MP_VAL; /*  Error: value is not able to be used. */
    }

    /* Steps to perform large number multiplication. Calculates Z = X x Y. The number of
     * bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Lock the hardware so no-one else uses it and wait until it is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write number of words required for result to the RSA_MODE_REG (now called RSA_LENGTH_REG).
     *     Number of words required for the result is 2 * words for operand - 1
     *  4. Load X, Y operands to memory blocks. Note the Y value must be written to
     *     right aligned.
     *  5. Start the operation by writing 1 to RSA_MULT_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  6. Read the result out.
     *  7. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. lock HW for use & wait until it is ready. */
    if ( ((ret = esp_mp_hw_lock()) != MP_OKAY) ||
         ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) ) {
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write number of words required for result. */
    if ( (WordsForOperand * BITS_IN_ONE_WORD * 2) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    DPORT_REG_WRITE(RSA_LENGTH_REG, (WordsForOperand * 2 - 1) );

    /* 4. Load X, Y operands. Maximum is 64 words (64*8*4 = 2048 bits) */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE,
                          X, BitsInX, WordsForOperand);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE + WordsForOperand * 4,
                          Y, BitsInY, WordsForOperand);


    /* 5. Start operation and wait until it completes. */
    process_start(RSA_MULT_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        return ret;
    }

    /* 6. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, WordsForResult);

    /* 7. clear and release HW                    */
    esp_mp_hw_unlock();

    /* end if CONFIG_IDF_TARGET_ESP32S3 */

#else /* not CONFIG_IDF_TARGET_ESP32S3 */
    /* assumed to be regular Xtensa here */

    /* determine count of bits. Used as HW parameter.   */
    Xs = mp_count_bits(X);
    Ys = mp_count_bits(Y);
    Zs = Xs + Ys;

    if (Zs <= sizeof(mp_digit)) {
        Z->dp[0] = X->dp[0] * Y->dp[0];
        Z->used = 1;
#if defined(WOLFSSL_SP_INT_NEGATIVE) || defined(USE_FAST_MATH)
        Z->sign = neg;
#endif
        return MP_OKAY;
    }

    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, Ys));
    hwWords_sz  = words2hwords(maxWords_sz);

    /* sanity check */
    if((hwWords_sz<<5) > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "exceeds max bit length(2048) (a)");
        return MP_VAL; /*  Error: value is not able to be used. */
    }

    /*Steps to use HW in the following order:
    * 1. wait until clean HW engine
    * 2. Write(2*N/512bits - 1 + 8) to MULT_MODE_REG
    * 3. Write X and Y to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 4. Write 1  to MUL_START_REG
    * 5. Wait for the first operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 6. Write 1 to RSA_INTERRUPT_REG to clear the interrupt.
    * 7. Read the Z from RSA_Z_MEM
    * 8. Write 1 to RSA_INTERUPT_REG to clear the interrupt.
    * 9. Release the HW engine
    */

    /* Y (left-extend)
     * Accelerator supports large-number multiplication with only
     * four operand lengths of N ∈ {512, 1024, 1536, 2048} */
    int left_pad_offset = maxWords_sz << 2; /* e.g. 32 words * 4 bytes = 128 bytes */

    if (left_pad_offset <= 512 >> 3) {
        left_pad_offset = 512 >> 3; /* 64 bytes (16 words) */
    }
    else {
        if (left_pad_offset <= 1024 >> 3) {
            left_pad_offset = 1024 >> 3; /* 128 bytes = 32 words */
        }
        else {
            if (left_pad_offset <= 1536 >> 3) {
                left_pad_offset = 1536 >> 3; /* 192 bytes = 48 words */
            }
            else {
                if (left_pad_offset <= 2048 >> 3) {
                    left_pad_offset = 2048 >> 3; /* 256 bytes = 64 words */
                }
                else {
                    ret = MP_VAL;
                    ESP_LOGE(TAG, "Unsupported operand length: %d", hwWords_sz);
                }
            }
        }
    }

    /* lock HW for use */
    ret = esp_mp_hw_lock(); /* enables HW clock */
    if (ret != MP_OKAY) {
        ESP_LOGW(TAG, "esp_mp_hw_lock fail");
        return ret;
    }

    if((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        ESP_LOGW(TAG, "esp_mp_hw_wait_clean");
        return ret;
    }

    /* step.1  (2*N/512) => N/256. 512 bits => 16 words */
    /* Write 2*N/512 - 1 + 8  */
    // DPORT_REG_WRITE(RSA_MULT_MODE_REG, (hwWords_sz >> 3) - 1 + 8);

    DPORT_REG_WRITE(RSA_MULT_MODE_REG, (2 * left_pad_offset * 8/512) - 1 + 8);

    /* step.2 write X into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE,
                          X,
                          Xs,
                          hwWords_sz);



    /* write zeros from RSA_MEM_Z_BLOCK_BASE to left_pad_offset - 1 */
    esp_zero_memblock((volatile void * )RSA_MEM_Z_BLOCK_BASE, (left_pad_offset -1)/sizeof(int));

    /* write the left-padded Y value into Z */
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE + (left_pad_offset), /* hwWords_sz<<2 */
                          Y,
                          Ys,
                          hwWords_sz);

#ifdef DEBUG_WOLFSSLx
    /* save value to peek at the result stored in RSA_MEM_Z_BLOCK_BASE */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE,
                          PEEK,
                          128);

#endif

    /* step.3 start process                           */
    process_start(RSA_MULT_START_REG);

    /* step.4,5 wait until done                       */
    ret = wait_until_done(RSA_INTERRUPT_REG);
    if (ret != MP_OKAY) {
        ESP_LOGE(TAG, "wait_until_done failed.");
        return ret;
    }
    /* step.6 read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Zs));

#endif /* CONFIG_IDF_TARGET_ESP32S3 or not */

    /* common exit for all chipset types */
#if defined(WOLFSSL_SP_INT_NEGATIVE) || defined(USE_FAST_MATH)
    if (!mp_iszero(Z) && neg) {
        /* for non-zero negative numbers, set negative flag for our result:
         *   Z->sign = FP_NEG */
        ESP_LOGV(TAG, "Setting Z to negative result!");
        mp_setneg(Z);
    }
    else {
        Z->sign = 0;
    }
#endif

    // TODO confirm WOLFSSL_SP_INT_NEGATIVE Z->sign = X->sign ^ Y->sign;
    esp_clean_result(Z, 0);

#ifdef WOLFSSL_HW_METRICS
    esp_mp_mul_usage_ct++;
#endif
#ifdef DEBUG_WOLFSSL
    if (fp_cmp(X, X2) != 0) {
        // ESP_LOGE(TAG, "mp_mul X vs X2 mismatch!");
    }
    if (fp_cmp(Y, Y2) != 0) {
        // ESP_LOGE(TAG, "mp_mul Y vs Y2 mismatch!");
    }
    if (fp_cmp(Z, Z2) != 0) {
        int found_z_used = Z->used;

        ESP_LOGE(TAG, "mp_mul Z vs Z2 mismatch!");
        ESP_LOGI(TAG, "Xs            = %d", Xs);
        ESP_LOGI(TAG, "Ys            = %d", Ys);
        ESP_LOGI(TAG, "Zs            = %d", Zs);
        ESP_LOGI(TAG, "found_z_used  = %d", found_z_used);
        ESP_LOGI(TAG, "z.used        = %d", Z->used);
        ESP_LOGI(TAG, "hwWords_sz    = %d", hwWords_sz);
        ESP_LOGI(TAG, "maxWords_sz   = %d", maxWords_sz);
        ESP_LOGI(TAG, "left_pad_offset = %d", left_pad_offset);
        ESP_LOGI(TAG, "hwWords_sz<<2   = %d", hwWords_sz << 2);
        esp_show_mp("X", X2);  /* show the copy in X2, as X may have been clobbered */
        esp_show_mp("Y", Y2);  /* show the copy in Y2, as Y may have been clobbered */
        esp_show_mp("Peek Z", PEEK); /* this is the Z before start */
        esp_show_mp("Z", Z);   /* this is the HW result */
        esp_show_mp("Z2", Z2); /* this is the SW result */
    #ifndef NO_RECOVER_SOFTWARE_CALC
        ESP_LOGW(TAG, "Recovering mp_mul error with software result");
        mp_copy(Z2, Z); /* copy (src = Z2) to (dst = Z) */
    #else
        ret = MP_VAL;
    #endif
    }
#endif

    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

#ifdef WOLFSSL_HW_METRICS
    if (ret != MP_OKAY) {
        esp_mp_mul_error_ct++; /* includes fallback */
    }
#endif

    ESP_LOGV(TAG, "\nEnd esp_mp_mul \n");

    return ret;
}
#endif /* ! NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MP_MUL*/

#ifndef NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MULMOD
/* Large Number Modular Multiplication
 *
 * See 24.3.3 of the ESP32 Technical Reference Manual
 *
 * Z = X × Y mod M */
int esp_mp_mulmod(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* M, MATH_INT_T* Z)
{
    int ret = MP_OKAY;
    int negcheck = 0;
#ifdef DEBUG_WOLFSSL
    int reti = 0; /* interim return value used only during HW==SW validation */
#endif
    word32 Xs = 0;
    word32 Ys = 0;
    word32 Ms = 0;
    word32 Rs = 0;
    word32 maxWords_sz = 0;
    word32 hwWords_sz = 0;
    word32 zwords = 0;
    uint32_t Exponent = 0;
    mp_digit mp = 0;

/*
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    MATH_INT_T* r_inv = NULL;
    MATH_INT_T* tmpZ = NULL;
#else
*/
    mp_digit mp2[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T r_inv[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T tmpZ[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */

#ifdef DEBUG_WOLFSSL
    MATH_INT_T X2[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T Y2[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T M2[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T Z2[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
    MATH_INT_T PEEK[1] = {0}; /* TODO WOLFSSL_SMALL_STACK */
#endif

    ESP_LOGV(TAG, "\nBegin esp_mp_mulmod \n");

    if ((M->dp[0] & 1) == 0) {
#ifdef WOLFSSL_HW_METRICS
        esp_mp_mulmod_even_mod_ct++;
#endif
        ESP_LOGV(TAG, "esp_mp_mulmod does not support even numbers");
        ret = MP_HW_FALLBACK;
    }

#ifdef DEBUG_WOLFSSL
    /* we're only validating HW when in debug mode */
    if (esp_hw_validation_active()) {
        ESP_LOGW(TAG, "MP_HW_VALIDATION_ACTIVE");
        return MP_HW_VALIDATION_ACTIVE;
    }
#endif

/*
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_NO_MALLOC)
    r_inv = (MATH_INT_T*)XMALLOC(sizeof(MATH_INT_T), key->heap, DYNAMIC_TYPE_DH);
    if (r_inv == NULL) {
        return FP_MEM;
    }
    tmpZ = (MATH_INT_T*)XMALLOC(sizeof(MATH_INT_T), key->heap, DYNAMIC_TYPE_DH);
    if (tmpZ == NULL) {
        XFREE(r_inv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return FP_MEM;
    }
#endif
*/

#ifdef DEBUG_WOLFSSL
    if (IS_HW_VALIDATION) {
        ESP_LOGE(TAG, "Caller must not try HW when validation active."); /* TODO handle with semaphore  */
    }
    else {
        /* when validating, save SW in [V]2 for later comparison to HW */
        mp_init(X2);
        mp_init(Y2);
        mp_init(M2);
        mp_init(Z2);

        mp_copy(X, X2); /* copy (src = X) to (dst = X2) */
        mp_copy(Y, Y2); /* copy (src = Y) to (dst = Y2) */
        mp_copy(M, M2); /* copy (src = M) to (dst = M2) */
        mp_copy(Z, Z2); /* copy (src = Z) to (dst = Z2) */

        SET_HW_VALIDATION;
        reti = mp_mulmod(X2, Y2, M2, Z2);
        if (reti == 0) {
            ESP_LOGV(TAG, "wolfSSL mp_mulmod during vaidation success");
        }
        else {
            ESP_LOGE(TAG, "wolfSSL mp_mulmod during vaidation failed");
        }
        CLR_HW_VALIDATION;
    }
#endif /* DEBUG_WOLFSSL */

#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    int WordsForOperand;
# endif

    if (ret == MP_OKAY) {
        /* neg check - X*Y becomes negative */
        negcheck = mp_isneg(X) != mp_isneg(Y) ? 1 : 0;

        /* determine number of bits for HW parameter */
        Xs = mp_count_bits(X);
        Ys = mp_count_bits(Y);
        Ms = mp_count_bits(M);
        ESP_LOGV(TAG, "Bits: Xs = %d, Ys = %d, Ms = %d", Xs, Ys, Ms);

        //    if ((Xs <= 8) || (Ys <= 8)) {
        //        ESP_LOGW(TAG, "FP_HW_FALLBACK Xs = %d, Ys = %d", Xs, Ys);
        //        ret = FP_HW_FALLBACK;
        //    }
        //    if (Xs <= 8) {
        //        ESP_LOGW(TAG, "FP_HW_FALLBACK Xs = %d", Xs);
        //        ret = FP_HW_FALLBACK;
        //    }
        if (Ys <= 8) {
    #ifdef WOLFSSL_HW_METRICS
            esp_mp_mulmod_small_y_ct++;
    #endif
            ESP_LOGV(TAG, "MP_HW_FALLBACK Ys = %d", Ys);
            ret = MP_HW_FALLBACK;
        }
    } /* sign & operand bit count check*/

    if (ret == MP_OKAY) {
        /* maximum bits and words for writing to HW */
        maxWords_sz = bits2words(max(Xs, max(Ys, Ms)));
        zwords      = bits2words(min(Ms, Xs + Ys));
        hwWords_sz  = words2hwords(maxWords_sz);

        ESP_LOGV(TAG,
                 "Words:, maxWords_sz = %d, zwords = %d, hwWords_sz = %d",
                 maxWords_sz,
                 zwords,
                 hwWords_sz);

        if ((hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
            ESP_LOGE(TAG, "exceeds HW maximum bits = %d", ESP_HW_RSAMAX_BIT);
            ret = MP_VAL; /*  Error: value is not able to be used. */
        }
    }

    /* calculate r_inv = R^2 mod M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */

    if (ret == MP_OKAY) {
#if CONFIG_IDF_TARGET_ESP32S3
        Exponent = maxWords_sz * BITS_IN_ONE_WORD * 2;
#else
        Exponent = hwWords_sz << 6;
#endif

        ret = mp_init_multi(tmpZ, r_inv, NULL, NULL, NULL, NULL);
        if (ret == 0 && (ret = esp_get_rinv(r_inv, M, Exponent)) != MP_OKAY) {
            ESP_LOGE(TAG, "calculate r_inv failed.");
            mp_clear(tmpZ);
            mp_clear(r_inv);
        }
        else {
            Rs = mp_count_bits(r_inv);
            /* success, show details if debugging*/
            #ifdef DEBUG_WOLFSSL
            //        esp_show_mp("r_inv", r_inv);
            //        ESP_LOGI(TAG, "r_inv bits    = %d", Rs);
            //        ESP_LOGI(TAG, "Exponent      = %lu (0x%08x)", Exponent, (unsigned int)Exponent);
                    /* optionally show M and R inv
                       esp_show_mp("    M", M);
                       esp_show_mp("r_inv", r_inv);
               */
               #endif
        }
    }

    if (ret == MP_OKAY) {
        ret = mp_montgomery_setup(M, mp2);
        /* Calculate M' */
        if (ret == MP_OKAY) {
            ret = esp_calc_Mdash(M, 32/* bits */, &mp);
        }
        else {
            ESP_LOGE(TAG, "failed to calculate M dash");
        }
    }

    /* lock HW for use, enable peripheral clock */
    if (ret == MP_OKAY) {
        ret = esp_mp_hw_lock();
    }

#if CONFIG_IDF_TARGET_ESP32S3
    if (ret == MP_OKAY) {
    /* Steps to perform large number modular multiplication. Calculates Z = (X x Y) modulo M.
     * The number of bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048. We must use the same number of words to represent
     * the bits in X, Y and M.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Wait until the hardware is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write the number of words required to represent the operands to the
     *     RSA_MODE_REG (now called RSA_LENGTH_REG).
     *  4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG).
     *  5. Load X, Y, M, r' operands to memory blocks.
     *  6. Start the operation by writing 1 to RSA_MOD_MULT_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  7. Read the result out.
     *  8. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. Wait until hardware is ready. */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        ESP_LOGE(TAG, "esp_mp_hw_wait_clean failed.");
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write (N_result_bits/32 - 1) to the RSA_MODE_REG. */
    OperandBits = max(max(Xs, Ys), Ms);
    if (OperandBits > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    WordsForOperand = bits2words(OperandBits);
    DPORT_REG_WRITE(RSA_LENGTH_REG, WordsForOperand - 1);

    /* 4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG) */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);

    /* Select acceleration options. */
    DPORT_REG_WRITE(RSA_CONSTANT_TIME_REG, 0);

    /* 5. Load X, Y, M, r' operands.
     * Note RSA_MEM_RB_BLOCK_BASE == RSA_MEM_Z_BLOC_BASE on ESP32s3*/
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X,     Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y,     Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M,     Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_RB_BLOCK_BASE, r_inv, mp_count_bits(r_inv), hwWords_sz);

    /* 6. Start operation and wait until it completes. */
    process_start(RSA_MOD_MULT_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        return ret;
    }

    /* 7. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, tmpZ, zwords);

    /* 8. clear and release HW                    */
    esp_mp_hw_unlock();

    if (negcheck) {
        mp_sub(M, tmpZ, tmpZ);
    }


    /* end if CONFIG_IDF_TARGET_ESP32S3 */
    }
#else
    /* Classic ESP32, non-S3 Xtensa */

    /*Steps to use HW in the following order:
    * prep:  wait until clean HW engine
    *
    * 1. Write (N/512bits - 1) to MULT_MODE_REG
    * 2. Write X,M(=G, X, P) to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 3. Write M' to M_PRIME_REG
    * 4. Write 1  to MODEXP_START_REG
    * 5. Wait for the first round of the operation to be completed.
    *    Poll RSA_INTERRUPT_REG until it reads 1,
    *    or until the RSA_INTR interrupt is generated.
    *    (Or until the INTER interrupt is generated.)
    * 6. Write 1 to RSA_INTERRUPT_REG to clear the interrupt.
    * 7. Write Yi (i ∈ [0, n) ∩ N) to RSA_X_MEM
    *    Users need to write to the memory block only according to the length
    *    of the number. Data beyond this length is ignored.
    * 8. Write 1 to RSA_MULT_START_REG
    * 9. Wait for the second operation to be completed.
    *    Poll INTERRUPT_REG until it reads 1.
    * 10. Read the Zi (i ∈ [0, n) ∩ N) from RSA_Z_MEM
    * 11. Write 1 to RSA_INTERUPT_REG to clear the interrupt.
    *
    * post: Release the HW engine
    *
    * After the operation, the RSA_MULT_MODE_REG register, and memory blocks
    * RSA_M_MEM and RSA_M_PRIME_REG remain unchanged. Users do not need to
    * refresh these registers or memory blocks if the values remain the same.
    */

    if (ret == MP_OKAY) {
        /* Prep wait for the engine */
        ret = esp_mp_hw_wait_clean();
    }

    if (ret == MP_OKAY) {
        /* step.1
         *  Write (N/512bits - 1) to MULT_MODE_REG
         *  512 bits => 16 words */
        DPORT_REG_WRITE(RSA_MULT_MODE_REG, (hwWords_sz >> 4) - 1);
        #ifdef DEBUG_WOLFSSL
        ESP_LOGI(TAG, "RSA_MULT_MODE_REG = %d", (hwWords_sz >> 4) - 1);
        #endif // WOLFSSL_DEBUG

        /* step.2 write X, M, and r_inv into memory.
         * The capacity of each memory block is 128 words.
         * The memory blocks use the little endian format for storage,
         * i.e. the least significant digit of each number is in lowest address.*/
        esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
        esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
        esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE, r_inv, Rs, hwWords_sz);

        /* step.3 write M' into memory                   */
        /* confirmed with Sean that mp2 does not support even modulus
         * indeed we see a failure, but we can predict when modules is odd
         * or when mp != mp2[0] */
        DPORT_REG_WRITE(RSA_M_DASH_REG, mp);
        asm volatile("memw");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");

        /* step.4 start process                           */
        process_start(RSA_MULT_START_REG);
        asm volatile("memw");

        /* step.5,6 wait until done                       */
        wait_until_done(RSA_INTERRUPT_REG);

        /* step.7 Y to MEM_X                              */
        esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, Y, Ys, hwWords_sz);

        #ifdef DEBUG_WOLFSSL
        /* save value to peek at the result stored in RSA_MEM_Z_BLOCK_BASE */
        esp_memblock_to_mpint(RSA_MEM_X_BLOCK_BASE,
                              PEEK,
                              128);
        esp_clean_result(PEEK, 0);
        #endif

        /* step.8 start process                           */
        process_start(RSA_MULT_START_REG);

        /* step.9,11 wait until done                      */
        wait_until_done(RSA_INTERRUPT_REG);

        /* step.12 read the result from MEM_Z             */
        esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, tmpZ, zwords);
    } /* step 1 .. 12 */

    /* step.13 clear and release HW                   */
    if (esp_mp_hw_islocked()) {
        esp_mp_hw_unlock();
    }
    #endif /* Classic ESP32, non-S3 Xtensa */

    if (ret == MP_OKAY) {
        /* additional steps                               */
        /* this is needed for known issue when Z is greater than M */
        if (mp_cmp(tmpZ, M) == MP_GT) {
            /*  Z -= M  */
            mp_sub(tmpZ, M, tmpZ);
            ESP_LOGV(TAG, "Z is greater than M");
        }
        if (negcheck) {
            mp_sub(M, tmpZ, tmpZ);
            ESP_LOGV(TAG, "neg check adjustment");
        }

        mp_copy(tmpZ, Z); /* copy tmpZ to result Z */

        esp_clean_result(Z, 0);
    }

#ifdef WOLFSSL_HW_METRICS
    esp_mp_mulmod_usage_ct++;
    if (ret == MP_HW_FALLBACK) {
        ESP_LOGV(TAG, "HW Fallback");
        esp_mp_mulmod_fallback_ct++;
    }
#endif
#ifdef DEBUG_WOLFSSL
    if (ret == MP_HW_FALLBACK) {
        ESP_LOGI(TAG, "HW Fallback");
    }
    else {
        if (fp_cmp(X, X2) != 0) {
            // ESP_LOGE(TAG, "mp_mul X vs X2 mismatch!");
        }
        if (fp_cmp(Y, Y2) != 0) {
            // ESP_LOGE(TAG, "mp_mul Y vs Y2 mismatch!");
        }

        if (fp_cmp(Z, Z2) != 0) {
            ESP_LOGE(TAG, "esp_mp_mulmod Z vs Z2 mismatch!");

            esp_mp_mulmod_error_ct++;
            int found_z_used = Z->used;

            ESP_LOGI(TAG, "Xs            = %d", Xs);
            ESP_LOGI(TAG, "Ys            = %d", Ys);
            // ESP_LOGI(TAG, "Zs            = %d", Zs);
            ESP_LOGI(TAG, "found_z_used  = %d", found_z_used);
            ESP_LOGI(TAG, "z.used        = %d", Z->used);
            ESP_LOGI(TAG, "hwWords_sz    = %d", hwWords_sz);
            ESP_LOGI(TAG, "maxWords_sz   = %d", maxWords_sz);
            // ESP_LOGI(TAG, "left_pad_offset = %d", left_pad_offset);
            ESP_LOGI(TAG, "hwWords_sz<<2   = %d", hwWords_sz << 2);
            esp_show_mp("X", X2); /* show the copy in X2, as X may have been clobbered */
            esp_show_mp("Y", Y2); /* show the copy in Y2, as Y may have been clobbered */
            esp_show_mp("M", M2); /* show the copy in M2, as M may have been clobbered */
            esp_show_mp("r_inv", r_inv); /*show r_inv  */
            ESP_LOGI(TAG, "mp            = 0x%08x = %u", mp, mp);

            if (mp == mp2[0]) {
                ESP_LOGI(TAG, "M' match esp_calc_Mdash vs mp_montgomery_setup = %d  !", mp);
            }
            else {
                ESP_LOGW(TAG,
                         "\n\n"
                         "M' MISMATCH esp_calc_Mdash = 0x%08x = %d \n"
                         "vs mp_montgomery_setup     = 0x%08x = %d \n\n",
                         mp,
                         mp,
                         mp2[0],
                         mp2[0]);
                mp = mp2[0];
            }


            // esp_show_mp("Peek X", PEEK); /* this is the X before second start */
            esp_show_mp("HW Z", Z); /* this is the HW result */
            esp_show_mp("SW Z2", Z2); /* this is the SW result */
            ESP_LOGI(TAG, "esp_mp_mulmod_usage_ct = %d tries", esp_mp_mulmod_usage_ct);
            ESP_LOGI(TAG, "esp_mp_mulmod_error_ct = %d failures", esp_mp_mulmod_error_ct);
            ESP_LOGI(TAG, "");


            #ifndef NO_RECOVER_SOFTWARE_CALC
            ESP_LOGW(TAG, "Recovering mp_mul error with software result");
            mp_copy(Z2, Z); /* copy (src = Z2) to (dst = Z) */
            #else
            ret = MP_VAL; /* if we are not recovering, then we have an error */
            #endif
        }
        else {
            ESP_LOGV(TAG, "esp_mp_mulmod success!");
        }
    }

#endif /* DEBUG_WOLFSSL */

    mp_clear(tmpZ);
    mp_clear(r_inv);

    ESP_LOGV(TAG, "\nEnd esp_mp_mulmod \n");
    return ret;

}
#endif /* ! NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_MULMOD */



int esp_mp_montgomery_init(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* M,
                           struct esp_mp_helper* mph)
{
    int ret = MP_OKAY;
    mph->Xs = mp_count_bits(X);
    mph->Ys = mp_count_bits(Y);
    mph->Ms = mp_count_bits(M);
    /* maximum bits and words for writing to HW */
    mph->maxWords_sz = bits2words(max(mph->Xs, max(mph->Ys, mph->Ms)));
    mph->hwWords_sz  = words2hwords(mph->maxWords_sz);

    ESP_LOGV(TAG, "hwWords_sz = %d", mph->hwWords_sz);

    if ((mph->hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGE(TAG, "exceeds HW maximum bits");
        ret = MP_VAL; /*  Error: value is not able to be used. */
    }

    /* calculate r_inv = R^2 mode M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */
    if (ret == MP_OKAY) {
        ret = mp_init(&(mph->r_inv));
        if ( (ret == 0) &&
         ((ret = esp_get_rinv(&(mph->r_inv), M, (mph->hwWords_sz << 6))) != MP_OKAY) ) {
            ESP_LOGE(TAG, "calculate r_inv failed.");
            return MP_VAL;
        }
    }
    mph->Rs = mp_count_bits(&(mph->r_inv));

    ret = mp_montgomery_setup(M, &(mph->mp2) );

    /* calc M' */
    /* if Pm is odd, uses mp_montgomery_setup() */
    if ( (ret = esp_calc_Mdash(M, 32/* bits */, &(mph->mp) )) != MP_OKAY ) {
        ESP_LOGE(TAG, "failed to calculate M dash");
    }

    if (mph->mp == mph->mp2) {
        ESP_LOGV(TAG, "M' match esp_calc_Mdash vs mp_montgomery_setup = %ul  !", mph->mp);
    }
    else {
        ESP_LOGW(TAG,
                 "\n\n"
                 "M' MISMATCH esp_calc_Mdash = 0x%08x = %d \n"
                 "vs mp_montgomery_setup     = 0x%08x = %d \n\n",
                 mph->mp,
                 mph->mp,
                 mph->mp2,
                 mph->mp2);
        mph->mp = mph->mp2;
    }

    return ret;
}

#ifndef NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD
/* Large Number Modular Exponentiation
 *
 *    Z = X^Y mod M
 *
 *  ESP32, Section 24.3.2  https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
 *  ESP32S3, Section 20.3.1, https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf
 *
 * The operation is based on Montgomery multiplication. Aside from the
 * arguments X, Y , and M, two additional ones are needed —r and M′
.* These arguments are calculated in advance by software.
.*
.* The RSA Accelerator supports operand lengths of N ∈ {512, 1024, 1536, 2048,
.* 2560, 3072, 3584, 4096} bits on the ESP32 and N ∈ [32, 4096] bits on the ESP32s3.
.* The bit length of arguments Z, X, Y , M, and r can be any one from the N set,
.* but all numbers in a calculation must be of the same length.
.* The bit length of M′ is always 32.
.*
 * Z = (X ^ Y) mod M   : Espressif generic notation
 * Y = (G ^ X) mod P   : wolfSSL DH reference notation */
int esp_mp_exptmod(MATH_INT_T* X, MATH_INT_T* Y, word32 Ys, MATH_INT_T* M, MATH_INT_T* Z)
{
     /* Danger! Do not initialize any function parameters, not even the result Z.
      * Some operations such as (rnd = rnd^e) will wipe out the rnd operand
      * value upon initialization.
      * (e.g. the address of X and Z could be the same when called) */
    struct esp_mp_helper mph[1];
    int ret = MP_OKAY;

#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    uint32_t WordsForOperand;
#endif

    ESP_LOGV(TAG, "\nBegin esp_mp_exptmod \n");

    if (mp_iszero(M)) {
        ESP_LOGW(TAG, "esp_mp_exptmod M is zero!");
        return MP_VAL;
    }

    if (mp_isone(M)) {
        mp_clear(Z);
        return MP_OKAY;
    }

    ret = esp_mp_montgomery_init(X, Y, M, mph);
    if (ret != MP_OKAY) {
        return ret;
    }

#ifdef DEBUG_WOLFSSL
    if (esp_hw_validation_active()) {
        /* recall there's only one HW for all math accelerations */
        return MP_HW_VALIDATION_ACTIVE;
    }

    if (esp_mp_exptmod_depth_counter != 0) {
        ESP_LOGE(TAG, "esp_mp_exptmod Depth Counter Error!");
    }
    esp_mp_exptmod_depth_counter++;
#endif

 /*
 max bits = 0x400 = 1024 bits
1024 / 8 = 128 bytes
 128 / 4 = 32 words (0x20)
 */

    /* lock and init the HW                           */
    if ( (ret = esp_mp_hw_lock()) != MP_OKAY ) {
        ESP_LOGE(TAG, "esp_mp_hw_lock failed");
#ifdef DEBUG_WOLFSSL
        esp_mp_exptmod_depth_counter--;
#endif
        return ret;
    }

#if CONFIG_IDF_TARGET_ESP32S3
    /* Steps to perform large number modular exponentiation. Calculates Z = (X ^ Y) modulo M.
     * The number of bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Wait until the hardware is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write (N_bits/32 - 1) to the RSA_MODE_REG (now called RSA_LENGTH_REG).
     *     Here N_bits is the maximum number of bits in X, Y and M.
     *  4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG).
     *  5. Load X, Y, M, r' operands to memory blocks.
     *  6. Start the operation by writing 1 to RSA_MODEXP_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  7. Read the result out.
     *  8. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. Wait until hardware is ready. */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        ESP_LOGW(TAG, "esp_mp_hw_wait_clean failed!");
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write (N_result_bits/32 - 1) to the RSA_MODE_REG. */
    OperandBits = max(max(Xs, Ys), Ms);
    if (OperandBits > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    WordsForOperand = bits2words(OperandBits);
    DPORT_REG_WRITE(RSA_LENGTH_REG, WordsForOperand - 1);

    /* 4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG) */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);

    /* 5. Load X, Y, M, r' operands. */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE, &r_inv,
                          mp_count_bits(&r_inv), hwWords_sz);

    /* 6. Start operation and wait until it completes. */
    process_start(RSA_MODEXP_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        ESP_LOGW(TAG, "wait_until_done failed");
        return ret;
    }

    /* 7. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Ms));

    /* 8. clear and release HW                    */
    esp_mp_hw_unlock();

    mp_clear(&r_inv);

    /* end if CONFIG_IDF_TARGET_ESP32S3 */
#else
    /* non-ESP32S3 Xtensa (regular ESP32) */

    /* Steps to use HW in the following order:
    * 1. Write(N/512bits - 1) to MODEXP_MODE_REG
    * 2. Write X, Y, M and r_inv to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 3. Write M' to M_PRIME_REG
    * 4. Write 1  to MODEXP_START_REG
    * 5. Wait for the operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 6. Read the result Z(=Y) from Z_MEM
    * 7. Write 1 to INTERRUPT_REG to clear the interrupt.
    */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        return ret;
    }

    /* step.1                                         */
    ESP_LOGV(TAG, "hwWords_sz = %d, num = %d",
                   mph->hwWords_sz, (mph->hwWords_sz >> 4) - 1);

    DPORT_REG_WRITE(RSA_MODEXP_MODE_REG, (mph->hwWords_sz >> 4) - 1);
    /* step.2 write G, X, P, r_inv and M' into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, mph->Xs, mph->hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, mph->Ys, mph->hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, mph->Ms, mph->hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE,
                          &(mph->r_inv),
                          mph->Rs,
                          mph->hwWords_sz);

    /* step.3 write M' into memory                    */
    ESP_LOGV(TAG, "M' = %d", mph->mp);
    DPORT_REG_WRITE(RSA_M_DASH_REG, mph->mp);
    asm volatile("memw");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");

    /* step.4 start process                           */
    process_start(RSA_MODEXP_START_REG); /* was RSA_START_MODEXP_REG;
                                          * RSA_MODEXP_START_REG as in docs? */

    /* step.5 wait until done                         */
    wait_until_done(RSA_INTERRUPT_REG);
    /* step.6 read a result form memory               */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(mph->Ms));
    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

#endif /* regular ESP32 */

#ifdef DEBUG_WOLFSSL
    if (esp_mp_exptmod_depth_counter != 1) {
        ESP_LOGE(TAG, "esp_mp_exptmod exit Depth Counter Error!");
    }
    esp_mp_exptmod_depth_counter--;
#endif

     esp_clean_result(Z, 0);

    return ret;
}
#endif /* ! NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI_EXPTMOD */

#endif /* WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI) &&
        * !NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI */

#endif /* !NO_RSA || HAVE_ECC */


#ifdef WOLFSSL_HW_METRICS
int esp_hw_show_mp_metrics(void)
{
    int ret = MP_OKAY;

    /* Metrics: esp_mp_mul() */
    ESP_LOGI(TAG, "Number of calls to esp_mp_mul: %lu",
                   esp_mp_mul_usage_ct);
    if (esp_mp_mul_error_ct == 0) {
        ESP_LOGI(TAG, "Success: no esp_mp_mul() errors.");
    }
    else {
        ESP_LOGW(TAG, "Number of esp_mp_mul failures: %lu",
                       esp_mp_mul_error_ct);
        ret = MP_VAL;
    }

    /* Metrics: esp_mp_mulmod() */
    ESP_LOGI(TAG, "Number of calls to esp_mp_mulmod: %lu",
                   esp_mp_mulmod_usage_ct);

    if (esp_mp_mulmod_error_ct == 0) {
        ESP_LOGI(TAG, "Success: no esp_mp_mulmod errors.");
    }
    else {
        ESP_LOGW(TAG, "Number of esp_mp_mulmod failures: %lu",
                       esp_mp_mulmod_error_ct);
        ret = MP_VAL;
    }

    if (esp_mp_mulmod_even_mod_ct == 0) {
        ESP_LOGI(TAG, "Success: no esp_mp_mulmod even mod.");
    }
    else {
        ESP_LOGW(TAG, "Number of esp_mp_mulmod even mod: %lu",
                       esp_mp_mulmod_even_mod_ct);
        ret = MP_VAL;
    }

    if (esp_mp_mulmod_error_ct == 0) {
        ESP_LOGI(TAG, "Success: no esp_mp_mulmod small y.");
    }
    else {
        ESP_LOGW(TAG, "Number of esp_mp_mulmod small y: %lu",
                       esp_mp_mulmod_small_y_ct);
        ret = MP_VAL;
    }

    return ret;
}
#endif

