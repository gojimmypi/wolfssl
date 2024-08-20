/* esp_crt_bundle.c
 *
 * Copyright (C) 2006-2024 wolfSSL Inc.
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* wolfSSL */
/* Always include wolfcrypt/settings.h before any other wolfSSL file.    */
/* Reminder: settings.h pulls in user_settings.h; Don't include it here. */
#ifdef WOLFSSL_USER_SETTINGS
    #include <wolfssl/wolfcrypt/settings.h>
#endif

#if defined(WOLFSSL_ESPIDF) /* Entire file is only for Espressif EDP-IDF */
#include "esp_log.h"

#if defined(CONFIG_ESP_TLS_USING_WOLFSSL)
#include <wolfssl/internal.h>
#include <wolfssl/ssl.h>

/* TODO Check minimum wolfSSL & ESP-IDF version else error */
#include <wolfssl/wolfcrypt/port/Espressif/esp_crt_bundle.h>

#define X509_MAX_SUBJECT_LEN 255

#ifdef IS_MBEDTLS_CERT_BUNDLE /* TODO needs better gate */
    #define BUNDLE_HEADER_OFFSET 2
    #define CRT_HEADER_OFFSET 4
#else
    #define BUNDLE_HEADER_OFFSET 2
    #define CRT_HEADER_OFFSET 2
#endif

/* A "Certificate Bundle" is this array of [size] + [x509 CA]
 * certs the client trusts: */
extern const uint8_t x509_crt_imported_bundle_wolfssl_bin_start[]
                     asm("_binary_x509_crt_bundle_wolfssl_start");

extern const uint8_t x509_crt_imported_bundle_wolfssl_bin_end[]
                     asm("_binary_x509_crt_bundle_wolfssl_end");

static const char *TAG = "esp_crt_bundle-wolfssl";

/* a dummy certificate so that
 * cacert_ptr passes non-NULL check during handshake */
/* TODO: actually used by wolfSSL? */
// static WOLFSSL_X509 s_dummy_crt;

/* This crt_bundle_t type must match other providers in esp-tls from ESP-IDF.
 * TODO: Move to common header in ESP-IDF. (requires ESP-IDF modification) */
typedef struct crt_bundle_t {
    const uint8_t **crts;
    uint16_t num_certs;
    size_t x509_crt_bundle_wolfssl_len;
} crt_bundle_t;

static crt_bundle_t s_crt_bundle;
static esp_err_t _esp_crt_bundle_is_valid = ESP_FAIL;

static esp_err_t esp_crt_bundle_init(const uint8_t *x509_bundle,
                                     size_t bundle_size);


// #if defined(WOLFSSL_X509_CRT_PARSE_C)
static int _cert_bundled_loaded = 0;
/* typedef int (*VerifyCallback)(int, WOLFSSL_X509_STORE_CTX*); */
static int wolfssl_ssl_conf_verify_cb(int preverify,
                                      WOLFSSL_X509_STORE_CTX* store)
{
    char subjectName[X509_MAX_SUBJECT_LEN];
    WOLFSSL_X509* x509 = NULL;
    WOLFSSL_X509_NAME* subject = NULL;
    WOLFSSL_X509_NAME* issuer = NULL;

    WOLFSSL_X509* cert = NULL;
    int cmp_res, last_cmp=-1; /* TODO what if first cert checked is bad? last_cmp may be wrong */

    int ret = -1;
    /* TODO */
    WOLFSSL_ENTER("wolfssl_ssl_conf_verify_cb");
    ESP_LOGI(TAG, "\n\nwolfssl_ssl_conf_verify_cb !\n\n");

#ifndef NO_SKIP_PREVIEW
    if (preverify == WOLFSSL_SUCCESS) {
        ESP_LOGW(TAG, "Pre-verification success."); /* or alt certs */
        /* So far, so good... we need to now check cert against alt */
        ret = WOLFSSL_SUCCESS;
    }
    else {
        ESP_LOGE(TAG, "Pre-verification failed.");
        ret = WOLFSSL_FAILURE;
    }
#else
    /* Skip pre-verification, so we'll start with success. */
    ret = WOLFSSL_SUCCESS;
#endif

    /* Check how many CA Certs in our bundle. Need at least one to proceed. */
    if (ret == WOLFSSL_SUCCESS) {
        if (s_crt_bundle.crts == NULL) {
            ESP_LOGE(TAG, "No certificates in bundle.");
            ret = WOLFSSL_FAILURE;
        }
        else {
            ESP_LOGI(TAG, "%d certificates in bundle.", s_crt_bundle.num_certs);
            ret = WOLFSSL_SUCCESS;
        }
    }

    /* TODO: iterate through all CA certs in bundle (or is the bundle a store?) */

    if (ret == WOLFSSL_SUCCESS) {
#ifdef OPENSSL_EXTRA
        cert = wolfSSL_X509_STORE_CTX_get_current_cert(store);
#else
        cert = store->current_cert;
#endif
        if (cert == NULL) {
            ESP_LOGE(TAG, "Failed to get current certificate.\n");
            ret = WOLFSSL_FAILURE;
        }
        else {
            ret = WOLFSSL_SUCCESS;
        }
    }

    if (ret == WOLFSSL_SUCCESS) {
        subject = wolfSSL_X509_get_subject_name(cert);
            if (wolfSSL_X509_NAME_oneline(subject, subjectName, sizeof(subjectName)) == NULL) {
                ESP_LOGE(TAG, "Error converting subject name to string.");
                wolfSSL_X509_free(cert);
                return -1;
            }
        issuer = wolfSSL_X509_get_issuer_name(cert);
        ESP_LOGI(TAG, "Store Cert Subject: %s", subjectName );
        ESP_LOGI(TAG, "Store Cert Issuer:  %s", issuer->name );
    }

    /* When the server presents its certificate, the client checks if this
     * certificate can be traced back to one of the CA certificates in the
     * bundle.
     */
    /* Find the cert: */
    if ((ret == WOLFSSL_SUCCESS) && (_cert_bundled_loaded == 0)) {
        _cert_bundled_loaded = 1;
        WOLFSSL_X509*child = cert;
        size_t name_len = 0;
        const uint8_t *crt_name;

        bool crt_found = false;
        int start = 0;
        int end = s_crt_bundle.num_certs - 1;
        int middle = (end - start) / 2;

        /* Look for the certificate using binary search on subject name */
        while (start <= end) {
            ESP_LOGW(TAG, "Looking at CA #%d; Start = %d, end = %d", middle, start, end);

#ifdef IS_MBEDTLS_CERT_BUNDLE /* TODO needs better gate */
            name_len = s_crt_bundle.crts[middle][0] << 8 | s_crt_bundle.crts[middle][1];
            crt_name = s_crt_bundle.crts[middle] + CRT_HEADER_OFFSET;
            ESP_LOGI(TAG, "String: %.*s", name_len, crt_name);
            int cmp_res =  memcmp(subject, crt_name, name_len );
#else
            int derCertLength = (s_crt_bundle.crts[middle][0] << 8) |
                                 s_crt_bundle.crts[middle][1];
            const unsigned char* cert_data = (const unsigned char*)s_crt_bundle.crts[middle] + BUNDLE_HEADER_OFFSET;

            ESP_LOGI(TAG, "s_crt_bundle ptr = 0x%x", (intptr_t)&cert_data);
            ESP_LOGI(TAG, "derCertLength = %d", derCertLength);

            /* Convert the DER format in the Cert Bundle to x509.
             * Reminder: Cert PEM files converted to DER by gen_crt_bundle.py */
            cert = wolfSSL_d2i_X509(&x509,  &cert_data, derCertLength);
            if (cert == NULL) {
                ESP_LOGE(TAG, "Error loading DER Certificate Authority (CA)"
                              "from bundle #%d.", middle);
                ret = ESP_FAIL;
            }
            else {
                ESP_LOGI(TAG, "Successfully loaded DER certificate!");
                ret = ESP_OK;
            }

            if (ret == ESP_OK) {
                subject = wolfSSL_X509_get_subject_name(cert);
                if (subject == NULL) {
                    ESP_LOGE(TAG, "Error getting subject name.");
                    ret = ESP_FAIL;
                }
                if (wolfSSL_X509_NAME_oneline(subject, subjectName, sizeof(subjectName)) == NULL) {
                    ESP_LOGE(TAG, "Error converting subject name to string.");
                    ret = ESP_FAIL;
                }
                ESP_LOGI(TAG, "Subject Name: %s", subjectName);
            }

            // Free the certificate
            wolfSSL_X509_free(cert);

            /* subject == issuer */
            if (ret == ESP_OK) {
                cmp_res = memcmp(issuer->name, subject->name, strlen((const char*)subject->name));
                last_cmp = cmp_res;
            }
            else {
                ESP_LOGW(TAG, "Skipping CA #%d", middle);
                cmp_res = last_cmp;
            }
#endif

            if (cmp_res == 0) {
                ESP_LOGW(TAG, "crt found %s", issuer->name );
                crt_found = true;
                break;
            } else if (cmp_res < 0) {
                end = middle - 1;
            } else {
                start = middle + 1;
            }
            middle = (start + end) / 2;
        }

        if (crt_found) {
            // derCertLength
            size_t key_len = s_crt_bundle.crts[middle][2] << 8 | s_crt_bundle.crts[middle][3];
            // TODO check sig: ret = esp_crt_check_signature(child, s_crt_bundle.crts[middle] + CRT_HEADER_OFFSET + name_len, key_len);
        }
        else {
            ESP_LOGW(TAG, "crt not found!");
        }

        if (ret == 0) {
            ESP_LOGI(TAG, "Certificate validated !!!");
        }
    }

    WOLFSSL_LEAVE( "wolfssl_ssl_conf_verify_cb complete", ret);

    return ret;
}

/* wolfssl_ssl_conf_verify() patterned after ESP-IDF.
 * Used locally here only. Not used directly by esp-tls. */
void wolfssl_ssl_conf_verify(wolfssl_ssl_config *conf,
                             int (*f_vrfy) WOLFSSL_X509_VERIFY_CALLBACK,
                             void (*p_vrfy) )
{
    /* Other Crytographic providers for reference:
    conf->f_vrfy      = f_vrfy;  (verification function callback)
    conf->p_vrfy      = p_vrfy;  (pre-verification value)
    */

    /* typedef int (*VerifyCallback)(int, WOLFSSL_X509_STORE_CTX*); */
    wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)(conf->priv_ctx),
                            WOLFSSL_VERIFY_PEER, wolfssl_ssl_conf_verify_cb);
}
//#endif /* WOLFSSL_X509_CRT_PARSE_C */

/* esp_crt_verify_callback() patterned after ESP-IDF.
 * Used locally here only. Not used directly by esp-tls.
 *
 * This callback is called for every certificate in the chain. If the chain
 * is proper each intermediate certificate is validated through its parent
 * in the x509_crt_verify_chain() function. So this callback should
 * only verify the first untrusted link in the chain is signed by the
 * root certificate in the trusted bundle
*/
int esp_crt_verify_callback(void *buf, WOLFSSL_X509 *crt, int depth,
                            uint32_t *flags)
{
    WOLFSSL_X509 *child;/* TODO ? */
    const uint8_t *crt_name;
    int ret = -1;
    int start = 0;
    int end  = 0;
    int middle =0;
    size_t name_len = 0;
    size_t key_len  = 0;
    bool crt_found = false;

    child = crt;

    /* It's OK for a trusted cert to have a weak signature hash alg.
       as we already trust this certificate */
//    uint32_t flags_filtered = *flags & ~(WOLFSSL_X509_BADCERT_BAD_MD);
//
//    if (flags_filtered != WOLFSSL_X509_BADCERT_NOT_TRUSTED) {
//        return 0;
//    }


    if (s_crt_bundle.crts == NULL) {
        ESP_LOGE(TAG, "No certificates in bundle");
        return -1;
    }

    ESP_LOGI(TAG, "esp_crt_verify_callback: %d certificates in bundle",
                  s_crt_bundle.num_certs);

    name_len = 0;

    crt_found = false;
    start = 0;
    end = s_crt_bundle.num_certs - 1;
    middle = (end - start) / 2;

    /* Look for the certificate using binary search on subject name */
    while (start <= end) {
        name_len = (s_crt_bundle.crts[middle][0] << 8) |
                   (s_crt_bundle.crts[middle][1]);
        crt_name = s_crt_bundle.crts[middle] + CRT_HEADER_OFFSET;

        int cmp_res = memcmp(child->altNames, crt_name, name_len );
        if (cmp_res == 0) {
            ESP_LOGI(TAG, "crt found %s", crt_name);
            crt_found = true;
            break;
        } else if (cmp_res < 0) {
            end = middle - 1;
        } else {
            start = middle + 1;
        }
        middle = (start + end) / 2;
    }

    ret = -1; // WOLFSSL_ERR_X509_FATAL_ERROR;
    if (crt_found) {
        key_len = (s_crt_bundle.crts[middle][2] << 8) |
                  (s_crt_bundle.crts[middle][3]);
        //TODO
        //ret = esp_crt_check_signature(child, s_crt_bundle.crts[middle] + CRT_HEADER_OFFSET + name_len, key_len);
    }
    else {
        ESP_LOGW(TAG, "crt not found!");
    }

    if (ret == 0) {
        ESP_LOGI(TAG, "Certificate validated (2)");
        *flags = 0;
        return 0;
    }

    ESP_LOGE(TAG, "Failed to verify certificate");
    return -1; // WOLFSSL_ERR_X509_FATAL_ERROR;
} /* esp_crt_verify_callback */

/* wolfssl_ssl_conf_authmode() patterned after ESP-IDF.
 * Used locally here only. Not used directly by esp-tls. */
void wolfssl_ssl_conf_authmode(wolfssl_ssl_config *conf, int authmode)
{
   // wolfSSL_CTX_set_verify( (WOLFSSL_CTX *)tls->priv_ctx, authmode, NULL);
    ESP_LOGW(TAG, "wolfssl_ssl_conf_authmode not implemented");
}

void wolfssl_x509_crt_init(WOLFSSL_X509 *crt)
{
    /* TODO: do we really need this? so far, only used for dummy cert bundle init */
    /* TODO: if we keep this, see if we can/should use ssl->heap param. */
    InitX509(crt, 0, NULL);
}

/* cert buffer compatilbility helper */
void wolfssl_ssl_conf_ca_chain(wolfssl_ssl_config *conf,
                               WOLFSSL_X509      *ca_chain,
                               WOLFSSL_X509_CRL  *ca_crl) /* TODO check crl type */
{
    conf->ca_chain   = ca_chain;
    conf->ca_crl     = ca_crl;

#if defined(WOLFSSL_X509_TRUSTED_CERTIFICATE_CALLBACK)
    /* wolfssl_ssl_conf_ca_chain() and wolfsslssl_ssl_conf_ca_cb()
     * cannot be used together. (TODO: confirm) */
    conf->f_ca_cb = NULL;
    conf->p_ca_cb = NULL;
#endif /* WOLFSSL_X509_TRUSTED_CERTIFICATE_CALLBACK */
}

static esp_err_t esp_crt_bundle_is_valid()
{
    return _esp_crt_bundle_is_valid;
}

/* Initialize the bundle into an array so we can do binary
 * search for certs; the bundle generated by the python utility is
 * already presorted by subject name.
 */
static esp_err_t esp_crt_bundle_init(const uint8_t *x509_bundle,
                                     size_t bundle_size)
{
    const uint8_t **crts;
    const uint8_t *bundle_end;
    const uint8_t *cur_crt;
    uint16_t num_certs;
    int i;

    WOLFSSL_ENTER(esp_crt_bundle_init);

    if (bundle_size < BUNDLE_HEADER_OFFSET + CRT_HEADER_OFFSET) {
        ESP_LOGE(TAG, "Invalid certificate bundle size");
        return ESP_ERR_INVALID_ARG;
    }

    num_certs = (x509_bundle[0] << 8) | x509_bundle[1];
    if (num_certs > CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_MAX_CERTS) {
        ESP_LOGE(TAG, "No. of certs in the certificate bundle = %d exceeds\n"
                      "Max allowed certificates in certificate bundle = %d\n"
                      "Please update the menuconfig option",
                      num_certs, CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_MAX_CERTS);
        return ESP_ERR_INVALID_ARG;
    }
    else {
        ESP_LOGI(TAG, "No. of certs in certificate bundle = % d", num_certs);
        ESP_LOGI(TAG, "Max allowed certificates in certificate bundle = %d",
                      CONFIG_WOLFSSL_CERTIFICATE_BUNDLE_MAX_CERTS);
    }

    crts = calloc(num_certs, sizeof(x509_bundle));
    if (crts == NULL) {
        ESP_LOGE(TAG, "Unable to allocate memory for bundle pointers");
        return ESP_ERR_NO_MEM;
    }

    /* This is the maximum region that is allowed to access */
    ESP_LOGV(TAG, "Bundle Start 0x%x", (intptr_t)x509_bundle);
    ESP_LOGV(TAG, "Bundle Size  %d", bundle_size);
    bundle_end = x509_bundle + bundle_size;
    ESP_LOGV(TAG, "Bundle End   0x%x", (intptr_t)bundle_end);
    cur_crt = x509_bundle + BUNDLE_HEADER_OFFSET;

    for (i = 0; i < num_certs; i++) {
        ESP_LOGV(TAG, "Init Cert %d", i);
        crts[i] = cur_crt;
        if (cur_crt + CRT_HEADER_OFFSET > bundle_end) {
            ESP_LOGE(TAG, "Invalid certificate bundle current offset");
            free(crts);
            return ESP_ERR_INVALID_ARG;
        }

//char subjectName[X509_MAX_SUBJECT_LEN];
//    WOLFSSL_X509_NAME* subject = NULL;
//    WOLFSSL_X509_NAME* issuer = NULL;
//
//    WOLFSSL_X509* cert = cur_crt;
//        subject = wolfSSL_X509_get_subject_name(cert);
//            if (wolfSSL_X509_NAME_oneline(subject, subjectName, sizeof(subjectName)) == NULL) {
//                ESP_LOGE(TAG, "Error converting subject name to string.");
//                wolfSSL_X509_free(cert);
//                return -1;
//            }
//        issuer = wolfSSL_X509_get_issuer_name(cert);
//        ESP_LOGI(TAG, "init Store Cert Subject: %s", subjectName );
//        ESP_LOGI(TAG, "init Store Cert Issuer:  %s", issuer->name );

#ifdef IS_MBEDTLS_CERT_BUNDLE /* TODO needs better gate */
        size_t name_len = cur_crt[0] << 8 | cur_crt[1];
        size_t key_len = cur_crt[2] << 8 | cur_crt[3];
        cur_crt = cur_crt + CRT_HEADER_OFFSET + name_len + key_len;
#else
        size_t key_len = cur_crt[0] << 8 | cur_crt[1];
        cur_crt = cur_crt + (CRT_HEADER_OFFSET + key_len);
#endif

    }

    if (cur_crt > bundle_end) {
        ESP_LOGE(TAG, "Invalid certificate bundle after end");
        free(crts);
        return ESP_ERR_INVALID_ARG;
    }

    /* The previous crt bundle is only updated when initialization of the
     * current crt_bundle is successful */
    /* Free previous crt_bundle */
    free(s_crt_bundle.crts);
    s_crt_bundle.num_certs = num_certs;
    s_crt_bundle.crts = crts;
    WOLFSSL_LEAVE(esp_crt_bundle_init, ret);
    return ESP_OK;
}

/* esp_crt_bundle_attach() used by ESP-IDF esp-tls layer. */
esp_err_t esp_crt_bundle_attach(void *conf)
{
    esp_err_t ret = ESP_OK;

    /* If no bundle has been set by the user,
     * then use the bundle embedded in the binary */
    if (s_crt_bundle.crts == NULL) {
        ESP_LOGI(TAG, "No bundle set by user; using the embedded binary.");
        ESP_LOGI(TAG, "x509_crt_imported_bundle_wolfssl_bin_start 0x%x", (intptr_t)x509_crt_imported_bundle_wolfssl_bin_start);
        ESP_LOGI(TAG, "x509_crt_imported_bundle_wolfssl_bin_end 0x%x", (intptr_t)x509_crt_imported_bundle_wolfssl_bin_end);
        ret = esp_crt_bundle_init(x509_crt_imported_bundle_wolfssl_bin_start,
                                  (x509_crt_imported_bundle_wolfssl_bin_end
                                 - x509_crt_imported_bundle_wolfssl_bin_start)
                                 );
    }
    else {
        ESP_LOGI(TAG, "Cert bundle set by user at 0x%x.",
                       (intptr_t)s_crt_bundle.crts);
    }

    if (ret == ESP_OK) {
        if (conf) {
            wolfssl_ssl_config *ssl_conf = (wolfssl_ssl_config *)conf;
            /* point to a dummy certificate
             * This is only required so that the
             * cacert_ptr passes non-NULL check during handshake
             */
             /* TODO: do we need a dummy cert? */
             //wolfssl_x509_crt_init(&s_dummy_crt);
             //wolfssl_ssl_conf_ca_chain(ssl_conf, &s_dummy_crt, NULL);

             /* typedef int (*VerifyCallback)(int, WOLFSSL_X509_STORE_CTX*); */
             /* TODO: cb not properly attached here: */
            wolfssl_ssl_conf_verify(ssl_conf, esp_crt_verify_callback, NULL);
        }
        else {
            ESP_LOGI(TAG, "esp_crt_bundle_attach no conf object supplied");
        }
    }
    else {
        ESP_LOGE(TAG, "Failed to attach bundle");
    }
    ESP_LOGI(TAG, "esp_crt_bundle_attach completed for wolfSSL");

    _esp_crt_bundle_is_valid = ret;
    return ret;
}

/* esp_crt_bundle_attach() used by ESP-IDF esp-tls layer. */
void esp_crt_bundle_detach(wolfssl_ssl_config *conf)
{
    free(s_crt_bundle.crts);
    s_crt_bundle.crts = NULL;
    if (conf) {
        wolfssl_ssl_conf_verify(conf, NULL, NULL);
        ESP_LOGE(TAG, "esp_crt_bundle_detach not implemented for wolfSSL");
    }
    ESP_LOGE(TAG, "Not implemented: esp_crt_bundle_detach");

    /* If there's no cert bundle attached, it is not valid */
    _esp_crt_bundle_is_valid = ESP_FAIL;
}

/* esp_crt_bundle_set() used by ESP-IDF esp-tls layer. */
esp_err_t esp_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size)
{
    return esp_crt_bundle_init(x509_bundle, bundle_size);
}

#endif /* CONFIG_ESP_TLS_USING_WOLFSSL */
#endif /* WOLFSSL_ESPIDF */
