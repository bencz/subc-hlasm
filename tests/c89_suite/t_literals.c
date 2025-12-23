/*
 * Test: Numeric literals
 * Tests: decimal, hex, octal
 */
#include <stdio.h>

int main(void) {
    int d, h, o;
    int passed;

    passed = 1;

    /* Decimal */
    d = 100;
    if (d != 100) {
        printf("FAIL: decimal 100 = %d\n", d);
        passed = 0;
    }

    /* Hexadecimal */
    h = 0xFF;
    if (h != 255) {
        printf("FAIL: 0xFF = %d (expected 255)\n", h);
        passed = 0;
    }

    h = 0x10;
    if (h != 16) {
        printf("FAIL: 0x10 = %d (expected 16)\n", h);
        passed = 0;
    }

    h = 0xABCD;
    if (h != 43981) {
        printf("FAIL: 0xABCD = %d (expected 43981)\n", h);
        passed = 0;
    }

    /* Octal */
    o = 010;
    if (o != 8) {
        printf("FAIL: 010 = %d (expected 8)\n", o);
        passed = 0;
    }

    o = 0777;
    if (o != 511) {
        printf("FAIL: 0777 = %d (expected 511)\n", o);
        passed = 0;
    }

    /* Zero */
    d = 0;
    if (d != 0) {
        printf("FAIL: 0 = %d\n", d);
        passed = 0;
    }

    /* Negative */
    d = -42;
    if (d != -42) {
        printf("FAIL: -42 = %d\n", d);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_literals\n");
    }
    return passed ? 0 : 1;
}
