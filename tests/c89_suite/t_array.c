/*
 * Test: Arrays
 * Tests: array declaration, indexing, initialization
 */
#include <stdio.h>

int g_arr[5];

int main(void) {
    int arr[10];
    int init_arr[5];
    int i, sum;
    int passed;

    passed = 1;

    /* Array assignment and access */
    for (i = 0; i < 10; i++) {
        arr[i] = i * 2;
    }
    if (arr[0] != 0 || arr[5] != 10 || arr[9] != 18) {
        printf("FAIL: array assignment\n");
        passed = 0;
    }

    /* Array sum */
    sum = 0;
    for (i = 0; i < 10; i++) {
        sum += arr[i];
    }
    /* sum = 0+2+4+6+8+10+12+14+16+18 = 90 */
    if (sum != 90) {
        printf("FAIL: array sum = %d (expected 90)\n", sum);
        passed = 0;
    }

    /* Global array */
    for (i = 0; i < 5; i++) {
        g_arr[i] = i + 1;
    }
    sum = 0;
    for (i = 0; i < 5; i++) {
        sum += g_arr[i];
    }
    if (sum != 15) {
        printf("FAIL: global array sum = %d (expected 15)\n", sum);
        passed = 0;
    }

    /* Initialized array */
    init_arr[0] = 1;
    init_arr[1] = 2;
    init_arr[2] = 3;
    init_arr[3] = 4;
    init_arr[4] = 5;
    sum = 0;
    for (i = 0; i < 5; i++) {
        sum += init_arr[i];
    }
    if (sum != 15) {
        printf("FAIL: init array sum = %d (expected 15)\n", sum);
        passed = 0;
    }

    /* Array with pointer arithmetic */
    arr[0] = 100;
    arr[1] = 200;
    arr[2] = 300;
    if (*(arr + 1) != 200) {
        printf("FAIL: *(arr+1) = %d (expected 200)\n", *(arr + 1));
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_array\n");
    }
    return passed ? 0 : 1;
}
