/*
 * Test: Static variables
 * Tests: static local, static global
 */
#include <stdio.h>

static int s_global = 100;

int counter(void) {
    static int count = 0;
    count++;
    return count;
}

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;

    /* Static local retains value */
    a = counter();
    b = counter();
    c = counter();
    if (a != 1 || b != 2 || c != 3) {
        printf("FAIL: counter() = %d, %d, %d (expected 1,2,3)\n", a, b, c);
        passed = 0;
    }

    /* Static global */
    if (s_global != 100) {
        printf("FAIL: s_global = %d (expected 100)\n", s_global);
        passed = 0;
    }
    s_global = 200;
    if (s_global != 200) {
        printf("FAIL: s_global = %d (expected 200)\n", s_global);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_static\n");
    }
    return passed ? 0 : 1;
}
