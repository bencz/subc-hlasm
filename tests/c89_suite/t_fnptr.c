/*
 * Test: Function pointers
 * Tests: function pointer declaration, assignment, call
 */
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int sub(int a, int b) {
    return a - b;
}

int mul(int a, int b) {
    return a * b;
}

int apply(int (*fn)(), int a, int b) {
    return fn(a, b);
}

int main(void) {
    int (*fp)();
    int r;
    int passed;

    passed = 1;

    /* Basic function pointer */
    fp = add;
    r = fp(3, 4);
    if (r != 7) {
        printf("FAIL: fp=add; fp(3,4) = %d (expected 7)\n", r);
        passed = 0;
    }

    /* Reassign function pointer */
    fp = sub;
    r = fp(10, 3);
    if (r != 7) {
        printf("FAIL: fp=sub; fp(10,3) = %d (expected 7)\n", r);
        passed = 0;
    }

    /* Function pointer as parameter */
    r = apply(add, 5, 6);
    if (r != 11) {
        printf("FAIL: apply(add,5,6) = %d (expected 11)\n", r);
        passed = 0;
    }

    r = apply(mul, 4, 5);
    if (r != 20) {
        printf("FAIL: apply(mul,4,5) = %d (expected 20)\n", r);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_fnptr\n");
    }
    return passed ? 0 : 1;
}
