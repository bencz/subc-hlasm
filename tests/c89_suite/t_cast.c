/*
 * Test: Type casting
 * Tests: int/char casts, pointer casts
 */
#include <stdio.h>

int main(void) {
    int i;
    char c;
    int *pi;
    char *pc;
    int passed;

    passed = 1;

    /* Int to char */
    i = 65;
    c = (char)i;
    if (c != 'A') {
        printf("FAIL: (char)65 = %c (expected A)\n", c);
        passed = 0;
    }

    /* Char to int */
    c = 'Z';
    i = (int)c;
    if (i != 90) {
        printf("FAIL: (int)'Z' = %d (expected 90)\n", i);
        passed = 0;
    }

    /* Truncation */
    i = 256 + 65;
    c = (char)i;
    if (c != 'A') {
        printf("FAIL: (char)321 = %d (expected 65)\n", c);
        passed = 0;
    }

    /* Pointer cast */
    i = 0x12345678;
    pi = &i;
    pc = (char *)pi;
    /* Just verify we can do the cast */
    if (pc == 0) {
        printf("FAIL: pointer cast resulted in NULL\n");
        passed = 0;
    }

    /* Cast in expression */
    i = 7;
    c = 3;
    if ((int)c + i != 10) {
        printf("FAIL: (int)c + i = %d (expected 10)\n", (int)c + i);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_cast\n");
    }
    return passed ? 0 : 1;
}
