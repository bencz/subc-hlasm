/*
 * Test: Assignment operators
 * Tests: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
 */
#include <stdio.h>

int main(void) {
    int a;
    int passed;

    passed = 1;

    /* Simple assignment */
    a = 10;
    if (a != 10) {
        printf("FAIL: a = 10 failed\n");
        passed = 0;
    }

    /* Add-assign */
    a = 10;
    a += 5;
    if (a != 15) {
        printf("FAIL: 10 += 5 = %d (expected 15)\n", a);
        passed = 0;
    }

    /* Sub-assign */
    a = 10;
    a -= 3;
    if (a != 7) {
        printf("FAIL: 10 -= 3 = %d (expected 7)\n", a);
        passed = 0;
    }

    /* Mul-assign */
    a = 10;
    a *= 2;
    if (a != 20) {
        printf("FAIL: 10 *= 2 = %d (expected 20)\n", a);
        passed = 0;
    }

    /* Div-assign */
    a = 10;
    a /= 2;
    if (a != 5) {
        printf("FAIL: 10 /= 2 = %d (expected 5)\n", a);
        passed = 0;
    }

    /* Mod-assign */
    a = 10;
    a %= 3;
    if (a != 1) {
        printf("FAIL: 10 %%= 3 = %d (expected 1)\n", a);
        passed = 0;
    }

    /* And-assign */
    a = 0xFF;
    a &= 0x0F;
    if (a != 0x0F) {
        printf("FAIL: 0xFF &= 0x0F = 0x%x (expected 0x0F)\n", a);
        passed = 0;
    }

    /* Or-assign */
    a = 0x0F;
    a |= 0xF0;
    if (a != 0xFF) {
        printf("FAIL: 0x0F |= 0xF0 = 0x%x (expected 0xFF)\n", a);
        passed = 0;
    }

    /* Xor-assign */
    a = 0xFF;
    a ^= 0x0F;
    if (a != 0xF0) {
        printf("FAIL: 0xFF ^= 0x0F = 0x%x (expected 0xF0)\n", a);
        passed = 0;
    }

    /* Left-shift-assign */
    a = 1;
    a <<= 4;
    if (a != 16) {
        printf("FAIL: 1 <<= 4 = %d (expected 16)\n", a);
        passed = 0;
    }

    /* Right-shift-assign */
    a = 16;
    a >>= 2;
    if (a != 4) {
        printf("FAIL: 16 >>= 2 = %d (expected 4)\n", a);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_assignment\n");
    }
    return passed ? 0 : 1;
}
