/*
 * Test: sizeof operator
 * Tests: sizeof types and variables
 */
#include <stdio.h>

struct test_struct {
    int a;
    int b;
    char c;
};

int main(void) {
    int i;
    char c;
    int arr[10];
    int *p;
    int passed;

    passed = 1;

    /* sizeof char is always 1 */
    if (sizeof(char) != 1) {
        printf("FAIL: sizeof(char) = %d (expected 1)\n", (int)sizeof(char));
        passed = 0;
    }

    /* sizeof variable */
    if (sizeof(c) != 1) {
        printf("FAIL: sizeof(c) = %d (expected 1)\n", (int)sizeof(c));
        passed = 0;
    }

    /* sizeof int (architecture dependent, but > 0) */
    if (sizeof(int) < 1) {
        printf("FAIL: sizeof(int) = %d\n", (int)sizeof(int));
        passed = 0;
    }

    /* sizeof array */
    if (sizeof(arr) != 10 * sizeof(int)) {
        printf("FAIL: sizeof(arr) = %d (expected %d)\n",
               (int)sizeof(arr), (int)(10 * sizeof(int)));
        passed = 0;
    }

    /* sizeof pointer */
    if (sizeof(p) < 1) {
        printf("FAIL: sizeof(p) = %d\n", (int)sizeof(p));
        passed = 0;
    }

    /* sizeof struct */
    if (sizeof(struct test_struct) < sizeof(int) * 2 + sizeof(char)) {
        printf("FAIL: sizeof(struct) too small\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_sizeof\n");
    }
    return passed ? 0 : 1;
}
