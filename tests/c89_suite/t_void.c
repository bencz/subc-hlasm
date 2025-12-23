/*
 * Test: Void type
 * Tests: void functions, void pointers
 */
#include <stdio.h>

int g_value;

void set_value(int v) {
    g_value = v;
}

void do_nothing(void) {
    /* empty */
}

int main(void) {
    void *vp;
    int x;
    int *ip;
    int passed;

    passed = 1;

    /* Void function */
    g_value = 0;
    set_value(123);
    if (g_value != 123) {
        printf("FAIL: set_value(123): g_value = %d\n", g_value);
        passed = 0;
    }

    /* Void function with no params */
    do_nothing();
    /* Just verify it compiles and runs */

    /* Void pointer */
    x = 42;
    vp = &x;
    ip = (int *)vp;
    if (*ip != 42) {
        printf("FAIL: void* cast: *ip = %d\n", *ip);
        passed = 0;
    }

    /* Void pointer assignment */
    ip = &x;
    vp = ip;
    if (vp != ip) {
        printf("FAIL: vp != ip\n");
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_void\n");
    }
    return passed ? 0 : 1;
}
