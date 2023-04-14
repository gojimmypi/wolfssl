/* wolfssl ecc key demo main.c
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

/* Espressif */
#include "esp_log.h"

/*
** This ESP32 project does not use the generated options.h
#include <wolfssl/options.h>
*/
#include <wolfssl/wolfcrypt/settings.h>

/* properly configured, user_settings.h should have been captured in above
** settings.h but is included here for reference: */
#include "user_settings.h"
#include <wolfssl/wolfcrypt/ecc.h>


static const char *TAG = "wolfSSL ecc key demo";
#define WOLFSSL_VERSION_PRINTF(...) ESP_LOGI(TAG, __VA_ARGS__)

/*
** code as-found in https://github.com/espressif/esp-wolfssl/issues/21
**
** "My program receives an ephemeral_key (publickey) in the format
** ProtobufCBinaryData.
**
** I want to use my private key (private_key ECC_SECP256R1) and
** ephemeral_key to generate a shared key, What do I do when I always
** return '-170' when using wc_ecc_shared_secret?
**
*/
struct ecc_key my_private_key; // ECC_SECP256R1

/* 042493a5d09d8beb47ccde8fac187b86ad009910c94122fd8deae398604d897e5941839388606b228f357ad17eb66c26ac01770b60e4869b20c799d3 765b5fc174 */
#define  MY_PUBLIC_KEY_SIZE 65
byte my_public_key[MY_PUBLIC_KEY_SIZE] = {0x04, 0x24, 0x93, 0xa5, 0xd0, 0x9d, 0x8b, 0xeb,
                          0x47, 0xcc, 0xde, 0x8f, 0xac, 0x18, 0x7b, 0x86,
                          0xad, 0x00, 0x99, 0x10, 0xc9, 0x41, 0x22, 0xfd,
                          0x8d, 0xea, 0xe3, 0x98, 0x60, 0x4d, 0x89, 0x7e,
                          0x59, 0x41, 0x83, 0x93, 0x88, 0x60, 0x6b, 0x22,
                          0x8f, 0x35, 0x7a, 0xd1, 0x7e, 0xb6, 0x6c, 0x26,
                          0xac, 0x01, 0x77, 0x0b, 0x60, 0xe4, 0x86, 0x9b,
                          0x20, 0xc7, 0x99, 0xd3, 0x76, 0x5b, 0x5f, 0xc1,
                          0x74 }; // wc_ecc_export_x963_ex()
int curveId = ECC_SECP256R1;
char* GATTC_TAG = "GATTC_TAG";
struct ecc_key my_ecc_public_key;

/* see
** https://protobuf-c.github.io/protobuf-c/structProtobufCBinaryData.html
*/
struct ProtobufCBinaryData
{
    size_t 	len;
    byte* data;
};

/* https://www.wolfssl.com/documentation/manuals/wolfssl/group__ECC.html
*/

#if (0)
static void generate_public_key(void)
{
    int ret;
    word32 buffSz = sizeof(my_public_key);

    ret = wc_ecc_export_x963_ex(&my_private_key, (byte *)my_public_key, &buffSz, 0);
    if (ret != 0) {
        // error exporting key
        ESP_LOGI(TAG, "error exporting public key");
    }
    ESP_LOGI(TAG, "public key: ");
    for (int i = 0; i < 65; i++) {
        printf("%02x", (unsigned char)my_public_key[i]);
    }
}
static void generate_private_key(void)
{
    int ret;

    int private_key_pem_end = 0, private_key_pem_start = 0;

    long fileSz = private_key_pem_end - private_key_pem_start;
    int inSz = (int)fileSz;

    word32 idx = 0;

    wc_ecc_init(&my_private_key); // initialize key

    byte der[ECC_BUFSIZE];

    wc_KeyPemToDer(private_key_pem_start, (int)fileSz, &der, ECC_BUFSIZE, NULL);

    ret = wc_EccPrivateKeyDecode(&der, &idx, &my_private_key, (word32)inSz);
    if (ret < 0) {
        ESP_LOGI(GATTC_TAG, "error decoding ecc key "); // error decoding ecc key
    }

    int check_result;

    check_result = wc_ecc_check_key(&my_private_key);

    if (check_result == MP_OKAY) {
        ESP_LOGI(GATTC_TAG, "check_private_key yes==============");
    }
    else {
        ESP_LOGI(GATTC_TAG, "check_private_key no==============");
    }
}

static void other_demo()
{
    int ret = 0;
    RNG rng;
    wc_InitRng(&rng);

    ecc_key *pub_key = (ecc_key *)XMALLOC(sizeof *pub_key, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    ecc_key *priv_key = (ecc_key *)XMALLOC(sizeof *priv_key, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);

    XMEMSET(pub_key, 0, sizeof *pub_key);
    XMEMSET(priv_key, 0, sizeof *priv_key);

    /* setup private and public keys */
    ret = wc_ecc_init(pub_key);
    if (ret == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_init for pub_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_init for pub_key. Error = %d", ret);
    }

    ret = wc_ecc_init(priv_key);

    ret = wc_ecc_make_key(&rng, 32, pub_key);
    if (ret == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_make_key for pub_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_make_key for pub_key. Error = %d", ret);
    }
    ret = wc_ecc_check_key(pub_key);
    if (ret == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_check_key for pub_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_check_ke for pub_keyy. Error = %d", ret);
    }


    wc_ecc_set_flags(pub_key, WC_ECC_FLAG_COFACTOR);
    wc_ecc_set_flags(priv_key, WC_ECC_FLAG_COFACTOR);
}
#endif

int to_load_key(struct ProtobufCBinaryData ephemeral_key)
{
    int rc = 0;
    byte shared_key[1024] = { 0 };
    word32 shared_key_len = sizeof(shared_key);
    ecc_key my_ecc_private_key;

    RNG rng;

    wc_InitRng(&rng);

    /*
    ** Public Key
    */
    ESP_LOGI(TAG, "\n********************************************************"
                  "\n\n  Public Key Processing:  \n"
                  "\n********************************************************");

    rc = wc_ecc_init(&my_ecc_public_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_init for my_ecc_public_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_init for my_ecc_public_key. Error = %d", rc);
    }

    rc = wc_ecc_import_x963((byte *)ephemeral_key.data, (word32)ephemeral_key.len, &my_ecc_public_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_import_x963 for my_ecc_public_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_import_x963 for my_ecc_public_key. Error = %d", rc);
    }

    rc = wc_ecc_check_key(&my_ecc_public_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_check_key for my_ecc_public_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_check_key for my_ecc_public_key. Error = %d", rc);
    }

    /*

    -----BEGIN PRIVATE KEY-----    PEM-encoded ASN.1
    -----BEGIN EC PRIVATE KEY----- PEM-encoded ANSI X9.62


    -----BEGIN EC PRIVATE KEY-----
    MHcCAQEEIGXOdeNRODqoWF2G1Pv5c/9606thVSgAR2b0MDRZErxwoAoGCCqGSM49
    AwEHoUQDQgAEJJOl0J2L60fM3o+sGHuGrQCZEMlBIv2N6uOYYE2JfllBg5OIYGsi
    jzV60X62bCasAXcLYOSGmyDHmdN2W1/BdA==
    -----END EC PRIVATE KEY-----

    show private key,

    openssl ec -in my_private_key_example.pem.txt  -text -noout
    */

    /* public/private keys from  openssl:

        $ openssl ec -in my_private_key_example.pem.txt  -text -noout
        read EC key
        Private-Key: (256 bit)
        priv:
            65:ce:75:e3:51:38:3a:a8:58:5d:86:d4:fb:f9:73:
            ff:7a:d3:ab:61:55:28:00:47:66:f4:30:34:59:12:
            bc:70
        pub:
            04:24:93:a5:d0:9d:8b:eb:47:cc:de:8f:ac:18:7b:
            86:ad:00:99:10:c9:41:22:fd:8d:ea:e3:98:60:4d:
            89:7e:59:41:83:93:88:60:6b:22:8f:35:7a:d1:7e:
            b6:6c:26:ac:01:77:0b:60:e4:86:9b:20:c7:99:d3:
            76:5b:5f:c1:74
        ASN1 OID: prime256v1
        NIST CURVE: P-256

    **/
    unsigned char private_key[] = {
        0x65, 0xce, 0x75, 0xe3, 0x51, 0x38, 0x3a, 0xa8, 0x58, 0x5d, 0x86, 0xd4, 0xfb, 0xf9, 0x73,
        0xff, 0x7a, 0xd3, 0xab, 0x61, 0x55, 0x28, 0x00, 0x47, 0x66, 0xf4, 0x30, 0x34, 0x59, 0x12,
        0xbc, 0x70
    };

    unsigned char public_key[] = {
        0x04, 0x24, 0x93, 0xa5, 0xd0, 0x9d, 0x8b, 0xeb, 0x47, 0xcc, 0xde, 0x8f, 0xac, 0x18, 0x7b,
        0x86, 0xad, 0x00, 0x99, 0x10, 0xc9, 0x41, 0x22, 0xfd, 0x8d, 0xea, 0xe3, 0x98, 0x60, 0x4d,
        0x89, 0x7e, 0x59, 0x41, 0x83, 0x93, 0x88, 0x60, 0x6b, 0x22, 0x8f, 0x35, 0x7a, 0xd1, 0x7e,
        0xb6, 0x6c, 0x26, 0xac, 0x01, 0x77, 0x0b, 0x60, 0xe4, 0x86, 0x9b, 0x20, 0xc7, 0x99, 0xd3,
        0x76, 0x5b, 0x5f, 0xc1, 0x74
    };



    /*
    ** Private Key Processing
    */
    ESP_LOGI(TAG, "\n********************************************************"
                  "\n\n  Private Key Processing:  \n"
                  "\n********************************************************");
    rc = wc_ecc_init(&my_private_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_init for my_private_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_init for my_private_key. Error = %d", rc);
    }

    rc = wc_ecc_import_private_key(private_key, /* pointer to the buffer containing the raw private key */
                                    sizeof(private_key),
                                    public_key, /*  pointer to the buffer containing the ANSI x9.63 formatted ECC public key */
                                    sizeof(public_key),
                                    &my_ecc_private_key);
    if (rc == 0) {
        ESP_LOGI(TAG, "Successfully imported private key to my_ecc_private_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to import private key to my_ecc_private_key. Error = %d", rc);
    }

    rc = wc_ecc_check_key(&my_ecc_private_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_check_key for my_ecc_private_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_check_key for my_ecc_private_key. Error = %d", rc);
    }

    rc = wc_ecc_set_rng(&my_ecc_private_key, &rng);
    if (rc == 0) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_set_rng for my_ecc_private_key");
    }
    else {
        ESP_LOGE(TAG, "Failed wc_ecc_set_rng for my_ecc_private_key. Error = %d", rc);
    }

    rc = wc_ecc_check_key(&my_ecc_private_key);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_check_key for my_ecc_private_key");
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_check_key for my_ecc_private_key. Error = %d", rc);
    }

    /*
    ** Shared Secret Processing
    */


    ESP_LOGI(TAG, "\n********************************************************"
                  "\n\n  Shared Secret Processing:  \n"
                  "\n********************************************************");
    shared_key_len = sizeof(shared_key);
    /*
    ** see https://www.wolfssl.com/documentation/manuals/wolfssl/group__ECC.html#function-wc_ecc_shared_secret
    */
    rc = wc_ecc_shared_secret(&my_ecc_private_key, &my_ecc_public_key, shared_key, &shared_key_len);
    if (rc == MP_OKAY) {
        ESP_LOGI(TAG, "Successfully called wc_ecc_shared_secret for my_ecc_private_key and my_ecc_public_key");
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, shared_key, shared_key_len, ESP_LOG_INFO);
    }
    else {
        ESP_LOGE(TAG, "Failed to call wc_ecc_shared_secret for my_ecc_private_key and my_ecc_public_key. Error = %d", rc);
    }

    return rc;
}

void app_main(void)
{
    struct ProtobufCBinaryData myProtobufCBinaryData;

    /* ephemeral_key.data  b'04f3476fb37270eef09966fd17ca7967ede63a2bb3d23b4aee6e8b459482aebd7a939a95cbbbd01a1ec46b1976509e1cb82990d8eec34c98d14b69c5d8f7cee21c' */
    unsigned char ephemeral_key_data[65] = { 0x04, 0xf3, 0x47, 0x6f, 0xb3, 0x72, 0x70, 0xee,
                                             0xf0, 0x99, 0x66, 0xfd, 0x17, 0xca, 0x79, 0x67,
                                             0xed, 0xe6, 0x3a, 0x2b, 0xb3, 0xd2, 0x3b, 0x4a,
                                             0xee, 0x6e, 0x8b, 0x45, 0x94, 0x82, 0xae, 0xbd,
                                             0x7a, 0x93, 0x9a, 0x95, 0xcb, 0xbb, 0xd0, 0x1a,
                                             0x1e, 0xc4, 0x6b, 0x19, 0x76, 0x50, 0x9e, 0x1c,
                                             0xb8, 0x29, 0x90, 0xd8, 0xee, 0xc3, 0x4c, 0x98,
                                             0xd1, 0x4b, 0x69, 0xc5, 0xd8, 0xf7, 0xce, 0xe2,
                                             0x1c };

    myProtobufCBinaryData.data = (byte*)&ephemeral_key_data;
    myProtobufCBinaryData.len = 65;

    to_load_key(myProtobufCBinaryData);

    ESP_LOGI(TAG,
             "\n********************************************************"
             "\n\n  Done!  \n"
             "\n********************************************************");
    while(1) {
        /* loop */
    }
}
