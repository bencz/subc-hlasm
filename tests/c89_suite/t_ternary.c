/*
 * Test: Ternary conditional operator
 * Tests: ? :
 */
#include <stdio.h>

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;
    a = 5;
    b = 10;

    /* True condition */
    c = (a < b) ? 1 : 0;
    if (c != 1) {
        printf("FAIL: (5 < 10) ? 1 : 0 = %d (expected 1)\n", c);
        passed = 0;
    }

    /* False condition */
    c = (a > b) ? 1 : 0;
    if (c != 0) {
        printf("FAIL: (5 > 10) ? 1 : 0 = %d (expected 0)\n", c);
        passed = 0;
    }

    /* Nested ternary */
    c = (a < b) ? ((a == 5) ? 100 : 200) : 300;
    if (c != 100) {
        printf("FAIL: nested ternary = %d (expected 100)\n", c);
        passed = 0;
    }

    /* Ternary with expressions */
    c = (a < b) ? a + b : a - b;
    if (c != 15) {
        printf("FAIL: (5<10) ? 5+10 : 5-10 = %d (expected 15)\n", c);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_ternary\n");
    }
    return passed ? 0 : 1;
}
