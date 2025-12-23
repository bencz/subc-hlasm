/*
 * Test: Bitwise operations
 * Tests: &, |, ^, ~, <<, >>
 */
#include <stdio.h>

int main(void) {
    int a, b, c;
    int passed;

    passed = 1;
    a = 0x0F;
    b = 0xF0;

    /* AND */
    c = a & b;
    if (c != 0x00) {
        printf("FAIL: 0x0F & 0xF0 = 0x%x (expected 0x00)\n", c);
        passed = 0;
    }

    /* OR */
    c = a | b;
    if (c != 0xFF) {
        printf("FAIL: 0x0F | 0xF0 = 0x%x (expected 0xFF)\n", c);
        passed = 0;
    }

    /* XOR */
    c = a ^ b;
    if (c != 0xFF) {
        printf("FAIL: 0x0F ^ 0xF0 = 0x%x (expected 0xFF)\n", c);
        passed = 0;
    }

    /* NOT */
    a = 0;
    c = ~a;
    if (c != -1) {
        printf("FAIL: ~0 = %d (expected -1)\n", c);
        passed = 0;
    }

    /* Left shift */
    a = 1;
    c = a << 4;
    if (c != 16) {
        printf("FAIL: 1 << 4 = %d (expected 16)\n", c);
        passed = 0;
    }

    /* Right shift */
    a = 16;
    c = a >> 2;
    if (c != 4) {
        printf("FAIL: 16 >> 2 = %d (expected 4)\n", c);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_bitwise\n");
    }
    return passed ? 0 : 1;
}
