/*
 * Test: Increment and decrement operators
 * Tests: ++, -- (prefix and postfix)
 */
#include <stdio.h>

int main(void) {
    int a, b;
    int passed;

    passed = 1;

    /* Prefix increment */
    a = 5;
    b = ++a;
    if (a != 6 || b != 6) {
        printf("FAIL: ++5: a=%d, b=%d (expected 6, 6)\n", a, b);
        passed = 0;
    }

    /* Postfix increment */
    a = 5;
    b = a++;
    if (a != 6 || b != 5) {
        printf("FAIL: 5++: a=%d, b=%d (expected 6, 5)\n", a, b);
        passed = 0;
    }

    /* Prefix decrement */
    a = 5;
    b = --a;
    if (a != 4 || b != 4) {
        printf("FAIL: --5: a=%d, b=%d (expected 4, 4)\n", a, b);
        passed = 0;
    }

    /* Postfix decrement */
    a = 5;
    b = a--;
    if (a != 4 || b != 5) {
        printf("FAIL: 5--: a=%d, b=%d (expected 4, 5)\n", a, b);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_increment\n");
    }
    return passed ? 0 : 1;
}
