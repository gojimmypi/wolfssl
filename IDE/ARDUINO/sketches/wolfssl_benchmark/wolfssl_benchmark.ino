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

/*
Tested with:

1) Intel Galileo acting as the Client, with a laptop acting as a server using
   the server example provided in examples/server.
   Legacy Arduino v1.86 was used to compile and program the Galileo

2) Espressif ESP32 WiFi
*/

/*
 * Note to code editors: the Arduino client and server examples are edited in
 * parallel for side-by-side comparison between examples.
 */

/* If you have a private include, define it here, otherwise edit WiFi params */
/* #define MY_PRIVATE_CONFIG "/workspace/my_private_config.h" */

/* set REPEAT_CONNECTION to a non-zero value to continually run the example. */
#define REPEAT_CONNECTION 1

/* Edit this with your other TLS host server address to connect to: */
// #define WOLFSSL_TLS_SERVER_HOST "192.168.1.38"

/* wolfssl TLS examples communciate on port 11111 */
#define WOLFSSL_PORT 11111

/* Choose a monitor serial baud rate: 9600, 14400, 19200, 57600, 74880, etc. */
#define SERIAL_BAUD 115200

/* We'll wait up to 2000 milliseconds to properly shut down connection */
#define SHUTDOWN_DELAY_MS 2000

/* Number of times to retry connection.  */
#define RECONNECT_ATTEMPTS 20

/* Optional stress test. Define to consume memory until exhausted: */
#define MEMORY_STRESS_TEST

/* Choose client or server example, not both. */
/* #define WOLFSSL_CLIENT_EXAMPLE */
#define WOLFSSL_SERVER_EXAMPLE

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

#define BROADCAST_ADDRESS "255.255.255.255"

/* There's an optional 3rd party NTPClient library by Fabrice Weinberg.
 * If it is installed, uncomment define USE_NTP_LIB here: */
/* #define USE_NTP_LIB */
// #define USE_NTP_LIB
#ifdef USE_NTP_LIB
    #include <NTPClient.h>
#endif

#include <wolfssl.h>
/* Important: make sure settings.h appears before any other wolfSSL headers */
#include <wolfssl/wolfcrypt/settings.h>
/* Reminder: settings.h includes user_settings.h
 * For ALL project wolfSSL settings, see:
 * [your path]/Arduino\libraries\wolfSSL\src\user_settings.h   */
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

// #include <Ethernet.h>

/* Define DEBUG_WOLFSSL in user_settings.h for more verbose logging. */
#if defined(DEBUG_WOLFSSL)
    #define PROGRESS_DOT F("")
#else
    #define PROGRESS_DOT F(".")
#endif

/* optional board-specific networking includes */
#if defined(ESP32)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    WiFiServer server(WOLFSSL_PORT);
    WiFiClient client;
    #ifdef USE_NTP_LIB
        WiFiUDP ntpUDP;
    #endif
#elif defined(ESP8266)
    #define USING_WIFI
    #include <ESP8266WiFi.h>
    WiFiServer server(WOLFSSL_PORT);
    WiFiClient client;
#elif defined(ARDUINO_SAM_DUE)
    EthernetClient server(WOLFSSL_PORT);
    EthernetClient client;
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    #define USING_WIFI
    #include <SPI.h>
    #include <WiFiNINA.h>
    WiFiServer server(WOLFSSL_PORT);
    WiFiClient client;
#elif defined(USING_WIFI)
    #define USING_WIFI
    #include <WiFi.h>
    #include <WiFiUdp.h>
    WiFiClient client;
    #ifdef USE_NTP_LIB
        WiFiUDP ntpUDP;
    #endif
/* TODO #elif defined(OTHER_BOARD) */
#else
    #define USING_WIFI
#endif


#if defined(HAVE_SNI)                           \
   || defined(WOLFSSL_LWIP) \
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

/* we expect our IP address from DHCP */
const int serial_baud = SERIAL_BAUD; /* local serial port to monitor */

WOLFSSL_CTX* ctx = NULL;
WOLFSSL* ssl = NULL;
char* wc_error_message = (char*)malloc(80 + 1);
char errBuf[80];

#if defined(MEMORY_STRESS_TEST)
    #define MEMORY_STRESS_ITERATIONS 100
    #define MEMORY_STRESS_BLOCK_SIZE 1024
    char* memory_stress[MEMORY_STRESS_ITERATIONS]; /* typically 1K per item */
    int mem_ctr        = 0;
#endif

static int EthernetSend(WOLFSSL* ssl, char* msg, int sz, void* ctx);
static int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx);
static int reconnect = RECONNECT_ATTEMPTS;
static int lng_index PROGMEM = 0; /* 0 = English */


#if defined(__arm__)
    #include <malloc.h>
    extern char _end;
    extern "C" char *sbrk(int i);
    char *ramstart=(char *)0x20070000;
    char *ramend=(char *)0x20088000;
#endif

/*****************************************************************************/
/* fail_wait - in case of unrecoverable error                                */
/*****************************************************************************/
static void fail_wait(void) {
    show_memory();

    Serial.println(F("Failed. Halt."));
    while (1) {
        delay(1000);
    }
}

/*****************************************************************************/
/* show_memory() to optionally view during debugging.                         */
/*****************************************************************************/
void show_memory(void)
{

#if defined(__arm__)
	struct mallinfo mi=mallinfo();
	char *heapend=sbrk(0);
	register char * stack_ptr asm("sp");
    #if defined(DEBUG_WOLFSSL_VERBOSE)
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
    #endif

    #if defined(DEBUG_WOLFSSL) || defined(MEMORY_STRESS_TEST)
        Serial.print("Estimated free memory: ");
        Serial.println(stack_ptr - heapend + mi.fordblks);
    #endif

    //	Serial.print("RAM Start %lx\n", (unsigned long)ramstart);
    //	Serial.print("Data/Bss end %lx\n", (unsigned long)&_end);
    //	Serial.print("Heap End %lx\n", (unsigned long)heapend);
    //	Serial.print("Stack Ptr %lx\n",(unsigned long)stack_ptr);
    //	Serial.print("RAM End %lx\n", (unsigned long)ramend);

    //	Serial.print("Heap RAM Used: ",mi.uordblks);
    //	Serial.print("Program RAM Used ",&_end - ramstart);
    //	Serial.print("Stack RAM Used ",ramend - stack_ptr);

    //	Serial.print("Estimated Free RAM: %d\n\n",stack_ptr - heapend + mi.fordblks);
#else
    Serial.println(F("show_memory() not implemented for this platform"));
#endif
}

/*****************************************************************************/
/* EthernetSend() to send a message string.                                  */
/*****************************************************************************/
int EthernetSend(WOLFSSL* ssl, char* message, int sz, void* ctx) {
    int sent = 0;
    (void)ssl;
    (void)ctx;

    sent = client.write((byte*)message, sz);
    return sent;
}

/*****************************************************************************/
/* EthernetReceive() to receive a reply string.                              */
/*****************************************************************************/
int EthernetReceive(WOLFSSL* ssl, char* reply, int sz, void* ctx) {
    int ret = 0;
    (void)ssl;
    (void)ctx;

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

#if defined(ARDUINO_SAMD_NANO_33_IOT)

#elif defined(__arm__)
    /* need to manually turn on random number generator on Arduino Due, etc. */
    Serial.println(F("Enabled ARM TRNG"));
    pmc_enable_periph_clk(ID_TRNG);
    trng_enable(TRNG);
#endif

    show_memory();
    randomSeed(analogRead(0));
    return ret;
}

/*****************************************************************************/
/* Arduino setup_datetime()                                                  */
/*   The device needs to have a valid date within the valid range of certs.  */
/*****************************************************************************/
static int setup_datetime(void) {
    int ret = 0;
    int ntp_tries = 20;

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
    configTime(0, 0, "pool.ntp.org");  // You can replace "pool.ntp.org" with your preferred NTP server

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
    #if defined(ESP8266) || defined(ESP32)
        WiFi.mode(WIFI_STA);
    #endif
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
    /* We'll assume the Ethernet connection is ready to go. */
#endif

    Serial.println(F("********************************************************"));
    Serial.print(F("      wolfSSL Example Server IP = "));
#if defined(USING_WIFI)
    Serial.println(WiFi.localIP());
#else
    Serial.println(Ethernet.localIP());
#endif
    /* In server mode, there's no host definition. See companion example. */
    /* wolfssl_client.ino */
    Serial.println(F("********************************************************"));

    /* Delay need to ensure connection to server */
    // delay(4000);
    Serial.println(F("Setup network complete."));

    return ret;
}

/*****************************************************************************/
/*****************************************************************************/
/* Arduino setup()                                                           */
/*****************************************************************************/
/*****************************************************************************/
void setup(void) {
    Serial.begin(SERIAL_BAUD);
    /* Pressing reset on som boards disconnects COM port.
     * Wait 5 seconds to allow reconnect after reset: */
    delay(5000);
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F("wolfSSL TLS Server Example Startup."));

    setup_hardware();

    setup_datetime();

    setup_network();

    wolfSSL_SetIOSend(ctx, EthernetSend);
    wolfSSL_SetIORecv(ctx, EthernetReceive);

#if defined THIS_USER_SETTINGS_VERSION
    Serial.print(F("This user_settings.h version:"))
    Serial.println(THIS_USER_SETTINGS_VERSION)
#endif

    /* Start the server
     * See https://www.arduino.cc/reference/en/libraries/ethernet/server.begin/
     */

    Serial.println(F("Completed Arduino setup()"));

    server.begin();
    Serial.println("Begin Server... (waiting for remote client to connect)");
    delay(1000);

    return;
}

/*****************************************************************************/
/* wolfSSL error_check()                                                     */
/*****************************************************************************/
static int error_check(int this_ret, bool halt_on_error,
                      const __FlashStringHelper* message) {
    int ret = 0;
    if (this_ret == WOLFSSL_SUCCESS) {
        Serial.print(F("Success: "));
        Serial.println(message);
    }
    else {
        Serial.print(F("ERROR: return = "));
        Serial.print(this_ret);
        Serial.print(F(": "));
        Serial.println(message);
        Serial.println(wc_GetErrorString(this_ret));
        if (halt_on_error) {
            fail_wait();
        }
    }
    show_memory();

    return ret;
}

/*****************************************************************************/
/*****************************************************************************/
/* Arduino loop()                                                            */
/*****************************************************************************/
/*****************************************************************************/
void loop() {

} /* Arduino loop repeats */
