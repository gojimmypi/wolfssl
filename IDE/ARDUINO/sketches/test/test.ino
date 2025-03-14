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

/* If you have a private include, define it here, otherwise edit WiFi params */
/* #define MY_PRIVATE_CONFIG "/workspace/my_private_config.h" */

/* Edit this with your other TLS host server address to connect to: */
#define EXAMPLE_HOST "192.168.1.38"

/* wolfssl TLS examples communciate on port 11111 */
#define WOLFSSL_PORT 11111

/* We'll wait up to 2000 milliseconds to properly shut down connection */
#define SHUTDOWN_DELAY_MS 2000

#include <wolfssl.h>
/* Important: make sure settings.h appears before any other wolfSSL headers */
#include <wolfssl/wolfcrypt/settings.h>
/* Reminder: settings.h includes user_settings.h
 * For ALL project wolfSSL settings, see:
 * [your path]/Arduino\libraries\wolfSSL\src\user_settings.h   */
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>


/* optional board-specific networking includes */
#if defined(ESP32)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    #ifdef USE_NTP_LIB
        WiFiUDP ntpUDP;
    #endif
    /* Ensure the F() flash macro is defined */
    #ifndef F
        #define F
    #endif
    WiFiClient client;

#elif defined(ESP8266)
    #define USING_WIFI
    #include <ESP8266WiFi.h>
    WiFiClient client;

#elif defined(ARDUINO_SAM_DUE)
    #include <SPI.h>
    /* There's no WiFi/Ethernet on the Due. Requires Ethernet Shield.
    /* Needs "Ethernet by Various" library to be installed. Tested with V2.0.2 */
    #include <Ethernet.h>
    EthernetClient client;

#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    #define USING_WIFI
    #include <SPI.h>
    #include <WiFiNINA.h> /* Needs Arduino WiFiNINA library installed manually */
    WiFiClient client;

#elif defined(ARDUINO_ARCH_RP2040)
    #define USING_WIFI
    #include <SPI.h>
    #include <WiFiNINA.h>
    WiFiClient client;

#elif defined(USING_WIFI)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    #ifdef USE_NTP_LIB
        WiFiUDP ntpUDP;
    #endif
    WiFiClient client;

/* TODO
#elif defined(OTHER_BOARD)
*/
#else
    #define USING_WIFI
    WiFiClient client;

#endif
#if defined(MY_PRIVATE_CONFIG)
    /* the /workspace directory may contain a private config
     * excluded from GitHub with items such as WiFi passwords */
#include MY_PRIVATE_CONFIG
    static const char ssid[]     PROGMEM = MY_ARDUINO_WIFI_SSID;
    static const char password[] PROGMEM = MY_ARDUINO_WIFI_PASSWORD;
#else
    /* when using WiFi capable boards: */
    static const char ssid[]     PROGMEM = "your_SSID";
    static const char password[] PROGMEM = "your_PASSWORD";
#endif

const char host[] = EXAMPLE_HOST; /* server to connect to */
const int port = WOLFSSL_PORT; /* port on server to connect to */
const int serial_baud = 115200; /* local serial port to monitor */

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);
int reconnect = 10;
static int lng_index = 0; /* 0 = English */

/* fail_wait - in case of unrecoverable error */
void fail_wait(void) {
    Serial.println("Failed.");
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
    Serial.print("      wolfSSL Example Client IP = ");
    Serial.println(WiFi.localIP());
    Serial.print("   Configured Server Host to connect to: ");
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

    Serial.println("Initializing certificates...");

    /* initialize wolfSSL using callback functions */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    rc = wolfSSL_CTX_load_verify_buffer(ctx,
                                        ca_cert_der_2048,
                                        sizeof_ca_cert_der_2048,
                                        WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.println("Success: load_verify ca_cert_der_2048");
    }
    else {
        Serial.println("Error: wolfSSL_CTX_load_verify_buffer failed: ");
        wc_ErrorString(rc, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Certificate */
    rc = wolfSSL_CTX_use_certificate_buffer(ctx,
                                            client_cert_der_2048,
                                            sizeof_client_cert_der_2048,
                                            WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.println("Success: use certificate ca_cert_der_2048");
    }
    else {
        Serial.println("Error: wolfSSL_CTX_use_certificate_buffer failed: ");
        wc_ErrorString(rc, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Private Key */
    rc = wolfSSL_CTX_use_PrivateKey_buffer(ctx,
                                           client_key_der_2048,
                                           sizeof_client_key_der_2048,
                                           WOLFSSL_FILETYPE_ASN1);
    if (rc == WOLFSSL_SUCCESS) {
        Serial.println("Success: use private key buffer ca_cert_der_2048.");
    }
    else {
        Serial.println("Error: wolfSSL_CTX_use_PrivateKey_buffer failed: ");
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
    char errBuf[80];
    char reply[80];
    char msg[32]       = "hello wolfssl!";
    const char* cipherName;
    int retry_shutdown = SHUTDOWN_DELAY_MS; /* max try, once per millisecond */
    int total_input    = 0;
    int msgSz          = 0;
    int input          = 0;
    int ret            = 0;
    int err            = 0;
    msgSz = (int)strlen(msg);

    if (reconnect) {
        reconnect--;
        if (client.connect(host, port)) {
            Serial.print("Connected to server at ");
            Serial.print(host);
            Serial.print(":");
            Serial.println(port);

            ret = wolfSSL_Init();
            if (ret == WOLFSSL_SUCCESS) {
                Serial.println("Successfully called wolfSSL_Init");
            }
            else {
                Serial.println("ERROR: wolfSSL_Init failed");
            }

            ssl = wolfSSL_new(ctx);
            if (ssl == NULL) {
                Serial.println("Unable to allocate SSL object");
                fail_wait();
            }
            else {
                Serial.println("Successfully created ssl object");
            }

            Serial.println("Connecting to wolfSSL server....");
            do {
                err = 0; /* reset error */
                ret = wolfSSL_connect(ssl);
                if ((ret != WOLFSSL_SUCCESS) && (ret != WC_PENDING_E)) {
                    err = wolfSSL_get_error(ssl, 0);
                    wolfSSL_ERR_error_string(ret, errBuf);
                    Serial.print("TLS Connect Error: ");
                    Serial.println(errBuf);
                }
            } while (err == WC_PENDING_E);

            Serial.print("SSL version is ");
            Serial.println(wolfSSL_get_version(ssl));

            cipherName = wolfSSL_get_cipher(ssl);
            Serial.print("SSL cipher suite is ");
            Serial.println(cipherName);

            /* see test.h
             * TODO: test.h needs a little bit of Arduino work for these:
            showPeerEx(ssl, lng_index);
            showPeerPEM(ssl);
            */

            Serial.print("Sending secure message to server: ");
            Serial.println(msg);
            if ((wolfSSL_write(ssl, msg, msgSz)) == msgSz) {
                Serial.println("Waiting for Server response...");
                while (!client.available()) {
                    /* wait for  data */
                }

                /* read data */
                do {
                    ret = wolfSSL_read(ssl, reply, sizeof(reply)-1);
                    if (ret < 0) {
                        err = wolfSSL_get_error(ssl, 0);
                        wolfSSL_ERR_error_string(err, errBuf);
                        Serial.print("ERROR during TLS Read: ");
                        Serial.println(errBuf);                    }
                } while (err == WC_PENDING_E);
                Serial.println(reply);

            } /* wolfSSL_write message size matched */
            else {
                err = wolfSSL_get_error(ssl, 0);
                wolfSSL_ERR_error_string(err, errBuf);
                Serial.print("ERROR: during TLS Write ");
                Serial.println(errBuf);
            }  /* any wolfSSL_write message size mismatch is an error */

            Serial.println("Shutdown!");
            do {
                delay(1);
                retry_shutdown--;
                ret = wolfSSL_shutdown(ssl);
            } while (   (ret == WOLFSSL_SHUTDOWN_NOT_DONE)
                     && (retry_shutdown > 0)
                    ); /* There may be pending data, so wait. */

            if (retry_shutdown <= 0) {
                /* if wolfSSL_free is called before properly shutting down the
                 * ssl object, undesired results may occur. */
                Serial.println("Warning! Shutdown did not properly complete.");
            }

            wolfSSL_free(ssl);
            client.stop();
            Serial.println("Connection complete.");
            reconnect = 10; /* to repeatedly test, reset this to non-zero */
        } /* client.connect(host, port) */
        else {
            Serial.println("Problem sending message. Trying to reconnect...");
        }
    }
    delay(1000);
    if (reconnect > 0) {
        Serial.println("Arduino loop repeating...");
        Serial.println();
    }
    else {
        Serial.println("Done!");
    }
} /* Arduino loop repeats */
