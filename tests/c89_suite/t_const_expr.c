/*
 * Test: Constant expressions
 * Tests: compile-time evaluation
 */
#include <stdio.h>

#define SIZE 10
#define DOUBLE(x) ((x) * 2)

int arr[SIZE];
int arr2[5 + 5];

int main(void) {
    int passed;
    int i;

    passed = 1;

    /* Array size from constant */
    for (i = 0; i < SIZE; i++) {
        arr[i] = i;
    }
    if (arr[9] != 9) {
        printf("FAIL: arr[9] = %d\n", arr[9]);
        passed = 0;
    }

    /* Array size from expression */
    for (i = 0; i < 10; i++) {
        arr2[i] = i * 2;
    }
    if (arr2[9] != 18) {
        printf("FAIL: arr2[9] = %d\n", arr2[9]);
        passed = 0;
    }

    /* Constant in switch case */
    i = SIZE;
    switch (i) {
    case SIZE:
        /* ok */
        break;
    default:
        printf("FAIL: switch case SIZE\n");
        passed = 0;
        break;
    }

    /* Constant expression in condition */
    if (SIZE > 5) {
        /* ok */
    } else {
        printf("FAIL: SIZE > 5\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_const_expr\n");
    }
    return passed ? 0 : 1;
}
