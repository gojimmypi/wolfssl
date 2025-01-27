/* This sketch is based on code provided (by Moderator "Anthony") in response to my
post on the wolfSSL forum: https://www.wolfssl.com/forums/post8171.html#p8171 and
subsequent email discussion with Jim Scarletta at wolfSSL (jim@wolfssl.com)

25Jan[d] revised ExpectIntEQ(), removed obsolete code & comments
24Jan[c] successfullky compiled using version provided by Jim S. @ wolfSSL
*/

#define CJ_V ".d"

#include "Streaming.h"

// do we need this? I suspect it's only relevant for 'wolfssl_client' example provided with library
// probably need to update user_settings.h to allow other examples (eg. AES-CTR)
#define WOLFSSL_CLIENT_EXAMPLE

#include "wolfssl.h"
#include "wolfssl/wolfcrypt/aes.h" // was #include <wolfssl/wolfcrypt/aes.h>

#if defined(NO_AES) or !defined(WOLFSSL_AES_COUNTER) or !defined(WOLFSSL_AES_128)
#error "Missing AES, WOLFSSL_AES_COUNTER or WOLFSSL_AES_128"
#endif


// for ExpectIntEQ source: https://github.com/wolfSSL/wolfssl/blob/master/tests/unit.h
#define ExpectIntEQ(p1, p2) if (p1 == p2) Serial << "success\n"; else Serial << "failed\n";

static int test_wc_AesCtrEncryptDecrypt(void) {
  Aes aesEnc;
  Aes aesDec;
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

  ExpectIntEQ(wc_AesSetKey(&aesEnc, key16, AES_BLOCK_SIZE * 2, iv, AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesEnc, enc, vector, sizeof(vector)/sizeof(byte) ), 0);
  // Decrypt with wc_AesCtrEncrypt()
  ExpectIntEQ(wc_AesSetKey(&aesDec, key16, AES_BLOCK_SIZE * 2, iv, AES_ENCRYPTION), 0);
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, enc, sizeof(enc)/sizeof(byte)),0);
  ExpectIntEQ(XMEMCMP(vector, dec, sizeof(vector)), 0);
  // Test bad args
  ExpectIntEQ(wc_AesCtrEncrypt(    NULL, dec, enc, sizeof(enc)/sizeof(byte)), WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, NULL, enc, sizeof(enc)/sizeof(byte)), WC_NO_ERR_TRACE(BAD_FUNC_ARG));
  ExpectIntEQ(wc_AesCtrEncrypt(&aesDec, dec, NULL, sizeof(enc)/sizeof(byte)), WC_NO_ERR_TRACE(BAD_FUNC_ARG));

  wc_AesFree(&aesEnc);
  wc_AesFree(&aesDec);
  return 0;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000) ;                                          // wait for serial, up to 1 sec
  Serial << "======== AES_CTR_combined" << CJ_V << " ========\n";
  test_wc_AesCtrEncryptDecrypt();
  Serial << "------ setup() done ------\n";
}
;
uint32_t  loopCount = 0;

void loop() {
  loopCount++;
  Serial << "loop: " << loopCount << '\n';
  delay(10000); // 10 secs
}
