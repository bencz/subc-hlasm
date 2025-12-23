/*
 * Test: Variable scope
 * Tests: local vs global, block scope
 */
#include <stdio.h>

int x = 100;

int get_x(void) {
    return x;
}

int main(void) {
    int x;
    int passed;

    passed = 1;

    /* Local shadows global */
    x = 50;
    if (x != 50) {
        printf("FAIL: local x = %d (expected 50)\n", x);
        passed = 0;
    }

    /* Function sees global */
    if (get_x() != 100) {
        printf("FAIL: get_x() = %d (expected 100)\n", get_x());
        passed = 0;
    }

    /* Block scope */
    {
        int x;
        x = 25;
        if (x != 25) {
            printf("FAIL: block x = %d (expected 25)\n", x);
            passed = 0;
        }
    }

    /* After block, outer x is visible */
    if (x != 50) {
        printf("FAIL: after block x = %d (expected 50)\n", x);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_scope\n");
    }
    return passed ? 0 : 1;
}
