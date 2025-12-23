/*
 * Test: Pointers
 * Tests: pointer declaration, dereference, address-of, arithmetic
 */
#include <stdio.h>

int main(void) {
    int a, b;
    int *p;
    int arr[5];
    int passed;

    passed = 1;

    /* Basic pointer */
    a = 42;
    p = &a;
    if (*p != 42) {
        printf("FAIL: *p = %d (expected 42)\n", *p);
        passed = 0;
    }

    /* Modify through pointer */
    *p = 100;
    if (a != 100) {
        printf("FAIL: a = %d after *p=100 (expected 100)\n", a);
        passed = 0;
    }

    /* Pointer reassignment */
    b = 200;
    p = &b;
    if (*p != 200) {
        printf("FAIL: *p = %d after p=&b (expected 200)\n", *p);
        passed = 0;
    }

    /* Pointer to array element */
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;
    p = &arr[2];
    if (*p != 30) {
        printf("FAIL: *p = %d (expected 30)\n", *p);
        passed = 0;
    }

    /* Pointer arithmetic */
    p = arr;
    if (*(p + 2) != 30) {
        printf("FAIL: *(p+2) = %d (expected 30)\n", *(p + 2));
        passed = 0;
    }

    /* Pointer increment */
    p = arr;
    p++;
    if (*p != 20) {
        printf("FAIL: *p after p++ = %d (expected 20)\n", *p);
        passed = 0;
    }

    /* Pointer decrement */
    p = &arr[3];
    p--;
    if (*p != 30) {
        printf("FAIL: *p after p-- = %d (expected 30)\n", *p);
        passed = 0;
    }

    /* Pointer subtraction */
    {
        int *p1, *p2;
        int diff;
        p1 = &arr[1];
        p2 = &arr[4];
        diff = p2 - p1;
        if (diff != 3) {
            printf("FAIL: p2-p1 = %d (expected 3)\n", diff);
            passed = 0;
        }
    }

    if (passed) {
        printf("PASS: t_pointer\n");
    }
    return passed ? 0 : 1;
}
