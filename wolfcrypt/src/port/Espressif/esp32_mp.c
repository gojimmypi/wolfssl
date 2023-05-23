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
//static const char* MP_TAG = "MATH_INT_T2 omit";

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

static const char* const TAG = "wolfssl_esp32_mp";

#define ESP_HW_RSAMAX_BIT           4096
#define ESP_HW_MULTI_RSAMAX_BITS    2048
#define ESP_HW_RSAMIN_BIT           512
#define BYTE_TO_WORDS(s)            (((s+3)>>2))         /* (s+(4-1))/ 4    */
#define BITS_TO_WORDS(s)            (((s+31)>>3)>>2)     /* (s+(32-1))/ 8/ 4*/
#define BITS_IN_ONE_WORD            32

#define MP_NG   -1

#define ESP_TIMEOUT(cnt)         (cnt >= ESP_RSA_TIMEOUT_CNT)

/* mutex */
static wolfSSL_Mutex mp_mutex;
static int espmp_CryptHwMutexInit = 0;

#ifdef DEBUG_WOLFSSL
    /* when debugging, we'll double-check the mutex with call depth */
    static int esp_mp_exptmod_depth_counter = 0;
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
   * hwcrypto_reg.h maintains RSA_CLEAN_REG for backwards compatibility:
   * so this block _might_ not be needed. */
    asm volatile("memw");

    /* wait until ready,
     * or timeout counter exceeds ESP_RSA_TIMEOUT_CNT in user_settings */

    while(!ESP_TIMEOUT(++timeout) && DPORT_REG_READ(RSA_CLEAN_REG) == 0) {
        /*  wait. expected delay 1 to 2 uS  */
        asm volatile("memw");
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
        /* ESP AES has already been initialized */
    }

    /* Set our mutex to indicate the HW is in use */
    if (ret == 0) {
        /* lock hardware */
        /* we don't actually want to wait forever  TODO */
        ret = esp_CryptHwMutexLock(&mp_mutex, portMAX_DELAY);
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
//        if (0 != peek_periph_module_counter(PERIPH_RSA_MODULE)) {
//            ESP_LOGW(TAG, "peek_periph_module_counter not zero!!");
//        };
        periph_module_enable(PERIPH_RSA_MODULE);

        /* clear bit to enable hardware operation; (set to disable) */

        /*
         * We may consider first disabling, then enable fresh:
         *
         * DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
         * asm volatile("memw");
         */
        DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
        asm volatile("memw");
    }
#endif

    /* reminder: wait until RSA_CLEAN_REG reads 1
     *  see esp_mp_hw_wait_clean()
     */

    ESP_LOGV(TAG, "leave esp_mp_hw_lock");
    return ret;
}

/*
*   Release HW engine
*/
static void esp_mp_hw_unlock( void )
{
#if CONFIG_IDF_TARGET_ESP32S3
    /* Deactivate the RSA accelerator. See 20.3 of ESP32-S3 technical manual.
     * periph_module_enable doesn't seem to be documented and in private folder
     * with v5 release. Maybe it will be deprecated? */
    DPORT_REG_SET_BIT(SYSTEM_RSA_PD_CTRL_REG, SYSTEM_RSA_MEM_PD);
    periph_module_disable(PERIPH_RSA_MODULE);

#else
    /* set bit to disabled hardware operation; (clear to enable)
     */
    DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    /* Disable RSA hardware */
    periph_module_disable(PERIPH_RSA_MODULE);
#endif

    /* unlock */
    esp_CryptHwMutexUnLock(&mp_mutex);

    ESP_LOGV(TAG, "unlock");
}
/* M' M-Prime Calculation for HW Accelerator */

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
    MATH_INT_T X[1] = { };
    MATH_INT_T P[1] = { };
    MATH_INT_T Y[1] = { };
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

    ESP_LOGI(TAG, "esp_calc_Mdash %u", *md);
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

/* the result may need to have extra bytes zeroed or used length adjusted */
static int esp_clean_result(MATH_INT_T* Z, int used_padding)
{
    int ret = MP_OKAY;
    int this_extra;
    this_extra = Z->used;

    while (Z->dp[this_extra] > 0 && (this_extra < FP_SIZE)) {
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
    return ret;
}

/* start HW process */
static void process_start(word32 reg)
{
    /* clear interrupt */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);
    /* start process  */
    DPORT_REG_WRITE(reg, 0);
    asm volatile("memw"); /* TODO confirm all compiler settings do this */
    DPORT_REG_WRITE(reg, 1);
    asm volatile("memw");
}

/* wait until done */
static int wait_until_done(volatile u_int32_t reg)
{
    word32 timeout = 0;
    /* wait until done && not timeout */
    asm volatile("memw");
    while (!ESP_TIMEOUT(++timeout) &&
                DPORT_REG_READ(reg) != 1) {
//        asm volatile("memw"); /* wait */
        asm volatile("nop"); /* wait */
    }

    /* clear interrupt */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);
    asm volatile("memw");

    if (ESP_TIMEOUT(timeout)) {
        ESP_LOGE(TAG, "rsa operation timed out.");
        return MP_NG;
    }

    return MP_OKAY;
}

/* read data from memory into mp_init          */
static // __attribute__((optimize("O0")))
int esp_memblock_to_mpint(volatile const u_int32_t mem_address,
                          volatile MATH_INT_T* mp,
                                 word32 numwords)
{
    int ret = FP_OKAY;
#ifdef USE_ESP_DPORT_ACCESS_READ_BUFFER
    esp_dport_access_read_buffer((uint32_t*)mp->dp, mem_address, numwords);
#else
    int try_ct = 20;
    // DPORT_INTERRUPT_DISABLE();
    portDISABLE_INTERRUPTS();
    __asm__ volatile ("memw");
    do {
        try_ct--;
        mp->dp[0] = DPORT_SEQUENCE_REG_READ(mem_address);
    } while (try_ct > 0 && (mp->dp[0] == 0));

        __asm__ __volatile__ ("memw"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
    for (volatile uint32_t i = 1;  i < numwords; ++i) {
        __asm__ __volatile__ ("memw");
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
        __asm__ __volatile__ ("nop"); /* wait */
//        __asm__ __volatile__ ("memw");
        DPORT_SEQUENCE_REG_READ(0x3FF40078);
        mp->dp[i] = DPORT_SEQUENCE_REG_READ(mem_address + i * 4);
    }
    portENABLE_INTERRUPTS();
#endif
    mp->used = numwords;

#if defined(ESP_VERIFY_MEMBLOCKx)
    ret = XMEMCMP((const void *)mem_address, mp->dp, numwords * sizeof(word32));
    if (ret != 0 ) {
        ESP_LOGW(TAG, "Validation Failure esp_memblock_to_mpint.\n"
                      "Reading %u Words at Address =  0x%08x",
                       (int)(numwords * sizeof(word32)),
                       (unsigned int)mem_address);
        ESP_LOGI(TAG, "Trying again... %d ", try_ct);
        esp_dport_access_read_buffer((uint32_t*)mp->dp, mem_address, numwords);
        mp->used = numwords;
        if (0 != XMEMCMP((const void *)mem_address, mp->dp, numwords * sizeof(word32))) {
            ESP_LOGE(TAG, "Validation Failure esp_memblock_to_mpint "
                           "a second time. Giving up.");
            ret = FP_VAL;
        }
        else {
            ESP_LOGI(TAG, "Successfully re-read after Validation Failure.");
            ret = FP_OKAY;
        }
    }
#endif
    return ret;
}

static int esp_zero_memblock(volatile const u_int32_t mem_address,
                            int wordSz)
{
    int ret = MP_OKAY;
    portDISABLE_INTERRUPTS();
    for (int i=0; i < wordSz; i++) {
        __asm__ volatile ("memw");
        DPORT_REG_WRITE((volatile void *)mem_address + (i * sizeof(word32)), 0);
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
    }
    return ret;
}

/* write mp_init into memory block
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

    portDISABLE_INTERRUPTS();
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
            //DPORT_REG_WRITE(mem_address + (i * sizeof(word32)), 0);
        }
    }

#if defined(ESP_VERIFY_MEMBLOCKx)
    len = XMEMCMP((const void *)mem_address, (const void*)mp->dp, (int)len);
    if (len != 0) {
        ESP_LOGE(TAG, "esp_mpint_to_memblock compare fails at %d", len);
        ret = FP_VAL;
    }
#endif
    portENABLE_INTERRUPTS();
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

/* get rinv */
static int esp_get_rinv(MATH_INT_T *rinv, MATH_INT_T *M, word32 exp)
{
    ESP_LOGV(TAG, "\nBegin esp_get_rinv \n");

    int ret = 0;

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

    ESP_LOGV(TAG, "\nEnd esp_get_rinv \n");
    return ret;
}

static int hw_validation = 0;
static int esp_mp_mul_usage_ct = 0;
static int esp_mp_mul_error_ct = 0;
#define SET_HW_VALIDATION {hw_validation = 1;}
#define CLR_HW_VALIDATION {hw_validation = 0;}
#define IS_HW_VALIDATION (hw_validation == 1)

int esp_show_usage_metrics(void)
{
    int ret = FP_OKAY;
    ESP_LOGI(TAG, "Number of calls to esp_mp_mul: %d", esp_mp_mul_usage_ct);
    if (esp_mp_mul_error_ct == 0) {
        ESP_LOGI(TAG, "Success: no esp_mp_mul() errors.");
    }
    else {
        ESP_LOGW(TAG, "Number of esp_mp_mul failures: %d", esp_mp_mul_error_ct);
        ret = FP_VAL;
    }
    return ret;
}

int esp_hw_validation_active(void)
{
    return IS_HW_VALIDATION;
}

/* Large Number Modular Multiplication
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

//    ESP_LOGV(TAG, "\nBegin esp_mp_mul \n");
//    if (X == Z) {
//        ESP_LOGW(TAG, "mp_mul X == Z");
//    }
//    if (Y == Z) {
//        ESP_LOGW(TAG, "mp_mul Y == Z");
//    }

    /* if either operand is zero, there's nothing to do.
     * Y checked first, as it was observed to be zero during
     * wolfcrypt tests more often than X */
    if (mp_iszero(Y) || mp_iszero(X)) {
        mp_forcezero(Z);
        return FP_OKAY;
    }

#ifdef DEBUG_WOLFSSL
    MATH_INT_T X2[1];
    MATH_INT_T Y2[1];
    MATH_INT_T Z2[1];
    MATH_INT_T PEEK[1];
    if (IS_HW_VALIDATION) {
        ESP_LOGE(TAG, "Caller must not try HW when validation active."); /* TODO handle with semaphore  */
    }
    else {

        mp_init(X2);
        mp_init(Y2);
        mp_init(Z2);

        mp_copy(X, X2); /* copy (src = X) to (dst = X2) */
        mp_copy(Y, Y2); /* copy (src = Y) to (dst = Y2) */
        mp_copy(Z, Z2); /* copy (src = Z) to (dst = Z2) */

        SET_HW_VALIDATION;
        mp_mul(X2, Y2, Z2);
        CLR_HW_VALIDATION;
    }
#endif /* DEBUG_WOLFSSL */

#ifdef WOLFSSL_SP_INT_NEGATIVE
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
                    ret = FP_VAL;
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

    DPORT_REG_WRITE(RSA_MULT_MODE_REG, (2*left_pad_offset*8/512) -1 + 8);

    /* step.2 write X into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE,
                          X,
                          Xs,
                          hwWords_sz);



    /* write zeros from RSA_MEM_Z_BLOCK_BASE to left_pad_offset - 1 */
    esp_zero_memblock(RSA_MEM_Z_BLOCK_BASE, (left_pad_offset -1)/sizeof(int));
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE + (left_pad_offset), /* hwWords_sz<<2 */
                          Y,
                          Ys,
                          hwWords_sz);
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE,
                          PEEK,
                          128);

    asm volatile("memw");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");

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
#ifdef WOLFSSL_SP_INT_NEGATIVE
    if (!mp_iszero(Z) && neg) {
        /* for non-zero negative numbers, set negative flag for our result:
         *   Z->sign = FP_NEG */
        mp_setneg(Z);
    }
#endif
    Z->sign = X->sign ^ Y->sign;
    int found_z_used = Z->used;
    esp_clean_result(Z, 0);

#ifdef DEBUG_WOLFSSL
    esp_mp_mul_usage_ct++;
    if (fp_cmp(X, X2) != 0) {
        // ESP_LOGE(TAG, "mp_mul X vs X2 mismatch!");
    }
    if (fp_cmp(Y, Y2) != 0) {
        // ESP_LOGE(TAG, "mp_mul Y vs Y2 mismatch!");
    }
    if (fp_cmp(Z, Z2) != 0) {
        esp_mp_mul_error_ct++;

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
        ret = FP_VAL;
    #endif
    }
#endif

    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

    ESP_LOGV(TAG, "\nEnd esp_mp_mul \n");

    return ret;
}

/* Large Number Modular Multiplication
 *
 * See 24.3.3 of the ESP32 Technical Reference Manual
 *
 * Z = X × Y mod M */
int esp_mp_mulmod(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* M, MATH_INT_T* Z)
{
    ESP_LOGV(TAG, "\nBegin esp_mp_mulmod \n");
    /* not working properly */
    int ret = 0;
    int negcheck;
    word32 Xs;
    word32 Ys;
    word32 Ms;
    word32 Rs;
    word32 maxWords_sz;
    word32 hwWords_sz;
    word32 zwords;
    uint32_t Exponent;
    mp_digit mp;
    MATH_INT_T r_inv[1];
    MATH_INT_T tmpZ[1];

#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    int WordsForOperand;
# endif

    if (X->used == 32)
    {
        ESP_LOGV(TAG, "X->used == 32!");
//        esp_show_mp("a", X);
//        esp_show_mp("b", Y);
//        esp_show_mp("c", M);
    }

    /* neg check - X*Y becomes negative */
    negcheck = mp_isneg(X) != mp_isneg(Y) ? 1 : 0;

    /* determine number of bits for HW parameter */
    Xs = mp_count_bits(X);
    Ys = mp_count_bits(Y);
    Ms = mp_count_bits(M);
    ESP_LOGV(TAG, "Bits: Xs = %d, Ys = %d, Ms = %d", Xs, Ys, Ms);

    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, max(Ys, Ms)));
    zwords      = bits2words(min(Ms, Xs + Ys));
    hwWords_sz  = words2hwords(maxWords_sz);

#ifdef SEARCHING_FOR_INTERESTING_VALUES
    if ( (Xs == 1023) && (Ys = 1024) && (Ms = 1024) ) {
        ESP_LOGI(TAG, "Challenge Property Match!");
        esp_show_mp("a[0]", X);
        esp_show_mp("b[0]", Y);
        esp_show_mp("c[0]", M);
    }
#endif

    ESP_LOGV(TAG, "Words:, maxWords_sz = %d, zwords = %d, hwWords_sz = %d",
                  maxWords_sz, zwords, hwWords_sz);

    if ((hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGE(TAG, "exceeds HW maximum bits = %d", ESP_HW_RSAMAX_BIT);
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    /* calculate r_inv = R^2 mod M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */
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
        return ret;
    }
    else {
        Rs = mp_count_bits(r_inv);
        /* success, show details */
#ifdef DEBUG_WOLFSSL
        ESP_LOGI(TAG, "r_inv bits = %d", Rs);
        ESP_LOGI(TAG, "Exponent   = %lu (0x%08x)", Exponent, (unsigned int)Exponent);
        /* optionally show M and R inv
           esp_show_mp("    M", M);
           esp_show_mp("r_inv", r_inv);
        */
#endif
    }

    /* Calculate M' */
    if ((ret = esp_calc_Mdash(M, 32/* bits */, &mp)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate M dash");
        mp_clear(tmpZ);
        mp_clear(r_inv);
        return ret;
    }
    else {
#ifdef DEBUG_WOLFSSL
        ESP_LOGI(TAG, "mp = %u", mp);
#endif
   }

    /* lock HW for use, enable peripheral clock */
    if ((ret = esp_mp_hw_lock()) != MP_OKAY) {
        mp_clear(tmpZ);
        mp_clear(r_inv);
        return ret;
    }

#if CONFIG_IDF_TARGET_ESP32S3
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
#else
    /* non-S3 Xtensa */

    /*Steps to use HW in the following order:
    * 1. wait until clean HW engine
    * 2. Write(N/512bits - 1) to MULT_MODE_REG
    * 3. Write X,M(=G, X, P) to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 4. Write M' to M_PRIME_REG
    * 5. Write 1  to MODEXP_START_REG
    * 6. Wait for the first operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 7. Write 1 to RSA_INTERRUPT_REG to clear the interrupt.
    * 8. Write Y to RSA_X_MEM
    * 9. Write 1 to RSA_MULT_START_REG
    * 10. Wait for the second operation to be completed. Poll INTERRUPT_REG until it reads 1.
    * 11. Read the Z from RSA_Z_MEM
    * 12. Write 1 to RSA_INTERUPT_REG to clear the interrupt.
    * 13. Release the HW engine
    */

    if ( (ret = esp_mp_hw_wait_clean()) != MP_OKAY ) {
        return ret;
    }
    /* step.1                     512 bits => 16 words */
    DPORT_REG_WRITE(RSA_MULT_MODE_REG, (hwWords_sz >> 4) - 1);

    /* step.2 write X, M and r_inv into memory.
     * The capacity of each memory block is 128 words.
     * The memory blocks use the little endian format for storage,
     * i.e. the least significant digit of each number is in lowest address.*/
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X,     Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M,     Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE, r_inv, Rs, hwWords_sz);

    /* step.3 write M' into memory                   */
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

    /* step.8 start process                           */
    process_start(RSA_MULT_START_REG);

    /* step.9,11 wait until done                      */
    wait_until_done(RSA_INTERRUPT_REG);

    /* step.12 read the result from MEM_Z             */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, tmpZ, zwords);

    /* step.13 clear and release HW                   */
    esp_mp_hw_unlock();
#endif

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
//  wip

    mp_copy(tmpZ, Z); /* copy tmpZ to result Z */

    mp_clear(tmpZ);
    mp_clear(r_inv);

    esp_clean_result(Z, 0);

    ESP_LOGV(TAG, "\nEnd esp_mp_mulmod \n");
    return ret;

}

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
.* Note some DH references may use: Y = (G ^ X) mod P
 */
int esp_mp_exptmod(MATH_INT_T* X, MATH_INT_T* Y, word32 Ys, MATH_INT_T* M, MATH_INT_T* Z)
{
    ESP_LOGV(TAG, "\nBegin esp_mp_exptmod \n");

#ifdef DEBUG_WOLFSSL
    if (esp_mp_exptmod_depth_counter != 0) {
        ESP_LOGE(TAG, "esp_mp_exptmod Depth Counter Error!");
    }
    esp_mp_exptmod_depth_counter++;
#endif
     /* Danger! Do not initialize any parameters, not even the result Z.
      * Some operations such as (rnd = rnd^e) will wipe out the rnd operand
      * value upon initialization.
      * (e.g. the address of X and Z could be the same when called) */
    int ret = 0;
    word32 Xs;
    word32 Ms;
    word32 maxWords_sz;
    word32 hwWords_sz;

    MATH_INT_T r_inv;
    mp_digit mp;

#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    uint32_t WordsForOperand;
#endif
/*
 max bits = 0x400 = 1024 bits
1024 / 8 = 128 bytes
 128 / 4 = 32 words (0x20)
 */

    /* ask bits number */
    Xs = mp_count_bits(X);
    Ms = mp_count_bits(M);
    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, max(Ys, Ms)));
    hwWords_sz  = words2hwords(maxWords_sz);

    ESP_LOGV(TAG, "hwWords_sz = %d", hwWords_sz);
    if ((hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGE(TAG, "exceeds HW maximum bits");
        return MP_VAL; /*  Error: value is not able to be used. */
    }

    /* calculate r_inv = R^2 mode M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */
    ret = mp_init(&r_inv);
    if ( (ret == 0) &&
         ((ret = esp_get_rinv(&r_inv, M, (hwWords_sz << 6))) != MP_OKAY) ) {
        ESP_LOGE(TAG, "calculate r_inv failed.");
        mp_clear(&r_inv);
        return ret;
    }

    /* lock and init the HW                           */
    if ( (ret = esp_mp_hw_lock()) != MP_OKAY ) {
        ESP_LOGE(TAG, "esp_mp_hw_lock failed");
        mp_clear(&r_inv);
        return ret;
    }
    /* calc M' */
    /* if Pm is odd, uses mp_montgomery_setup() */
    if ( (ret = esp_calc_Mdash(M, 32/* bits */, &mp)) != MP_OKAY ) {
        ESP_LOGE(TAG, "failed to calculate M dash");
        mp_clear(&r_inv);
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
                   hwWords_sz, (hwWords_sz >> 4) - 1);

    DPORT_REG_WRITE(RSA_MODEXP_MODE_REG, (hwWords_sz >> 4) - 1);
    /* step.2 write G, X, P, r_inv and M' into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE,
                          &r_inv,
                          mp_count_bits(&r_inv),
                          hwWords_sz);
    /* step.3 write M' into memory                    */
    ESP_LOGV(TAG, "M' = %d", mp);
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);
    /* step.4 start process                           */
    process_start(RSA_MODEXP_START_REG); /* was RSA_START_MODEXP_REG;
                                          * RSA_MODEXP_START_REG as in docs? */

    /* step.5 wait until done                         */
    wait_until_done(RSA_INTERRUPT_REG);
    /* step.6 read a result form memory               */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Ms));
    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

    mp_clear(&r_inv);

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

#endif /* WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI) &&
        * !NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI */

#endif /* !NO_RSA || HAVE_ECC */
