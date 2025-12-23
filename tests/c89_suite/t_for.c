/*
 * Test: For loop
 * Tests: for, break, continue
 */
#include <stdio.h>

int main(void) {
    int i, sum;
    int passed;

    passed = 1;

    /* Basic for */
    sum = 0;
    for (i = 0; i < 5; i++) {
        sum += i;
    }
    if (sum != 10) {
        printf("FAIL: for sum = %d (expected 10)\n", sum);
        passed = 0;
    }

    /* For with break */
    sum = 0;
    for (i = 0; i < 100; i++) {
        if (i == 5) {
            break;
        }
        sum += i;
    }
    if (sum != 10 || i != 5) {
        printf("FAIL: for break: sum=%d, i=%d\n", sum, i);
        passed = 0;
    }

    /* For with continue */
    sum = 0;
    for (i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            continue;
        }
        sum += i;
    }
    /* sum = 1+3+5+7+9 = 25 */
    if (sum != 25) {
        printf("FAIL: for continue: sum=%d (expected 25)\n", sum);
        passed = 0;
    }

    /* For with empty parts */
    i = 0;
    sum = 0;
    for (;;) {
        sum += i;
        i++;
        if (i >= 5) {
            break;
        }
    }
    if (sum != 10) {
        printf("FAIL: for(;;) sum=%d (expected 10)\n", sum);
        passed = 0;
    }

    /* Nested for */
    sum = 0;
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            sum++;
        }
    }
    if (sum != 9) {
        printf("FAIL: nested for sum=%d (expected 9)\n", sum);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_for\n");
    }
    return passed ? 0 : 1;
}
