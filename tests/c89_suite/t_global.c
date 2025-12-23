/*
 * Test: Global variables
 * Tests: global int, char, arrays, initialization
 */
#include <stdio.h>

int g_int;
char g_char;
int g_arr[10];
int g_init_int = 42;
char g_init_char = 'X';
int g_init_arr[5] = {1, 2, 3, 4, 5};

int main(void) {
    int i, sum;
    int passed;

    passed = 1;

    /* Uninitialized globals should be zero */
    if (g_int != 0) {
        printf("FAIL: g_int = %d (expected 0)\n", g_int);
        passed = 0;
    }
    if (g_char != 0) {
        printf("FAIL: g_char = %d (expected 0)\n", g_char);
        passed = 0;
    }

    /* Modify global */
    g_int = 100;
    if (g_int != 100) {
        printf("FAIL: g_int = %d (expected 100)\n", g_int);
        passed = 0;
    }

    /* Global array */
    for (i = 0; i < 10; i++) {
        g_arr[i] = i;
    }
    sum = 0;
    for (i = 0; i < 10; i++) {
        sum += g_arr[i];
    }
    if (sum != 45) {
        printf("FAIL: g_arr sum = %d (expected 45)\n", sum);
        passed = 0;
    }

    /* Initialized global int */
    if (g_init_int != 42) {
        printf("FAIL: g_init_int = %d (expected 42)\n", g_init_int);
        passed = 0;
    }

    /* Initialized global char */
    if (g_init_char != 'X') {
        printf("FAIL: g_init_char = %c (expected X)\n", g_init_char);
        passed = 0;
    }

    /* Initialized global array */
    sum = 0;
    for (i = 0; i < 5; i++) {
        sum += g_init_arr[i];
    }
    if (sum != 15) {
        printf("FAIL: g_init_arr sum = %d (expected 15)\n", sum);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_global\n");
    }
    return passed ? 0 : 1;
}
