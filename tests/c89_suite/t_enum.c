/*
 * Test: Enumerations
 * Tests: enum declaration, values
 */
#include <stdio.h>

enum color {
    RED,
    GREEN,
    BLUE
};

enum values {
    VAL_A = 10,
    VAL_B,
    VAL_C = 100,
    VAL_D
};

int main(void) {
    enum color c;
    int passed;

    passed = 1;

    /* Default enum values start at 0 */
    if (RED != 0 || GREEN != 1 || BLUE != 2) {
        printf("FAIL: RED=%d, GREEN=%d, BLUE=%d\n", RED, GREEN, BLUE);
        passed = 0;
    }

    /* Enum variable */
    c = GREEN;
    if (c != 1) {
        printf("FAIL: c = %d (expected 1)\n", c);
        passed = 0;
    }

    /* Explicit enum values */
    if (VAL_A != 10 || VAL_B != 11) {
        printf("FAIL: VAL_A=%d, VAL_B=%d\n", VAL_A, VAL_B);
        passed = 0;
    }
    if (VAL_C != 100 || VAL_D != 101) {
        printf("FAIL: VAL_C=%d, VAL_D=%d\n", VAL_C, VAL_D);
        passed = 0;
    }

    /* Enum in switch */
    c = BLUE;
    switch (c) {
    case RED:
        passed = 0;
        break;
    case GREEN:
        passed = 0;
        break;
    case BLUE:
        /* correct */
        break;
    default:
        passed = 0;
        break;
    }

    if (passed) {
        printf("PASS: t_enum\n");
    }
    return passed ? 0 : 1;
}
