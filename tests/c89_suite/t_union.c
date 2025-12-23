/*
 * Test: Unions
 * Tests: union declaration, member access
 */
#include <stdio.h>

union data {
    int i;
    char c;
};

int main(void) {
    union data d;
    int passed;

    passed = 1;

    /* Union stores one value at a time */
    d.i = 0x12345678;
    /* Reading char should give low byte (on little-endian) */
    /* We just test that we can access both members */

    d.i = 100;
    if (d.i != 100) {
        printf("FAIL: d.i = %d (expected 100)\n", d.i);
        passed = 0;
    }

    d.c = 'A';
    if (d.c != 'A') {
        printf("FAIL: d.c = %c (expected A)\n", d.c);
        passed = 0;
    }

    /* Union size should be max of members */
    /* Just verify we can use it */
    d.i = 255;
    if (d.i != 255) {
        printf("FAIL: d.i after = %d\n", d.i);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_union\n");
    }
    return passed ? 0 : 1;
}
