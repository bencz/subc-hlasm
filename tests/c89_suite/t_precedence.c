/*
 * Test: Operator precedence
 * Tests: correct evaluation order
 */
#include <stdio.h>

int main(void) {
    int a, b, c, r;
    int passed;

    passed = 1;

    /* Multiplication before addition */
    r = 2 + 3 * 4;
    if (r != 14) {
        printf("FAIL: 2+3*4 = %d (expected 14)\n", r);
        passed = 0;
    }

    /* Division before subtraction */
    r = 10 - 6 / 2;
    if (r != 7) {
        printf("FAIL: 10-6/2 = %d (expected 7)\n", r);
        passed = 0;
    }

    /* Parentheses override */
    r = (2 + 3) * 4;
    if (r != 20) {
        printf("FAIL: (2+3)*4 = %d (expected 20)\n", r);
        passed = 0;
    }

    /* Comparison vs arithmetic */
    r = 5 + 3 < 10;
    if (r != 1) {
        printf("FAIL: 5+3<10 = %d (expected 1)\n", r);
        passed = 0;
    }

    /* Logical vs comparison */
    a = 1;
    b = 0;
    r = a == 1 && b == 0;
    if (r != 1) {
        printf("FAIL: a==1 && b==0 = %d (expected 1)\n", r);
        passed = 0;
    }

    /* Bitwise vs comparison */
    r = 5 & 3 == 3;
    /* 3==3 is 1, then 5&1 is 1 */
    if (r != 1) {
        printf("FAIL: 5&3==3 = %d (expected 1)\n", r);
        passed = 0;
    }

    /* Assignment is right-to-left */
    a = b = c = 5;
    if (a != 5 || b != 5 || c != 5) {
        printf("FAIL: a=b=c=5: a=%d, b=%d, c=%d\n", a, b, c);
        passed = 0;
    }

    /* Shift vs addition */
    r = 1 << 2 + 1;
    /* 2+1=3, then 1<<3=8 */
    if (r != 8) {
        printf("FAIL: 1<<2+1 = %d (expected 8)\n", r);
        passed = 0;
    }

    if (passed) {
        printf("PASS: t_precedence\n");
    }
    return passed ? 0 : 1;
}
