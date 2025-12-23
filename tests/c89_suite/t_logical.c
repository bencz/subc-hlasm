/*
 * Test: Logical operators
 * Tests: &&, ||, !
 */
#include <stdio.h>

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;
    a = 1;
    b = 0;

    /* Logical AND */
    c = a && a;
    if (c != 1) {
        printf("FAIL: 1 && 1 = %d (expected 1)\n", c);
        passed = 0;
    }
    c = a && b;
    if (c != 0) {
        printf("FAIL: 1 && 0 = %d (expected 0)\n", c);
        passed = 0;
    }
    c = b && b;
    if (c != 0) {
        printf("FAIL: 0 && 0 = %d (expected 0)\n", c);
        passed = 0;
    }

    /* Logical OR */
    c = a || a;
    if (c != 1) {
        printf("FAIL: 1 || 1 = %d (expected 1)\n", c);
        passed = 0;
    }
    c = a || b;
    if (c != 1) {
        printf("FAIL: 1 || 0 = %d (expected 1)\n", c);
        passed = 0;
    }
    c = b || b;
    if (c != 0) {
        printf("FAIL: 0 || 0 = %d (expected 0)\n", c);
        passed = 0;
    }

    /* Logical NOT */
    c = !a;
    if (c != 0) {
        printf("FAIL: !1 = %d (expected 0)\n", c);
        passed = 0;
    }
    c = !b;
    if (c != 1) {
        printf("FAIL: !0 = %d (expected 1)\n", c);
        passed = 0;
    }

    /* Short-circuit evaluation */
    c = 0;
    if (b && (c = 1)) {
        /* should not execute */
    }
    if (c != 0) {
        printf("FAIL: && short-circuit failed\n");
        passed = 0;
    }

    c = 0;
    if (a || (c = 1)) {
        /* should not execute c=1 */
    }
    if (c != 0) {
        printf("FAIL: || short-circuit failed\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_logical\n");
    }
    return passed ? 0 : 1;
}
