/* main.c
 *
 * Copyright (C) 2006-2025 wolfSSL Inc.
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

#include "client-tls.h"
/* Espressif FreeRTOS */
#ifndef SINGLE_THREADED
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include <freertos/event_groups.h>
#endif

/* Espressif */
#include <esp_log.h>

/* socket includes */
#include <lwip/netdb.h>
#include <lwip/sockets.h>

/* wolfSSL */
#include <wolfssl/wolfcrypt/settings.h>
/* This project not yet using the library */
#include <wolfssl/ssl.h>

#if defined(WOLFSSL_WC_MLKEM)
    #include <wolfssl/wolfcrypt/mlkem.h>
    #include <wolfssl/wolfcrypt/wc_mlkem.h>
#endif
#if defined(USE_CERT_BUFFERS_2048) || defined(USE_CERT_BUFFERS_1024)
    #include <wolfssl/certs_test.h>
#endif
#ifdef WOLFSSL_TRACK_MEMORY
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

#ifndef NO_DH
    /* see also wolfssl/test.h */
    #undef  DEFAULT_MIN_DHKEY_BITS
    #define DEFAULT_MIN_DHKEY_BITS 1024

    #undef  DEFAULT_MAX_DHKEY_BITS
    #define DEFAULT_MAX_DHKEY_BITS 2048
#endif


/*
 * Optionally define explicit ciphers, for example these TLS 1.3 options.
 *
 * TLS13-AES128-GCM-SHA256
 * TLS13-AES256-GCM-SHA384
 * TLS13-AES128-CCM-SHA256
 * TLS13-AES128-CCM-8-SHA256
 * TLS13-AES128-CCM8-SHA256
 *
 * examples:
 * #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-GCM-SHA256:PSK-AES128-GCM-SHA256"
 * #define WOLFSSL_ESP32_CIPHER_SUITE "TLS13-AES128-CCM-8-SHA256"
 */


/* Project */
#include "wifi_connect.h"
#include "time_helper.h"

/* working TLS 1.2 VS client app commandline param:
 *
 *  -h 192.168.1.128 -v 3 -l ECDHE-ECDSA-SM4-CBC-SM3  -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem -A ./certs/sm2/root-sm2.pem -C
 *
 * working Linux, non-working VS c app
 *
 *  -h 192.168.1.128 -v 4 -l TLS13-SM4-CCM-SM3        -c ./certs/sm2/client-sm2.pem -k ./certs/sm2/client-sm2-priv.pem -A ./certs/sm2/root-sm2.pem -C
 *
 **/
#define TAG "client-tls"

// file included in CMakeLists.txt
extern const unsigned char mozilla_root_certs_pem_start[] asm("_binary_mozillarootcerts_pem_start");
extern const unsigned char mozilla_root_certs_pem_end[] asm("_binary_mozillarootcerts_pem_end");

int ShowCiphers(WOLFSSL* ssl)
{
    #define CLIENT_TLS_MAX_CIPHER_LENGTH 4096
    char ciphers[CLIENT_TLS_MAX_CIPHER_LENGTH];
    const char* cipher_used;
    int ret = 0;

    if (ssl == NULL) {
        ESP_LOGI(TAG, "WOLFSSL* ssl is NULL, so no cipher in use");
        ret = wolfSSL_get_ciphers(ciphers, (int)sizeof(ciphers));
        if (ret == WOLFSSL_SUCCESS) {
            for (int i = 0; i < CLIENT_TLS_MAX_CIPHER_LENGTH; i++) {
                if (ciphers[i] == ':') {
                    ciphers[i] = '\n';
                }
            }
            ESP_LOGI(TAG, "Available Ciphers:\n%s\n", ciphers);
        }
        else {
            ESP_LOGE(TAG, "Failed to call wolfSSL_get_ciphers. Error %d", ret);
        }
    }
    else {
        cipher_used = wolfSSL_get_cipher_name(ssl);
        ESP_LOGI(TAG, "WOLFSSL* ssl using %s", cipher_used);
    }

    return ret;
}

/*
static int verifyIgnoreDateError(int preverify, WOLFSSL_X509_STORE_CTX* store)
{
    if (store->error == ASN_BEFORE_DATE_E)
        return 1; // override error
    else
        return preverify;
}
*/

void my_log_cb(int level, const char* msg) {
    ESP_LOGI(TAG, "wolfSSL [%d] %s", level, msg);
}

WOLFSSL_ESP_TASK tls_smp_client_task_2(void *args)
{
#undef  TLS_SMP_TARGET_HOST
#define TLS_SMP_TARGET_HOST "www.google.com"

#undef  TLS_SMP_DEFAULT_PORT
#define TLS_SMP_DEFAULT_PORT 443

#if defined(SINGLE_THREADED)
    int ret = ESP_OK;
#define TLS_SMP_CLIENT_TASK_RET ret
#else
#define TLS_SMP_CLIENT_TASK_RET
#endif
    char buff[256];
    const char sndMsg[] = "GET / HTTP/1.1\r\n"
                          "Host: www.google.com\r\n"
                          "Accept: */*\r\n"
                          "\r\n";
    const char *ch = TLS_SMP_TARGET_HOST; /* see wifi_connect.h */
    struct sockaddr_in servAddr;

    struct hostent *hp;
    struct ip4_addr *ip4_addr;
    int ret_i; /* interim return values */
    int err;   /* interim return values */
    int sockfd;
#ifndef NO_DH
    int minDhKeyBits = DEFAULT_MIN_DHKEY_BITS;
#endif

    /* declare wolfSSL objects */
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    size_t len;

    WOLFSSL_ENTER(TLS_SMP_CLIENT_TASK_NAME);

#ifdef DEBUG_WOLFSSL
    WOLFSSL_MSG("Debug ON");
    ShowCiphers(NULL);
#endif


    /* Initialize wolfSSL */
    wolfSSL_Init();

    /* Create a socket that uses an Internet IPv4 address,
     * Sets the socket to be stream based (TCP),
     * 0 means choose the default protocol. */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        ESP_LOGE(TAG, "ERROR: failed to create the socket\n");
    }

    ESP_LOGI(TAG, "get target IP address");

    hp = gethostbyname(TLS_SMP_TARGET_HOST);
    if (!hp)
    {
        ESP_LOGE(TAG, "Failed to get host name: '%s'", TLS_SMP_TARGET_HOST);
        ip4_addr = NULL;
    }
    else
    {
        ip4_addr = (struct ip4_addr *)hp->h_addr;
        ESP_LOGI(TAG, "Host name: %s", hp->h_name);
    }
#ifdef NO_RSA
    ESP_LOGE(TAG, "Warning: NO_RSA defined");
#endif

    /* Create and initialize WOLFSSL_CTX */
#ifdef WOLFSSL_TLS13
    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()); /* SSL 3.0 - TLS 1.3. */
    wolfSSL_CTX_SetMinVersion(ctx, WOLFSSL_TLSV1_3);
#else
    ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    wolfSSL_CTX_SetMinVersion(ctx, WOLFSSL_TLSV1_2);
#endif
    if (ctx == NULL) {
        ret_i = WOLFSSL_FAILURE;
        ESP_LOGE(TAG, "Failed to create ctx, ret = %d", ret_i);
    }
    else {
        ret_i = WOLFSSL_SUCCESS;
        ESP_LOGE(TAG, "Create ctx success, ret = %d", ret_i);
    }

#ifdef HAVE_SNI
    ret_i = wolfSSL_CTX_UseSNI(ctx, WOLFSSL_SNI_HOST_NAME, TLS_SMP_TARGET_HOST, strlen(TLS_SMP_TARGET_HOST));
    if (ret_i != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "ERROR: failed to use SNI %d, please check the file.\n",
                 ret_i);
    }
    else {
        ESP_LOGI(TAG, "CA cert Use SNI success");
    }
#else
    ESP_LOGI(TAG, "CA cert not using SNI");
#endif

    /*   options:   */
    /* ctx = wolfSSL_CTX_new(wolfSSLv1_2_client_method());      only TLS 1.2 */
    /* ctx = wolfSSL_CTX_new(wolfSSLv1_3_client_method());      only TLS 1.3 */
    /* wolfSSL_CTX_NoTicketTLSv12(); */
    /* wolfSSL_NoTicketTLSv12();     */
    if (ctx == NULL)
    {
        ESP_LOGE(TAG, "ERROR: failed to create WOLFSSL_CTX\n");
    }

    // load certs
//
    int this_size = mozilla_root_certs_pem_end - mozilla_root_certs_pem_start;
    #define EXPECTED_SIZE 8408
    ESP_LOGI(TAG, "Loading %d bytes of PEM certs", (int)(mozilla_root_certs_pem_end - mozilla_root_certs_pem_start));
    if (this_size < EXPECTED_SIZE) {
        ESP_LOGE(TAG, "Expecting %d bytes", EXPECTED_SIZE);
    }

    wolfSSL_SetLoggingCb(my_log_cb);
    wolfSSL_Debugging_ON();

    ret_i = wolfSSL_CTX_load_verify_buffer(ctx,
                                           mozilla_root_certs_pem_start,
                                           mozilla_root_certs_pem_end - mozilla_root_certs_pem_start,
                                           WOLFSSL_FILETYPE_PEM);

    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    // wolfSSL_Debugging_ON();

    if (ret_i != WOLFSSL_SUCCESS)
    {
        ESP_LOGE(TAG, "ERROR: failed to load CA cert %d, "
                      "please check the file.\n",
                 ret_i);
    }
    else
    {
        ESP_LOGI(TAG, "CA cert loaded successfully");
    }

    /* Initialize the server address struct with zeros */
    memset(&servAddr, 0, sizeof(servAddr));

    /* Fill in the server address */
    servAddr.sin_family = AF_INET;                   /* using IPv4      */
    servAddr.sin_port = htons(TLS_SMP_DEFAULT_PORT); /* on DEFAULT_PORT */

    if (*ch >= '1' && *ch <= '9')
    {
        /* Get the server IPv4 address from the command line call */
        WOLFSSL_MSG("inet_pton");
        if ((ret_i = inet_pton(AF_INET,
                               TLS_SMP_TARGET_HOST,
                               &servAddr.sin_addr)) != 1)
        {
            ESP_LOGE(TAG, "ERROR: invalid address ret=%d\n", ret_i);
        }
        ESP_LOGI(TAG, "Using IP address: %s", TLS_SMP_TARGET_HOST);
    }
    else
    {
        servAddr.sin_addr.s_addr = ip4_addr->addr;
        ESP_LOGI(TAG, "Using host name: %s", TLS_SMP_TARGET_HOST);
    }

    /* Connect to the server */
    sprintf(buff,
            "Connecting to server....%s (port:%d)",
            TLS_SMP_TARGET_HOST,
            TLS_SMP_DEFAULT_PORT);
    ESP_LOGI(TAG, "%s\n", buff);

    if ((ret_i = connect(sockfd,
                         (struct sockaddr *)&servAddr,
                         sizeof(servAddr))) == -1)
    {
        ESP_LOGE(TAG, "ERROR: failed to connect ret=%d\n", ret_i);
    }

    WOLFSSL_MSG("Create a WOLFSSL object");
    /* Create a WOLFSSL object */
    if ((ssl = wolfSSL_new(ctx)) == NULL)
    {
        ESP_LOGE(TAG, "ERROR: failed to create WOLFSSL object\n");
    }
    else
    {
        ESP_LOGI(TAG, "WOLFSSL object created successfully");
    }

    /* Attach wolfSSL to the socket */
    ret_i = wolfSSL_set_fd(ssl, sockfd);
    if (ret_i == WOLFSSL_SUCCESS)
    {
        ESP_LOGI(TAG, "wolfSSL_set_fd success");
    }
    else
    {
        ESP_LOGE(TAG, "ERROR: failed wolfSSL_set_fd. Error: %d\n", ret_i);
    }

    // wolfSSL_Debugging_ON();

    ESP_LOGI(TAG, "Connect to wolfSSL server...");
    ret_i = wolfSSL_connect(ssl);

    if (ret_i == WOLFSSL_SUCCESS)
    {

        ESP_LOGI(TAG, "Connect success! Sending message...");
        /* Get a message for the server from stdin */
        WOLFSSL_MSG("Message for server: ");
        memset(buff, 0, sizeof(buff));

        len = XSTRLEN(sndMsg);
        strncpy(buff, sndMsg, len);
        buff[len] = '\0';
        ESP_LOGI(TAG, "SSL connect ok, sending message:\n\n%s\n", buff);

        /* Send the message to the server */
        do
        {
            err = 0; /* reset error */
            ret_i = wolfSSL_write(ssl, buff, len);
            if (ret_i <= 0)
            {
                err = wolfSSL_get_error(ssl, 0);
            }
        } while (err == WOLFSSL_ERROR_WANT_WRITE ||
                 err == WOLFSSL_ERROR_WANT_READ);

        if (ret_i != len)
        {
            ESP_LOGE(TAG, "ERROR: failed to write\n");
        }
        else
        {
            ESP_LOGI(TAG, "Message sent! Awaiting response...");
        }

        /* Read the server data into our buff array */
        memset(buff, 0, sizeof(buff));

        do
        {
            err = 0; /* reset error */
            ret_i = wolfSSL_read(ssl, buff, sizeof(buff));
            if (ret_i <= 0)
            {
                err = wolfSSL_get_error(ssl, 0);
            }
        } while ((err == WOLFSSL_ERROR_WANT_READ) ||
                 (err == WOLFSSL_ERROR_WANT_WRITE));

        if (ret_i < 0)
        {
            ESP_LOGE(TAG, "ERROR: failed to read\n");
        }

        /* Show any data the server sends */
        ESP_LOGI(TAG, "Server response: \n\n%s\n", buff);

        ret_i = wolfSSL_shutdown(ssl);
        while (ret_i == WOLFSSL_SHUTDOWN_NOT_DONE)
        {
            ret_i = wolfSSL_shutdown(ssl); /* bidirectional shutdown */
            if (ret_i == WOLFSSL_SUCCESS)
            {
                ESP_LOGI(TAG, "Bidirectional shutdown complete\n");
                break;
            }
            else if (ret_i != WOLFSSL_SHUTDOWN_NOT_DONE)
            {
                ESP_LOGE(TAG, "Bidirectional shutdown failed\n");
                break;
            }
        }
        if (ret_i != WOLFSSL_SUCCESS)
        {
            ESP_LOGE(TAG, "Bidirectional shutdown failed\n");
        }

    } /* wolfSSL_connect(ssl) == WOLFSSL_SUCCESS) */
    else
    {
        ESP_LOGE(TAG, "ERROR: wolfSSL_connect failed. ret=%d\n", ret_i);
        int err = wolfSSL_get_error(ssl, 0);
        ESP_LOGE(TAG, "wolfSSL_get_error: %d\n",
                 err);
    }

    ESP_LOGI(TAG, "Cleanup and exit");
    wolfSSL_free(ssl);     /* Release the wolfSSL object memory        */
    wolfSSL_CTX_free(ctx); /* Free the wolfSSL context object          */
    wolfSSL_Cleanup();     /* Cleanup the wolfSSL environment          */
    close(sockfd);         /* Close the connection to the server       */

    vTaskDelete(NULL);

    return TLS_SMP_CLIENT_TASK_RET;
}
