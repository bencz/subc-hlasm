/*
 * Test: If-else statements
 * Tests: if, else, else if
 */
#include <stdio.h>

int main(void) {
    int a;
    int passed;
    int result;

    passed = 1;

    /* Simple if */
    a = 5;
    result = 0;
    if (a == 5) {
        result = 1;
    }
    if (result != 1) {
        printf("FAIL: simple if\n");
        passed = 0;
    }

    /* If-else (true branch) */
    a = 5;
    if (a == 5) {
        result = 10;
    } else {
        result = 20;
    }
    if (result != 10) {
        printf("FAIL: if-else true branch\n");
        passed = 0;
    }

    /* If-else (false branch) */
    a = 3;
    if (a == 5) {
        result = 10;
    } else {
        result = 20;
    }
    if (result != 20) {
        printf("FAIL: if-else false branch\n");
        passed = 0;
    }

    /* Else-if chain */
    a = 2;
    if (a == 1) {
        result = 100;
    } else if (a == 2) {
        result = 200;
    } else if (a == 3) {
        result = 300;
    } else {
        result = 400;
    }
    if (result != 200) {
        printf("FAIL: else-if chain = %d (expected 200)\n", result);
        passed = 0;
    }

    /* Nested if */
    a = 5;
    result = 0;
    if (a > 0) {
        if (a < 10) {
            result = 1;
        }
    }
    if (result != 1) {
        printf("FAIL: nested if\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_if_else\n");
    }
    return passed ? 0 : 1;
}
