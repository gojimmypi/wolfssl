/* ESP HTTP Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// #include "esp_idf_version.h"

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h" /* example connect */
#include "protocol_examples_utils.h" /* example uri encode */
#include "esp_tls.h"

#if defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE) && defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE)
    #error "Both mbedTLS and wolfSSL Certificate Bundles Enabled. Pick one."
#elif CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
    /* "Certificate Bundles" are specific to mbedTLS and not part of any RFC */
    #include "esp_crt_bundle.h"
#elif defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE) && CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
    #include <wolfssl/wolfcrypt/settings.h>
    #include <wolfssl/wolfcrypt/port/Espressif/esp-sdk-lib.h>
    #include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
    #include <wolfssl/wolfcrypt/port/Espressif/esp_crt_bundle.h>
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

/* Espressif */
#include <esp_log.h>

/* wolfSSL */
/* Always include wolfcrypt/settings.h before any other wolfSSL file.    */
/* Reminder: settings.h pulls in user_settings.h; don't include it here. */
#ifdef WOLFSSL_USER_SETTINGS
    #include <wolfssl/wolfcrypt/settings.h>
    #ifndef WOLFSSL_ESPIDF
        #warning "Problem with wolfSSL user_settings."
        #warning "Check components/wolfssl/include"
    #endif
    #include <wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h>
#else
    /* Define WOLFSSL_USER_SETTINGS project wide for settings.h to include   */
    /* wolfSSL user settings in ./components/wolfssl/include/user_settings.h */
    #error "Missing WOLFSSL_USER_SETTINGS in CMakeLists or Makefile:\
    CFLAGS +=-DWOLFSSL_USER_SETTINGS"
#endif
#if defined(DEBUG_WOLFSSL) || defined(WOLFSSL_DEBUG_CERT_BUNDLE)
    #include <esp_task_wdt.h>
#endif

#include "esp_http_client.h"

/* set to 0 for one test,
** set to 1 for continuous test loop */
#undef WOLFSSL_TEST_LOOP
#define WOLFSSL_TEST_LOOP 1
#define MAX_LOOPS 1
#ifndef  WOLFSSL_TEST_LOOP
    #define WOLFSSL_TEST_LOOP 0
#endif

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";

#if defined(WOLFSSL_TEST_LOOP) && WOLFSSL_TEST_LOOP
    SemaphoreHandle_t task_semaphore;
#endif

/* The original example uses 8K WORDS (32K bytes) wolfSSL uses only 9K bytes */
#define EXAMPLE_TASK_STACK_SIZE_BYTES 10240 + 2048
#define EXAMPLE_TASK_STACK_SIZE_WORDS (EXAMPLE_TASK_STACK_SIZE_BYTES / 4)
/* Root cert for howsmyssl.com, taken from howsmyssl_com_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const char howsmyssl_com_root_cert_pem_start[] asm("_binary_howsmyssl_com_root_cert_pem_start");
extern const char howsmyssl_com_root_cert_pem_end[]   asm("_binary_howsmyssl_com_root_cert_pem_end");

extern const char postman_root_cert_pem_start[] asm("_binary_postman_root_cert_pem_start");
extern const char postman_root_cert_pem_end[]   asm("_binary_postman_root_cert_pem_end");
static unsigned int stack_start = 0;
static unsigned int stack_current = 0;

static esp_err_t ret = ESP_OK;
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // Clean the buffer in case of a new request
            if (output_len == 0 && evt->user_data) {
                // we are just starting to copy the output data into the use
                memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
            }
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                int copy_len = 0;
                if (evt->user_data) {
                    // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                    copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                    if (copy_len) {
                        memcpy(evt->user_data + output_len, evt->data, copy_len);
                    }
                } else {
                    int content_len = esp_http_client_get_content_length(evt->client);
                    if (output_buffer == NULL) {
                        // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                        // ESP_LOGI(TAG, "event handler calloc");
                        output_buffer = (char *) calloc(content_len + 1, sizeof(char));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    copy_len = MIN(evt->data_len, (content_len - output_len));
                    if (copy_len) {
                        memcpy(output_buffer + output_len, evt->data, copy_len);
                    }
                }
                output_len += copy_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
#if CONFIG_EXAMPLE_ENABLE_RESPONSE_BUFFER_DUMP
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
#endif
                // ESP_LOGI(TAG, "event handler free 1");
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int esp_tls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &esp_tls_err, NULL);
            if (err != 0) {
                /* ret = err; disconnected; this is not a hard error */
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                #ifdef CONFIG_ESP_TLS_USING_MBEDTLS
                {
                    ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", esp_tls_err);
                }
                #endif
            }
            if (output_buffer != NULL) {
                ESP_LOGI(TAG, "event handler free 2");
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
            esp_http_client_set_header(evt->client, "From", "user@example.com");
            esp_http_client_set_header(evt->client, "Accept", "text/html");
            esp_http_client_set_redirection(evt->client);
            break;
#if defined(ESP_IDF_VERSION) && (ESP_IDF_VERSION == ESP_IDF_VERSION_VAL(6, 0, 0))
        case HTTP_EVENT_ON_HEADERS_COMPLETE:
            ESP_LOGW(TAG, "New HTTP_EVENT_REDIRECT");
            break;
        case HTTP_EVENT_ON_STATUS_CODE:
            ESP_LOGW(TAG, "New HTTP_EVENT_ON_STATUS_CODE");
            break;
#endif
    }
    return ESP_OK;
}

static void http_rest_with_url(void)
{
    // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be specified either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        .host = CONFIG_EXAMPLE_HTTP_ENDPOINT,
        .path = "/get",
        .query = "esp",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
        .disable_auto_redirect = true,
    };
    ESP_LOGI(TAG, "HTTP request with url =>");
    //esp_task_wdt_reset();
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOG_BUFFER_HEX(TAG, local_response_buffer, strlen(local_response_buffer));

    // POST
    const char *post_data = "{\"field1\":\"value1\"}";
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/post");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    //PUT
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/put");
    esp_http_client_set_method(client, HTTP_METHOD_PUT);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }

    //PATCH
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/patch");
    esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    esp_http_client_set_post_field(client, NULL, 0);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PATCH Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    }

    //DELETE
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/delete");
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
    }

    //HEAD
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get");
    esp_http_client_set_method(client, HTTP_METHOD_HEAD);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP HEAD Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

static void http_rest_with_hostname_path(void)
{
    esp_http_client_config_t config = {
        .host = CONFIG_EXAMPLE_HTTP_ENDPOINT,
        .path = "/get",
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP request with hostname and path =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    // POST
    const char *post_data = "field1=value1&field2=value2";
    esp_http_client_set_url(client, "/post");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    //PUT
    esp_http_client_set_url(client, "/put");
    esp_http_client_set_method(client, HTTP_METHOD_PUT);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }

    //PATCH
    esp_http_client_set_url(client, "/patch");
    esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    esp_http_client_set_post_field(client, NULL, 0);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP PATCH Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    }

    //DELETE
    esp_http_client_set_url(client, "/delete");
    esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
    }

    //HEAD
    esp_http_client_set_url(client, "/get");
    esp_http_client_set_method(client, HTTP_METHOD_HEAD);
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP HEAD Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
static void http_auth_basic(void)
{
    /**
     * Note: `max_authorization_retries` in esp_http_client_config_t
     * can be used to configure number of retry attempts to be performed
     * in case unauthorized status code is received.
     *
     * To disable authorization retries, set max_authorization_retries to -1.
     */
    esp_http_client_config_t config = {
        .url = "http://user:passwd@"CONFIG_EXAMPLE_HTTP_ENDPOINT"/basic-auth/user/passwd",
        .event_handler = _http_event_handler,
        .auth_type = HTTP_AUTH_TYPE_BASIC,
        .max_authorization_retries = -1,
    };
    ESP_LOGI(TAG, "HTTP Basic Auth request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Basic Auth Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_auth_basic_redirect(void)
{
    esp_http_client_config_t config = {
        .url = "http://user:passwd@"CONFIG_EXAMPLE_HTTP_ENDPOINT"/basic-auth/user/passwd",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP Basic Auth redirect request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Basic Auth redirect Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
#endif

#if CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
static void http_auth_digest_md5(void)
{
    esp_http_client_config_t config = {
        .url = "http://user:passwd@"CONFIG_EXAMPLE_HTTP_ENDPOINT"/digest-auth/auth/user/passwd/MD5/never",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP MD5 Digest Auth request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP MD5 Digest Auth Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error performing http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_auth_digest_sha256(void)
{
    esp_http_client_config_t config = {
        .url = "http://user:passwd@"CONFIG_EXAMPLE_HTTP_ENDPOINT"/digest-auth/auth/user/passwd/SHA-256/never",
        .event_handler = _http_event_handler,
        .buffer_size_tx = 1024, // Increase buffer size as header size will increase as it contains SHA-256.
    };
    ESP_LOGI(TAG, "HTTP SHA256 Digest Auth request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP SHA256 Digest Auth Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error performing http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
#endif

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE || CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
static void https_with_url(void)
{
    esp_http_client_config_t config = {
        .url = "https://www.howsmyssl.com",
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    ESP_LOGI(TAG, "HTTPS request with url =>");
    //esp_task_wdt_reset();
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}
#endif /* CONFIG_[PROVIDER]_CERTIFICATE_BUNDLE */

static void https_with_hostname_path(void)
{
    esp_http_client_config_t config = {
        .host = "www.howsmyssl.com",
        .path = "/",
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .event_handler = _http_event_handler,
        .cert_pem = howsmyssl_com_root_cert_pem_start,
    };
    ESP_LOGI(TAG, "HTTPS request with hostname and path =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_encoded_query(void)
{
    esp_http_client_config_t config = {
        .host = CONFIG_EXAMPLE_HTTP_ENDPOINT,
        .path = "/get",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP GET request with encoded query =>");

    static const char query_val[] = "ABC xyz!012@#%&";
    char query_val_enc[64] = {0};

    uint32_t enc_len = example_uri_encode(query_val_enc, query_val, strlen(query_val));
    if (enc_len > 0) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, query_val_enc, enc_len, ESP_LOG_DEBUG);
        config.query = query_val_enc;
    }

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOGW(TAG, "No cleanout in upstream example here?");
    esp_http_client_cleanup(client);
}

static void http_relative_redirect(void)
{
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/relative-redirect/3",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP Relative path redirect request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Relative path redirect Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_absolute_redirect(void)
{
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/absolute-redirect/3",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP Absolute path redirect request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Absolute path redirect Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_absolute_redirect_manual(void)
{
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/absolute-redirect/3",
        .event_handler = _http_event_handler,
        .disable_auto_redirect = true,
    };
    ESP_LOGI(TAG, "HTTP Absolute path redirect (manual) request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Absolute path redirect (manual) Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_redirect_to_https(void)
{
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/redirect-to?url=https://www.howsmyssl.com",
        .event_handler = _http_event_handler,
        .cert_pem = howsmyssl_com_root_cert_pem_start,
    };
    ESP_LOGI(TAG, "HTTP redirect to HTTPS request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP redirect to HTTPS Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}


static void http_download_chunk(void)
{
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/stream-bytes/8912",
        .event_handler = _http_event_handler,
    };
    ESP_LOGI(TAG, "HTTP chunk encoding request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP chunk encoding Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void http_perform_as_stream_reader(void)
{
    char *buffer = malloc(MAX_HTTP_RECV_BUFFER + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Cannot malloc http receive buffer");
        return;
    }
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get",
    };
    ESP_LOGI(TAG, "HTTP Stream reader request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ret = err;
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        free(buffer);
        return;
    }
    int content_length =  esp_http_client_fetch_headers(client);
    int total_read_len = 0, read_len;
    if (total_read_len < content_length && content_length <= MAX_HTTP_RECV_BUFFER) {
        read_len = esp_http_client_read(client, buffer, content_length);
        if (read_len <= 0) {
            ESP_LOGE(TAG, "Error read data");
        }
        buffer[read_len] = 0;
        ESP_LOGD(TAG, "read_len = %d", read_len);
    }
    ESP_LOGI(TAG, "HTTP Stream reader Status = %d, content_length = %"PRId64,
                    esp_http_client_get_status_code(client),
                    esp_http_client_get_content_length(client));
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(buffer);
}

static void https_async(void)
{
#if defined(CONFIG_ESP_TLS_USING_WOLFSSL) && !defined(WOLFSSL_ALT_CERT_CHAINS)
    #warning "The strict wolfSSL default certificate handling may cause failure without WOLFSSL_ALT_CERT_CHAINS"
    ESP_LOGW(TAG, "The strict wolfSSL default certificate handling may cause failure without WOLFSSL_ALT_CERT_CHAINS");
#endif
    esp_http_client_config_t config = {
        .url = "https://postman-echo.com/post",
        .event_handler = _http_event_handler,
        .cert_pem = postman_root_cert_pem_start,
        .is_async = true,
        .timeout_ms = 5000,
    };
    ESP_LOGI(TAG, "\n\n\nHTTPS async requests =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    const char *post_data = "Using a PalantÃ­r requires a person with great strength of will and wisdom. The PalantÃ­ri were meant to "
                            "be used by the DÃºnedain to communicate throughout the Realms in Exile. During the War of the Ring, "
                            "the PalantÃ­ri were used by many individuals. Sauron used the Ithil-stone to take advantage of the users "
                            "of the other two stones, the Orthanc-stone and Anor-stone, but was also susceptible to deception himself.";
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    while (1) {
        err = esp_http_client_perform(client);
        if (err != ESP_ERR_HTTP_EAGAIN) {
            break;
        }
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);

    // Test HTTP_METHOD_HEAD with is_async enabled
    config.url = "https://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get";
    ESP_LOGI(TAG, "config.url = %s", config.url);
    config.event_handler = _http_event_handler;
#if defined(CONFIG_MBEDTLS_CERTIFICATE_BUNDLE) || defined(CONFIG_WOLFSSL_CERTIFICATE_BUNDLE)
    config.crt_bundle_attach = esp_crt_bundle_attach;
#else
    ESP_LOGW(TAG, "Warning: setting crt_bundle_attach to NULL");
    config.crt_bundle_attach = NULL;
#endif
    config.is_async = true;
    config.timeout_ms = 5000;

    client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_HEAD);

    while (1) {
        err = esp_http_client_perform(client);
        if (err != ESP_ERR_HTTP_EAGAIN) {
            break;
        }
    }
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

static void https_with_invalid_url(void)
{
    esp_http_client_config_t config = {
            .url = "https://not.existent.url",
            .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        //ret = ESP_FAIL; /* an invalid URL should never be successful! */
        ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %"PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        /* ret = err; we expect an error here, as it is an invalid URL! */
        ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

/*
 *  http_native_request() demonstrates use of low level APIs to connect to a server,
 *  make a http request and read response. Event handler is not used in this case.
 *  Note: This approach should only be used in case use of low level APIs is required.
 *  The easiest way is to use esp_http_perform()
 */
static void http_native_request(void)
{
    // Declare local_response_buffer with size (MAX_HTTP_OUTPUT_BUFFER + 1) to prevent out of bound access when
    // it is used by functions like strlen(). The buffer should only be used upto size MAX_HTTP_OUTPUT_BUFFER
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};   // Buffer to store response of http request
    int content_length = 0;
    esp_http_client_config_t config = {
        .url = "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/get",
    };
    ESP_LOGI(TAG, "HTTP native request =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET Request
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ret = err;
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, data_read);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }
    esp_http_client_close(client);

    // POST Request
    const char *post_data = "{\"field1\":\"value1\"}";
    esp_http_client_set_url(client, "http://"CONFIG_EXAMPLE_HTTP_ENDPOINT"/post");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    err = esp_http_client_open(client, strlen(post_data));
    if (err != ESP_OK) {
        ret = err;
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        int wlen = esp_http_client_write(client, post_data, strlen(post_data));
        if (wlen < 0) {
            ESP_LOGE(TAG, "Write failed");
        }
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, strlen(output_buffer));
            } else {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }
    esp_http_client_cleanup(client);
}

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE || CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
static void http_partial_download(void)
{
    esp_http_client_config_t config = {
        .url = "https://dl.espressif.com/dl/esp-idf/ci/esp_http_client_demo.txt",
        .event_handler = _http_event_handler,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    ESP_LOGI(TAG, "HTTP partial download =>");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Download a file excluding first 10 bytes
    esp_http_client_set_header(client, "Range", "bytes=10-");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }

    // Download last 10 bytes of a file
    esp_http_client_set_header(client, "Range", "bytes=-10");
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }

    // Download 10 bytes from 11 to 20
    esp_http_client_set_header(client, "Range", "bytes=11-20");
    err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP Status = %d, content_length = %"PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ret = err;
        ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE

#pragma region MyRegion
/* new demo */
// esp_http_client stuff
esp_http_client_handle_t hclient; /* hcloud->hclient */
esp_http_client_method_t httpmethod; /*  hcloud->httpmethod */
esp_http_client_method_t http_method;

static esp_err_t heap_peek(int i) {
    size_t free_heap_size;
    size_t min_free_heap_size;
    size_t free_internal_heap_size;
    static size_t last_min_free_heap_size;

    ESP_LOGI(TAG, "Heap Peek #%d", i);
    free_heap_size = esp_get_free_heap_size();
    ESP_LOGI(TAG, "Free heap size: %u bytes", free_heap_size);

    // Get minimum ever free heap size (since boot)
    min_free_heap_size = esp_get_minimum_free_heap_size();
    if (min_free_heap_size != last_min_free_heap_size) {
        ESP_LOGI(TAG, "Minimum ever free heap size: %u bytes; last: %u; d: %d",
                       min_free_heap_size, last_min_free_heap_size,
                       min_free_heap_size - last_min_free_heap_size);

    }
    else {
        ESP_LOGW(TAG, "Minimum ever free heap size: %u bytes", min_free_heap_size);
    }

    // Get the amount of free memory in internal RAM
    free_internal_heap_size = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGI(TAG, "Free internal heap size: %u bytes", free_internal_heap_size);

    last_min_free_heap_size = min_free_heap_size;
    return ESP_OK;
}

static void http_test_task(void *pvParameters)
{
#if defined(DEBUG_WOLFSSL) || defined(WOLFSSL_DEBUG_CERT_BUNDLE)
   // ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
#endif

loop:
    ret = ESP_OK;
    ESP_LOGI(TAG, "\n\n\nBegin http_test_task\n\n");
#ifdef CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY
    ESP_LOGW(TAG, "Warning: CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY set");
#endif
    heap_peek(1);
    http_rest_with_url();

/* #define SINGLE_TEST */
#ifdef SINGLE_TEST
#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE || CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
    /* bundle tests */
    https_async();
#endif
    https_async();

#else
    heap_peek(2);
    http_rest_with_url();
    heap_peek(3);
    http_rest_with_hostname_path();
#if CONFIG_ESP_HTTP_CLIENT_ENABLE_BASIC_AUTH
    http_auth_basic();
    http_auth_basic_redirect();
#endif
#if CONFIG_ESP_HTTP_CLIENT_ENABLE_DIGEST_AUTH
    http_auth_digest_md5();
    http_auth_digest_sha256();
#endif
    heap_peek(4);
    http_encoded_query();
    heap_peek(5);
    http_relative_redirect();
    heap_peek(6);
    http_absolute_redirect();
    heap_peek(7);
    http_absolute_redirect_manual();
    heap_peek(8);

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE || CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
    https_with_url();
#endif
    heap_peek(9);
    https_with_hostname_path();
    heap_peek(10);
    http_redirect_to_https();
    heap_peek(11);
    http_download_chunk();
    heap_peek(12);
    http_perform_as_stream_reader();
    heap_peek(13);
    https_async();
    heap_peek(14);
    https_with_invalid_url();
    heap_peek(15);
    http_native_request();
    goto loop;

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE || CONFIG_WOLFSSL_CERTIFICATE_BUNDLE
    heap_peek(16);
    http_partial_download();
#endif

#endif /* SINGLE_TEST */

#if defined(DEBUG_WOLFSSL) && defined(WOLFSSL_ESP32_CRYPT_RSA_PRI)
    esp_hw_show_mp_metrics();
#endif

#if WOLFSSL_TEST_LOOP
    #ifdef WOLFSSL_ESPIDF_VERBOSE_EXIT_MESSAGE
        if (ret != 0) {
            ESP_LOGE(TAG, WOLFSSL_ESPIDF_VERBOSE_EXIT_MESSAGE("Failed!", ret));
        }
    #endif
    xSemaphoreGive(task_semaphore);
#else
    #ifdef WOLFSSL_ESPIDF_VERBOSE_EXIT_MESSAGE
        if (ret == 0) {
            ESP_LOGI(TAG, WOLFSSL_ESPIDF_VERBOSE_EXIT_MESSAGE("Success!", ret));
        }
        else {
            ESP_LOGE(TAG, WOLFSSL_ESPIDF_VERBOSE_EXIT_MESSAGE("Failed!", ret));
        }
    #elif defined(WOLFSSL_ESPIDF_EXIT_MESSAGE)
        ESP_LOGI(TAG, WOLFSSL_ESPIDF_EXIT_MESSAGE);
    #else
        ESP_LOGI(TAG, "\n\nDone!\n\n"
                      "If running from idf.py monitor, press twice: Ctrl+]");
    #endif
#endif
#if defined(WOLFSSL_HW_METRICS)
    esp_hw_show_metrics();
#endif
    vTaskDelay(pdMS_TO_TICKS(1000));
#if !CONFIG_IDF_TARGET_LINUX
    vTaskDelete(NULL);
#endif

} /* http_test_task */

void app_main(void)
{
    int loops = 0;
#if WOLFSSL_TEST_LOOP
    task_semaphore =  xSemaphoreCreateBinary();
#endif
#if defined(ESP_IDF_VERSION) && (ESP_IDF_VERSION == ESP_IDF_VERSION_VAL(5, 2, 2))
    ESP_LOGI(TAG, "Found esp-IDF v5.2.2");
#else
    ESP_LOGW(TAG, "This example is intended for ESP-IDF version 5.2.2");
#endif
#ifdef CONFIG_ESP_TLS_USING_WOLFSSL
    stack_start = esp_sdk_stack_pointer();
    stack_current = esp_sdk_stack_pointer();
#endif
    ESP_LOGI(TAG, "------------------ wolfSSL Test Example ----------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "Stack Start: 0x%x", stack_start);
#ifdef WOLFSSL_ESP_NO_WATCHDOG
    ESP_LOGW(TAG, "Found WOLFSSL_ESP_NO_WATCHDOG, disabling...");
    esp_DisableWatchdog();
#endif

#if defined(HAVE_VERSION_EXTENDED_INFO)
    esp_ShowExtendedSystemInfo();
#else
    ESP_LOGW(TAG, "HAVE_VERSION_EXTENDED_INFO not defined");
#endif

#ifdef ESP_SDK_MEM_LIB_VERSION
    /* Set time for cert validation.
     * Some lwIP APIs, including SNTP functions, are not thread safe. */
    // ret = set_time(); /* need to setup NTP before WiFi */
    set_time_from_string("Tue Aug 25 3:41:45 2025 -0700");
#endif

    ESP_LOGI(TAG, "nvs flash init..");
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "nvs flash erase..");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_LOGI(TAG, "nvs flash erase..");
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "esp netif init...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "esp event loop create default...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
#if defined(CONFIG_IDF_TARGET_ESP32H2)
    ESP_LOGI(TAG, "There's no WiFi on the ESP32-H2");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#else
    ESP_LOGI(TAG, "example connect...");
    ESP_ERROR_CHECK(example_connect());
#endif

#ifdef USE_WOLFSSL_ESP_SDK_TIME
    /* Once we are connected to the network, start & wait for NTP time */
    ret = set_time_wait_for_ntp();
#endif
    #ifdef DEBUG_WOLFSSL
        wolfSSL_Debugging_OFF();
    #endif

    esp_log_level_set("esp-tls",         ESP_LOG_DEBUG);
    esp_log_level_set("esp-tls-wolfssl", ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Connected to AP, begin http example");

#if CONFIG_IDF_TARGET_LINUX
    http_test_task(NULL);
#else


#if !defined(CONFIG_ESP_TASK_WDT_INIT) && \
    defined(DEBUG_WOLFSSL) || defined(WOLFSSL_DEBUG_CERT_BUNDLE)

//    esp_task_wdt_config_t cfg = {
//        .timeout_ms     = 5000,
//        .idle_core_mask = 0,       // or (1<<0) | (1<<1) to watch idle tasks
//        .trigger_panic  = true,
//    };
//    ESP_ERROR_CHECK(esp_task_wdt_init(&cfg));
#endif
    /* Assume success in the test task unless proven otherwise. */
    ret = ESP_OK;
    do {
        ESP_LOGI(TAG, "Main Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));

        xTaskCreate(&http_test_task, "http_test_task",
                    EXAMPLE_TASK_STACK_SIZE_WORDS, NULL, 5, NULL);
        ESP_LOGI(TAG, "Task created!");
    #if WOLFSSL_TEST_LOOP
        if (xSemaphoreTake(task_semaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Task completed!");
        }
    #endif
        loops++; /* count of the number of tests run before fail. */
        ESP_LOGI(TAG, "Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));
        ESP_LOGI(TAG, "loops = %d; ret = %d", loops, ret);
        vTaskDelay(pdMS_TO_TICKS(1000));
    } while (WOLFSSL_TEST_LOOP && (ret == 0) && loops < MAX_LOOPS);
#endif
#ifdef CONFIG_ESP_TLS_USING_WOLFSSL
    stack_current = esp_sdk_stack_pointer();
    ESP_LOGI(TAG, "Stack current (end main): 0x%x", stack_current);
#endif
    /* Done in main task, cleanp and free memory. */
    vTaskDelete(NULL);
}
