#include "alt_hw_test.h"
#include "wolfcrypt/test/test.h"
#include <esp_log.h>
#include <wolfssl/wolfcrypt/wolfmath.h>

#include <mbedtls/bignum.h>

static const char * TAG = "ALT_HW_TEST";
/* Alternate mulmod hardware calc comparison
 * Large Number Modular Multiplication Z = X × Y mod M */
int alt_mulmod_hw_compare(MATH_INT_T* a, MATH_INT_T* b, MATH_INT_T* c, MATH_INT_T* e)
{
    int ret = MP_OKAY; /* assume success until proven otherwise */
    //int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
    mbedtls_mpi * mz = malloc(sizeof(mbedtls_mpi));
    mbedtls_mpi * mx = malloc(sizeof(mbedtls_mpi));
    mbedtls_mpi * my = malloc(sizeof(mbedtls_mpi));
    mbedtls_mpi * mm = malloc(sizeof(mbedtls_mpi));

    mbedtls_mpi_init(mz);
    mbedtls_mpi_init(mx);
    mbedtls_mpi_init(my);
    mbedtls_mpi_init(mm);
    mx->private_n = a->used;
    mx->private_p = (mbedtls_mpi_uint*)&a->dp;

    my->private_n = b->used;
    my->private_p = (mbedtls_mpi_uint*)&b->dp;

    mm->private_n = c->used;
    mm->private_p = (mbedtls_mpi_uint*)&c->dp;

    // int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
    /* Z = (X * Y) mod M */
    ret = esp_mpi_mul_mpi_mod(mz, mx, my, mm);

    if (0 == XMEMCMP(e->dp, mz->private_p, mz->private_s * 4)) {
        ESP_LOGI(TAG, "mbedtls match!");
    }
    else {
        ESP_LOGW(TAG, "no mbedtls match!");
    }

    return ret;
}

