/*
 * Test: Functions
 * Tests: function declaration, call, return, parameters
 */
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int sum_array(int *arr, int len) {
    int i, sum;
    sum = 0;
    for (i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum;
}

void modify(int *p) {
    *p = 999;
}

int global_var;

void set_global(int val) {
    global_var = val;
}

int main(void) {
    int a, b, c;
    int arr[5];
    int passed;

    passed = 1;

    /* Simple function call */
    c = add(3, 4);
    if (c != 7) {
        printf("FAIL: add(3,4) = %d (expected 7)\n", c);
        passed = 0;
    }

    /* Recursive function */
    c = factorial(5);
    if (c != 120) {
        printf("FAIL: factorial(5) = %d (expected 120)\n", c);
        passed = 0;
    }

    /* Function with array parameter */
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    arr[3] = 4;
    arr[4] = 5;
    c = sum_array(arr, 5);
    if (c != 15) {
        printf("FAIL: sum_array = %d (expected 15)\n", c);
        passed = 0;
    }

    /* Function modifying through pointer */
    a = 0;
    modify(&a);
    if (a != 999) {
        printf("FAIL: modify(&a): a = %d (expected 999)\n", a);
        passed = 0;
    }

    /* Function modifying global */
    global_var = 0;
    set_global(123);
    if (global_var != 123) {
        printf("FAIL: set_global: global_var = %d\n", global_var);
        passed = 0;
    }

    /* Nested function calls */
    c = add(add(1, 2), add(3, 4));
    if (c != 10) {
        printf("FAIL: nested add = %d (expected 10)\n", c);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_function\n");
    }
    return passed ? 0 : 1;
}
