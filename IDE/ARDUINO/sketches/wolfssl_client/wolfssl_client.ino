/* wolfssl_client.ino
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

/*
Tested with:

1) Intel Galileo acting as the Client, with a laptop acting as a server using
   the server example provided in examples/server.
   Legacy Arduino v1.86 was used to compile and program the Galileo

2) Espressif ESP32 WiFi
*/

// #define USE_CERT_BUFFERS_2048
#include <wolfssl.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <Ethernet.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

/* There's a 3rd party NTPClient library by Fabrice Weinberg.
 * If it is installed, uncomment define USE_NTP_LIB here: */
/* #define USE_NTP_LIB */
#ifdef USE_NTP_LIB
    #include <NTPClient.h>
#endif

/* optional board-specific networking includes */
#if defined(ESP32)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    WiFiClient client;
#elif defined(ESP8266)
    #define USING_WIFI
    #include <ESP8266WiFi.h>
    WiFiClient client;
/* #elif defined(OTHER_BOARD) */
    /* TODO other boards here */
#else
    EthernetClient client;
#endif

/* If you have a private include, define it here, otherwise edit WiFi params */
#define MY_PRIVATE_CONFIG "/workspace/my_private_config.h"
#if defined(MY_PRIVATE_CONFIG)
    /* the /workspace directory may contain a private config
     * excluded from GitHub with items such as WiFi passwords */
    #include MY_PRIVATE_CONFIG
    const char* ssid = CONFIG_ESP_WIFI_SSID;
    const char* password = CONFIG_ESP_WIFI_PASSWORD;
#else
    /* when using WiFi capable boards: */
    const char* ssid = "your_SSID";
    const char* password = "your_PASSWORD";
#endif

const char host[] = "192.168.1.38"; /* server to connect to */
const int port = 11111; /* port on server to connect to */
const int serial_baud = 115200; /* local serial port to monitor */

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);
int reconnect = 10;


void fail_wait(void) {
    Serial.println("Failed");
    while(1) {
        delay(1000);
    }
}
/*****************************************************************************/
/* Arduino setup()                                                           */
/*****************************************************************************/
void setup(void) {
    WOLFSSL_METHOD* method;
    int rc = 0;
    int ntp_tries = 20;
    char* wc_error_message = (char*)malloc(255 + 1);
#ifdef USE_NTP_LIB
    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "pool.ntp.org");
#endif

    Serial.begin(serial_baud);

#if defined(USING_WIFI)
    /* Connect to WiFi */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print("Connecting to WiFi ");
        Serial.println(ssid);
    }

    Serial.print("Connected to WiFi ");
    Serial.println(ssid);
#else
    /* We'll assume the Ethernet connection is ready to go.*/
#endif
    Serial.println("********************************************************");
    Serial.print("   wolfSSL Example Client IP = ");
    Serial.println(WiFi.localIP());
    Serial.print("Configured Server Host to connect to: ");
    Serial.println(host);
    Serial.println("********************************************************");

    /* we need a date in the range of cert expiration */
#ifdef USE_NTP_LIB
    timeClient.begin();
    timeClient.update();
    delay(1000);
    while (!timeClient.isTimeSet() && (ntp_tries > 0)) {
        timeClient.forceUpdate();
        Serial.println("Waiting for NTP update");
        delay(2000);
        ntp_tries--;
    }
    if (ntp_tries <= 0) {
        Serial.println("Warning: gave up waiting on NTP");
    }
    Serial.println(timeClient.getFormattedTime());
    Serial.println(timeClient.getEpochTime());
#endif

#if defined(ESP32)
    /* see esp32-hal-time.c */
    ntp_tries = 5;
    configTime(0, 0, "pool.ntp.org");  // You can replace "pool.ntp.org" with your preferred NTP server

    /* Wait for time to be set */
    while ((time(nullptr) <= 100000) && ntp_tries > 0) {
        Serial.println("Waiting for time to be set...");
        delay(2000);
        ntp_tries--;
    }
#endif

    /* Delay need to ensure connection to server */
    delay(4000);
    Serial.println("Here we go!");
    method = wolfSSLv23_client_method();
    if (method == NULL) {
        Serial.println("unable to get method");
        return;
    }
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        Serial.println("unable to get ctx");
        return;
    }
    /* initialize wolfSSL using callback functions */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    rc = wolfSSL_CTX_load_verify_buffer(ctx, ca_cert_der_2048,\
                                        sizeof_ca_cert_der_2048,\
                                        WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.print("Success: load_verify ca_cert_der_2048");
    }
    else {
        Serial.print("Error: wolfSSL_CTX_load_verify_buffer failed: ");
        wc_ErrorString(rc, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Certificate */
    rc = wolfSSL_CTX_use_certificate_buffer(ctx, client_cert_der_2048,\
                                            sizeof_client_cert_der_2048,\
                                            WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.print("Success: use certificate ca_cert_der_2048");
    }
    else {
        Serial.print("Error: wolfSSL_CTX_use_certificate_buffer failed: ");
        wc_ErrorString(rc, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Private Key */
    rc = wolfSSL_CTX_use_PrivateKey_buffer(ctx, client_key_der_2048,\
                                            sizeof_client_key_der_2048,\
                                            WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.print("Success: use private key buffer ca_cert_der_2048.");
    }
    else {
        Serial.print("Error: wolfSSL_CTX_use_PrivateKey_buffer failed: ");
        wc_ErrorString(rc, wc_error_message);
        Serial.println(wc_error_message);
    }

    wolfSSL_SetIOSend(ctx, EthernetSend);
    wolfSSL_SetIORecv(ctx, EthernetReceive);
    return;
}

/*****************************************************************************/
/* EthernetSend() to send a message msg string.                              */
/*****************************************************************************/
int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx) {
    int sent = 0;
    sent = client.write((byte*)msg, sz);
    return sent;
}

/*****************************************************************************/
/* EthernetReceive() to receive a reply string.                              */
/*****************************************************************************/
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx) {
    int ret = 0;
    while (client.available() > 0 && ret < sz) {
        reply[ret++] = client.read();
    }
    return ret;
}

/*****************************************************************************/
/* Arduino loop()                                                            */
/*****************************************************************************/
void loop() {
    int err            = 0;
    int input          = 0;
    int total_input    = 0;
    char msg[32]       = "hello wolfssl!";
    int msgSz          = (int)strlen(msg);
    char errBuf[80];
    char reply[80];
    const char* cipherName;
    if (reconnect) {
        reconnect--;
        if (client.connect(host, port)) {
            Serial.print("Connected to ");
            Serial.println(host);
            ssl = wolfSSL_new(ctx);
            if (ssl == NULL) {
                Serial.println("Unable to allocate SSL object");
                fail_wait();
            }
            err = wolfSSL_connect(ssl);
            if (err != WOLFSSL_SUCCESS) {
                err = wolfSSL_get_error(ssl, 0);
                wolfSSL_ERR_error_string(err, errBuf);
                Serial.print("TLS Connect Error: ");
                Serial.println(errBuf);
            }
            Serial.print("SSL version is ");
            Serial.println(wolfSSL_get_version(ssl));
            cipherName = wolfSSL_get_cipher(ssl);
            Serial.print("SSL cipher suite is ");
            Serial.println(cipherName);
            if ((wolfSSL_write(ssl, msg, msgSz)) == msgSz) {
                Serial.print("Server response: ");
                /* wait for data */
                while (!client.available()) {}
                /* read data */
                while (wolfSSL_pending(ssl)) {
                    Serial.println("Start wolfSSL_read");
                    input = wolfSSL_read(ssl, reply, sizeof(reply) - 1);
                    Serial.println("Finish wolfSSL_read");
                    total_input += input;
                    if (input < 0) {
                        err = wolfSSL_get_error(ssl, 0);
                        wolfSSL_ERR_error_string(err, errBuf);
                        Serial.print("TLS Read Error: ");
                        Serial.println(errBuf);
                        break;
                    }
                    else if (input > 0) {
                        reply[input] = '\0';
                        Serial.print(reply);
                    }
                    else {
                        Serial.println();
                    }
                }
            }
            else {
                err = wolfSSL_get_error(ssl, 0);
                wolfSSL_ERR_error_string(err, errBuf);
                Serial.print("TLS Write Error: ");
                Serial.println(errBuf);
            }
            wolfSSL_shutdown(ssl);
            wolfSSL_free(ssl);
            client.stop();
            Serial.println("Connection complete.");
            reconnect = 0;
        }
        else {
            Serial.println("Trying to reconnect...");
        }
    }
    delay(1000);
}
