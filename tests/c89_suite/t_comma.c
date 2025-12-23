/*
 * Test: Comma operator
 * Tests: ,
 */
#include <stdio.h>

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;

    /* Comma operator returns last value */
    a = (1, 2, 3);
    if (a != 3) {
        printf("FAIL: (1, 2, 3) = %d (expected 3)\n", a);
        passed = 0;
    }

    /* Comma with side effects */
    a = 0;
    b = 0;
    c = (a = 5, b = 10, a + b);
    if (c != 15 || a != 5 || b != 10) {
        printf("FAIL: comma side effects: a=%d, b=%d, c=%d\n", a, b, c);
        passed = 0;
    }

    /* Comma in for loop */
    a = 0;
    b = 0;
    for (a = 0, b = 10; a < 5; a++, b--) {
        /* empty */
    }
    if (a != 5 || b != 5) {
        printf("FAIL: for comma: a=%d, b=%d (expected 5, 5)\n", a, b);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_comma\n");
    }
    return passed ? 0 : 1;
}
