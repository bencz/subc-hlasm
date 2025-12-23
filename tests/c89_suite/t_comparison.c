/*
 * Test: Comparison operators
 * Tests: ==, !=, <, >, <=, >=
 */
#include <stdio.h>

int main(void) {
    int a, b;
    int passed;

    passed = 1;
    a = 5;
    b = 10;

    /* Equal */
    if ((a == a) != 1) {
        printf("FAIL: 5 == 5 should be true\n");
        passed = 0;
    }
    if ((a == b) != 0) {
        printf("FAIL: 5 == 10 should be false\n");
        passed = 0;
    }

    /* Not equal */
    if ((a != b) != 1) {
        printf("FAIL: 5 != 10 should be true\n");
        passed = 0;
    }
    if ((a != a) != 0) {
        printf("FAIL: 5 != 5 should be false\n");
        passed = 0;
    }

    /* Less than */
    if ((a < b) != 1) {
        printf("FAIL: 5 < 10 should be true\n");
        passed = 0;
    }
    if ((b < a) != 0) {
        printf("FAIL: 10 < 5 should be false\n");
        passed = 0;
    }

    /* Greater than */
    if ((b > a) != 1) {
        printf("FAIL: 10 > 5 should be true\n");
        passed = 0;
    }
    if ((a > b) != 0) {
        printf("FAIL: 5 > 10 should be false\n");
        passed = 0;
    }

    /* Less or equal */
    if ((a <= b) != 1) {
        printf("FAIL: 5 <= 10 should be true\n");
        passed = 0;
    }
    if ((a <= a) != 1) {
        printf("FAIL: 5 <= 5 should be true\n");
        passed = 0;
    }

    /* Greater or equal */
    if ((b >= a) != 1) {
        printf("FAIL: 10 >= 5 should be true\n");
        passed = 0;
    }
    if ((b >= b) != 1) {
        printf("FAIL: 10 >= 10 should be true\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_comparison\n");
    }
    return passed ? 0 : 1;
}
