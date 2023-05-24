#include "alt_hw_test.h"
#include "wolfcrypt/test/test.h"
#include <esp_log.h>
#include <wolfssl/wolfcrypt/wolfmath.h>


#ifdef INCLUDE_ALT_HW_TEST
#include <mbedtls/bignum.h>

static const char * TAG = "ALT_HW_TEST";

/* Alternate mulmod hardware calc comparison
 * Large Number Modular Multiplication Z = X × Y mod M
 *
 * The expected answer needs to be supplied in E.
 */
int alt_mulmod_hw_compare(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* M, MATH_INT_T* e)
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
    mx->private_n = X->used;
    mx->private_p = (mbedtls_mpi_uint*)&X->dp;

    my->private_n = Y->used;
    my->private_p = (mbedtls_mpi_uint*)&Y->dp;

    mm->private_n = M->used;
    mm->private_p = (mbedtls_mpi_uint*)&M->dp;

    // int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M)
    /* Z = (X * Y) mod M */
    ret = esp_mpi_mul_mpi_mod(mz, mx, my, mm);

    if (0 == XMEMCMP(e->dp, mz->private_p, mz->private_s * 4)) {
        ESP_LOGI(TAG, "mbedtls success: matched result!");
    }
    else {
        ESP_LOGW(TAG, "mbedtls failed! calculated result did not match expected E value.");
    }

    return ret;
}
#endif

