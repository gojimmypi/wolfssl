/* This sketch is based on code provided (by Moderator "Anthony") in response to 
my post on the wolfSSL forum: https://www.wolfssl.com/forums/post8171.html#p8171
*/

#if defined(WOLFSSL_CJ_TEST_A) 
#warning "WOLFSSL_CJ_TEST_A is defined (sketch - 1)"
#endif

#if defined(WOLFSSL_CJ_TEST_B) 
#warning "WOLFSSL_CJ_TEST_B is defined (sketch - 2)"
#endif

#define WOLFSSL_CLIENT_EXAMPLE

#if defined(NO_AES) 
#error "Missing AES"
#endif
/*
#if !defined(WOLFSSL_AES_COUNTER) 
#error "Missing AES-CTR"
#endif

#if !defined(WOLFSSL_AES_128)
#error "Missing AES_128"
#endif
*/
#include "Streaming.h"
//#include "AES.h"
// includes per wolfssl_client.ino example
#include "wolfssl.h"

/*
// Important: make sure settings.h appears before any other wolfSSL headers 
#include <wolfssl/wolfcrypt/settings.h> // Reminder: settings.h includes user_settings.h 
                                        // For ALL settings, see ~\Arduino\libraries\wolfSSL\src\user_settings.h
#include <wolfssl/ssl.h>
#include <wolfssl/certs_test.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>

//extern int test_wc_AesCtrEncryptDecrypt(void);

//#include "ZZ.h"

//char *sketch = truncateString(FILENAME,4);
*/

static int test_wc_AesCtrEncryptDecrypt(void) {
  //EXPECT_DECLS;
  Aes aesEnc;
  Aes aesDec;
  /*
  byte key32[] = {
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
      0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
      0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
      0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66};

  byte vector[] = { 
      0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
      0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
      0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20 };
    // 'Now is the time for all' (without null terminator)
  byte iv[] = "1234567890abcdef";
  */

  // Key 128 bits = 16 bytes
  byte key16[] = {0x55, 0x24, 0xcc, 0x11, 0xd9, 0x51, 0xb6, 0x37,
                  0x96, 0xd0, 0x5a, 0x9a, 0xcc, 0xe7, 0x31, 0x7 }; 

  // encrypted data [from "RAW fiddle (c).txt"]
  byte vector[] = {0xF8, 0x74, 0x60, 0x1C, 0x3F, 0x86, 0x9B, 0x6A,
                   0x19, 0x62, 0x58, 0xEB, 0x22, 0xED, 0xD3, 0x14};

  // initialisation vector [from "RAW fiddle (c).txt"] Also called or "salt" or "Nonce/Counter" 
  byte iv[] = {0x5D, 0x97};
  
  byte enc[AES_BLOCK_SIZE * 2];
  byte dec[AES_BLOCK_SIZE * 2];

  // Init stack variables
  XMEMSET(&aesEnc, 0, sizeof(Aes));
  XMEMSET(&aesDec, 0, sizeof(Aes));
  XMEMSET(enc, 0, AES_BLOCK_SIZE * 2);
  XMEMSET(dec, 0, AES_BLOCK_SIZE * 2);

  ExpectIntEQ(wc_AesInit(&aesEnc, NULL, INVALID_DEVID), 0);
  ExpectIntEQ(wc_AesInit(&aesDec, NULL, INVALID_DEVID), 0);

  ExpectIntEQ(wc_AesSetKey(&aesEnc, key16, AES_BLOCK_SIZE * 2, iv,
      AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesEnc, enc, vector,
      sizeof(vector)/sizeof(byte)), 0);
  // Decrypt with wc_AesCtrEncrypt()
  ExpectIntEQ(wc_AesSetKey(&aesDec, key16, AES_BLOCK_SIZE * 2, iv,
      AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, enc, sizeof(enc)/sizeof(byte)),
      0);
  ExpectIntEQ(XMEMCMP(vector, dec, sizeof(vector)), 0);

  // Test bad args
  ExpectIntEQ(wc_AesCtrEncrypt(NULL, dec, enc, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, NULL, enc, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, NULL, sizeof(enc)/sizeof(byte)),
      WC_NO_ERR_TRACE(BAD_FUNC_ARG));

  wc_AesFree(&aesEnc);
  wc_AesFree(&aesDec);
  //return EXPECT_RESULT();
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000) ;                                          // wait for serial, up to 1 sec
  Serial << "======== AES-CTR_combined.b ========\n";   
  test_wc_AesCtrEncryptDecrypt();
  Serial << "------ setup() done ------\n";   
} 

void loop() {}

/*
int      delaySecs = 3;
uint32_t loopCount = 0;

void loop() {
  loopCount++; 
  Serial << "loop: " << loopCount << '\n';
  delay(delaySecs*1000);
}
*/