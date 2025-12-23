/*
 * Test: #if with hex and octal literals
 */

/* Hexadecimal */
#if 0x10 == 16
int hex1 = 1;
#else
int hex1 = 0;
#endif

#if 0xFF == 255
int hex2 = 1;
#else
int hex2 = 0;
#endif

#if 0xABCD == 43981
int hex3 = 1;
#else
int hex3 = 0;
#endif

/* Octal */
#if 010 == 8
int oct1 = 1;
#else
int oct1 = 0;
#endif

#if 0777 == 511
int oct2 = 1;
#else
int oct2 = 0;
#endif

/* Character constants */
#if 'A' == 65
int char1 = 1;
#else
int char1 = 0;
#endif

#if '\n' == 10
int char2 = 1;
#else
int char2 = 0;
#endif

int main(void) {
    if (!hex1) return 1;
    if (!hex2) return 2;
    if (!hex3) return 3;
    if (!oct1) return 4;
    if (!oct2) return 5;
    if (!char1) return 6;
    if (!char2) return 7;
    return 0;
}
