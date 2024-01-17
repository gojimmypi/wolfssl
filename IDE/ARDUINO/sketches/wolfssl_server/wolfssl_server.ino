/* wolfssl_server.ino
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

#include <wolfssl.h>
#include <wolfssl/ssl.h>
#include <Ethernet.h>

#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#ifdef NO_WOLFSSL_SERVER
    #error Please undefine NO_WOLFSSL_SERVER for this example
#endif

#define WOLFSSL_PORT 11111
/* optional board-specific networking includes */
#if defined(ESP32)
    #define USING_WIFI
    #include <WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>
    WiFiServer server(WOLFSSL_PORT);
    WiFiClient client;
#elif defined(ESP8266)
    #define USING_WIFI
    #include <ESP8266WiFi.h>
    #include <NTPClient.h>
    #include <WiFiUdp.h>
    WiFiServer server(WOLFSSL_PORT);
    WiFiClient client;
/* #elif defined(OTHER_BOARD) */
    /* TODO other boards here */
#else
    EthernetServer server(WOLFSSL_PORT);
    EthernetClient client;
#endif

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

/* we expect our IP address from DHCP */
const int serial_baud = 115200; /* local serial port to monitor */

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);

/*****************************************************************************/
/* Arduino setup()
/*****************************************************************************/
void setup(void) {
    WOLFSSL_METHOD* method;
    int err;

    Serial.begin(serial_baud);

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "pool.ntp.org");

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
        Serial.print("Server IP = ");
        Serial.println(WiFi.localIP());
        
        timeClient.begin();
        delay(2000);
        timeClient.update();
        Serial.println(timeClient.getFormattedTime());
    #else
        /* We'll assume the Ethernet connection is ready to go.*/
    #endif

    /* Delay need to ensure connection to server */
    delay(4000);

  method = wolfTLSv1_2_server_method();
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
  wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
  wolfSSL_SetIOSend(ctx, EthernetSend);
  wolfSSL_SetIORecv(ctx, EthernetReceive);

  /* setup the private key and certificate */
  err = wolfSSL_CTX_use_PrivateKey_buffer(ctx, ecc_key_der_256,
    sizeof_ecc_key_der_256, WOLFSSL_FILETYPE_ASN1);
  if (err != WOLFSSL_SUCCESS) {
    Serial.println("error setting key");
    return;
  }
  err = wolfSSL_CTX_use_certificate_buffer(ctx, serv_ecc_der_256,
    sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1);
  if (err != WOLFSSL_SUCCESS) {
    Serial.println("error setting certificate");
    return;
  }

  /* Start the server */
  server.begin();

  return;
}

/*****************************************************************************/
/* EthernetSend() to send a message string. See Arduino loop()
/*****************************************************************************/
int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx) {
    int sent = 0;
    sent = client.write((byte*)msg, sz);
    return sent;
}

/*****************************************************************************/
/* EthernetReceive() to receive a reply string. See Arduino loop()
/*****************************************************************************/
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx) {
    int ret = 0;
    while (client.available() > 0 && ret < sz) {
        reply[ret++] = client.read();
    }
    return ret;
}

/*****************************************************************************/
/* Arduino loop()
/*****************************************************************************/
void loop() {
    int err            = 0;
    int input          = 0;
    char errBuf[80]    = "(no error";
    char reply[80]     = "(no reply)";
    int replySz = 0;
    const char* cipherName;

    /* Listen for incoming client requests. */
    client = server.available();
    if (!client) {
      return;
    }

  if (client.connected()) {

    Serial.println("Client connected");

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
      Serial.println("Unable to allocate SSL object");
      return;
    }

    err = wolfSSL_accept(ssl);
    if (err != WOLFSSL_SUCCESS) {
      err = wolfSSL_get_error(ssl, 0);
      wolfSSL_ERR_error_string(err, errBuf);
      Serial.print("TLS Accept Error: ");
      Serial.println(errBuf);
    }

    Serial.print("SSL version is ");
    Serial.println(wolfSSL_get_version(ssl));

    cipherName = wolfSSL_get_cipher(ssl);
    Serial.print("SSL cipher suite is ");
    Serial.println(cipherName);

    Serial.print("Server Read: ");
    /* wait for data */
    while (!client.available()) {}
    /* read data */
    while (wolfSSL_pending(ssl)) {
      input = wolfSSL_read(ssl, reply, sizeof(reply) - 1);
      if (input < 0) {
        err = wolfSSL_get_error(ssl, 0);
        wolfSSL_ERR_error_string(err, errBuf);
        Serial.print("TLS Read Error: ");
        Serial.println(errBuf);
        break;
      } else if (input > 0) {
        replySz = input;
        reply[input] = '\0';
        Serial.print(reply);
      } else {
        Serial.println();
      }
    }

    /* echo data */
    if ((wolfSSL_write(ssl, reply, replySz)) != replySz) {
      err = wolfSSL_get_error(ssl, 0);
      wolfSSL_ERR_error_string(err, errBuf);
      Serial.print("TLS Write Error: ");
      Serial.println(errBuf);
    }

    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
  }

  client.stop();
  Serial.println("Connection complete");
}
