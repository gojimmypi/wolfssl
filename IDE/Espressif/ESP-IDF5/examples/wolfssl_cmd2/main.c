#include <stdio.h>
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>

int main()
{
    printf("hello world\n");

    // Initialize wolfSSL library
    wolfSSL_Init();

    // Use wolfSSL library...
    // ...

    // Clean up wolfSSL library
    wolfSSL_Cleanup();

    return 0;
}
