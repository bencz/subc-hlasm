/*
 * Test: Do-while loop
 * Tests: do-while, break, continue
 */
#include <stdio.h>

int main(void) {
    int i, sum;
    int passed;

    passed = 1;

    /* Basic do-while */
    i = 0;
    sum = 0;
    do {
        sum += i;
        i++;
    } while (i < 5);
    if (sum != 10) {
        printf("FAIL: do-while sum = %d (expected 10)\n", sum);
        passed = 0;
    }

    /* Do-while executes at least once */
    i = 100;
    sum = 0;
    do {
        sum = 1;
    } while (i < 5);
    if (sum != 1) {
        printf("FAIL: do-while should execute at least once\n");
        passed = 0;
    }

    /* Do-while with break */
    i = 0;
    sum = 0;
    do {
        if (i == 5) {
            break;
        }
        sum += i;
        i++;
    } while (i < 100);
    if (sum != 10 || i != 5) {
        printf("FAIL: do-while break: sum=%d, i=%d\n", sum, i);
        passed = 0;
    }

    /* Do-while with continue */
    i = 0;
    sum = 0;
    do {
        i++;
        if (i % 2 == 0) {
            continue;
        }
        sum += i;
    } while (i < 10);
    /* sum = 1+3+5+7+9 = 25 */
    if (sum != 25) {
        printf("FAIL: do-while continue: sum=%d (expected 25)\n", sum);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_do_while\n");
    }
    return passed ? 0 : 1;
}
