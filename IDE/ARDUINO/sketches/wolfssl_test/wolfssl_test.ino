#include <Arduino.h>
#include <wolfssl.h>
#include <wolfssl/version.h>

void setup() {
  printf("wolfSSL Version: %s", LIBWOLFSSL_VERSION_STRING);
}

void loop() {
  delay(1000);
}
