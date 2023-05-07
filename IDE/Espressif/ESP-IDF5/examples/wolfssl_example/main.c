#include "user_settings.h"
#include <include/user_settings.h>
#include <stdio.h>
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
int main()
{
    printf("Hello, world!\n");

    WOLFSSL_METHOD* method;
    WOLFSSL_CTX* ctx;

    wolfSSL_Init();

    method = wolfTLSv1_2_client_method();
    ctx = wolfSSL_CTX_new(method);

    // ... use the SSL context ...

    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();

    return 0;
}
