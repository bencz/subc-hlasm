/*
 * Test: Character types
 * Tests: char, char arrays, string literals
 */
#include <stdio.h>

int main(void) {
    char c;
    char str[20];
    char *p;
    int i;
    int passed;

    passed = 1;

    /* Basic char */
    c = 'A';
    if (c != 'A') {
        printf("FAIL: c = %c (expected A)\n", c);
        passed = 0;
    }

    /* Char arithmetic */
    c = 'A' + 1;
    if (c != 'B') {
        printf("FAIL: 'A'+1 = %c (expected B)\n", c);
        passed = 0;
    }

    /* Char comparison */
    c = 'Z';
    if (c < 'A' || c > 'Z') {
        printf("FAIL: 'Z' comparison\n");
        passed = 0;
    }

    /* Char array */
    str[0] = 'H';
    str[1] = 'i';
    str[2] = '\0';
    if (str[0] != 'H' || str[1] != 'i' || str[2] != '\0') {
        printf("FAIL: char array\n");
        passed = 0;
    }

    /* String literal */
    p = "Hello";
    if (p[0] != 'H' || p[4] != 'o' || p[5] != '\0') {
        printf("FAIL: string literal\n");
        passed = 0;
    }

    /* String length */
    p = "Test";
    i = 0;
    while (p[i] != '\0') {
        i++;
    }
    if (i != 4) {
        printf("FAIL: string length = %d (expected 4)\n", i);
        passed = 0;
    }

    /* Escape sequences */
    c = '\n';
    if (c != 10) {
        printf("FAIL: '\\n' = %d (expected 10)\n", c);
        passed = 0;
    }
    c = '\t';
    if (c != 9) {
        printf("FAIL: '\\t' = %d (expected 9)\n", c);
        passed = 0;
    }
    c = '\\';
    if (c != 92) {
        printf("FAIL: '\\\\' = %d (expected 92)\n", c);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_char\n");
    }
    return passed ? 0 : 1;
}
