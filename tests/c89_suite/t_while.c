/*
 * Test: While loop
 * Tests: while, break, continue
 */
#include <stdio.h>

int main(void) {
    int i, sum;
    int passed;

    passed = 1;

    /* Basic while */
    i = 0;
    sum = 0;
    while (i < 5) {
        sum += i;
        i++;
    }
    if (sum != 10) {
        printf("FAIL: while sum = %d (expected 10)\n", sum);
        passed = 0;
    }

    /* While with break */
    i = 0;
    sum = 0;
    while (i < 100) {
        if (i == 5) {
            break;
        }
        sum += i;
        i++;
    }
    if (sum != 10 || i != 5) {
        printf("FAIL: while break: sum=%d, i=%d\n", sum, i);
        passed = 0;
    }

    /* While with continue */
    i = 0;
    sum = 0;
    while (i < 10) {
        i++;
        if (i % 2 == 0) {
            continue;
        }
        sum += i;
    }
    /* sum = 1+3+5+7+9 = 25 */
    if (sum != 25) {
        printf("FAIL: while continue: sum=%d (expected 25)\n", sum);
        passed = 0;
    }

    /* While false condition */
    i = 0;
    while (0) {
        i = 100;
    }
    if (i != 0) {
        printf("FAIL: while(0) should not execute\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_while\n");
    }
    return passed ? 0 : 1;
}
