
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char  byte;
typedef unsigned int   word32;

const byte good[] = "A+Gd\0\0\0";

int peek(const byte* in)
{
    printf("Start peek! \n");

    printf("Step 1 \n");
    printf("  good = 0x%08x \n", (unsigned int)good); /* ESP-IDF = 0x3f40f9c8 */

    printf("Step 2 \n");
    printf("   &in = 0x%08x \n", (unsigned int)&in);  /* ESP-IDF = 0x3ffc29cc */

    printf("Step 3 \n");
    printf("    in = 0x%08x \n", (unsigned int)in);   /* ESP-IDF = 0x3f40f9c8 */

    word32 j = *in;

    printf("Step 4 \n");
    printf("     j = 0x%08x \n", (unsigned int)j);    /* ESP-IDF = 0x00000041 */

    printf("Step 5 \n");
    printf("    &j = 0x%08x \n", (unsigned int)&j);   /* ESP-IDF = 0x3ffc29dc */
    return 0;
}

void main()
{
    printf("Here we go v1.0a Linux! \n");

    peek(good);

    printf("done! \n");

}




