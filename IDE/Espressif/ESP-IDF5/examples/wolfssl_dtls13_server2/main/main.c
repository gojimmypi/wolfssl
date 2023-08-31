/* main.c
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

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

//#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <wolfssl/wolfcrypt/settings.h>

#include <wolfssl/ssl.h>
#include <errno.h>

#include "server-dtls13.h"
#include "dtls-common.h"
/* project */
#include "main.h"
#include "wifi_connect.h"
#include "time_helper.h"

#define PORT CONFIG_EXAMPLE_PORT

#define INVALID_SOCKET -1
#define TLS_SMP_SERVER_TASK_NAME         "dtls13"
#define TLS_SMP_SERVER_TASK_WORDS        10240*2
#define TLS_SMP_SERVER_TASK_PRIORITY     5


static const char *TAG = "example";
WOLFSSL_CTX*  ctx = NULL;
WOLFSSL*      ssl = NULL;
int           listenfd = INVALID_SOCKET;   /* Initialize our socket */
#if 0
int main(int argc, char** argv)
#else
WOLFSSL_ESP_TASK udp_server_task(void *pvParameters)
#endif
{
    /* Loc short for "location" */
#if defined(SINGLE_THREADED)
    #define TLS_SMP_SERVER_TASK_RET exitVal
#else
    #define TLS_SMP_SERVER_TASK_RET
#endif
    int           exitVal = 0;
    struct sockaddr_in servAddr = { 0 };        /* our server's address */
    struct sockaddr_in cliaddr = { 0 };         /* the client's address */
    int           ret;
    int           err;
    int           recvLen = 0;    /* length of message */
    socklen_t     cliLen;
    char          buff[MAXLINE];   /* the incoming message */
    int ip_protocol = 0;
    char          ack[] = "I hear you fashizzle!\n";
    exitVal = 1;
        ESP_LOGI(TAG, "Init Stack: %d words", TLS_SMP_SERVER_TASK_WORDS);

    ESP_LOGI(TAG, "Stack used: %d words", TLS_SMP_SERVER_TASK_WORDS
                                    - (uxTaskGetStackHighWaterMark(NULL)));
    /* Initialize wolfSSL before assigning ctx */
    if (wolfSSL_Init() != WOLFSSL_SUCCESS) {
        ESP_LOGE(TAG, "wolfSSL_Init error.\n");
        return TLS_SMP_SERVER_TASK_RET;
    }
        ESP_LOGI(TAG, "init Stack used: %d words", TLS_SMP_SERVER_TASK_WORDS
                                        - (uxTaskGetStackHighWaterMark(NULL)));
    /* No-op when debugging is not compiled in */
     wolfSSL_Debugging_ON();

    /* Set ctx to DTLS 1.3 */
#ifndef USE_DTLS12
     ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
#else
     ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method());
#endif
    if(ctx == NULL) {
        ESP_LOGE(TAG, "wolfSSL_CTX_new error.\n");
        // goto cleanup;
    }

    (void)ctx;
        ESP_LOGI(TAG, "ctx Stack used: %d words", TLS_SMP_SERVER_TASK_WORDS
                                        - (uxTaskGetStackHighWaterMark(NULL)));
#ifdef NO_FILESYSTEM
    /* Load CA certificates */
//    ret = wolfSSL_CTX_load_verify_buffer(ctx,
//                                         CTX_CA_CERT,
//                                         CTX_CA_CERT_SIZE,
//                                         CTX_SERVER_CERT_TYPE);

//    if (wolfSSL_CTX_load_verify_locations(ctx,caCertLoc,0) !=
//            SSL_SUCCESS) {
//        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", caCertLoc);
//        goto cleanup;
//    }
    /* Load server certificates */
//    ret = wolfSSL_CTX_use_certificate_chain_buffer_format(ctx,
//                //server_cert_der_2048, sizeof_server_cert_der_2048,
//                CTX_SERVER_CERT, CTX_SERVER_CERT_SIZE,
//                CTX_SERVER_CERT_TYPE);
//    if (wolfSSL_CTX_use_certificate_buffer(ctx, servCertLoc, SSL_FILETYPE_PEM) !=
//                                                                 SSL_SUCCESS) {
//        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", servCertLoc);
//        goto cleanup;
//    }
    /* Load server Keys */
//    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx,
//                                            server_key_der_2048, sizeof_server_key_der_2048,
//                                            CTX_SERVER_KEY,
//                                            CTX_SERVER_KEY_SIZE,
//                                            CTX_SERVER_KEY_TYPE);
//    if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, servKeyLoc,
//                SSL_FILETYPE_PEM) != SSL_SUCCESS) {
//        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", servKeyLoc);
//        goto cleanup;
//    }
#else
    /* Load CA certificates */
    if (wolfSSL_CTX_load_verify_locations(ctx,caCertLoc,0) !=
            SSL_SUCCESS) {
        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", caCertLoc);
        goto cleanup;
    }
    /* Load server certificates */
    if (wolfSSL_CTX_use_certificate_file(ctx, servCertLoc, SSL_FILETYPE_PEM) !=
                                                                 SSL_SUCCESS) {
        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", servCertLoc);
        goto cleanup;
    }
    /* Load server Keys */
    if (wolfSSL_CTX_use_PrivateKey_file(ctx, servKeyLoc,
                SSL_FILETYPE_PEM) != SSL_SUCCESS) {
        ESP_LOGE(TAG, "Error loading %s, please check the file.\n", servKeyLoc);
        goto cleanup;
    }
#endif

    memset((char *)&servAddr, 0, sizeof(servAddr));
    /* host-to-network-long conversion (htonl) */
    /* host-to-network-short conversion (htons) */
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port        = htons(SERV_PORT);
    ip_protocol = IPPROTO_IP;

    /* Create a UDP/IP socket */
    listenfd = socket(AF_INET, SOCK_DGRAM, ip_protocol);
    if (listenfd < 0 ) {
        ESP_LOGE(TAG, "socket()");
        goto cleanup;
    }
    ESP_LOGI(TAG, "Socket allocated\n");

#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
    int enable = 1;
    lwip_setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &enable, sizeof(enable));
#endif


    struct timeval timeout;
    timeout.tv_sec = 10; // 10 seconds
    timeout.tv_usec = 0;

    if (setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
         ESP_LOGE(TAG, "setsockopt for receive timeout");
    }

    /* Bind Socket */
    if (bind(listenfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        ESP_LOGE(TAG, "bind()");
        goto cleanup;
    }

//    if (setsockopt(listenfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
//         ESP_LOGE(TAG, "setsockopt for send timeout");
//    }

#ifndef WOLFSSL_ESPIDF
    /* Signals and signal handling are not implemented in esp-idf.
     * Calling raise() will abort the program.
     * see https://esp32.com/viewtopic.php?t=29988&p=103871 */
    signal(SIGINT, sig_handler);
#endif

        ESP_LOGI(TAG, "While Stack used: %d words", TLS_SMP_SERVER_TASK_WORDS
                                        - (uxTaskGetStackHighWaterMark(NULL)));
    while (1) {
        ESP_LOGI(TAG, "Awaiting client connection on port %d\n", SERV_PORT);

        cliLen = sizeof(cliaddr);

        ret = 0;
        while (ret <= 0) {
#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
            int len = recvmsg(sock, &msg, 0);
#else
//          int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            ret = (int)recvfrom(listenfd, buff, sizeof(buff) -1 , 0,
                    (struct sockaddr*)&cliaddr, &cliLen);
#endif
            ESP_LOGI(TAG, " ret %d", ret);
        }

        if (ret < 0) {
            ESP_LOGE(TAG, "recvfrom()");
            goto cleanup;
        }
        else if (ret == 0) {
            ESP_LOGE(TAG, "recvfrom zero return\n");
            goto cleanup;
        }

        ESP_LOGI(TAG, "Data!");

        /* Create the WOLFSSL Object */
        if ((ssl = wolfSSL_new(ctx)) == NULL) {
            ESP_LOGE(TAG, "wolfSSL_new error.\n");
            goto cleanup;
        }

        if (wolfSSL_dtls_set_peer(ssl, &cliaddr, cliLen) != WOLFSSL_SUCCESS) {
            ESP_LOGE(TAG, "wolfSSL_dtls_set_peer error.\n");
            goto cleanup;
        }

        if (wolfSSL_set_fd(ssl, listenfd) != WOLFSSL_SUCCESS) {
            ESP_LOGE(TAG, "wolfSSL_set_fd error.\n");
            break;
        }

        if (wolfSSL_accept(ssl) != SSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
            ESP_LOGE(TAG, "error = %d, %s\n", err, wolfSSL_ERR_reason_error_string(err));
            ESP_LOGE(TAG, "SSL_accept failed.\n");
            goto cleanup;
        }
        showConnInfo(ssl);
        while (1) {
            if ((recvLen = wolfSSL_read(ssl, buff, sizeof(buff)-1)) > 0) {
                ESP_LOGI(TAG, "heard %d bytes\n", recvLen);

                buff[recvLen] = '\0';
                ESP_LOGI(TAG, "I heard this: \"%s\"\n", buff);
            }
            else if (recvLen <= 0) {
                err = wolfSSL_get_error(ssl, 0);
                if (err == WOLFSSL_ERROR_ZERO_RETURN) /* Received shutdown */
                    break;
                ESP_LOGE(TAG, "error = %d, %s\n", err, wolfSSL_ERR_reason_error_string(err));
                ESP_LOGE(TAG, "SSL_read failed.\n");
                goto cleanup;
            }
            ESP_LOGI(TAG, "Sending reply.\n");
            if (wolfSSL_write(ssl, ack, sizeof(ack)) < 0) {
                err = wolfSSL_get_error(ssl, 0);
                ESP_LOGE(TAG, "error = %d, %s\n", err, wolfSSL_ERR_reason_error_string(err));
                ESP_LOGE(TAG, "wolfSSL_write failed.\n");
                goto cleanup;
            }
        }

        ESP_LOGI(TAG, "reply sent \"%s\"\n", ack);

        /* Attempt a full shutdown */
        ret = wolfSSL_shutdown(ssl);
        if (ret == WOLFSSL_SHUTDOWN_NOT_DONE)
            ret = wolfSSL_shutdown(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
            ESP_LOGE(TAG, "err = %d, %s\n", err, wolfSSL_ERR_reason_error_string(err));
            ESP_LOGE(TAG, "wolfSSL_shutdown failed\n");
        }
        wolfSSL_free(ssl);
        ssl = NULL;

        ESP_LOGI(TAG, "Awaiting new connection\n");
cleanup:
        if (listenfd != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(listenfd, 0);
            close(listenfd);
        }
        else {
            ESP_LOGI(TAG, "restarting...");

        }
    }

    ESP_LOGI(TAG, "Exit %d", exitVal);
    exitVal = 0;
//cleanup_exit:
    // free_resources();
    wolfSSL_Cleanup();

    vTaskDelete(NULL);

    return TLS_SMP_SERVER_TASK_RET;
}


void app_main(void)
{
    ESP_LOGI(TAG, "-------------- wolfSSL DTLS 1.3 Server Example ---------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
#ifdef HAVE_VERSION_EXTENDED_INFO
    esp_ShowExtendedSystemInfo();
#endif

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize WiFi */
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    ESP_LOGI(TAG, "CONFIG_ESP_MAIN_TASK_STACK_SIZE = %d bytes (%d words)",
                   CONFIG_ESP_MAIN_TASK_STACK_SIZE,
                   (int)(CONFIG_ESP_MAIN_TASK_STACK_SIZE / sizeof(void*)));


#ifdef CONFIG_EXAMPLE_IPV4
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
#endif
#ifdef CONFIG_EXAMPLE_IPV6
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET6, 5, NULL);
#endif

}
