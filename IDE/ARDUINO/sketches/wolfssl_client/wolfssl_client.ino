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
#define MY_PRIVATE_CONFIG "/workspace/my_private_config.h"

/* Edit this with your other TLS host server address to connect to: */
#define EXAMPLE_HOST "192.168.1.38"

/* wolfssl TLS examples communciate on port 11111 */
#define WOLFSSL_PORT 11111

/* We'll wait up to 2000 milliseconds to properly shut down connection */
#define SHUTDOWN_DELAY_MS 2000

/* Number of times to retry. Set to zero for single test, no retry */
#define RECONNECT_ATTEMPTS 0

#include <wolfssl.h>
/* Important: make sure settings.h appears before any other wolfSSL headers */
#include <wolfssl/wolfcrypt/settings.h>
/* Reminder: settings.h includes user_settings.h
 * For ALL project wolfSSL settings, see:
 * [your path]/Arduino\libraries\wolfSSL\src\user_settings.h   */
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <Ethernet.h>

/* There's an optional 3rd party NTPClient library by Fabrice Weinberg.
 * If it is installed, uncomment define USE_NTP_LIB here: */
/* #define USE_NTP_LIB */
#define USE_NTP_LIB
#ifdef USE_NTP_LIB
    #include <NTPClient.h>
#endif

/* optional board-specific networking includes */
#if defined(ESP32)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    WiFiClient client;
    #ifdef USE_NTP_LIB
        WiFiUDP ntpUDP;
    #endif
#elif defined(ESP8266)
    #define USING_WIFI
    #include <ESP8266WiFi.h>
    WiFiClient client;
/* #elif defined(OTHER_BOARD) */
/* TODO define other boards here */
#else
    EthernetClient client;
#endif

#if defined(MY_PRIVATE_CONFIG)
    /* the /workspace directory may contain a private config
     * excluded from GitHub with items such as WiFi passwords */
    #include MY_PRIVATE_CONFIG
    const char* ssid PROGMEM = CONFIG_ESP_WIFI_SSID;
    const char* password PROGMEM  = CONFIG_ESP_WIFI_PASSWORD;
#else
    /* when using WiFi capable boards: */
    const char* ssid PROGMEM  = "your_SSID";
    const char* password PROGMEM = "your_PASSWORD";
#endif


#if defined(HAVE_SNI)                           \
   || defined(HAVE_MAX_FRAGMENT)                  \
   || defined(HAVE_TRUSTED_CA)                    \
   || defined(HAVE_TRUNCATED_HMAC)                \
   || defined(HAVE_CERTIFICATE_STATUS_REQUEST)    \
   || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2) \
   || defined(HAVE_SUPPORTED_CURVES)              \
   || defined(HAVE_ALPN)                          \
   || defined(HAVE_SESSION_TICKET)                \
   || defined(HAVE_SECURE_RENEGOTIATION)          \
   || defined(HAVE_SERVER_RENEGOTIATION_INFO)
#endif

const char host[] PROGMEM = EXAMPLE_HOST; /* server to connect to */
const int port PROGMEM = WOLFSSL_PORT; /* port on server to connect to */
const int serial_baud PROGMEM = 115200; /* local serial port to monitor */

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;
char* wc_error_message = (char*)malloc(80 + 1);

int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);
int reconnect = 10;
static int lng_index PROGMEM = 0; /* 0 = English */


/* fail_wait - in case of unrecoverable error */
static void fail_wait(void) {
    Serial.println(F("Failed. Halt."));
    while(1) {
        delay(1000);
    }
}


extern char _end;
extern "C" char *sbrk(int i);
char *ramstart=(char *)0x20070000;
char *ramend=(char *)0x20088000;

#include <malloc.h>
extern "C" char *sbrk(int i);

void ShowMemory(void)
{
	struct mallinfo mi=mallinfo();

	char *heapend=sbrk(0);
	register char * stack_ptr asm("sp");

	Serial.print("    arena=");
  Serial.println(mi.arena);
	Serial.print("  ordblks=");
  Serial.println(mi.ordblks);
	Serial.print(" uordblks=");
  Serial.println(mi.uordblks);
	Serial.print(" fordblks=");
  Serial.println(mi.fordblks);
	Serial.print(" keepcost=");
  Serial.println(mi.keepcost);
	Serial.print("My guess at free mem: ");
  Serial.println(stack_ptr - heapend + mi.fordblks);
//	Serial.print("RAM Start %lx\n", (unsigned long)ramstart);
//	Serial.print("Data/Bss end %lx\n", (unsigned long)&_end);
//	Serial.print("Heap End %lx\n", (unsigned long)heapend);
//	Serial.print("Stack Ptr %lx\n",(unsigned long)stack_ptr);
//	Serial.print("RAM End %lx\n", (unsigned long)ramend);

//	Serial.print("Heap RAM Used: ",mi.uordblks);
//	Serial.print("Program RAM Used ",&_end - ramstart);
//	Serial.print("Stack RAM Used ",ramend - stack_ptr);

//	Serial.print("Estimated Free RAM: %d\n\n",stack_ptr - heapend + mi.fordblks);
}

/*****************************************************************************/
/* EthernetSend() to send a message string.                                  */
/*****************************************************************************/
int EthernetSend(WOLFSSL* ssl, char* message, int sz, void* ctx) {
    int sent = 0;
    sent = client.write((byte*)message, sz);
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
/* Arduino setup_hardware()                                                  */
/*****************************************************************************/
static int setup_hardware(void) {
    int ret = 0;
    pmc_enable_periph_clk(ID_TRNG);
    trng_enable(TRNG);

    ShowMemory();
    randomSeed(analogRead(0));
    return ret;
}

static int setup_datetime(void) {
    int ret = 0;
    /* we need a date in the range of cert expiration */
#ifdef USE_NTP_LIB
    #if defined(ESP32)
        NTPClient timeClient(ntpUDP, "pool.ntp.org");

    timeClient.begin();
    timeClient.update();
    delay(1000);
    while (!timeClient.isTimeSet() && (ntp_tries > 0)) {
        timeClient.forceUpdate();
        Serial.println(F("Waiting for NTP update"));
        delay(2000);
        ntp_tries--;
    }
    if (ntp_tries <= 0) {
        Serial.println(F("Warning: gave up waiting on NTP"));
    }
    Serial.println(timeClient.getFormattedTime());
    Serial.println(timeClient.getEpochTime());
    #endif
#endif

#if defined(ESP32)
    /* see esp32-hal-time.c */
    ntp_tries = 5;
    configTime(0, 0, "pool.ntp.org"));  // You can replace "pool.ntp.org" with your preferred NTP server

    /* Wait for time to be set */
    while ((time(nullptr) <= 100000) && ntp_tries > 0) {
        Serial.println(F("Waiting for time to be set..."));
        delay(2000);
        ntp_tries--;
    }
#endif

    return ret;
}
/*****************************************************************************/
/* Arduino setup_network()                                                   */
/*****************************************************************************/
static int setup_network(void) {
    int ret = 0;
#if defined(USING_WIFI)
    /* Connect to WiFi */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(F("Connecting to WiFi "));
        Serial.println(ssid);
    }

    Serial.print(F("Connected to WiFi "));
    Serial.println(ssid);
#else
    /* Newer Ethernet shields have a
     * MAC address printed on a sticker on the shield */
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(192, 168, 1, 42);
    IPAddress myDns(192, 168, 1, 1);
    Ethernet.init(10); /* Most Arduino shields */
    /*Ethernet.init(5);   // MKR ETH Shield */
    /*Ethernet.init(0);   // Teensy 2.0 */
    /*Ethernet.init(20);  // Teensy++ 2.0 */
    /*Ethernet.init(15);  // ESP8266 with Adafruit FeatherWing Ethernet */
    /*Ethernet.init(33);  // ESP32 with Adafruit FeatherWing Ethernet */
    Serial.println(F("Initialize Ethernet with DHCP:"));
    if (Ethernet.begin(mac) == 0) {
        Serial.println(F("Failed to configure Ethernet using DHCP"));
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            Serial.println(F("Ethernet shield was not found."));
            while (true) {
                delay(1); /* do nothing */
            }
        }
        if (Ethernet.linkStatus() == LinkOFF) {
            Serial.println(F("Ethernet cable is not connected."));
        }
        /* try to configure using IP address instead of DHCP : */
        Ethernet.begin(mac, ip, myDns);
    }
    else {
        Serial.print(F("  DHCP assigned IP "));
        Serial.println(Ethernet.localIP());
    }
    /* We'll assume the Ethernet connection is ready to go.*/
#endif
    Serial.println(F("********************************************************"));
    Serial.print(F("      wolfSSL Example Client IP = "));
#if defined(USING_WIFI)
    Serial.println(WiFi.localIP());
#else
    Serial.println(Ethernet.localIP());
#endif
    Serial.print(F("   Configured Server Host to connect to: "));
    Serial.println(host);
    Serial.println(F("********************************************************"));
    return ret;
}

/*****************************************************************************/
/* Arduino setup_wolfssl()                                                   */
/*****************************************************************************/
static int setup_wolfssl(void) {
    int ret = 0;
    WOLFSSL_METHOD* method;

    method = wolfSSLv23_client_method();
    if (method == NULL) {
        Serial.println(F("unable to get method"));
        ret = -1;
    }
    ctx = wolfSSL_CTX_new(method);
    if (ctx == NULL) {
        Serial.println(F("unable to get ctx"));
        ret = -1;
    }

    return ret;
}

/*****************************************************************************/
/* Arduino setup_certificates()                                              */
/*****************************************************************************/
static int setup_certificates(void) {
    int ret = 0;

    Serial.println(F("Initializing certificates..."));
    ShowMemory();
    /* initialize wolfSSL using callback functions */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);

    ret = wolfSSL_CTX_load_verify_buffer(ctx,
                                         ca_cert_der_2048,
                                         sizeof_ca_cert_der_2048,
                                         WOLFSSL_FILETYPE_ASN1);
    if (ret == WOLFSSL_SUCCESS) {
        Serial.println(F("Success: load_verify ca_cert_der_2048"));
    }
    else {
        Serial.println(F("Error: wolfSSL_CTX_load_verify_buffer failed: "));
        wc_ErrorString(ret, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Certificate */
    ret = wolfSSL_CTX_use_certificate_buffer(ctx,
                                             client_cert_der_2048,
                                             sizeof_client_cert_der_2048,
                                             WOLFSSL_FILETYPE_ASN1);
    if (ret == WOLFSSL_SUCCESS) {
        Serial.println(F("Success: use certificate ca_cert_der_2048"));
    }
    else {
        Serial.println(F("Error: wolfSSL_CTX_use_certificate_buffer failed: "));
        wc_ErrorString(ret, wc_error_message);
        Serial.println(wc_error_message);
    }

    /* Private Key */
    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx,
                                            client_key_der_2048,
                                            sizeof_client_key_der_2048,
                                            WOLFSSL_FILETYPE_ASN1);
    if (ret == WOLFSSL_SUCCESS) {
        Serial.println(F("Success: use private key buffer ca_cert_der_2048."));
    }
    else {
        Serial.println(F("Error: wolfSSL_CTX_use_PrivateKey_buffer failed: "));
        wc_ErrorString(ret, wc_error_message);
        Serial.println(wc_error_message);
    }

    return ret;
}

/*****************************************************************************/
/*****************************************************************************/
/* Arduino setup()                                                           */
/*****************************************************************************/
/*****************************************************************************/
void setup(void) {
    Serial.begin(serial_baud);
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F("wolfSSL TLS Client Startup."));

    setup_hardware();

    setup_datetime();

    setup_network();

    int ntp_tries = 20;






    /* Delay need to ensure connection to server */
    delay(4000);
    Serial.println(F("Here we go!"));
    setup_wolfssl();

    setup_certificates();

    wolfSSL_SetIOSend(ctx, EthernetSend);
    wolfSSL_SetIORecv(ctx, EthernetReceive);

    return;
}

/*****************************************************************************/
/*****************************************************************************/
/* Arduino loop()                                                            */
/*****************************************************************************/
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
// wolfSSL_Debugging_ON();
    if (reconnect) {
        reconnect--;
        if (client.connect(host, port)) {
            Serial.print(F("Connected to server at "));
            Serial.print(host);
            Serial.print(F(":"));
            Serial.println(port);

            ret = wolfSSL_Init();
            if (ret == WOLFSSL_SUCCESS) {
                Serial.println(F("Successfully called wolfSSL_Init"));
            }
            else {
                Serial.println(F("ERROR: wolfSSL_Init failed"));
            }
ShowMemory();
            ssl = wolfSSL_new(ctx);
            if (ssl == NULL) {
                Serial.println(F("Unable to allocate SSL object"));
ShowMemory();
                fail_wait();
            }
            else {
                Serial.println(F("Successfully created ssl object"));
            }
ShowMemory();
            Serial.println(F("Connecting to wolfSSL server...."));
            do {
                err = 0; /* reset error */
                ret = wolfSSL_connect(ssl);
                if ((ret != WOLFSSL_SUCCESS) && (ret != WC_PENDING_E)) {
                    err = wolfSSL_get_error(ssl, 0);
                    wolfSSL_ERR_error_string(ret, errBuf);
                    Serial.print(F("TLS Connect Error: "));
                    Serial.println(errBuf);
                }
            } while (err == WC_PENDING_E);
ShowMemory();
            Serial.print(F("SSL version is "));
            Serial.println(wolfSSL_get_version(ssl));

            cipherName = wolfSSL_get_cipher(ssl);
            Serial.print(F("SSL cipher suite is "));
            Serial.println(cipherName);

            /* see test.h
             * TODO: test.h needs a little bit of Arduino work for these:
            showPeerEx(ssl, lng_index);
            showPeerPEM(ssl);
            */

            Serial.print(F("Sending secure message to server: "));
            Serial.println(msg);
            if ((wolfSSL_write(ssl, msg, msgSz)) == msgSz) {
                Serial.println(F("Waiting for Server response..."));
                while (!client.available()) {
                    /* wait for  data */
                }

                /* read data */
                do {
                    ret = wolfSSL_read(ssl, reply, sizeof(reply)-1);
                    if (ret < 0) {
                        err = wolfSSL_get_error(ssl, 0);
                        wolfSSL_ERR_error_string(err, errBuf);
                        Serial.print(F("ERROR during TLS Read: "));
                        Serial.println(errBuf);                    }
                } while (err == WC_PENDING_E);
                Serial.println(reply);

            } /* wolfSSL_write message size matched */
            else {
                err = wolfSSL_get_error(ssl, 0);
                wolfSSL_ERR_error_string(err, errBuf);
                Serial.print(F("ERROR: during TLS Write "));
                Serial.println(errBuf);
            }  /* any wolfSSL_write message size mismatch is an error */

            Serial.println(F("Shutdown!"));
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
                Serial.println(F("Warning! Shutdown did not properly complete."));
            }

            wolfSSL_free(ssl);
            client.stop();
            Serial.println(F("Connection complete."));
            reconnect = RECONNECT_ATTEMPTS; /* non-zero to repeat */
        } /* client.connect(host, port) */
        else {
            Serial.println(F("Problem sending message. Trying to reconnect..."));
        }
    }
    delay(1000);
    if (reconnect > 0) {
        Serial.println(F("Arduino loop repeating..."));
        Serial.println();
    }
    else {
        Serial.println(F("Done!"));
        while(1) {
          /* wait forever */
        }
    }
} /* Arduino loop repeats */
