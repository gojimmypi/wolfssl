#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/ecc.h>

int main(void) {
    int ret = 0;

    // Load the ECC certificate file
    const char* cert_file = "/path/to/ecc/certificate.pem";
    FILE* fp = fopen(cert_file, "r");
    if (fp == NULL) {
        printf("Failed to open certificate file: %s\n", cert_file);
        return 1;
    }
    WOLFSSL_X509* cert = wolfSSL_X509_new();
    ret = wolfSSL_X509_read(cert, fp, NULL, 0);
    if (ret != 1) {
        printf("Failed to read ECC certificate from file: %s\n", cert_file);
        wolfSSL_X509_free(cert);
        fclose(fp);
        return 1;
    }
    fclose(fp);

    // Get the ECC key from the certificate
    WOLFSSL_ECC_KEY* ecc_key = wolfSSL_X509_getECC(cert);
    if (ecc_key == NULL) {
        printf("Failed to get ECC key from certificate\n");
        wolfSSL_X509_free(cert);
        return 1;
    }

    // Print the ECC key's public point
    byte pub_key_buf[256];
    word32 pub_key_size = sizeof(pub_key_buf);
    ret = wc_ecc_export_x963(ecc_key, pub_key_buf, &pub_key_size);
    if (ret != 0) {
        printf("Failed to export ECC public key\n");
        wolfSSL_X509_free(cert);
        return 1;
    }
    printf("ECC public key:\n");
    for (int i = 0; i < pub_key_size; i++) {
        printf("%02x", pub_key_buf[i]);
    }
    printf("\n");

    wolfSSL_X509_free(cert);
    return 0;
}
