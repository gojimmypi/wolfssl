/* esp_crt_bundle-wolfssl.h
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
#ifndef __ESP_CRT_BUNDLE_wolfssl_LIB_H__

#define __ESP_CRT_BUNDLE_wolfssl_LIB_H__

/* This file is typically NOT directly used by applications utilizing the
 * wolfSSL libraries. It is used when the wolfssl libary component is configured
 * to be utilized by the Espressif ESP-IDF, specifically the esp-tls layer.
 *
 * See:
 * https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/protocols/esp_tls.html
 * https://github.com/espressif/esp-idf/blob/master/components/esp-tls/esp_tls.h
 */

/* Always include wolfcrypt/settings.h before any other wolfSSL file.      */
/* Reminder: settings.h pulls in user_settings.h; don't include it here.   */
#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_ESPIDF) /* Entire file is only for Espressif EDP-IDF   */

#if defined(CONFIG_ESP_TLS_USING_WOLFSSL)
    #include <wolfssl/ssl.h>


#ifdef __cplusplus
extern "C" {
#endif

#define WOLFSSL_X509_VERIFY_CALLBACK (void *, WOLFSSL_X509 *, int, uint32_t *)

typedef struct wolfssl_ssl_config wolfssl_ssl_config;

struct wolfssl_ssl_config
{
    WOLFSSL_X509* ca_chain;
    WOLFSSL_X509_CRL* ca_crl;
    void *priv_ctx;
    void *priv_ssl;

    //void* f_vrfy; /* TODO: not used? */
    //void* p_vrfy; /* TODO: not used? */
};


/**
 * @brief      Attach and enable use of a bundle for certificate verification
 *
 * Used by ESP-IDF esp-tls layer.
 *
 * Attach and enable use of a bundle for certificate verification through a
 * verification callback.
 * If no specific bundle has been set through esp_crt_bundle_set() it will
 * default to the bundle defined in menuconfig and embedded in the binary.
 *
 * @param[in]  conf      The config struct for the SSL connection.
 *
 * @return
 *             - ESP_OK  if adding certificates was successful.
 *             - Other   if an error occured or an action must be taken by the
 *                       calling process.
 */
esp_err_t esp_crt_bundle_attach(void *conf);


/**
 * @brief      Disable and dealloc the certification bundle
 *
 * Used by ESP-IDF esp-tls layer.
 *
 * Removes the certificate verification callback and deallocates used resources
 *
 * @param[in]  conf      The config struct for the SSL connection.
 */
void esp_crt_bundle_detach(wolfssl_ssl_config *conf);


/**
 * @brief      Set the default certificate bundle used for verification
 *
 * Used by ESP-IDF esp-tls layer.
 *
 * Overrides the default certificate bundle only in case of successful
 * initialization. In most use cases the bundle should be set through
 * menuconfig. The bundle needs to be sorted by subject name since binary
 * search is used to find certificates.
 *
 * @param[in]  x509_bundle     A pointer to the certificate bundle.
 *
 * @param[in]  bundle_size     Size of the certificate bundle in bytes.
 *
 * @return
 *             - ESP_OK  if adding certificates was successful.
 *             - Other   if an error occured or an action must be taken
 *                       by the calling process.
 */
esp_err_t esp_crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size);


WOLFSSL_LOCAL void wolfssl_ssl_conf_verify(wolfssl_ssl_config *conf,
                             int (*f_vrfy) WOLFSSL_X509_VERIFY_CALLBACK,
                             void *p_vrfy);

WOLFSSL_LOCAL void wolfssl_ssl_conf_authmode(wolfssl_ssl_config *conf,
                                             int authmode);

WOLFSSL_LOCAL void wolfssl_ssl_conf_ca_chain(wolfssl_ssl_config *conf,
                                             WOLFSSL_X509 *ca_chain,
                                             WOLFSSL_X509_CRL *ca_crl);

WOLFSSL_LOCAL void wolfssl_x509_crt_init(WOLFSSL_X509 *crt);

WOLFSSL_LOCAL int esp_crt_verify_callback(void *buf, WOLFSSL_X509 *crt,
                                          int depth, uint32_t *flags);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_ESP_TLS_USING_WOLFSSL */

#endif /* WOLFSSL_ESPIDF */

#endif /* __ESP_CRT_BUNDLE_wolfssl_LIB_H__ */
