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

/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

#define HAS_MY_PRIVATE_CONFIG
#ifdef HAS_MY_PRIVATE_CONFIG
    #include "/workspace/my_private_config.h"
#endif

#ifndef EXAMPLE_ESP_WIFI_SSID
    #define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#endif

#ifndef EXAMPLE_ESP_WIFI_PASS
    #define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#endif

#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY


#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "wifi station";
#define WOLFSSL_VERSION_PRINTF(...) ESP_LOGI(TAG, __VA_ARGS__)

static int s_retry_num = 0;

/* #include "server-tls.h" */

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Authmode threshold resets to WPA2 as default if password matches WPA2 standards (pasword len => 8).
             * If you want to connect the device to deprecated WEP/WPA networks, Please set the threshold value
             * to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set the password with length and format matching to
	     * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
             */
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
//        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
//                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void app_main(void)
{
    #define DER_SIZE 4096
    #define PEM_SIZE 4096

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
    // wifi_init_sta();
    // bootloader_random_enable();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_LOGW(TAG, "Warning test");
    ESP_LOGE(TAG, "Error test");
    ESP_LOGV(TAG, "Verbose test");

    esp_log_level_set("*", ESP_LOG_VERBOSE);

    wc_InitRng(&rng);

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

    ESP_LOGI(TAG, "Generating an RSA key %d bits long", WOLFSSL_RSA_KEY_SIZE);
    ret = wc_MakeRsaKey(&genKey, WOLFSSL_RSA_KEY_SIZE, 65537, &rng);
    if (ret != 0) {
        ESP_LOGI(TAG, "wc_MakeRsaKey error %d", ret);
    }


    derSz = wc_RsaKeyToDer(&genKey, der, sizeof(der));
    if (derSz < 0) {
        ESP_LOGE(TAG, "wc_RsaKeyToDer error %d", ret);
    }

    int  pemSz = wc_DerToPem(der, derSz,
                             pem, sizeof(pem),
                             PRIVATEKEY_TYPE);
    if (pemSz < 0) {
        ESP_LOGE(TAG, "wc_DerToPem error %d", ret);
    }
    else {
        if (pemSz < PEM_SIZE) {
            pem[pemSz] = 0x0; /* We'll print this, so add string terminator. */
        }
    }

    ESP_LOGI(TAG, "\n\n"
                   "Cert pem:"
                   "\n\n"
                   "%s"
                   "\n\n"
                   , pem
            );


    /*
    ***************************************************************************
    ** Create Request
    ***************************************************************************
    */
    wc_InitCert(&request);

    strncpy(request.subject.country, "NZ", CTC_COUNTRY_SIZE + 1); /* +1 null terminated */
    strncpy(request.subject.state, "Waikato", CTC_NAME_SIZE);
    strncpy(request.subject.locality, "Hamilton", CTC_NAME_SIZE);
    strncpy(request.subject.org, "Blue Leaf Software, Ltd", CTC_NAME_SIZE);

#if defined(NO_ESP32WROOM32_CRYPT)
    strncpy(request.subject.commonName, "Lumos SW, serialNumber = BLS-001", CTC_NAME_SIZE);
#else
    strncpy(request.subject.commonName, "Lumos HW, serialNumber = BLS-002", CTC_NAME_SIZE);
#endif

    request.version = 0;
    ret = wc_MakeCertReq_ex(&request, der, sizeof(der), RSA_TYPE, &genKey);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Make Cert Req failed: %d\n", ret);
    }
    derSz = ret;


    /*
    ***************************************************************************
    ** Sign our cert request with the key generated (see above)
    ***************************************************************************
    */
    request.sigType = CTC_SHA256wRSA;
    ret = wc_SignCert_ex(request.bodySz, request.sigType,
                         der, sizeof(der),
                         RSA_TYPE,
                         &genKey, &rng
                        );
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
    ** Paste the following "Request pem:" into an output file.
    **
    ** To validate:
    **
    ** openssl req -in "./output/gojimmypi-ESP32_request.crt" -noout -text
    **
    ***************************************************************************
    */
    ESP_LOGI(TAG, "\n\n");
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

openssl x509 -req -in "./output/gojimmypi-ESP32_request.crt" -days 10     \
    -CA "private/CertificateAuthority-4096-PublicKey.crt"                 \
    -CAkey "private/CertificateAuthority-4096-PrivateKey.pem"             \
    -passin pass:secret -CAcreateserial                                   \
    -extfile "private/CertificateExtensions.txt" -extensions LumosDevice  \
    -out ./output/gojimmypi-Device-Certificate_hw.crt -text

    */
    ESP_LOGI(TAG, "Done!");

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

}
