/*
 * Test: Basic arithmetic operations
 * Tests: +, -, *, /, %, unary -, unary +
 */
#include <stdio.h>

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;
    a = 10;
    b = 3;

    /* Addition */
    c = a + b;
    if (c != 13) {
        printf("FAIL: 10 + 3 = %d (expected 13)\n", c);
        passed = 0;
    }

    /* Subtraction */
    c = a - b;
    if (c != 7) {
        printf("FAIL: 10 - 3 = %d (expected 7)\n", c);
        passed = 0;
    }

    /* Multiplication */
    c = a * b;
    if (c != 30) {
        printf("FAIL: 10 * 3 = %d (expected 30)\n", c);
        passed = 0;
    }

    /* Division */
    c = a / b;
    if (c != 3) {
        printf("FAIL: 10 / 3 = %d (expected 3)\n", c);
        passed = 0;
    }

    /* Modulo */
    c = a % b;
    if (c != 1) {
        printf("FAIL: 10 %% 3 = %d (expected 1)\n", c);
        passed = 0;
    }

    /* Unary minus */
    c = -a;
    if (c != -10) {
        printf("FAIL: -10 = %d (expected -10)\n", c);
        passed = 0;
    }

    /* Unary plus */
    c = +a;
    if (c != 10) {
        printf("FAIL: +10 = %d (expected 10)\n", c);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_arithmetic\n");
    }
    return passed ? 0 : 1;
}
