/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/*
** This ESP32 project does not use the generated options.h
#include <wolfssl/options.h>
*/
#include <wolfssl/wolfcrypt/settings.h>

/* properly configured, user_settings.h should have been captured in above
** settings.h but is included here for reference: */
#include "user_settings.h"

#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/ed25519.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/asn.h>

#include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - i.e. #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define WOLFSSL_VERSION_PRINTF(...) ESP_LOGI(TAG, __VA_ARGS__)

#define DER_SIZE 4096
#define PEM_SIZE 4096
#include <wolfssl/ssl.h>

static const char *TAG = "key test";
char error_buffer[80];

int CertDemo() {
    byte der[DER_SIZE];
    byte pem[PEM_SIZE];
    esp_err_t ret; /* return codes */
    int derSz = 0; /* DER size found */

    Cert request;
    RsaKey genKey;
    RNG rng;
    XMEMSET(der, 0, DER_SIZE);
    XMEMSET(pem, 0, PEM_SIZE);

#if !defined(WOLFSSL_CERT_REQ) || !defined(WOLFSSL_CERT_GEN) || !defined(WOLFSSL_KEY_GEN)
    ESP_LOGI(TAG,
             "ERROR: Need to enable "
             "WOLFSSL_CERT_REQ, and/or "
             "WOLFSSL_CERT_GEN, and/or "
             "WOLFSSL_KEY_GEN");
#endif

    /* Initialize non-volatile storage */
    /*
    Used DATA_FLASH: 167KB out of 8192KB (2%) [+584]
    Used INSTR_FLASH: 750KB out of 3264KB (22%) [+12K]
    Used INSTR_RAM: 97KB out of 128KB (76%) [+2832]
    */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    /* Not only are certs needed for secure connections, but secure certs
    ** need random numbers; Espressif RNG uses WiFi for entropy.
    **
    ** see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/random.html
    **
    ** Otherwise consider using bootloader_random_enable().  (see bootloader_random.h)
    */
    // bootloader_random_enable();

     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
     ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_LOGW(TAG, "  Warning test");
    ESP_LOGE(TAG, "  Error test");
    ESP_LOGV(TAG, "  Verbose test");

    esp_log_level_set("*", ESP_LOG_VERBOSE);

    wc_InitRng(&rng);
    ESP_LOG_BUFFER_HEXDUMP(TAG, &rng, sizeof(rng), ESP_LOG_INFO);

    /*
    ** ensure Digital Signature (DS) is not active
    ** Exit the operation: Write 1 to DS_SET_FINISH_REG, then poll DS_QUERY_BUSY_REG until it reads 0
    **
    DPORT_REG_WRITE(DS_SET_FINISH_REG,1);
    while (DPORT_REG_READ(DS_QUERY_BUSY_REG) != 0)
    {
        ESP_LOGI(TAG, "Waiting for the DS_QUERY_BUSY_REG to complete.");
    }
    periph_module_disable(PERIPH_DS_MODULE);
    asm volatile("memw");
    */

    /*
    ***************************************************************************
    ** Create our RSA private key for the cert.
    ***************************************************************************
    */
    wc_InitRsaKey(&genKey, 0);
    if (WOLFSSL_RSA_KEY_SIZE <= RSA_MAX_SIZE) {
        ESP_LOGI(TAG, "Generating an RSA key %d bits long", WOLFSSL_RSA_KEY_SIZE);
    }
    else {
        ESP_LOGE(TAG, "Error: WOLFSSL_RSA_KEY_SIZE = %d > RSA_MAX_SIZE = %d",
                              WOLFSSL_RSA_KEY_SIZE,       RSA_MAX_SIZE);
    }
    ESP_LOGI(TAG, "Generating an RSA key %d bits long", WOLFSSL_RSA_KEY_SIZE);
    ESP_LOG_BUFFER_HEXDUMP(TAG, &rng, sizeof(rng), ESP_LOG_INFO);
    ret = wc_MakeRsaKey(&genKey, WOLFSSL_RSA_KEY_SIZE, 65537, &rng);
    if (ret != 0) {
        char * c;
        c = wolfSSL_ERR_error_string(ret, error_buffer);
        ESP_LOGE(TAG, "wc_MakeRsaKey error %d; message = %s", ret, error_buffer);
    }


    derSz = wc_RsaKeyToDer(&genKey, der, sizeof(der));
    if (derSz < 0) {
        ESP_LOGE(TAG, "wc_RsaKeyToDer error %d", ret);
    }
    else {
        ESP_LOGI(TAG, "wc_RsaKeyToDer derSz %d", derSz);
    }

    int  pemSz = wc_DerToPem(der, derSz,
                             pem, sizeof(pem),
                             PRIVATEKEY_TYPE);
    if (pemSz < 0) {
        ESP_LOGE(TAG, "wc_DerToPem error %d", ret);
    }
    else {
        ESP_LOGI(TAG, "wc_DerToPem pemSz %d", pemSz);
        if (pemSz < PEM_SIZE) {
            pem[pemSz] = 0x0; /* We'll print this, so add string terminator. */
        }
    }

#if (0)
    ESP_LOGI(TAG, "\n\n"
                   "Cert PEM:"
                   "\n\n"
                   "%s"
                   "\n\n"
                   , pem
            );
#endif

    /*
    ***************************************************************************
    ** Create Request
    ***************************************************************************
    */
    ESP_LOGI(TAG, "Begin  wc_InitCert");
    wc_InitCert(&request);
    ESP_LOGI(TAG, "End    wc_InitCert");

    strncpy(request.subject.country, "NZ\0", CTC_COUNTRY_SIZE + 1); /* +1 null terminated */
    strncpy(request.subject.state, "Waikato\0", CTC_NAME_SIZE);
    strncpy(request.subject.locality, "Hamilton\0", CTC_NAME_SIZE);
    strncpy(request.subject.org, "Blue Leaf Software, Ltd\0", CTC_NAME_SIZE);

#if defined(NO_ESP32WROOM32_CRYPT)
    strncpy(request.subject.commonName, "Lumos SW, SN = BLS-001\0", CTC_NAME_SIZE);
#else
    strncpy(request.subject.commonName, "Lumos HW, SN = BLS-002\0", CTC_NAME_SIZE);
#endif

    request.version = 0;
    ESP_LOGI(TAG, "Begin  wc_MakeCertReq_ex");
    ret = wc_MakeCertReq_ex(&request, der, sizeof(der), RSA_TYPE, &genKey);
    ESP_LOGI(TAG, "Begin  wc_MakeCertReq_ex");
    if (ret <= 0) {
        ESP_LOGE(TAG, "Make Cert Request failed: %d\n", ret);
    }
    derSz = ret;


    /*
    ***************************************************************************
    ** Sign our cert request with the key generated (see above)
    ***************************************************************************
    */
    request.sigType = CTC_SHA256wRSA;
    ESP_LOGI(TAG, "Begin  wc_SignCert_ex");
    ESP_LOG_BUFFER_HEXDUMP(TAG, &rng, sizeof(rng), ESP_LOG_INFO);
    ret = wc_SignCert_ex(request.bodySz, request.sigType,
                         der, sizeof(der),
                         RSA_TYPE,
                         &genKey, &rng
                        );
    ESP_LOGI(TAG, "End  wc_SignCert_ex");
    if (ret <= 0) {
        ESP_LOGE(TAG, "Sign Cert failed: %d\n", ret);
    }
    derSz = ret;

    XMEMSET(pem, 0, PEM_SIZE);

    /* convert the DER to PEM */
    pemSz = wc_DerToPem(der, derSz,
                        pem, sizeof(pem),
                        CERTREQ_TYPE
                       );
    if (pemSz < 0) {
        ESP_LOGE(TAG, "wc_DerToPem error %d", ret);
    }
    else {
        if (pemSz < PEM_SIZE) {
            pem[pemSz] = 0x0;
        }
    }

    /*
    ***************************************************************************
    ** Show Certificate Request
    **
    ** Paste the following "Request PEM:" into an output file.
    **
    ** To validate:
    **
    ** openssl req -in "./output/test_request.crt" -noout -text
    **
    ***************************************************************************
    */
    ESP_LOGI(TAG, "\n\n"
                   "Request pem: (paste into file for CA sign)"
                   "\n\n"
                   "%s"
                   "\n\n"
                   , pem
            );

    /*
    ***************************************************************************
    ** Sign the Certificate Request with OpenSSL
    ***************************************************************************
    */
#define OS "openssl x509 -req -in ./output/test_request.crt -days 10    \\ \n \
    -CA private/CertificateAuthority-4096-PublicKey.crt                 \\ \n \
    -CAkey private/CertificateAuthority-4096-PrivateKey.pem             \\ \n \
    -passin pass:secret -CAcreateserial                                 \\ \n \
    -extfile private/CertificateExtensions.txt -extensions LumosDevice  \\ \n \
    -out ./output/test-Device-Certificate_hw.crt -text                     \n \
        "

    ESP_LOGI(TAG, OS);

         /*
openssl x509 -req -in "./output/test_request.crt" -days 10     \
    -CA "private/CertificateAuthority-4096-PublicKey.crt"                 \
    -CAkey "private/CertificateAuthority-4096-PrivateKey.pem"             \
    -passin pass:secret -CAcreateserial                                   \
    -extfile "private/CertificateExtensions.txt" -extensions LumosDevice  \
    -out ./output/test-Device-Certificate_hw.crt -text

    */

    /* check to see if we are using hardware encryption */
#if defined(NO_ESP32WROOM32_CRYPT)
    WOLFSSL_VERSION_PRINTF("NO_ESP32WROOM32_CRYPT defined! "
                           "HW acceleration DISABLED.");
#else
    /* first show what platform hardware acceleration is enabled
    ** (some new platforms may not be supported yet) */
    #if defined(CONFIG_IDF_TARGET_ESP32)
        WOLFSSL_VERSION_PRINTF("ESP32WROOM32_CRYPT is enabled for ESP32.");
    #elif defined(CONFIG_IDF_TARGET_ESP32S2)
        WOLFSSL_VERSION_PRINTF("ESP32WROOM32_CRYPT is enabled for ESP32-S2.");
    #elif defined(CONFIG_IDF_TARGET_ESP32S3)
        WOLFSSL_VERSION_PRINTF("ESP32WROOM32_CRYPT is enabled for ESP32-S3.");
    #else
        #error "ESP32WROOM32_CRYPT not yet supported on this IDF TARGET"
    #endif

    /* Even though enabled, some specifics may be disabled */
    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH)
        WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH is defined!"
                               "(disabled HW SHA).");
    #endif

    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_AES)
        WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32WROOM32_CRYPT_AES is defined!"
                               "(disabled HW AES).");
    #endif

    #if defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI)
        WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI defined!"
                               "(disabled HW RSA)");
    #endif
#endif

    return 0;
}

void app_main(void)
{
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    esp_ShowExtendedSystemInfo();

    CertDemo();

    WOLFSSL_VERSION_PRINTF("Stack HWM: %d",
                           uxTaskGetStackHighWaterMark(NULL));
    ESP_LOGI(TAG, "Done!");
    while (1) {
        /* do something other than nothing to help next program/debug session*/
#ifndef SINGLE_THREADED
        vTaskDelay(1000);
#endif
    }

}