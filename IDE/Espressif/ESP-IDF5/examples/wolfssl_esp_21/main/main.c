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

// #define HAS_MY_PRIVATE_CONFIG
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
ecc_key my_private_key; // ECC_SECP256R1

/* 042493a5d09d8beb47ccde8fac187b86ad009910c94122fd8deae398604d897e5941839388606b228f357ad17eb66c26ac01770b60e4869b20c799d3 765b5fc174 */
byte my_public_key[65] = {0x04, 0x24, 0x93, 0xa5, 0xd0, 0x9d, 0x8b, 0xeb,
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
struct ecc_key other_pub_key;

/* see
** https://protobuf-c.github.io/protobuf-c/structProtobufCBinaryData.html
*/
struct ProtobufCBinaryData
{
    size_t 	len;
    byte* data;
};


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

int to_load_key(struct ProtobufCBinaryData ephemeral_key)
{
    int ret = 0;
    byte shared_key = 0;
    word32 shared_key_len = 0;

    wc_ecc_init(&other_pub_key);
    ret = wc_ecc_import_x963((byte *)ephemeral_key.data, (word32)ephemeral_key.len, &other_pub_key);
    ESP_LOGI(TAG, "wc_ecc_import_x963 result = %d", ret);

    //int curve_idx = wc_ecc_get_curve_idx(ECC_SECP256R1);
    //other_pub_key= wc_ecc_new_point(); // ecc_point *other_pub_key;
    //ret = wc_ecc_import_point_der(ephemeral_key.data, ephemeral_key.len, curve_idx, other_pub_key);

    /*
    ** see
    ** https://github.com/wolfSSL/wolfssl/blob/6bed0c57579c8cbb68a85b01ca67e0e4ef4228ea/wolfssl/wolfcrypt/ecc.h#L602
    **
    ** int wc_ecc_shared_secret_ex(ecc_key* private_key, ecc_point* point,
    **                             byte* out, word32 *outlen);
    */
    ret = wc_ecc_shared_secret(&my_private_key, &other_pub_key, (byte *)&shared_key, (word32 *)&shared_key_len);
    if (ret != 0) {
        ESP_LOGI(TAG, "========= wc_ecc_shared_secret error %d", ret);
        wc_ecc_free(&other_pub_key);
        return -4; // Error computing shared key
    }
    return 0;
}

void app_main(void)
{
    esp_err_t ret; /* return codes */
    RNG rng;

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



}
