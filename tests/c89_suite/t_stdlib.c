/*
 * Test: Standard library functions (stdlib.h)
 * Tests: abs, atoi
 */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int passed;

    passed = 1;

    /* abs */
    if (abs(5) != 5) {
        printf("FAIL: abs(5) = %d\n", abs(5));
        passed = 0;
    }
    if (abs(-5) != 5) {
        printf("FAIL: abs(-5) = %d\n", abs(-5));
        passed = 0;
    }
    if (abs(0) != 0) {
        printf("FAIL: abs(0) = %d\n", abs(0));
        passed = 0;
    }

    /* atoi */
    if (atoi("123") != 123) {
        printf("FAIL: atoi(\"123\") = %d\n", atoi("123"));
        passed = 0;
    }
    if (atoi("-456") != -456) {
        printf("FAIL: atoi(\"-456\") = %d\n", atoi("-456"));
        passed = 0;
    }
    if (atoi("0") != 0) {
        printf("FAIL: atoi(\"0\") = %d\n", atoi("0"));
        passed = 0;
    }
    if (atoi("  42") != 42) {
        printf("FAIL: atoi(\"  42\") = %d\n", atoi("  42"));
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_stdlib\n");
    }
    return passed ? 0 : 1;
}
