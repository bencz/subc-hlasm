/*
 * Test: Double pointers
 * Tests: pointer to pointer
 */
#include <stdio.h>

int main(void) {
    int a;
    int *p;
    int **pp;
    int passed;

    passed = 1;

    /* Double pointer */
    a = 42;
    p = &a;
    pp = &p;
    if (**pp != 42) {
        printf("FAIL: **pp = %d (expected 42)\n", **pp);
        passed = 0;
    }

    /* Modify through double pointer */
    **pp = 100;
    if (a != 100) {
        printf("FAIL: a = %d after **pp=100 (expected 100)\n", a);
        passed = 0;
    }

    /* Modify pointer through double pointer */
    {
        int b;
        b = 200;
        *pp = &b;
        if (*p != 200) {
            printf("FAIL: *p = %d after *pp=&b (expected 200)\n", *p);
            passed = 0;
        }
    }

    if (passed) {
        printf("PASS: t_pointer2\n");
    }
    return passed ? 0 : 1;
}
