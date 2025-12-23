/*
 * Test: Array and pointer equivalence
 * Tests: array decay to pointer, pointer indexing
 */
#include <stdio.h>

int sum_ptr(int *arr, int n) {
    int i, s;
    s = 0;
    for (i = 0; i < n; i++) {
        s += arr[i];
    }
    return s;
}

int main(void) {
    int arr[5];
    int *p;
    int i;
    int passed;

    passed = 1;

    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;

    /* Array name decays to pointer */
    p = arr;
    if (p[0] != 1 || p[4] != 5) {
        printf("FAIL: p = arr; p[0]=%d, p[4]=%d\n", p[0], p[4]);
        passed = 0;
    }

    /* Pointer arithmetic same as indexing */
    if (*(p + 2) != arr[2]) {
        printf("FAIL: *(p+2) != arr[2]\n");
        passed = 0;
    }

    /* Pass array to function expecting pointer */
    i = sum_ptr(arr, 5);
    if (i != 15) {
        printf("FAIL: sum_ptr(arr,5) = %d (expected 15)\n", i);
        passed = 0;
    }

    /* Pointer can be indexed like array */
    p = arr;
    for (i = 0; i < 5; i++) {
        if (p[i] != arr[i]) {
            printf("FAIL: p[%d] != arr[%d]\n", i, i);
            passed = 0;
            break;
        }
    }

    /* &arr[0] == arr */
    if (&arr[0] != arr) {
        printf("FAIL: &arr[0] != arr\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_array_ptr\n");
    }
    return passed ? 0 : 1;
}
