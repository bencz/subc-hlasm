/*
 * Test: Switch statement
 * Tests: switch, case, default, break
 */
#include <stdio.h>

int main(void) {
    int a, result;
    int passed;

    passed = 1;

    /* Basic switch */
    a = 2;
    result = 0;
    switch (a) {
    case 1:
        result = 10;
        break;
    case 2:
        result = 20;
        break;
    case 3:
        result = 30;
        break;
    }
    if (result != 20) {
        printf("FAIL: switch case 2 = %d (expected 20)\n", result);
        passed = 0;
    }

    /* Switch with default */
    a = 99;
    result = 0;
    switch (a) {
    case 1:
        result = 10;
        break;
    case 2:
        result = 20;
        break;
    default:
        result = 100;
        break;
    }
    if (result != 100) {
        printf("FAIL: switch default = %d (expected 100)\n", result);
        passed = 0;
    }

    /* Fall-through */
    a = 1;
    result = 0;
    switch (a) {
    case 1:
        result += 10;
        /* fall through */
    case 2:
        result += 20;
        break;
    case 3:
        result += 30;
        break;
    }
    if (result != 30) {
        printf("FAIL: switch fall-through = %d (expected 30)\n", result);
        passed = 0;
    }

    /* Multiple cases same value */
    a = 2;
    result = 0;
    switch (a) {
    case 1:
    case 2:
    case 3:
        result = 100;
        break;
    default:
        result = 0;
        break;
    }
    if (result != 100) {
        printf("FAIL: switch multiple cases = %d (expected 100)\n", result);
        passed = 0;
    }

    /* Switch with negative case */
    a = -1;
    result = 0;
    switch (a) {
    case -1:
        result = 50;
        break;
    case 0:
        result = 60;
        break;
    }
    if (result != 50) {
        printf("FAIL: switch negative case = %d (expected 50)\n", result);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_switch\n");
    }
    return passed ? 0 : 1;
}
